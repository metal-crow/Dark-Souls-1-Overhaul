/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Sean Pesce  -  C++

*/

#pragma once

#ifndef _DS1_OVERHAUL_MOD_DATA_H_
    #define _DS1_OVERHAUL_MOD_DATA_H_

/////////////////// USER PREFERENCE & KEYBIND CONSTANTS ///////////////////
#define _DS1_OVERHAUL_SETTINGS_FILE_ ".\\d3d11_mod.ini"
#define _DS1_OVERHAUL_PREFS_SECTION_ "Dark Souls Overhaul Settings"
#define _DS1_OVERHAUL_KEYBINDS_SECTION_ "Dark Souls Overhaul Keybinds"
// Startup preferences:
#define _DS1_OVERHAUL_PREF_LEGACY_MODE_ "LegacyMode"
#define _DS1_OVERHAUL_PREF_CUSTOM_GAME_ARCHIVE_ "CustomArchiveFiles"
#define _DS1_OVERHAUL_PREF_CUSTOM_SAVE_FILE_ "CustomSaveFile"
#define _DS1_OVERHAUL_PREF_CUSTOM_GAME_CFG_FILE_ "CustomGameConfigFile"
// General settings:
#define _DS1_OVERHAUL_PREF_DISABLE_LOW_FPS_DISCONNECT_ "DisableLowFpsDisconnect"
#define _DS1_OVERHAUL_PREF_USE_STEAM_NAMES_ "UseSteamNames"
#define _DS1_OVERHAUL_PREF_FIX_HP_BAR_SIZE_ "FixHpBarSize"
#define _DS1_OVERHAUL_PREF_STOP_DURABILITY_DAMAGE_ "StopDurabilityDamage"

#define _DS1_OVERHAUL_SETTINGS_STRING_BUFF_LEN_ 256  // Length of the string buffer used when obtaining user preference-related string data

////////////////////////////// MOD CONSTANTS //////////////////////////////
#define _DS1_OVERHAUL_KEYPRESS_DELAY_ 200  // Sleep time (in milliseconds) after a keybind function is called, to avoid the keypress being detected multiple times


#include <vector>

enum ModMode
{
    ModeNone,
    Overhaul,
    Legacy,
    Compatability
};

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

    // Determines if we want to be in legacy mode or not
    static bool prefer_legacy_mode;

    // Determines whether we are in legacy mode (only applies fixes, no gameplay changes)
    static bool legacy_mode;

    // Determines to disable the game's "Framerate insufficient for online play" disconnection
    static bool disable_low_fps_disconnect;

    // If the user wants to use the default broken UI for the HP bar
    static bool fix_hp_bar_size;

    // If we use steam names or character names for players
    static bool use_steam_names;

    // Enables stopDurabilityDamage function
    static bool stop_durability_damage;

    // Custom game archive files to load instead of the vanilla game files
    static std::wstring custom_game_archive_path;

    // Custom game configuration file to load instead of the vanilla file
    static std::wstring custom_config_file_path;

    // Get user-defined startup preferences from the settings file
    static void get_init_preferences();

    // Get all user-defined keybinds from the settings file
    static void get_user_keybinds();

    // Plays the credits for the Dark Souls Overhaul Mod (displays the list of people who contributed to development)
    static void play_credits();

    static void set_mode(bool, bool);

    static ModMode get_mode();

	static bool set_preferred_mode(void * unused);

private:
    // Helper function for get_user_keybinds() that loads the specified keybind from the config file and binds it to the specified function
    static void get_single_user_keybind(const char *keybind_name, int(*function)());

    // Get custom game files from the settings file
    static void get_custom_game_files();
};




#endif // _DS1_OVERHAUL_MOD_DATA_H_
