/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        IAmV3nom     -  Original CE script
        Jellybaby34  -  Original CE script
        Sean Pesce   -  C++
        Xiang        -  Original CE script
*/


#pragma once

#ifndef DS1_CHALLENGE_GRAVELORD_PHANTOMS_H_
    #define DS1_CHALLENGE_GRAVELORD_PHANTOMS_H_

namespace Challenge {
namespace GravelordPhantoms {

extern bool     active;
extern void    *injection_address;
extern void    *despawn_write_address;
extern uint32_t asm_return_address;

// Spawns additional Gravelord phantom enemies (even if player hasn't been Gravelorded)
void enable();

// Disables auto-spawning of Gravelord phantoms. Also de-spawns additional Gravelord phantom enemies, but only if player is in NG+0.
//   If player is in NG+1 or higher, they have to wait 10 minutes for the phantoms
//   to disappear on their own. This is to prevent the user from de-spawning the
//   additional enemies in the event that they were legitimately Gravelorded by
//   another player (which can only happen in NG+1 or higher).
void disable();

// De-spawns existing Gravelord Phantoms (only if player is in NG+0)
void despawn_gravelord_phantoms();

// Toggles Gravelord phantoms challenge mod
void toggle();

// Injected assembly function that enables Gravelord phantoms challenge mod
void __stdcall asm_enable_gravelord_phantoms();

} // namespace GravelordPhantoms
} // namespace Challenge

#endif // DS1_CHALLENGE_GRAVELORD_PHANTOMS_H_