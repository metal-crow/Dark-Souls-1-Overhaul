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

namespace BossGuard {
    extern bool active;
    void start();

    // Injected BossGuard assembly function
    void __stdcall asm_check();
} // namespace BossGuard


namespace NpcGuard {
    extern bool active;
    void start();

    // Injected NpcGuard assembly function
    void __stdcall asm_check();
} // namespace NpcGuard


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