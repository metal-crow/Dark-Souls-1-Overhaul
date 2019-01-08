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

namespace TeleBackstabProtect {
    extern bool active;
    void start();

    // Injected TeleBackstabProtect assembly functions
    void __stdcall asm_check();
    void __stdcall asm_store_new_animation_id();
} // namespace TeleBackstabProtect


namespace BinocsTriggerBlock {
    extern bool active;
    void enable();
    void disable();
    void toggle();
} // namespace BinocsTriggerBlock


namespace DragonTriggerBlock {
    extern bool active;
    void enable();
    void disable();
    void toggle();
} // namespace DragonTriggerBlock

} // namespace AntiCheat

#endif // DS1_OVERHAUL_ANTI_CHEAT_MAIN_H_
