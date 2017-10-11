/*
	DARK SOULS OVERHAUL
	
	Contributors to this file:
		Sean Pesce	-	C++, DirectX9 overlay


	This file defines functions that will be associated
	with keybinds at runtime.
*/

#include "DllMain.h"



// Fixes input bug that causes players to be stuck at a bonfire (usually after turning human with framerate unlocked)
int kf_fix_bonfire_input()
{
	// Apply fix and print message to text feed
	int return_value = Game::fix_bonfire_input(true);
	Sleep(_DS1_OVERHAUL_KEYPRESS_DELAY_); // Sleep to avoid keypress being detected multiple times
	return return_value;
}


// Toggles node count element of overlay text feed
int kf_toggle_node_count()
{
	Mod::show_node_count = !Mod::show_node_count;
	Sleep(_DS1_OVERHAUL_KEYPRESS_DELAY_);
	return ERROR_SUCCESS;
}


// Toggles dimmed lava visual effects
int kf_toggle_dim_lava()
{
	if (Game::lava_luminosity == NULL)
	{
		if (!Game::characters_loaded)
			print_console("ERROR: Unable to change lava brightness (try loading a character first)");
		else
			print_console("ERROR: Unable to change lava brightness (failed to find lava effects in memory)");
		Sleep(_DS1_OVERHAUL_KEYPRESS_DELAY_);
		return ERROR_FILE_NOT_FOUND;
	}
	Game::enable_dim_lava(!Game::dim_lava_enabled());
	Mod::dim_lava_pref = Game::dim_lava_enabled();
	Sleep(_DS1_OVERHAUL_KEYPRESS_DELAY_);
	return ERROR_SUCCESS;
}


// Toggles armor sound effects
int kf_toggle_armor_sfx()
{
	if (Game::protector_params.base == NULL)
	{
		if (!Game::characters_loaded)
			print_console("ERROR: Unable to toggle armor sounds (try loading a character first)");
		else
			print_console("ERROR: Unable to toggle armor sounds (failed to find armor data in memory)");
		Sleep(_DS1_OVERHAUL_KEYPRESS_DELAY_);
		return ERROR_FILE_NOT_FOUND;
	}
	Game::enable_armor_sfx(!Game::armor_sfx_enabled());
	Mod::disable_armor_sfx_pref = !Game::armor_sfx_enabled();
	Sleep(_DS1_OVERHAUL_KEYPRESS_DELAY_);
	return ERROR_SUCCESS;
}