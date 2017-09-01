/*
	DARK SOULS OVERHAUL
	
	Contributors to this file:
		Sean Pesce	-	C++
	
*/

#pragma once

#ifndef _DS1_OVERHAUL_MOD_DATA_H_
	#define _DS1_OVERHAUL_MOD_DATA_H_




//////////////////////// USER PREFERENCE CONSTANTS ////////////////////////
#define _DS1_OVERHAUL_SETTINGS_FILE_ ".\\d3d9_Mod.ini"
#define _DS1_OVERHAUL_PREFS_SECTION_ "Dark Souls Overhaul Settings"
#define _DS1_OVERHAUL_KEYBINDS_SECTION_ "Dark Souls Overhaul Keybinds"
#define _DS1_OVERHAUL_PREF_SHOW_NODE_COUNT_ "DisplayNodeCount"
#define _DS1_OVERHAUL_HOTKEY_BONFIRE_INPUT_FIX_ "BonfireInputFix"

#define _DS1_OVERHAUL_SETTINGS_STRING_BUFF_LEN_ 256		// Length of the string buffer used when obtaining user preference-related string data



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

	// Get user-defined settings preferences from the settings file
	static void get_user_preferences();

	// Get user-defined keybinds from the settings file
	static void get_user_keybinds();

	
};




#endif // _DS1_OVERHAUL_MOD_DATA_H_