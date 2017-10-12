/*
	DARK SOULS OVERHAUL
	
	Contributors to this file:
		Sean Pesce	-	C++
	
*/

#pragma once

#ifndef _DS1_OVERHAUL_MOD_DATA_H_
	#define _DS1_OVERHAUL_MOD_DATA_H_




/////////////////// USER PREFERENCE & KEYBIND CONSTANTS ///////////////////
#define _DS1_OVERHAUL_SETTINGS_FILE_ ".\\d3d9_Mod.ini"
#define _DS1_OVERHAUL_PREFS_SECTION_ "Dark Souls Overhaul Settings"
#define _DS1_OVERHAUL_KEYBINDS_SECTION_ "Dark Souls Overhaul Keybinds"
// Startup preferences:
#define _DS1_OVERHAUL_PREF_LEGACY_MODE_ "LegacyMode"
#define _DS1_OVERHAUL_PREF_CUSTOM_GAME_ARCHIVE_ "CustomArchiveFiles"
// Settings:
#define _DS1_OVERHAUL_PREF_SHOW_NODE_COUNT_ "DisplayNodeCount"
#define _DS1_OVERHAUL_PREF_DIM_LAVA_ "DimLava"
#define _DS1_OVERHAUL_PREF_DISABLE_ARMOR_SFX_ "DisableArmorSounds"
// Keybinds:
#define _DS1_OVERHAUL_HOTKEY_BONFIRE_INPUT_FIX_ "BonfireInputFix"
#define _DS1_OVERHAUL_HOTKEY_TOGGLE_NODE_COUNT_ "ToggleNodeCount"
#define _DS1_OVERHAUL_HOTKEY_TOGGLE_DIM_LAVA_ "ToggleLavaBrightness"
#define _DS1_OVERHAUL_HOTKEY_TOGGLE_ARMOR_SFX_ "ToggleArmorSounds"

#define _DS1_OVERHAUL_SETTINGS_STRING_BUFF_LEN_ 256		// Length of the string buffer used when obtaining user preference-related string data



////////////////////////////// MOD CONSTANTS //////////////////////////////
#define _DS1_OVERHAUL_KEYPRESS_DELAY_ 200	// Sleep time (in milliseconds) after a keybind function is called, to avoid the keypress being detected multiple times



//////////////////////////// OVERLAY MESSAGES ////////////////////////////
#define _DS1_MOD_MSG_BONFIRE_INPUT_FIX_SUCCESS_ "SUCCESS: Bonfire input fix applied"
#define _DS1_MOD_MSG_BONFIRE_INPUT_FIX_NO_INIT_FAIL_ "FAILED: Bonfire input fix was not applied (error reading memory)"
#define _DS1_MOD_MSG_BONFIRE_INPUT_FIX_NO_CHAR_FAIL_ "FAILED: Bonfire input fix was not applied (no character loaded)"
#define _DS1_MOD_MSG_BONFIRE_INPUT_FIX_NOT_AT_BONFIRE_FAIL_ "FAILED: Bonfire input fix was not applied (character is not sitting at a bonfire)"
#define _DS1_MOD_MSG_BONFIRE_INPUT_FIX_AUTOMATIC_ "WARNING: Bonfire input glitch detected; applying input fix"



class Mod
{
public:

	// Set to true after inititalize_plugin() has executed
	static bool initialized;

	// Set to true after deferred_tasks() has executed (or this boolean can be set to true to cancel unfinished deferred tasks)
	static bool deferred_tasks_complete;

	// Console messages from events that took place before the in-game console was loaded
	static std::vector<std::string> startup_messages;

	// List of supported game versions
	static std::vector<uint8_t> supported_game_versions;

	// Determines whether to start in legacy mode (only applies fixes, no gameplay changes)
	static bool legacy_mode;

	// Cheats on/off. If cheats are enabled, saving and multiplayer are disabled until the game is restarted
	static bool cheats;

	// Determines whether node count is displayed on the overlay text feed info header
	static bool show_node_count;

	// User preference setting; determines whether the brightness of lava visual effects should be lowered
	static bool dim_lava_pref;

	// User preference setting; determines whether armor sound effects will be disabled
	static bool disable_armor_sfx_pref;

	// Custom game archive files to load instead of the vanilla game files
	static std::wstring custom_game_archives;


	// Registers all commands in ConsoleCommands.cpp for use with the in-game console
	static void register_console_commands();


	// Get user-defined startup preferences from the settings file
	static void get_startup_preferences();

	// Get user-defined settings preferences from the settings file
	static void get_user_preferences();

	// Get all user-defined keybinds from the settings file
	static void get_user_keybinds();

	// Helper function for get_user_keybinds() that loads the specified keybind from the config file and binds it to the specified function
	static void get_single_user_keybind(const char *keybind_name, int(*function)());

	// Get custom game archive file name prefix from the settings file
	static void get_custom_archive_files();
	


	#ifdef _DS1_OVERHAUL_MOD_DBG_
		
		// Registers all commands in ConsoleCommandsDebug.cpp for use with the in-game console
		static void register_console_commands_debug();

	#endif // _DS1_OVERHAUL_MOD_DBG_

};




#endif // _DS1_OVERHAUL_MOD_DATA_H_