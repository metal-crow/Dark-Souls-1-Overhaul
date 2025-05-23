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
#include "GameEnum.h"

#define _SP_DEFINE_VK_NAME_STRINGS_  // Must be defined to use Virtual-key code name strings from SP_IO_Strings.hpp (opt-in by default because it increases filesize by a few KB)

/*
    Initialize static variables:
*/


// Used in console messages to inform users that a message is being printed by the Overhaul mod
const std::string Mod::output_prefix = "[Overhaul Mod] ";

// This sets the default mode when the user is hosting/in their own world. I.e the mode the game is biased towards.
ModMode Mod::user_selected_default_mode = ModMode::Legacy;

// The mode we will next enter as soon as the mode_setting_process is capable
ModMode Mod::next_mode = ModMode::InvalidMode;

// This should start as legacy, and only change once game is first loaded and we've set up everything needed for overhaul mode
ModMode Mod::current_mode = ModMode::Legacy;

// If the user wants to use the default broken UI for the HP bar
bool Mod::fix_hp_bar_size = true;

// If we use steam names or character names for players
bool Mod::use_steam_names = false;

// Enables stopDurabilityDamage function
bool Mod::stop_durability_damage = false;

// Make any current non-symmetric options symmetric only
// For example, armor can now only be swapped if 2 players have the mod
bool Mod::fairness_flag = false;

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
    bool default_legacy_mode = true;
    default_legacy_mode = ((int)GetPrivateProfileInt(_DS1_OVERHAUL_PREFS_SECTION_, _DS1_OVERHAUL_PREF_LEGACY_MODE_, (int)default_legacy_mode, _DS1_OVERHAUL_SETTINGS_FILE_) != 0);
    if (default_legacy_mode)
    {
        ConsoleWrite("Legacy mode enabled. Gameplay changes will not be applied.");
        ModNetworking::allow_connect_with_legacy_mod_host = true;
        Mod::user_selected_default_mode = ModMode::Legacy;
    }
    else
    {
        ConsoleWrite("Overhaul mode enabled. Gameplay changes will be applied.");
        ModNetworking::allow_connect_with_overhaul_mod_host = true;
        Mod::user_selected_default_mode = ModMode::Overhaul;
    }

    // Check for custom game files
    Mod::get_custom_game_files();

    Mod::use_steam_names = ((int)GetPrivateProfileInt(_DS1_OVERHAUL_PREFS_SECTION_, _DS1_OVERHAUL_PREF_USE_STEAM_NAMES_, (int)Mod::use_steam_names, _DS1_OVERHAUL_SETTINGS_FILE_) != 0);

    Mod::fix_hp_bar_size = ((int)GetPrivateProfileInt(_DS1_OVERHAUL_PREFS_SECTION_, _DS1_OVERHAUL_PREF_FIX_HP_BAR_SIZE_, (int)Mod::fix_hp_bar_size, _DS1_OVERHAUL_SETTINGS_FILE_) != 0);

    Mod::stop_durability_damage = ((int)GetPrivateProfileInt(_DS1_OVERHAUL_PREFS_SECTION_, _DS1_OVERHAUL_PREF_STOP_DURABILITY_DAMAGE_, (int)Mod::stop_durability_damage, _DS1_OVERHAUL_SETTINGS_FILE_) != 0);

    Mod::fairness_flag = ((int)GetPrivateProfileInt(_DS1_OVERHAUL_PREFS_SECTION_, _DS1_OVERHAUL_PREF_ENABLE_FAIRNESS_, (int)Mod::fairness_flag, _DS1_OVERHAUL_SETTINGS_FILE_) != 0);

    ModNetworking::allow_connect_with_non_mod_host = ((int)GetPrivateProfileInt(_DS1_OVERHAUL_PREFS_SECTION_, _DS1_OVERHAUL_PREF_CONNECT_NONMODHOST_, (int)ModNetworking::allow_connect_with_non_mod_host, _DS1_OVERHAUL_SETTINGS_FILE_) != 0);
    ModNetworking::allow_connect_with_legacy_mod_host = ((int)GetPrivateProfileInt(_DS1_OVERHAUL_PREFS_SECTION_, _DS1_OVERHAUL_PREF_CONNECT_LEGACYHOST_, (int)ModNetworking::allow_connect_with_legacy_mod_host, _DS1_OVERHAUL_SETTINGS_FILE_) != 0);
    ModNetworking::allow_connect_with_overhaul_mod_host = ((int)GetPrivateProfileInt(_DS1_OVERHAUL_PREFS_SECTION_, _DS1_OVERHAUL_PREF_CONNECT_OVERHAULHOST_, (int)ModNetworking::allow_connect_with_overhaul_mod_host, _DS1_OVERHAUL_SETTINGS_FILE_) != 0);
    ModNetworking::allow_connect_with_non_mod_guest = ((int)GetPrivateProfileInt(_DS1_OVERHAUL_PREFS_SECTION_, _DS1_OVERHAUL_PREF_CONNECT_NONMODGUEST_, (int)ModNetworking::allow_connect_with_non_mod_guest, _DS1_OVERHAUL_SETTINGS_FILE_) != 0);
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

// Immediatly change the mode, and also reload the files that we modify now that we're using the original/new ones
// This shouldn't be called directly, but only through mode_setting_process so it can be called at the approprite time to not crash the game
void Mod::change_mode(ModMode mode)
{
    ConsoleWrite("Setting mode: %s", ModModes_To_String.at(mode));

    //only change if we're not already in the mode
    if (Mod::current_mode != mode && mode != ModMode::InvalidMode)
    {
        Files::UseOverhaulFiles = (mode == ModMode::Overhaul);
        FileReloading::SetParamsToUse(mode != ModMode::Overhaul);
        FileReloading::ReloadPlayer(Mod::current_mode, mode);
        FileReloading::RefreshPlayerStats();
        ModNetworking::UpdatePersistentLobbyData(mode);
        Mod::current_mode = mode;
    }

    Mod::next_mode = ModMode::InvalidMode; //unset this so we know we don't have a next_mode still to go
}

// A infinite callback function that is responsible for setting the mode based on parameters
// Continually monitors the desired/needed mode, and sets it as soon as possible when the game can accept the change
bool Mod::mode_setting_process(void* unused)
{
    if (Game::playerchar_is_loaded() && FileReloading::GameParamsLoaded)
    {
        auto playerIns_o = Game::get_PlayerIns();
        if (playerIns_o.has_value() && playerIns_o.value() != NULL)
        {
            uint64_t playerIns = (uint64_t)playerIns_o.value();

            // Check if we are not in any multiplayer setting, so that the user's preferred legacy mode setting can be applied
            // Also make sure we're in our home world, since the MP setting can be 0 even in another world. It's set to that right after a BC.
            auto session_action_result = Game::get_SessionManagerImp_session_action_result();
            if (session_action_result.has_value() && session_action_result.value() == NoSession &&
                (Game::get_player_chr_type(playerIns) == PLAYER_STATUS::HUMAN || Game::get_player_chr_type(playerIns) == PLAYER_STATUS::HOLLOW))
            {
                //need to check both current_mode and next_mode in case we have a queued mode change that shouldn't happen anymore
                if (Mod::get_mode() != Mod::user_selected_default_mode)
                {
                    Mod::change_mode(Mod::user_selected_default_mode);
                }
            }
            // If we are in a multiplayer session, then we may need to change the mode to something else if forced
            // Wait until we've loaded into the other world before we change modes (and reload anything, otherwise we may crash)
            else
            {
                if (Mod::next_mode != ModMode::InvalidMode && Mod::current_mode != Mod::next_mode &&
                    (Game::get_player_chr_type(playerIns) == PLAYER_STATUS::HUMAN || Game::get_player_chr_type(playerIns) == PLAYER_STATUS::COOP || Game::get_player_chr_type(playerIns) == PLAYER_STATUS::INVADER))
                {
                    Mod::change_mode(Mod::next_mode);
                }
            }
        }
    }

    return true;
}

DWORD WINAPI mod_mode_visuals_setting_thread(LPVOID unused)
{
    while (true)
    {
        if (Game::playerchar_is_loaded())
        {
            auto playerIns_o = Game::get_PlayerIns();
            if (playerIns_o.has_value() && playerIns_o.value() != NULL)
            {
                uint64_t playerIns = (uint64_t)playerIns_o.value();

                // Handle setting the speffect to show the current mode
                //add the speffect for the current mode, if applicable and not present
                uint32_t currentModeSpeffectId = ModModes_To_Speffect.at(Mod::get_mode());
                if (currentModeSpeffectId != -1)
                {
                    auto playerCurrentModeSpeffect_o = Game::player_get_speffect(playerIns, currentModeSpeffectId);
                    if (!playerCurrentModeSpeffect_o.has_value())
                    {
                        Game::player_add_speffect(currentModeSpeffectId);
                    }
                }
                //delete the speffects for the other modes
                for (auto const& modespeffect : ModModes_To_Speffect)
                {
                    if (modespeffect.first != Mod::get_mode() && modespeffect.second != -1)
                    {
                        auto otherModeSpeffect = Game::player_get_speffect(playerIns, modespeffect.second);
                        if (otherModeSpeffect.has_value())
                        {
                            //update life to 0
                            *(float*)(otherModeSpeffect.value() + 0) = 0.0f;
                        }
                    }
                }
            }
        }

        Sleep(400);
    }
}

ModMode Mod::get_mode()
{
    //we'll be in this mode shortly, just may have to wait for a loading screen
    if (Mod::next_mode != ModMode::InvalidMode)
    {
        return Mod::next_mode;
    }
    return Mod::current_mode;
}

void Mod::set_mode(ModMode mode)
{
    Mod::next_mode = mode;
}
