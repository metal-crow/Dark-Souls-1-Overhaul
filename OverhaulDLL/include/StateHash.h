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
        d.havok    = hash_FrpgHavokManImp(s->havokman, ordered, n);
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
        t += "=== FrpgHavokManImp ===\n";  t += print_FrpgHavokManImp(s->havokman, ordered, n);
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

    // Called from rollback_save_game_state_callback with the frame GGPO is
    // saving. Returns the digest so the caller can reuse it (e.g. as the synctest
    // checksum) without hashing the whole state twice.
    inline StateDigest record(int frame, RollbackState* s)
    {
        StateDigest d = digest_of(s);
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
        dump_request_frame = -1;
        dump_frame = -1;
        dump_confirmed = false;
        dump_text_store.clear();
        dump_file.clear();
    }
}

#endif // STATE_HASH_H
