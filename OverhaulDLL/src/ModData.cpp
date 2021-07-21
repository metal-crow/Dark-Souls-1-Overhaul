/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Sean Pesce  -  C++

*/

#include "DarkSoulsOverhaulMod.h"
#include "SP/preferences/user_setting.h"
#include "SP/io/keybinds.h"
#include "AntiCheat.h"
#include "AnimationEdits.h"
#include "Files.h"
#include "FileReloading.h"
#include "SpellDesyncFixes.h"
#include "ModNetworking.h"
#include "MainLoop.h"

#define _SP_DEFINE_VK_NAME_STRINGS_  // Must be defined to use Virtual-key code name strings from SP_IO_Strings.hpp (opt-in by default because it increases filesize by a few KB)

/*
    Initialize static variables:
*/


// Used in console messages to inform users that a message is being printed by the Overhaul mod
const std::string Mod::output_prefix = "[Overhaul Mod] ";

// Determines if we want to be in legacy mode or not
bool Mod::prefer_legacy_mode = true;

// Determines whether we are in legacy mode (only applies fixes, no gameplay changes)
// This shouls start as true, and only change to false once game is first loaded and we've set up everything needed for overhaul mode
bool Mod::legacy_mode = true;

// Determines to disable the game's "Low framerate detected" disconnection
bool Mod::disable_low_fps_disconnect = false;

// If the user wants to use the default broken UI for the HP bar
bool Mod::fix_hp_bar_size = true;

// If we use steam names or character names for players
bool Mod::use_steam_names = false;

// Enables stopDurabilityDamage function
bool Mod::stop_durability_damage = false;

// Custom game archive files to load instead of the vanilla game files
std::wstring Mod::custom_game_archive_path;

// Custom game configuration file to load instead of the vanilla file
std::wstring Mod::custom_config_file_path;


// Get user-defined startup preferences from the settings file
void Mod::get_init_preferences()
{
    // Buffer for reading string preferences
    char buffer[_DS1_OVERHAUL_SETTINGS_STRING_BUFF_LEN_];
    buffer[0] = '\0';

    // Temporary string for printing console messages
    std::string msg;

    // Begin loading startup preferences
    ConsoleWrite("Loading settings preferences...");

    // Check if legacy mode is enabled
    // Don't update the actual variable, only the preferred. So we only switch on initial character load
    // This helps us keep consistant what is loaded first
    Mod::prefer_legacy_mode = ((int)GetPrivateProfileInt(_DS1_OVERHAUL_PREFS_SECTION_, _DS1_OVERHAUL_PREF_LEGACY_MODE_, (int)Mod::prefer_legacy_mode, _DS1_OVERHAUL_SETTINGS_FILE_) != 0);
    if (Mod::prefer_legacy_mode)
    {
        ConsoleWrite("Legacy mode enabled. Gameplay changes will not be applied.");
        ModNetworking::allow_connect_with_legacy_mod_host = true;
    }
    else
    {
        ConsoleWrite("Overhaul mode enabled. Gameplay changes will be applied.");
        ModNetworking::allow_connect_with_overhaul_mod_host = true;
    }

    // Check for custom game files
    Mod::get_custom_game_files();

    Mod::disable_low_fps_disconnect = ((int)GetPrivateProfileInt(_DS1_OVERHAUL_PREFS_SECTION_, _DS1_OVERHAUL_PREF_DISABLE_LOW_FPS_DISCONNECT_, (int)Mod::disable_low_fps_disconnect, _DS1_OVERHAUL_SETTINGS_FILE_) != 0);

    Mod::use_steam_names = ((int)GetPrivateProfileInt(_DS1_OVERHAUL_PREFS_SECTION_, _DS1_OVERHAUL_PREF_USE_STEAM_NAMES_, (int)Mod::use_steam_names, _DS1_OVERHAUL_SETTINGS_FILE_) != 0);

    Mod::fix_hp_bar_size = ((int)GetPrivateProfileInt(_DS1_OVERHAUL_PREFS_SECTION_, _DS1_OVERHAUL_PREF_FIX_HP_BAR_SIZE_, (int)Mod::fix_hp_bar_size, _DS1_OVERHAUL_SETTINGS_FILE_) != 0);

    Mod::stop_durability_damage = ((int)GetPrivateProfileInt(_DS1_OVERHAUL_PREFS_SECTION_, _DS1_OVERHAUL_PREF_STOP_DURABILITY_DAMAGE_, (int)Mod::stop_durability_damage, _DS1_OVERHAUL_SETTINGS_FILE_) != 0);
}

bool check_hotkeys(void* unused)
{
    sp::io::keybinds::check_hotkeys();
    return true;
}

// Get user-defined keybinds from the settings file
void Mod::get_user_keybinds()
{
    // Begin loading keybinds
    ConsoleWrite("Loading keybinds...");

    // Load keybinds here
    // Toggle mouse input keybind
    //get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_TOGGLE_MOUSE_INPUT_, kf_toggle_mouse_input);

    // Enable the keybinds check
    MainLoop::setup_mainloop_callback(check_hotkeys, NULL, "check_hotkeys");
}

// Helper function for get_user_keybinds() that loads the specified keybind from the config file and binds it to the specified function
void Mod::get_single_user_keybind(const char *keybind_name, int(*function)())
{
    // Variable that holds the Virtual-key code of the keybind when read from settings file
    uint8_t key;

    if ((key = user_setting<uint8_t>(_DS1_OVERHAUL_KEYBINDS_SECTION_, keybind_name, 0).read()) // Obtain user's preferred key
        && sp::io::keybinds::register_hotkey_function(key, function))    // Register the keybind
    {
        // Successfully loaded and registered keybind; now print feedback to console
        std::string output = std::string("    Registered ").append(keybind_name).append(" keybind: ");
        // Get the key name as a string
        CHAR keyname[512];
        UINT scanCode = MapVirtualKeyW(key, MAPVK_VK_TO_VSC);
        LONG lParamValue = (scanCode << 16);
        int result = GetKeyNameText(lParamValue, keyname, 512);
        output.append(keyname).append(" (0x");
        if (key < 0x10)
            output += '0';
        std::stringstream hex_stream;
        hex_stream << std::hex << (int)key; // Convert Virtual-key code to hex string
        output.append(hex_stream.str());
        output += ')';
        ConsoleWrite(output.c_str());
    }
}

// Get custom game files from the settings file
void Mod::get_custom_game_files()
{

    // Custom game archive file name prefix
    char custom_file_name_buff[MAX_PATH + 1];
    custom_file_name_buff[MAX_PATH] = '\0';
    GetPrivateProfileString(_DS1_OVERHAUL_PREFS_SECTION_,
                            _DS1_OVERHAUL_PREF_CUSTOM_GAME_ARCHIVE_,
                            NULL,
                            custom_file_name_buff,
                            MAX_PATH + 1,
                            _DS1_OVERHAUL_SETTINGS_FILE_);

    // Convert string to wide chars
    Files::string_mb_to_wide(custom_file_name_buff, Mod::custom_game_archive_path);
    if (std::string(custom_file_name_buff).length() > 0)
    {
        ConsoleWrite("Found custom game archive file definition: \"%s\"",custom_file_name_buff);
    }


    // Custom game save file
    custom_file_name_buff[MAX_PATH] = '\0';
    GetPrivateProfileString(_DS1_OVERHAUL_PREFS_SECTION_,
                            _DS1_OVERHAUL_PREF_CUSTOM_SAVE_FILE_,
                            NULL,
                            custom_file_name_buff,
                            MAX_PATH + 1,
                            _DS1_OVERHAUL_SETTINGS_FILE_);

    // Convert string to wide chars
    Files::save_file = custom_file_name_buff;
    if (std::string(custom_file_name_buff).length() > 0)
    {
        ConsoleWrite("Found custom game save file definition: \"%s\"",custom_file_name_buff);
    }



    // Custom game config file
    custom_file_name_buff[MAX_PATH] = '\0';
    GetPrivateProfileString(_DS1_OVERHAUL_PREFS_SECTION_,
                            _DS1_OVERHAUL_PREF_CUSTOM_GAME_CFG_FILE_,
                            NULL,
                            custom_file_name_buff,
                            MAX_PATH + 1,
                            _DS1_OVERHAUL_SETTINGS_FILE_);

    // Convert string to wide chars
    Files::string_mb_to_wide(custom_file_name_buff, Mod::custom_config_file_path);
    if (std::string(custom_file_name_buff).length() > 0)
    {
        ConsoleWrite("Found custom game config file definition: \"%s\"",custom_file_name_buff);
    }
}

// Change the legacy mode, and also reload the files that we modify now that we're using the original/new ones
// Also update any specific settings required to be compatable with non-mod users
void Mod::set_mode(bool legacy, bool mod_installed)
{
    if (!mod_installed)
    {
        legacy = true;
    }
    SpellDesync::enabled = mod_installed; //since this requires reciving custom packets to work, not compatable with non-mod

    //only change if we're not already in the mode
    if (Mod::legacy_mode != legacy)
    {
        legacy_mode = legacy;
        Files::UseOverhaulFiles = !legacy;
        FileReloading::SetParamsToUse(legacy);
        FileReloading::ReloadPlayer();
        FileReloading::RefreshPlayerStats();
    }
}

ModMode Mod::get_mode()
{
    if (Mod::legacy_mode && !SpellDesync::enabled)
    {
        return Compatability;
    }
    else if (Mod::legacy_mode && SpellDesync::enabled)
    {
        return Legacy;
    }
    else if (!Mod::legacy_mode)
    {
        return Overhaul;
    }
    return ModeNone;
}

bool Mod::set_preferred_mode(void* unused)
{
    if (Game::playerchar_is_loaded() && FileReloading::GameParamsLoaded)
    {
        // Check if we are not in any multiplayer setting, so that the user's preferred legacy mode setting can be applied
        auto session_action_result = Game::get_SessionManagerImp_session_action_result();
        if (session_action_result.has_value() && session_action_result.value() == NoSession)
        {
            if (Mod::legacy_mode != Mod::prefer_legacy_mode)
            {
                Mod::set_mode(Mod::prefer_legacy_mode, true);
            }
        }
    }

    return true;
}
