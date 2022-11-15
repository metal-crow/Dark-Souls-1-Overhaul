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

static const uint64_t NpcGuard_offset = 0x2e191ae;
static const uint64_t BossGuard_offset = 0x322915;
static const uint64_t TeleBackstab_getBSAnimation_offset = 0x3ac6f3;
static const uint64_t TeleBackstab_setPlayerLocation_offset = 0x2bb56a;
static const uint64_t NameCrash_prevention_offset = 0x761041;
static const uint64_t ReadParseType18_packet_offset = 0x23b4342;
static const uint64_t ReadParseType35_packet_offset = 0x2a5ec57;

} // namespace AntiCheat

#endif // DS1_OVERHAUL_ANTI_CHEAT_MAIN_H_
