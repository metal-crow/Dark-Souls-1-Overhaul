/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Sean Pesce  -  C++ & Ladder glitch fix technique
*/


#pragma once

#ifndef DS1_OVERHAUL_LADDER_FIX_H_
    #define DS1_OVERHAUL_LADDER_FIX_H_

#include <cstdint>

namespace LadderFix {
    extern bool enable_pref;

    void apply();
    void unpatch();
    bool is_active();

    // Injected ladder glitch fix assembly function
    void __stdcall ladder_fix();
} // namespace LadderFix

#endif // DS1_OVERHAUL_LADDER_FIX_H_