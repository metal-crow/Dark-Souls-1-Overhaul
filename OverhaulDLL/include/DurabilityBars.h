/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Sean Pesce  -  C++ & Durability bars technique
*/


#pragma once

#ifndef DS1_OVERHAUL_DURABILITY_BARS_H_
    #define DS1_OVERHAUL_DURABILITY_BARS_H_

#include "d3d9.h"
#include <cstdint>

#include "SpPointer.h"


namespace DurabilityBars {

    // DirectX structures for drawing weapon durability bars on HUD
    struct dx_data {
        D3DRECT rects[4];
        D3DRECT* backgrounds  = rects;
        D3DRECT& background_l = rects[0];
        D3DRECT& background_r = rects[1];
        D3DRECT* bars  = &rects[2];
        D3DRECT& bar_l = rects[2];
        D3DRECT& bar_r = rects[3];
        D3DDISPLAYMODE display_mode;
        long offset_left_x  = 0L;
        long offset_right_x = 0L;
        long offset_y = 0L;
        float scale = 1.0f;
        bool display  = false;
    };

    extern DurabilityBars::dx_data render_data;


    extern bool enable_pref;

    // Equipment data in game memory
    extern SpPointer equip_data;

    extern uint32_t l_slot_address;
    extern uint32_t r_slot_address;
    extern uint32_t l_slot_address2;
    extern uint32_t r_slot_address2;

    extern uint32_t l_durability;  // Current durability of primary   weapon (left hand)
    extern uint32_t r_durability;  // Current durability of primary   weapon (right hand)
    extern uint32_t l_durability2; // Current durability of secondary weapon (left hand)
    extern uint32_t r_durability2; // Current durability of secondary weapon (right hand)

    extern uint16_t l_max_durability;  // Max durability of primary   weapon (left hand)
    extern uint16_t r_max_durability;  // Max durability of primary   weapon (right hand)
    extern uint16_t l_max_durability2; // Max durability of secondary weapon (left hand)
    extern uint16_t r_max_durability2; // Max durability of secondary weapon (right hand)

    //void __stdcall durability_intercept();

    void apply(bool silent = false);
    void unpatch();
    bool intercept_active();

    void update_data();

} // namespace DurabilityBars

#endif // DS1_OVERHAUL_DURABILITY_BARS_H_