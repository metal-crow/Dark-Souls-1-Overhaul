#pragma once
#ifndef PLAYER_HANDLES_H
#define PLAYER_HANDLES_H

#include <cstdint>
#include "StateSerializer.h"

/*
 * A character handle (ChrIns+0x8) is local to each machine for the session's players: the local player is always
 * Game::PC_Handle and the others get the handles after it in connection order, so the same character has a different
 * handle on each machine. Anything that stores a player's handle (a lock-on target, a throw's attacker, a damage
 * entry's owner, ...) has to be translated when it crosses machines, and compared in a machine-independent form.
 *
 * The canonical form is CANONICAL_TAG | the player's rank by steam id among the session's players (the same order the
 * oracle hashes players in). Every other handle (enemies, objects, -1) passes through unchanged.
 */
namespace PlayerHandles
{
    static const uint32_t CANONICAL_TAG = 0xC0DE0000;

    uint32_t to_canonical(uint32_t handle);
    uint32_t from_canonical(uint32_t canonical);
}

// A field holding a character handle: compared (and sent) in canonical form, and written back in this machine's form
inline void serialize_handle(StateVisitor& v, const char* n, const uint32_t& handle)
{
    if (v.mode == StateVisitor::Mode::Apply)
    {
        uint32_t theirs;
        if (v.apply_value(n, &theirs))
        {
            *(uint32_t*)&handle = PlayerHandles::from_canonical(theirs);
        }
    }
    else
    {
        v.field(n, PlayerHandles::to_canonical(handle));
    }
}

#endif
