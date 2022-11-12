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
private:
    static void set_default_weapon_faith_requirements();
    static void set_weapon_faith_requirements();
    static const uint32_t weapon_toggle_injection_offset = 0x20a319;
    static const uint32_t control_timer_injection_offset = 0x669303;
    static const uint32_t main_rally_injection_offset = 0x2e191ae;
};

#endif //_DS1_OVERHAUL_BB_RALLY_SYSTEM_H_
