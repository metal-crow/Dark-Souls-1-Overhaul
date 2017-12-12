/*
	DARK SOULS OVERHAUL
	
	Contributors to this file:
        Ainsley Harriott  -  NPC Guard & Boss Guard
        Ashley            -  Anti-Tele-Backstab
        Metal-crow        -  NPC Guard
		Sean Pesce        -  C++ conversions
*/


#pragma once

#ifndef _DS1_OVERHAUL_ANTI_CHEAT_H_
	#define _DS1_OVERHAUL_ANTI_CHEAT_H_


#include <cstdint>


class AntiCheat
{
public:

    static void start();

    class BossGuard
    {
    public:
        // Denotes whether BossGuard anti-cheat is active
        static bool active;

        // Offset (from DARKSOULS.exe) where BossGuard anti-cheat code will be injected
        static const uint32_t injection_offset = 0xA688E5;

        static void start();

        // Injected BossGuard assembly function
        static void __stdcall check();
    };




    class NpcGuard
    {
    public:
        // Denotes whether NpcGuard anti-cheat is active
        static bool active;

        // Offset (from DARKSOULS.exe) where NpcGuard anti-cheat code will be injected
        static const uint32_t injection_offset = 0xA6BFF0;

        static void start();

        // Injected NpcGuard assembly function
        static void __stdcall check();
    };




    class TeleBackstabProtect
    {
    public:
        // Denotes whether Teleporting-Backstab-Protection anti-cheat is active
        static bool active;

        // Offset (from DARKSOULS.exe) where Teleporting-Backstab-Protection anti-cheat code will be injected
        static const uint32_t store_anim_injection_offset = 0xA06A49;
        static const uint32_t check_injection_offset      = 0xABDBB9;

        static void start();
        
        // Injected TeleBackstabProtect assembly functions
        static void __stdcall check();
        static void __stdcall store_new_animation_id();
    };

};
typedef AntiCheat::BossGuard BossGuard;
typedef AntiCheat::NpcGuard NpcGuard;
typedef AntiCheat::TeleBackstabProtect TeleBackstabProtect;

#endif // _DS1_OVERHAUL_ANTI_CHEAT_H_