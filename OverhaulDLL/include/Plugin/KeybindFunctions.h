/*
	DARK SOULS OVERHAUL
	
	Contributors to this file:
		Sean Pesce	-	C++


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



// Fixes input bug that causes players to be stuck at a bonfire (usually after turning human with framerate unlocked)
int kf_fix_bonfire_input();


// Toggles node count element of overlay text feed
int kf_toggle_node_count();

// Toggles dimmed lava visual effects
int kf_toggle_dim_lava();

// Toggles armor sound effects
int kf_toggle_armor_sfx();

// Toggles for additional HUD elements
int kf_toggle_hud_compass_radial();
int kf_toggle_hud_compass_bar();
int kf_toggle_hud_elevation_meter();



#endif // _DS1_OVERHAUL_KEYBIND_FUNCTIONS_H_
