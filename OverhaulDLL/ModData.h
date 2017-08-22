/*
	DARK SOULS OVERHAUL
	
	Contributors to this file:
		Sean Pesce	-	C++
	
*/

#pragma once

#ifndef _DS1_OVERHAUL_MOD_DATA_H_
	#define _DS1_OVERHAUL_MOD_DATA_H_




//////////////////////// USER PREFERENCE CONSTANTS ////////////////////////
#define _DS1_MOD_SETTINGS_FILE_ "d3d9_Mod.ini"
#define _DS1_MOD_SETTINGS_SECTION_PREFS_ "Preferences"
#define _DS1_MOD_SETTINGS_SECTION_KEYBINDS_ "Keybinds"
#define _DS1_MOD_PREF_SHOW_NODE_COUNT_ "DisplayNodeCount"
#define _DS1_MOD_HOTKEY_BONFIRE_INPUT_FIX_ "BonfireInputFix"



//////////////////////////// OVERLAY MESSAGES ////////////////////////////
#define _DS1_MOD_MSG_BONFIRE_INPUT_FIX_ "Bonfire input fix applied"
#define _DS1_MOD_MSG_CANT_BONFIRE_INPUT_FIX_ "Bonfire input fix was not applied; no character loaded"


class ModData
{
public:

	// Signifies whether the mod has finished executing the inititalize_plugin() function
	static bool initialized;

	// Determines whether node count is displayed on the overlay text feed info header
	static bool show_node_count;


	// Registers all commands in ConsoleCommands.cpp for use with the in-game console
	static void register_console_commands();

	// Get user-defined settings preferences and keybinds from the settings file
	static void get_user_prefs();

	
};




#endif // _DS1_OVERHAUL_MOD_DATA_H_