/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Sean Pesce  -  C++ & Ladder glitch fix technique
*/


#pragma once

#ifndef DS1_OVERHAUL_LADDER_FIX_H_
    #define DS1_OVERHAUL_LADDER_FIX_H_

#include <cstdint>

#define DS1_DEFAULT_LADDER_FIX_EXIT_RUNG_ 3

namespace LadderFix {
    extern bool enable_pref;

    void apply();
    void unpatch();
    bool is_active();

    // Returns the index of the ladder rung under which the player will exit the ladder when sliding
    uint32_t exit_rung();

    // Sets the index of the ladder rung under which the player will exit the ladder when sliding
    void set_exit_rung(uint32_t index);

    // Injected ladder glitch fix assembly function
    void __stdcall ladder_fix();
} // namespace LadderFix

#endif // DS1_OVERHAUL_LADDER_FIX_H_