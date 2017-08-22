/*
	DARK SOULS OVERHAUL
	
	Contributors to this file:
		Sean Pesce	-	C++

*/

#include "DllMain.h"



/*
	Initialize static variables:
*/


// Signifies whether the mod has finished executing the inititalize_plugin() function
bool ModData::initialized = false;

// Determines whether node count is displayed on the overlay text feed info header
bool ModData::show_node_count = true;




// Get user-defined settings preferences and keybinds from the settings file
void ModData::get_user_prefs()
{
	// Load setting preferences
	print_console("[Overhaul Mod] Loading user preferences...");

	ModData::show_node_count = ((int)GetPrivateProfileInt(_DS1_MOD_SETTINGS_SECTION_PREFS_, _DS1_MOD_PREF_SHOW_NODE_COUNT_, 1, _DS1_MOD_SETTINGS_FILE_) != 0);



	// Load keybinds
	print_console("[Overhaul Mod] Loading keybinds...");

	unsigned int key = 0;

	if (key = get_vk_hotkey(_DS1_MOD_SETTINGS_FILE_, _DS1_MOD_SETTINGS_SECTION_KEYBINDS_, _DS1_MOD_HOTKEY_BONFIRE_INPUT_FIX_))
	{
		register_hotkey_function(key, GameData::fix_bonfire_input);
	}
	key = 0;
}


