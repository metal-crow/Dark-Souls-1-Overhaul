/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Ashley            -  Anti-Tele-Backstab
        Metal-crow        -  in-line ASM fixes
        Sean Pesce        -  C++
*/


#pragma once

#ifndef DS1_ANTICHEAT_TELE_BACKSTAB_PROTECT_H_
    #define DS1_ANTICHEAT_TELE_BACKSTAB_PROTECT_H_

#include <cstdint>

namespace AntiCheat {
namespace TeleBackstabProtect {

// Offset (from DARKSOULS.exe) where Teleporting-Backstab-Protection anti-cheat code will be injected
const uint32_t ASM_STORE_ANIM_INJECTION_OFFSET = 0xA06A49;
const uint32_t      ASM_CHECK_INJECTION_OFFSET = 0xABDBB9;

// Denotes whether Teleporting-Backstab-Protection anti-cheat is active
extern bool      active;
extern uint32_t *anim_id_ptr;
extern float    *math_storage_ptr;
uint32_t  check_return;
uint32_t  store_anim_id_return;
uint32_t  anim_id;
uint32_t  no_move;
float     math_storage;


void start();
void stop();

// Injected assembly functions
void __stdcall asm_check();
void __stdcall asm_store_new_animation_id();

} // namespace TeleBackstabProtect
} // namespace AntiCheat

#endif // DS1_ANTICHEAT_TELE_BACKSTAB_PROTECT_H_