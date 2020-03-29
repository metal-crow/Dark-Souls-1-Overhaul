/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Metal-Crow  -  Reverse engineering, C++

*/
#pragma once

#ifndef DS1_OVERHAUL_SPELL_DESYNC_FIXES_H_
#define DS1_OVERHAUL_SPELL_DESYNC_FIXES_H_

#include <cstdint>

class SpellDesync
{
public:
    static void start();
private:
    static const uint64_t homing_spell_trigger_injection_offset = 0x42816A;
};

#endif
