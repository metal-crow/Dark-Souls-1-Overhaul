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
    static const uint64_t homing_spell_trigger_injection_offset = 0x428c6a;
    static const uint64_t type1_p2pPacket_parsing_injection_offset = 0x5045da;
    static const uint64_t homing_spell_checkIfTriggered_injection_offset = 0x428c21;
};

#endif
