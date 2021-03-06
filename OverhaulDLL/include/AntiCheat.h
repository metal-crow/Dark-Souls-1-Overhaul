/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Ainsley Harriott  -  NPC Guard & Boss Guard
        Ashley            -  Anti-Tele-Backstab
        Metal-crow        -  NPC Guard, in-line ASM fixes
        Sean Pesce        -  C++
*/


#pragma once

#ifndef DS1_OVERHAUL_ANTI_CHEAT_MAIN_H_
    #define DS1_OVERHAUL_ANTI_CHEAT_MAIN_H_

#include <cstdint>

namespace AntiCheat {

void start();

static const uint64_t NpcGuard_offset = 0x25E611;
static const uint64_t BossGuard_offset = 0x3206C5;
static const uint64_t TeleBackstab_getBSAnimation_offset = 0x3AD0E3;
static const uint64_t TeleBackstab_setPlayerLocation_offset = 0x2B9A6A;
static const uint64_t NameCrash_prevention_offset = 0x75E101;
static const uint64_t ReadParseType18_packet_offset = 0x6e066b;
static const uint64_t ReadParseType35_packet_offset = 0x0221de33;

} // namespace AntiCheat

#endif // DS1_OVERHAUL_ANTI_CHEAT_MAIN_H_
