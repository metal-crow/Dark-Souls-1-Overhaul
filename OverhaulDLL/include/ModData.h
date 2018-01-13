/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Sean Pesce  -  C++

*/

#pragma once

#ifndef _DS1_OVERHAUL_MOD_DATA_H_
    #define _DS1_OVERHAUL_MOD_DATA_H_




/////////////////// USER PREFERENCE & KEYBIND CONSTANTS ///////////////////
#define _DS1_OVERHAUL_SETTINGS_FILE_ ".\\d3d9_Mod.ini"
#define _DS1_OVERHAUL_PREFS_SECTION_ "Dark Souls Overhaul Settings"
#define _DS1_OVERHAUL_KEYBINDS_SECTION_ "Dark Souls Overhaul Keybinds"
#define _DS1_OVERHAUL_INPUT_SECTION_ "Game Input"
#define _DS1_OVERHAUL_ANTICHEAT_SECTION_ "Anti-Cheat"
#define _DS1_OVERHAUL_HUD_SECTION_ "HUD"
#define _DS1_OVERHAUL_DEBUG_SECTION_ "Dark Souls Overhaul Debug"
#define _DS1_OVERHAUL_CHALLENGE_SECTION_ "Challenge Mods"
// Startup preferences:
#define _DS1_OVERHAUL_PREF_LEGACY_MODE_ "LegacyMode"
#define _DS1_OVERHAUL_PREF_MEMORY_LIMIT_ "MemoryLimit"
#define _DS1_OVERHAUL_PREF_CUSTOM_GAME_ARCHIVE_ "CustomArchiveFiles"
#define _DS1_OVERHAUL_PREF_CUSTOM_SAVE_FILE_ "CustomSaveFile"
#define _DS1_OVERHAUL_PREF_CUSTOM_GAME_CFG_FILE_ "CustomGameConfigFile"
// Game input settings:
#define _DS1_OVERHAUL_PREF_MOUSE_INPUT_ "MouseInput"
#define _DS1_OVERHAUL_PREF_CONSOLE_LOCK_CAM_ "ConsoleLockCamera"
// Anti-cheat:
#define _DS1_OVERHAUL_PREF_AC_BOSS_GUARD_ "BossGuard"
#define _DS1_OVERHAUL_PREF_AC_NPC_GUARD_ "NpcGuard"
#define _DS1_OVERHAUL_PREF_AC_TELEBACKSTAB_PROT_ "TeleBackstabProtect"
#define _DS1_OVERHAUL_PREF_AC_BINOCS_TRIGGER_BLOCK_ "BinocsTriggerBlock"
#define _DS1_OVERHAUL_PREF_AC_DRAGON_TRIGGER_BLOCK_ "DragonTriggerBlock"
// General settings:
#define _DS1_OVERHAUL_PREF_SHOW_NODE_COUNT_ "DisplayNodeCount"
#define _DS1_OVERHAUL_PREF_DIM_LAVA_ "DimLava"
#define _DS1_OVERHAUL_PREF_DISABLE_ARMOR_SFX_ "DisableArmorSounds"
#define _DS1_OVERHAUL_PREF_DISABLE_LOW_FPS_DISCONNECT_ "DisableLowFpsDisconnect"
#define _DS1_OVERHAUL_PREF_CHEATS_WARNING_ "CheatsWarning"
// HUD settings:
#define _DS1_OVERHAUL_PREF_COMPASS_RADIAL_ "CompassRadial"
#define _DS1_OVERHAUL_PREF_COMPASS_BAR_ "CompassBar"
#define _DS1_OVERHAUL_PREF_ELEVATION_ "ElevationMeter"
#define _DS1_OVERHAUL_PREF_NODE_GRAPH_ "NodeGraph"
// Challenge mods:
#define _DS1_OVERHAUL_PREF_CM_AGGRO_AI_ "AggressiveAI"
#define _DS1_OVERHAUL_PREF_CM_BP_ENEMIES_ "BlackPhantomEnemies"
#define _DS1_OVERHAUL_PREF_CM_BP_ENEMY_DRAW_TYPE_ "BlackPhantomEnemyDrawType"
#define _DS1_OVERHAUL_PREF_CM_GL_PHANTOMS_ "GravelordPhantoms"
// Debug:
#define _DS1_OVERHAUL_PREF_MONITOR_BDT_ "MonitorBdtFiles"
#define _DS1_OVERHAUL_PREF_MONITOR_BHD_ "MonitorBhdFiles"
#define _DS1_OVERHAUL_PREF_MONITOR_SL2_ "MonitorSaveFile"
#define _DS1_OVERHAUL_PREF_IO_OUT_FILTER_ "IoMonitorFilter"
// Keybinds:
#define _DS1_OVERHAUL_HOTKEY_TOGGLE_MOUSE_INPUT_ "ToggleMouseInput"
#define _DS1_OVERHAUL_HOTKEY_TOGGLE_CONSOLE_LOCK_CAM_ "ToggleConsoleLockCamera"
#define _DS1_OVERHAUL_HOTKEY_BONFIRE_INPUT_FIX_ "BonfireInputFix"
#define _DS1_OVERHAUL_HOTKEY_DESPAWN_GL_PHANTOMS_ "DespawnGravelordPhantoms"
#define _DS1_OVERHAUL_HOTKEY_TOGGLE_NODE_COUNT_ "ToggleNodeCount"
#define _DS1_OVERHAUL_HOTKEY_TOGGLE_DIM_LAVA_ "ToggleLavaBrightness"
#define _DS1_OVERHAUL_HOTKEY_TOGGLE_ARMOR_SFX_ "ToggleArmorSounds"
#define _DS1_OVERHAUL_HOTKEY_SAVE_FILE_NEXT_ "NextSaveFile"
#define _DS1_OVERHAUL_HOTKEY_SAVE_FILE_PREV_ "PrevSaveFile"
#define _DS1_OVERHAUL_HOTKEY_TOGGLE_HUD_COMPASS_RADIAL_ "ToggleHudCompassRadial"
#define _DS1_OVERHAUL_HOTKEY_TOGGLE_HUD_COMPASS_BAR_ "ToggleHudCompassBar"
#define _DS1_OVERHAUL_HOTKEY_TOGGLE_HUD_ELEVATION_METER_ "ToggleHudElevationMeter"
#define _DS1_OVERHAUL_HOTKEY_TOGGLE_HUD_NODE_GRAPH_ "ToggleHudNodeGraph"
#define _DS1_OVERHAUL_HOTKEY_TOGGLE_AC_BINOCS_TRIG_BLOCK_ "ToggleBinocsTriggerBlockAC"
#define _DS1_OVERHAUL_HOTKEY_TOGGLE_AC_DRAGON_TRIG_BLOCK_ "ToggleDragonTriggerBlockAC"
#define _DS1_OVERHAUL_HOTKEY_TOGGLE_CM_AGGRO_AI_ "ToggleChallengeAggressiveAI"
#define _DS1_OVERHAUL_HOTKEY_TOGGLE_CM_BP_ENEMIES_ "ToggleChallengeBlackPhantomEnemies"
#define _DS1_OVERHAUL_HOTKEY_TOGGLE_CM_GL_PHANTOMS_ "ToggleChallengeGravelordPhantoms"

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

    // List of people who directly contributed to the Dark Souls Overhaul Mod
    static std::vector<std::string> authors;

    // List of people who indirectly contributed to the mod, but had no direct involvement
    static std::vector<std::string> contributors;

    // Others who made the project possible
    static std::vector<std::string> special_thanks;

    // Used in console messages to inform users that a message is being printed by the Overhaul mod
    static const std::string output_prefix;

    // Set to true after inititalize_plugin() has executed
    static bool initialized;

    // Set to true after deferred_tasks() has executed (or this boolean can be set to true to cancel unfinished deferred tasks)
    static bool deferred_tasks_complete;

    // Console messages from events that took place before the in-game console was loaded
    static std::vector<std::string> startup_messages;

    // List of supported game versions
    static std::vector<uint8_t> supported_game_versions;

    // Enable/disable mouse input
    static bool mouse_input;

    // Lock camera when console is open
    static bool console_lock_camera;

    // Determines whether to start in legacy mode (only applies fixes/anti-cheat/QoL changes, no gameplay modifications)
    static bool legacy_mode;

    // Cheats on/off. If cheats are enabled, saving and multiplayer are disabled until the game is restarted
    static bool cheats;

    // Cheats warning message on/off
    static bool cheats_warning;

    // Determines to disable the game's "Framerate insufficient for online play" disconnection
    static bool disable_low_fps_disconnect;

    // Determines whether node count is displayed on the overlay text feed info header
    static bool show_node_count;

    // User preference setting; determines whether the brightness of lava visual effects should be lowered
    static bool dim_lava_pref;

    // User preference setting; determines whether armor sound effects will be disabled
    static bool disable_armor_sfx_pref;

    // User preference setting; determines whether multiplayer node graph HUD element will be enabled when network is loaded
    static bool hud_node_graph_pref;

    // Custom game archive files to load instead of the vanilla game files
    static std::wstring custom_game_archive_path;

    // Custom character save file to load instead of the vanilla file
    static std::wstring custom_save_file_path;

    // Custom game configuration file to load instead of the vanilla file
    static std::wstring custom_config_file_path;

    // Determines whether gesture cancelling is enabled
    static bool gesture_cancelling;


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

    // Get custom game files from the settings file
    static void get_custom_game_files();

    // Plays the credits for the Dark Souls Overhaul Mod (displays the list of people who contributed to development)
    static void play_credits();



    class Debug {
    public:
        // Configurable flags for monitoring game file I/O
        static bool monitor_bdt;
        static bool monitor_bhd;
        static bool monitor_sl2;
    };

};




#endif // _DS1_OVERHAUL_MOD_DATA_H_