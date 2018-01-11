/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Metal-Crow    -    Reverse engineering, Bloodborne rally system, C++

*/

#pragma once

#ifndef _DS1_OVERHAUL_BB_RALLY_SYSTEM_H_
    #define _DS1_OVERHAUL_BB_RALLY_SYSTEM_H_


#include <cstdint>


class BloodborneRally
{
public:
    static void start();
    static void on_char_load();
private:
    static void set_weapon_faith_requirements();
    static const uint32_t weapon_toggle_injection_offset = 0xB37F31;
    static void __stdcall weapon_toggle_injection();
    static const uint32_t current_selected_bar_injection_offset = 0x386D04;
    static void __stdcall selected_bar_injection();
    static const uint32_t control_timer_injection_offset = 0x888CED;
    static void __stdcall control_timer_injection();
    static const uint32_t main_rally_injection_offset = 0xA6BFE6;
    static void __stdcall main_rally_injection();
};

#endif //_DS1_OVERHAUL_BB_RALLY_SYSTEM_H_
