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
	int return_value = GameData::fix_bonfire_input(true);
	Sleep(_DS1_OVERHAUL_KEYPRESS_DELAY_); // Sleep to avoid keypress being detected multiple times
	return return_value;
}


// Toggles node count element of overlay text feed
int kf_toggle_node_count()
{
	ModData::show_node_count = !ModData::show_node_count;
	Sleep(_DS1_OVERHAUL_KEYPRESS_DELAY_);
	return ERROR_SUCCESS;
}