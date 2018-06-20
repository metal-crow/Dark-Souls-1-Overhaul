/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Sean Pesce  -  C++


    This file defines functions that will be associated
    with keybinds at runtime.
*/

#pragma once

#ifndef _DS1_OVERHAUL_KEYBIND_FUNCTIONS_H_
    #define _DS1_OVERHAUL_KEYBIND_FUNCTIONS_H_


///////////////////////////////////////////////////////////////////////
//////////////// Keybind function forward declarations ////////////////
///////////////////////////////////////////////////////////////////////
/*
    Keybind function names should be prefixed with "kf".
*/



// Toggles mouse input
int kf_toggle_mouse_input();

// Toggles camera lock when console is open
int kf_toggle_console_lock_cam();

// Fixes input bug that causes players to be stuck at a bonfire (usually after turning human with framerate unlocked)
int kf_fix_bonfire_input();

// Toggles node count element of overlay text feed
int kf_toggle_node_count();

// Toggles dimmed lava visual effects
int kf_toggle_dim_lava();

// Toggles ladder fix
int kf_toggle_ladder_fix();

// Toggles armor sound effects
int kf_toggle_armor_sfx();

// De-spawns existing Gravelord phantoms, but only if current character is in NG+0
int kf_gravelord_phantoms_despawn();

// Next/previous save file
int kf_save_file_next();
int kf_save_file_prev();

// Toggles for additional HUD elements
int kf_toggle_hud_compass_radial();
int kf_toggle_hud_compass_bar();
int kf_toggle_hud_elevation_meter();
int kf_toggle_hud_node_graph();

// Toggles for anti-cheat services
int kf_toggle_ac_binocs_trigger_block();
int kf_toggle_ac_dragon_trigger_block();

// Toggles for challenge mods
int kf_toggle_cm_aggressive_ai();
int kf_toggle_cm_bp_enemies();
int kf_toggle_cm_gravelord_phantoms();


#endif // _DS1_OVERHAUL_KEYBIND_FUNCTIONS_H_
