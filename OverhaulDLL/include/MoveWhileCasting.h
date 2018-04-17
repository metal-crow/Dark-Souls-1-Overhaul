/*
DARK SOULS OVERHAUL

Contributors to this file:
Metal-Crow    -    Reverse engineering, C++

*/

#pragma once

#ifndef _DS1_OVERHAUL_MOVE_WHILE_CASTING_H_
#define _DS1_OVERHAUL_MOVE_WHILE_CASTING_H_

#include <cstdint>

class CastingMovement
{
public:
    static void start();
private:
    static const uint32_t walkfb_check_1 = 0xBE165B;
    static const uint32_t walkfb_check_2 = 0xA0F57E;
    static void __stdcall set_check2_var_to_zero_injection();
    static const uint32_t walkfb_check_3 = 0xA0F308;
    static const uint32_t walkfb_check_4 = 0x9966AF;
    static const uint32_t set_animation_category = 0xBDD623;
    static void __stdcall force_animation_to_attack_category_instead_of_spattack_injection();
    static const uint32_t bullet_generation = 0xC06178;
    static void __stdcall bullet_genetation_notify();
};
#endif
