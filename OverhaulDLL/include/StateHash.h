#pragma once
#ifndef STATE_HASH_H
#define STATE_HASH_H

#include <cstdint>
#include <cstdio>
#include <map>
#include <string>

#include "Rollback.h"
#include "PlayerInsStructFunctions.h"
#include "BulletManStructFunctions.h"
#include "DamageManStructFunctions.h"
#include "FrpgHavokManImpStructFunctions.h"
#include "ThrowManStructFunctions.h"
#include "DmgHitRecordManImpStructFunctions.h"
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

    inline StateDigest digest_of(RollbackState* s)
    {
        StateDigest d;

        uint64_t pl = 1469598103934665603ULL;
        for (uint32_t i = 0; i < Rollback::ggpoCurrentPlayerCount; i++)
        {
            pl = _fold(pl, hash_PlayerIns(s->playerins[i]));
        }
        d.player   = pl;
        d.bullet   = hash_BulletMan(s->bulletman);
        d.damage   = hash_DamageMan(s->damageman);
        d.havok    = hash_FrpgHavokManImp(s->havokman);
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

    // Called from rollback_save_game_state_callback with the frame GGPO is
    // saving. Returns the digest so the caller can reuse it (e.g. as the synctest
    // checksum) without hashing the whole state twice.
    inline StateDigest record(int frame, RollbackState* s)
    {
        StateDigest d = digest_of(s);
        _store()[frame] = d;
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
            ConsoleWrite("STATEHASH frame=%d player=%016llx bullet=%016llx damage=%016llx havok=%016llx throw=%016llx dmghit=%016llx comb=%016llx",
                it->first,
                (unsigned long long)d.player,
                (unsigned long long)d.bullet,
                (unsigned long long)d.damage,
                (unsigned long long)d.havok,
                (unsigned long long)d.throwman,
                (unsigned long long)d.dmghit,
                (unsigned long long)combined(d));
            it = m.erase(it);
        }
    }

    // Tier-1 localizer: dump the full canonical text of a saved state to
    // statedump_<tag>_<frame>.txt next to the game exe. Drive this from the
    // harness on the offending frame (re-run the deterministic scenario with
    // dumping enabled) and diff the host vs guest file.
    inline void dump_state(int frame, RollbackState* s, const char* tag)
    {
        char name[128];
        snprintf(name, sizeof(name), "statedump_%s_%d.txt", tag, frame);
        FILE* fp = nullptr;
        fopen_s(&fp, name, "w");
        if (!fp)
        {
            return;
        }
        std::string t;
        for (uint32_t i = 0; i < Rollback::ggpoCurrentPlayerCount; i++)
        {
            t += "=== Player ";
            t += std::to_string(i);
            t += " ===\n";
            t += print_PlayerIns(s->playerins[i]);
        }
        t += "=== BulletMan ===\n";        t += print_BulletMan(s->bulletman);
        t += "=== DamageMan ===\n";        t += print_DamageMan(s->damageman);
        t += "=== FrpgHavokManImp ===\n";  t += print_FrpgHavokManImp(s->havokman);
        t += "=== ThrowMan ===\n";         t += print_ThrowMan(s->throwman);
        t += "=== DmgHitRecordManImp ===\n"; t += print_DmgHitRecordManImp(s->dmghitrecordman);
        fwrite(t.data(), 1, t.size(), fp);
        fclose(fp);
    }
}

#endif // STATE_HASH_H
