#pragma once
#ifndef STATE_HASH_H
#define STATE_HASH_H

#include <cstdint>
#include <cstdio>
#include <deque>
#include <map>
#include <string>
#include <utility>

#include "Rollback.h"
#include "PlayerInsStructFunctions.h"
#include "BulletManStructFunctions.h"
#include "DamageManStructFunctions.h"
#include "FrpgHavokManImpStructFunctions.h"
#include "ThrowManStructFunctions.h"
#include "DmgHitRecordManImpStructFunctions.h"

static const char* hash_logfilename = "dsoverhaul_statehash_logging.txt";
extern FILE* hash_logfile;

// SfxMan is currently disabled in the active rollback state (state->sfxman is
// NULL -- the copy_SfxMan calls are commented out in Rollback.cpp), so it is not
// hashed here. Re-enable the two SfxMan lines below once it is restored.

/*
 * Tier-0 / Tier-1 oracle orchestration (header-only).
 *
 * Tier 0 (detect): record() hashes the saved RollbackState every frame and keys
 * it by GGPO frame number; emit_confirmed() writes one STATEHASH log line per
 * frame once that frame can no longer be rolled back. Run both instances, then
 * diff the two logs: the first frame whose digests differ is the desync, and the
 * per-subsystem columns say which subsystem.
 *
 * Tier 1 (localize): dump_state() writes the full canonical text dump for one
 * frame so the two instances' dumps can be diffed down to the exact field.
 *
 * combined() also feeds GGPO synctest's checksum, covering single-process
 * save/restore determinism with the same hash.
 *
 * All per-subsystem hashes come from serialize_X (StateSerializer.h), which is
 * shared with print_X, so a hash mismatch always maps to a textual diff.
 */
namespace RollbackHash
{
    struct StateDigest
    {
        uint64_t player = 0;   // fold of all connected players' PlayerIns hashes
        uint64_t bullet = 0;
        uint64_t damage = 0;
        uint64_t havok  = 0;
        uint64_t throwman = 0;
        uint64_t dmghit = 0;
        // uint64_t sfx = 0;   // re-enable when SfxMan is restored
    };

    inline uint64_t _fold(uint64_t h, uint64_t x)
    {
        const uint64_t P = 1099511628211ULL;
        h ^= x; h *= P;
        return h;
    }

    // Canonical player order for hashing.
    // Order this so that the resulting array is deterministic regardless of viewpoint (ordered by steam id)
    inline void canonical_player_order(const RollbackState* s, uint32_t n, uint32_t* order)
    {
        for (uint32_t i = 0; i < n; i++) order[i] = i;
        for (uint32_t i = 1; i < n; i++)   // insertion sort; n <= GGPO_MAX_PLAYERS
        {
            uint32_t k = order[i];
            uint64_t key = s->player_steam_ids[k];
            int j = (int)i - 1;
            while (j >= 0 && s->player_steam_ids[order[j]] > key)
            {
                order[j + 1] = order[j];
                j--;
            }
            order[j + 1] = k;
        }
    }

    inline StateDigest digest_of(RollbackState* s)
    {
        StateDigest d;

        const uint32_t n = Rollback::ggpoCurrentPlayerCount;
        uint32_t order[GGPO_MAX_PLAYERS];
        canonical_player_order(s, n, order);

        // Hash the players in canonical (steam-id) order
        PlayerIns* ordered[GGPO_MAX_PLAYERS] = {};
        uint64_t pl = 1469598103934665603ULL;
        for (uint32_t i = 0; i < n; i++)
        {
            ordered[i] = s->playerins[order[i]];
            pl = _fold(pl, s->player_steam_ids[order[i]]);
            pl = _fold(pl, hash_PlayerIns(ordered[i]));
        }
        d.player   = pl;
        d.bullet   = hash_BulletMan(s->bulletman);
        d.damage   = hash_DamageMan(s->damageman);
        d.havok    = hash_FrpgHavokManImp(s->havokman, order, n);
        d.throwman = hash_ThrowMan(s->throwman);
        d.dmghit   = hash_DmgHitRecordManImp(s->dmghitrecordman);
        // d.sfx   = hash_SfxMan(s->sfxman);
        return d;
    }

    // Order-sensitive fold so swapping two subsystems' values still changes the
    // combined digest. Also used as the GGPO synctest checksum.
    inline uint64_t combined(const StateDigest& d)
    {
        uint64_t h = 1469598103934665603ULL;
        const uint64_t parts[] = { d.player, d.bullet, d.damage, d.havok, d.throwman, d.dmghit };
        for (size_t i = 0; i < sizeof(parts) / sizeof(parts[0]); i++) h = _fold(h, parts[i]);
        return h;
    }

    inline std::map<int, StateDigest>& _store()
    {
        static std::map<int, StateDigest> m;   // frame -> last digest saved for it
        return m;
    }

    // ---- confirmed-digest history, served by the control plane's "hashes" command ----
    // The live verifier (rollback_verify.py) polls "hashes since=<frame>" on both
    // instances and aligns by frame, so it needs no log file access (the sandboxed
    // instance's files are inside Sandboxie). ~2.3 minutes at 60 fps; a poller
    // that falls further behind than that sees "gap":true and knows to restart.
    inline const size_t HISTORY_MAX = 8192;
    inline std::deque<std::pair<int, StateDigest>>& _history()
    {
        static std::deque<std::pair<int, StateDigest>> h;
        return h;
    }
    inline int last_confirmed_emitted = -1;   // newest frame in _history(), -1 if none

    inline std::string _hex(uint64_t v)
    {
        char b[24];
        snprintf(b, sizeof(b), "%016llx", (unsigned long long)v);
        return b;
    }

    inline std::string _digest_json(int frame, const StateDigest& d)
    {
        std::string s = "{\"f\":" + std::to_string(frame);
        s += ",\"player\":\"" + _hex(d.player) + "\"";
        s += ",\"bullet\":\"" + _hex(d.bullet) + "\"";
        s += ",\"damage\":\"" + _hex(d.damage) + "\"";
        s += ",\"havok\":\"" + _hex(d.havok) + "\"";
        s += ",\"throw\":\"" + _hex(d.throwman) + "\"";
        s += ",\"dmghit\":\"" + _hex(d.dmghit) + "\"";
        s += ",\"comb\":\"" + _hex(combined(d)) + "\"}";
        return s;
    }

    // Confirmed digests with frame > since_frame, oldest first, at most max_count.
    // "gap" is true when history no longer reaches back to since_frame+1.
    inline std::string hashes_json(int since_frame, int max_count)
    {
        std::deque<std::pair<int, StateDigest>>& h = _history();
        int oldest = h.empty() ? -1 : h.front().first;
        int newest = h.empty() ? -1 : h.back().first;
        std::string s = "{\"frames\":[";
        int n = 0;
        for (const auto& e : h)
        {
            if (e.first <= since_frame) continue;
            if (n >= max_count) break;
            if (n) s += ",";
            s += _digest_json(e.first, e.second);
            n++;
        }
        s += "],\"count\":" + std::to_string(n);
        s += ",\"oldest\":" + std::to_string(oldest);
        s += ",\"newest\":" + std::to_string(newest);
        bool gap = (since_frame >= 0 && oldest > since_frame + 1);
        s += ",\"gap\":" + std::string(gap ? "true" : "false") + "}";
        return s;
    }

    // ---- Tier-1 localizer: full canonical text dump of one saved frame ----
    // The control plane arms dump_request_frame ("dump_at <frame>"); record()
    // captures that frame's text each time GGPO saves it (a rollback re-save
    // overwrites with the corrected state) until emit_confirmed sees it confirmed.
    // "dump_get" then returns the text, so both instances can be dumped at the
    // SAME GGPO frame and diffed field by field. Also written to
    // statedump_harness_<frame>.txt next to the exe.
    inline int         dump_request_frame = -1;
    inline int         dump_frame = -1;       // frame the stored dump is for, -1 = none
    inline bool        dump_confirmed = false;
    inline std::string dump_text_store;
    inline std::string dump_file;

    inline std::string dump_text_of(RollbackState* s)
    {
        std::string t;
        const uint32_t n = Rollback::ggpoCurrentPlayerCount;
        uint32_t order[GGPO_MAX_PLAYERS];
        canonical_player_order(s, n, order);
        PlayerIns* ordered[GGPO_MAX_PLAYERS] = {};
        for (uint32_t i = 0; i < n; i++)
        {
            ordered[i] = s->playerins[order[i]];
            // The steam id goes in the header so a dump says WHICH character each
            // block is; the game slot alone does not, and differs per instance.
            char idbuf[32];
            snprintf(idbuf, sizeof(idbuf), "%llx", (unsigned long long)s->player_steam_ids[order[i]]);
            t += "=== Player ";
            t += std::to_string(i);
            t += " (steam_id ";
            t += idbuf;
            t += ", game slot ";
            t += std::to_string(order[i]);
            t += ") ===\n";
            t += print_PlayerIns(ordered[i]);
        }
        t += "=== BulletMan ===\n";        t += print_BulletMan(s->bulletman);
        t += "=== DamageMan ===\n";        t += print_DamageMan(s->damageman);
        t += "=== Havok ===\n";            t += print_FrpgHavokManImp(s->havokman, order, n);
        t += "=== ThrowMan ===\n";         t += print_ThrowMan(s->throwman);
        t += "=== DmgHitRecordManImp ===\n"; t += print_DmgHitRecordManImp(s->dmghitrecordman);
        return t;
    }

    inline void _write_text_file(const char* name, const std::string& t)
    {
        FILE* fp = nullptr;
        fopen_s(&fp, name, "w");
        if (!fp) return;
        fwrite(t.data(), 1, t.size(), fp);
        fclose(fp);
    }

    // Dump a saved state to statedump_<tag>_<frame>.txt next to the game exe.
    inline void dump_state(int frame, RollbackState* s, const char* tag)
    {
        char name[128];
        snprintf(name, sizeof(name), "statedump_%s_%d.txt", tag, frame);
        _write_text_file(name, dump_text_of(s));
    }

    inline std::string dump_status_fields()
    {
        std::string s = "\"dump_requested\":" + std::to_string(dump_request_frame);
        s += ",\"dump_frame\":" + std::to_string(dump_frame);
        s += ",\"dump_confirmed\":" + std::string(dump_confirmed ? "true" : "false");
        s += ",\"dump_size\":" + std::to_string(dump_text_store.size());
        s += ",\"dump_file\":\"" + dump_file + "\"";
        return s;
    }

#ifdef GGPO_SYNCTEST
    // ---- GGPO synctest: which subsystems a re-simulated frame disagrees in ----
    // Synctest saves every frame twice: once as it first runs, then again after loading an
    // earlier frame and re-simulating up to it. The second save reaches record() while the
    // first one's digest is still in _store(), so comparing them names the subsystems that
    // did not survive save -> load -> resim. GGPO's own int checksum only says *something* did.
    // The "original" state GGPO logs on a mismatch is its copy_buffer deep copy (StateTarget::Copy), not the save
    // itself, so a lossy Copy path shows up as dump differences no real session has. synctest_check_copy tests it.
    inline const char* const SYNCTEST_SUBSYSTEMS[] = { "player", "bullet", "damage", "havok", "throw", "dmghit" };
    inline const int SYNCTEST_SUBSYSTEM_COUNT = 6;
    inline const uint32_t SYNCTEST_ALL = (1u << SYNCTEST_SUBSYSTEM_COUNT) - 1;

    struct SyncTestStats
    {
        uint64_t checked = 0;               // re-simulated saves compared against the original
        uint64_t mismatched = 0;            // ...of which differed
        int first_mismatch_frame = -1;
        int last_mismatch_frame = -1;
        uint64_t by_subsystem[SYNCTEST_SUBSYSTEM_COUNT] = {};
        uint64_t copies = 0;                // copy_buffer calls checked
        uint64_t copy_mismatched = 0;       // ...whose copy hashed differently from its source
        uint64_t copy_by_subsystem[SYNCTEST_SUBSYSTEM_COUNT] = {};
        int dump_files_written = 0;         // full-state dumps written by rollback_log_game_state
        // player 0 around each live and each re-simulated frame (Rollback.cpp synctest_observe): state that moves in live
        // frames but never in re-simulated ones is stepped outside Step_GameSimulation, not merely left unrestored
        uint64_t live_frames = 0, resim_frames = 0;
        uint64_t sp_up_live = 0, sp_up_resim = 0;                             // frames curSp rose
        uint64_t attach_head_changes_live = 0, attach_head_changes_resim = 0; // frames the ChrAttachSys head slot changed type
    };
    inline SyncTestStats synctest;

    // Which mismatches get full-state dumps ("synctest dump"): one subsystem can mismatch every frame and use up the budget
    struct SyncTestDumpConfig
    {
        uint32_t mask = SYNCTEST_ALL;       // bit i = SYNCTEST_SUBSYSTEMS[i]
        int from_frame = -1;
    };
    inline SyncTestDumpConfig synctest_dump;
    inline bool synctest_dump_this_mismatch = false;   // set by the latest compare, read by rollback_log_game_state

    inline const uint64_t SYNCTEST_LOG_FIRST = 20;   // mismatches logged one by one...
    inline const uint64_t SYNCTEST_LOG_EVERY = 600;  // ...then one line per this many
    inline const int SYNCTEST_MAX_DUMP_FILES = 16;   // 8 mismatching frames, original + replay each

    // Comma-separated names of the subsystems whose digests differ; mask gets bit i for SYNCTEST_SUBSYSTEMS[i]
    inline std::string synctest_diff(const StateDigest& a, const StateDigest& b, uint32_t& mask)
    {
        const uint64_t av[] = { a.player, a.bullet, a.damage, a.havok, a.throwman, a.dmghit };
        const uint64_t bv[] = { b.player, b.bullet, b.damage, b.havok, b.throwman, b.dmghit };
        std::string names;
        mask = 0;
        for (int i = 0; i < SYNCTEST_SUBSYSTEM_COUNT; i++)
        {
            if (av[i] == bv[i]) continue;
            mask |= 1u << i;
            if (!names.empty()) names += ",";
            names += SYNCTEST_SUBSYSTEMS[i];
        }
        return names;
    }

    inline void synctest_compare(int frame, const StateDigest& orig, const StateDigest& replay)
    {
        synctest.checked++;
        uint32_t mask;
        std::string diff = synctest_diff(orig, replay, mask);
        synctest_dump_this_mismatch = false;
        if (mask == 0) return;

        for (int i = 0; i < SYNCTEST_SUBSYSTEM_COUNT; i++)
        {
            if (mask & (1u << i)) synctest.by_subsystem[i]++;
        }
        synctest.mismatched++;
        if (synctest.first_mismatch_frame < 0) synctest.first_mismatch_frame = frame;
        synctest.last_mismatch_frame = frame;
        synctest_dump_this_mismatch = (mask & synctest_dump.mask) != 0 && frame >= synctest_dump.from_frame;
        if (synctest.mismatched <= SYNCTEST_LOG_FIRST || synctest.mismatched % SYNCTEST_LOG_EVERY == 0)
        {
            ConsoleWrite("SYNCTEST mismatch #%llu frame=%d subsystems=%s", (unsigned long long)synctest.mismatched, frame, diff.c_str());
        }
    }

    inline void synctest_check_copy(RollbackState* src, RollbackState* dst)
    {
        synctest.copies++;
        uint32_t mask;
        std::string diff = synctest_diff(digest_of(src), digest_of(dst), mask);
        if (mask == 0) return;

        for (int i = 0; i < SYNCTEST_SUBSYSTEM_COUNT; i++)
        {
            if (mask & (1u << i)) synctest.copy_by_subsystem[i]++;
        }
        synctest.copy_mismatched++;
        if (synctest.copy_mismatched <= SYNCTEST_LOG_FIRST || synctest.copy_mismatched % SYNCTEST_LOG_EVERY == 0)
        {
            ConsoleWrite("SYNCTEST copy_buffer mismatch #%llu subsystems=%s", (unsigned long long)synctest.copy_mismatched, diff.c_str());
        }
    }

    // "player,damage" / "all" / "none" -> mask; false on an unknown name
    inline bool synctest_parse_mask(const std::string& names, uint32_t& mask)
    {
        if (names == "all") { mask = SYNCTEST_ALL; return true; }
        if (names == "none") { mask = 0; return true; }
        mask = 0;
        size_t start = 0;
        while (start <= names.size())
        {
            size_t end = names.find(',', start);
            if (end == std::string::npos) end = names.size();
            std::string name = names.substr(start, end - start);
            int i = 0;
            while (i < SYNCTEST_SUBSYSTEM_COUNT && name != SYNCTEST_SUBSYSTEMS[i]) i++;
            if (i == SYNCTEST_SUBSYSTEM_COUNT) return false;
            mask |= 1u << i;
            start = end + 1;
        }
        return true;
    }

    inline std::string _synctest_counts_json(const uint64_t* counts)
    {
        std::string s = "{";
        for (int i = 0; i < SYNCTEST_SUBSYSTEM_COUNT; i++)
        {
            if (i) s += ",";
            s += "\"" + std::string(SYNCTEST_SUBSYSTEMS[i]) + "\":" + std::to_string(counts[i]);
        }
        return s + "}";
    }

    inline std::string synctest_json()
    {
        const SyncTestStats& t = synctest;
        std::string s = "{\"checked\":" + std::to_string(t.checked);
        s += ",\"mismatched\":" + std::to_string(t.mismatched);
        s += ",\"first_mismatch_frame\":" + std::to_string(t.first_mismatch_frame);
        s += ",\"last_mismatch_frame\":" + std::to_string(t.last_mismatch_frame);
        s += ",\"by_subsystem\":" + _synctest_counts_json(t.by_subsystem);
        s += ",\"copies\":" + std::to_string(t.copies);
        s += ",\"copy_mismatched\":" + std::to_string(t.copy_mismatched);
        s += ",\"copy_by_subsystem\":" + _synctest_counts_json(t.copy_by_subsystem);
        s += ",\"dump_mask\":" + std::to_string(synctest_dump.mask);
        s += ",\"dump_from_frame\":" + std::to_string(synctest_dump.from_frame);
        s += ",\"live_frames\":" + std::to_string(t.live_frames);
        s += ",\"resim_frames\":" + std::to_string(t.resim_frames);
        s += ",\"sp_up_live\":" + std::to_string(t.sp_up_live);
        s += ",\"sp_up_resim\":" + std::to_string(t.sp_up_resim);
        s += ",\"attach_head_changes_live\":" + std::to_string(t.attach_head_changes_live);
        s += ",\"attach_head_changes_resim\":" + std::to_string(t.attach_head_changes_resim);
        s += ",\"dump_files_written\":" + std::to_string(t.dump_files_written) + "}";
        return s;
    }
#endif

    // Called from rollback_save_game_state_callback with the frame GGPO is
    // saving. Returns the digest so the caller can reuse it (e.g. as the synctest
    // checksum) without hashing the whole state twice.
    inline StateDigest record(int frame, RollbackState* s)
    {
        StateDigest d = digest_of(s);
#ifdef GGPO_SYNCTEST
        auto prev = _store().find(frame);
        if (prev != _store().end())
        {
            synctest_compare(frame, prev->second, d);
        }
#endif
        _store()[frame] = d;

        if (frame == dump_request_frame)
        {
            dump_text_store = dump_text_of(s);
            dump_frame = frame;
            dump_confirmed = false;
            char name[128];
            snprintf(name, sizeof(name), "statedump_harness_%d.txt", frame);
            _write_text_file(name, dump_text_store);
            dump_file = name;
            ConsoleWrite("StateHash: captured state dump for frame %d (%u bytes)", frame, (unsigned)dump_text_store.size());
        }
        return d;
    }

    // Emit + erase every frame now confirmed (<= last_confirmed_frame). A
    // confirmed frame can no longer be re-saved, so its last digest is final.
    inline void emit_confirmed(int last_confirmed_frame)
    {
        std::map<int, StateDigest>& m = _store();
        for (auto it = m.begin(); it != m.end(); )
        {
            if (it->first > last_confirmed_frame)
            {
                break;   // std::map is ordered ascending; nothing later is confirmed
            }
            const StateDigest& d = it->second;
            if (hash_logfile == NULL)
            {
                // NB: its own file. The main log is already open deny-write, so
                // opening *that* name here fails and leaves a NULL stream.
                hash_logfile = _fsopen(hash_logfilename, "w", _SH_DENYWR);
            }
            if (hash_logfile != NULL)
            {
                fprintf(hash_logfile, "STATEHASH frame=%d player=%016llx bullet=%016llx damage=%016llx havok=%016llx throw=%016llx dmghit=%016llx comb=%016llx\n",
                    it->first,
                    (unsigned long long)d.player,
                    (unsigned long long)d.bullet,
                    (unsigned long long)d.damage,
                    (unsigned long long)d.havok,
                    (unsigned long long)d.throwman,
                    (unsigned long long)d.dmghit,
                    (unsigned long long)combined(d));
                fflush(hash_logfile);
            }

            std::deque<std::pair<int, StateDigest>>& h = _history();
            h.emplace_back(it->first, d);
            if (h.size() > HISTORY_MAX) h.pop_front();
            last_confirmed_emitted = it->first;

            it = m.erase(it);
        }

        if (dump_frame >= 0 && !dump_confirmed && dump_frame <= last_confirmed_frame)
        {
            dump_confirmed = true;                 // no more re-saves of that frame: the text is final
            if (dump_request_frame == dump_frame) dump_request_frame = -1;
            ConsoleWrite("StateHash: state dump for frame %d is confirmed", dump_frame);
        }
    }

    // Session boundary: GGPO frames restart at 0, so the per-frame stores must not
    // carry stale entries across sessions.
    inline void reset_session()
    {
        _store().clear();
        _history().clear();
        last_confirmed_emitted = -1;
        //dump_request_frame is deliberately NOT cleared. A request armed before the session
        //starts means "capture frame N of the next session" -- which is the only way to dump
        //an early frame at all, since the control plane cannot arm one mid-handshake before
        //frame 0 has already been saved. Everything else here is per-session and must go.
        dump_frame = -1;
        dump_confirmed = false;
        dump_text_store.clear();
        dump_file.clear();
#ifdef GGPO_SYNCTEST
        synctest = SyncTestStats{};
#endif
    }
}

#endif // STATE_HASH_H
