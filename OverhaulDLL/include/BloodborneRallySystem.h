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
    static void set_default_weapon_faith_requirements();
    static void set_weapon_faith_requirements();
    static const uint32_t weapon_toggle_injection_offset = 0x204389;
    static const uint32_t control_timer_injection_offset = 0x65F8C3;
    static const uint32_t main_rally_injection_offset = 0x3629DFD;
};

#endif //_DS1_OVERHAUL_BB_RALLY_SYSTEM_H_
