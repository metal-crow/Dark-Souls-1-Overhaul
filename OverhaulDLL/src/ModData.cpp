/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Sean Pesce  -  C++

*/

#include "DllMain.h"
#include "AntiCheat.h"
#include "Challenge/GravelordPhantoms.h"
#include "Challenge/BlackPhantomEnemies.h"

#define _SP_DEFINE_VK_NAME_STRINGS_		// Must be defined to use Virtual-key code name strings from SP_IO_Strings.hpp (opt-in by default because it increases filesize by a few KB)

#include "SP_IO.hpp"
#include "SP_IO_Strings.hpp"



/*
    Initialize static variables:
*/


// Used in console messages to inform users that a message is being printed by the Overhaul mod
const std::string Mod::output_prefix = "[Overhaul Mod] ";

// Set to true after inititalize_plugin() has executed
bool Mod::initialized = false;

// Set to true after deferred_tasks() has executed (or can be set to true to cancel unfinished deferred tasks)
bool Mod::deferred_tasks_complete = false;

// Console messages from events that took place before the in-game console was loaded
std::vector<std::string> Mod::startup_messages;

// List of supported game versions
std::vector<uint8_t> Mod::supported_game_versions = { DS1_VERSION_RELEASE };

// Enable/disable mouse input
bool Mod::mouse_input = true;

// Lock camera when console is open
bool Mod::console_lock_camera = true;

// Determines whether to start in legacy mode (only applies fixes, no gameplay changes)
bool Mod::legacy_mode = false;

// Cheats on/off. If cheats are enabled, saving and multiplayer are disabled until the game is restarted
bool Mod::cheats = false;

// Cheats warning message on/off
bool Mod::cheats_warning = true;

// Determines to disable the game's "Low framerate detected" disconnection
bool Mod::disable_low_fps_disconnect = true;

// Determines whether node count is displayed on the overlay text feed info header
bool Mod::show_node_count = true;

// User preference setting; determines whether the brightness of lava visual effects should be lowered
bool Mod::dim_lava_pref = false;

// User preference setting; determines whether armor sound effects will be disabled
bool Mod::disable_armor_sfx_pref = false;

// User preference setting; determines whether multiplayer node graph HUD element is enabled
bool Mod::hud_node_graph_pref = false;

// Custom game archive files to load instead of the vanilla game files
std::wstring Mod::custom_game_archive_path;

// Custom character save file to load instead of the vanilla file
std::wstring Mod::custom_save_file_path;

// Custom game configuration file to load instead of the vanilla file
std::wstring Mod::custom_config_file_path;

// Determines whether gesture cancelling is enabled
bool Mod::gesture_cancelling = true;

// Configurable flags for monitoring game file I/O
bool Mod::Debug::monitor_bdt = false;
bool Mod::Debug::monitor_bhd = false;
bool Mod::Debug::monitor_sl2 = false;



// Get user-defined startup preferences from the settings file
void Mod::get_startup_preferences()
{
    // Begin loading startup preferences
    Mod::startup_messages.push_back(Mod::output_prefix + "Loading startup preferences...");

    // Check if mouse input should be disabled
    Mod::mouse_input = ((int)GetPrivateProfileInt(_DS1_OVERHAUL_INPUT_SECTION_, _DS1_OVERHAUL_PREF_MOUSE_INPUT_, (int)Mod::mouse_input, _DS1_OVERHAUL_SETTINGS_FILE_) != 0);
    if (!Mod::mouse_input)
        Mod::startup_messages.push_back("    Mouse input disabled.");

    // Check if legacy mode is enabled
    Mod::legacy_mode = ((int)GetPrivateProfileInt(_DS1_OVERHAUL_PREFS_SECTION_, _DS1_OVERHAUL_PREF_LEGACY_MODE_, (int)Mod::legacy_mode, _DS1_OVERHAUL_SETTINGS_FILE_) != 0);
    if (Mod::legacy_mode)
        Mod::startup_messages.push_back("    Legacy mode enabled. Gameplay changes will not be applied.");

    // Check for custom game files
    Mod::get_custom_game_files();

    // Memory limit
    Game::memory_limit = (uint32_t)GetPrivateProfileInt(_DS1_OVERHAUL_PREFS_SECTION_, _DS1_OVERHAUL_PREF_MEMORY_LIMIT_, (int)Game::memory_limit, _DS1_OVERHAUL_SETTINGS_FILE_);


    // Anti-cheat
    AntiCheat::BossGuard::active = ((int)GetPrivateProfileInt(_DS1_OVERHAUL_ANTICHEAT_SECTION_, _DS1_OVERHAUL_PREF_AC_BOSS_GUARD_, (int)AntiCheat::BossGuard::active, _DS1_OVERHAUL_SETTINGS_FILE_) != 0);
    AntiCheat::NpcGuard::active = ((int)GetPrivateProfileInt(_DS1_OVERHAUL_ANTICHEAT_SECTION_, _DS1_OVERHAUL_PREF_AC_NPC_GUARD_, (int)AntiCheat::NpcGuard::active, _DS1_OVERHAUL_SETTINGS_FILE_) != 0);
    AntiCheat::TeleBackstabProtect::active = ((int)GetPrivateProfileInt(_DS1_OVERHAUL_ANTICHEAT_SECTION_, _DS1_OVERHAUL_PREF_AC_TELEBACKSTAB_PROT_, (int)AntiCheat::TeleBackstabProtect::active, _DS1_OVERHAUL_SETTINGS_FILE_) != 0);
    AntiCheat::BinocsTriggerBlock::active = ((int)GetPrivateProfileInt(_DS1_OVERHAUL_ANTICHEAT_SECTION_, _DS1_OVERHAUL_PREF_AC_BINOCS_TRIGGER_BLOCK_, (int)AntiCheat::BinocsTriggerBlock::active, _DS1_OVERHAUL_SETTINGS_FILE_) != 0);
    AntiCheat::DragonTriggerBlock::active = ((int)GetPrivateProfileInt(_DS1_OVERHAUL_ANTICHEAT_SECTION_, _DS1_OVERHAUL_PREF_AC_DRAGON_TRIGGER_BLOCK_, (int)AntiCheat::DragonTriggerBlock::active, _DS1_OVERHAUL_SETTINGS_FILE_) != 0);


    // Debug settings
    Mod::Debug::monitor_bdt = ((int)GetPrivateProfileInt(_DS1_OVERHAUL_DEBUG_SECTION_, _DS1_OVERHAUL_PREF_MONITOR_BDT_, (int)Mod::Debug::monitor_bdt, _DS1_OVERHAUL_SETTINGS_FILE_) != 0);
    if (Mod::Debug::monitor_bdt) {
        Files::io_monitors[Files::BDT0].monitor = true;
        Files::io_monitors[Files::BDT1].monitor = true;
        Files::io_monitors[Files::BDT2].monitor = true;
        Files::io_monitors[Files::BDT3].monitor = true;
    }
    Mod::Debug::monitor_bhd = ((int)GetPrivateProfileInt(_DS1_OVERHAUL_DEBUG_SECTION_, _DS1_OVERHAUL_PREF_MONITOR_BHD_, (int)Mod::Debug::monitor_bhd, _DS1_OVERHAUL_SETTINGS_FILE_) != 0);
    if (Mod::Debug::monitor_bhd) {
        Files::io_monitors[Files::BHD0].monitor = true;
        Files::io_monitors[Files::BHD1].monitor = true;
        Files::io_monitors[Files::BHD2].monitor = true;
        Files::io_monitors[Files::BHD3].monitor = true;
    }
    Mod::Debug::monitor_sl2 = ((int)GetPrivateProfileInt(_DS1_OVERHAUL_DEBUG_SECTION_, _DS1_OVERHAUL_PREF_MONITOR_SL2_, (int)Mod::Debug::monitor_sl2, _DS1_OVERHAUL_SETTINGS_FILE_) != 0);
    if (Mod::Debug::monitor_sl2) {
        Files::io_monitors[Files::SL2].monitor = true;
    }

    // @TODO Load additional startup preferences here

}



// Get user-defined settings preferences from the settings file
void Mod::get_user_preferences()
{
    // Buffer for reading string preferences
    char buffer[_DS1_OVERHAUL_SETTINGS_STRING_BUFF_LEN_];
    buffer[0] = '\0';

    // Temporary string for printing console messages
    std::string msg;


    // Begin loading setting preferences
    print_console(Mod::output_prefix + "Loading user preferences...");

    // Check if camera should be locked when console is open
    Mod::console_lock_camera = ((int)GetPrivateProfileInt(_DS1_OVERHAUL_INPUT_SECTION_, _DS1_OVERHAUL_PREF_CONSOLE_LOCK_CAM_, (int)Mod::console_lock_camera, _DS1_OVERHAUL_SETTINGS_FILE_) != 0);

    Mod::cheats_warning = ((int)GetPrivateProfileInt(_DS1_OVERHAUL_PREFS_SECTION_, _DS1_OVERHAUL_PREF_CHEATS_WARNING_, (int)Mod::cheats_warning, _DS1_OVERHAUL_SETTINGS_FILE_) != 0);

    Mod::disable_low_fps_disconnect = ((int)GetPrivateProfileInt(_DS1_OVERHAUL_PREFS_SECTION_, _DS1_OVERHAUL_PREF_DISABLE_LOW_FPS_DISCONNECT_, (int)Mod::disable_low_fps_disconnect, _DS1_OVERHAUL_SETTINGS_FILE_) != 0);

    // Display multiplayer node count in text feed info header
    Mod::show_node_count = ((int)GetPrivateProfileInt(_DS1_OVERHAUL_PREFS_SECTION_, _DS1_OVERHAUL_PREF_SHOW_NODE_COUNT_, (int)Mod::show_node_count, _DS1_OVERHAUL_SETTINGS_FILE_) != 0);
    msg = "    Display multiplayer node count = ";
    if (Mod::show_node_count)
        print_console(msg.append("enabled").c_str());
    else
        print_console(msg.append("disabled").c_str());


    // Check whether to lower the brightness of lava effects
    Mod::dim_lava_pref = ((int)GetPrivateProfileInt(_DS1_OVERHAUL_PREFS_SECTION_, _DS1_OVERHAUL_PREF_DIM_LAVA_, 0, _DS1_OVERHAUL_SETTINGS_FILE_) != 0);
    if (Mod::dim_lava_pref)
        print_console("    Lava visual effects will be dimmed when a character is loaded");


    // Check whether to disable armor sound effects
    Mod::disable_armor_sfx_pref = ((int)GetPrivateProfileInt(_DS1_OVERHAUL_PREFS_SECTION_, _DS1_OVERHAUL_PREF_DISABLE_ARMOR_SFX_, 0, _DS1_OVERHAUL_SETTINGS_FILE_) != 0);
    if (Mod::disable_armor_sfx_pref)
        print_console("    Armor sound effects will be disabled when a character is loaded");


    // Check if additional HUD elements should be displayed
    if ((int)GetPrivateProfileInt(_DS1_OVERHAUL_HUD_SECTION_, _DS1_OVERHAUL_PREF_COMPASS_RADIAL_, 0, _DS1_OVERHAUL_SETTINGS_FILE_) != 0) {
        Hud::set_show_compass_radial(true);
        print_console("    HUD: Radial compass enabled");
    }

    if ((int)GetPrivateProfileInt(_DS1_OVERHAUL_HUD_SECTION_, _DS1_OVERHAUL_PREF_COMPASS_BAR_, 0, _DS1_OVERHAUL_SETTINGS_FILE_) != 0) {
        Hud::set_show_compass_bar(true);
        print_console("    HUD: Bar compass enabled");
    }

    if ((int)GetPrivateProfileInt(_DS1_OVERHAUL_HUD_SECTION_, _DS1_OVERHAUL_PREF_ELEVATION_, 0, _DS1_OVERHAUL_SETTINGS_FILE_) != 0) {
        Hud::set_show_elevation_meter(true);
        print_console("    HUD: Elevation meter enabled");
    }

    if ((int)GetPrivateProfileInt(_DS1_OVERHAUL_HUD_SECTION_, _DS1_OVERHAUL_PREF_NODE_GRAPH_, Hud::get_show_node_graph(), _DS1_OVERHAUL_SETTINGS_FILE_) != 0) {
        Hud::set_show_node_graph(true, false);
        print_console("    HUD: Multiplayer node graph enabled");
    }

    // Check if challenge mods should be enabled
    if ((int)GetPrivateProfileInt(_DS1_OVERHAUL_CHALLENGE_SECTION_, _DS1_OVERHAUL_PREF_CM_GL_PHANTOMS_, Challenge::GravelordPhantoms::active, _DS1_OVERHAUL_SETTINGS_FILE_) != 0) {
        print_console("    Challenge: Auto-spawning Gravelord Phantoms will be enabled");
    }
    if ((int)GetPrivateProfileInt(_DS1_OVERHAUL_CHALLENGE_SECTION_, _DS1_OVERHAUL_PREF_CM_BP_ENEMIES_, Challenge::BlackPhantomEnemies::active, _DS1_OVERHAUL_SETTINGS_FILE_) != 0) {
        print_console("    Challenge: Black Phantom Enemies will be enabled");
    }
    Challenge::BlackPhantomEnemies::DRAW_TYPE = (uint8_t)GetPrivateProfileInt(_DS1_OVERHAUL_CHALLENGE_SECTION_, _DS1_OVERHAUL_PREF_CM_BP_ENEMY_DRAW_TYPE_, Challenge::BlackPhantomEnemies::DRAW_TYPE_DEFAULT, _DS1_OVERHAUL_SETTINGS_FILE_);

    // @TODO Load additional user preferences here



}



// Get user-defined keybinds from the settings file
void Mod::get_user_keybinds()
{
    // Begin loading keybinds
    print_console(Mod::output_prefix + "Loading keybinds...");


    // Toggle mouse input keybind
    get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_TOGGLE_MOUSE_INPUT_, kf_toggle_mouse_input);

    // Toggle camera lock when console is open
    get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_TOGGLE_CONSOLE_LOCK_CAM_, kf_toggle_console_lock_cam);

    // Bonfire input fix keybind
    get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_BONFIRE_INPUT_FIX_, kf_fix_bonfire_input);

    // Toggle multiplayer node count in text feed info header
    get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_TOGGLE_NODE_COUNT_, kf_toggle_node_count);


    // Toggle dimmed lava visual effects
    get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_TOGGLE_DIM_LAVA_, kf_toggle_dim_lava);

    // Toggle armor sound effects
    get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_TOGGLE_ARMOR_SFX_, kf_toggle_armor_sfx);

    // De-spawn existing Gravelord phantoms
    get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_DESPAWN_GL_PHANTOMS_, kf_gravelord_phantoms_despawn);

    // Next/previous save file
    get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_SAVE_FILE_NEXT_, kf_save_file_next);
    get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_SAVE_FILE_PREV_, kf_save_file_prev);

    // Toggle additional HUD elements
    get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_TOGGLE_HUD_COMPASS_RADIAL_, kf_toggle_hud_compass_radial);
    get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_TOGGLE_HUD_COMPASS_BAR_, kf_toggle_hud_compass_bar);
    get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_TOGGLE_HUD_ELEVATION_METER_, kf_toggle_hud_elevation_meter);
    get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_TOGGLE_HUD_NODE_GRAPH_, kf_toggle_hud_node_graph);

    // Toggle anti-cheats (not all are togglable)
    get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_TOGGLE_AC_BINOCS_TRIG_BLOCK_, kf_toggle_ac_binocs_trigger_block);
    get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_TOGGLE_AC_DRAGON_TRIG_BLOCK_, kf_toggle_ac_dragon_trigger_block);

    // Toggle challenge mods
    get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_TOGGLE_CM_BP_ENEMIES_, kf_toggle_cm_bp_enemies);
    get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_TOGGLE_CM_GL_PHANTOMS_, kf_toggle_cm_gravelord_phantoms);

    // @TODO Load additional keybinds here


}



// Helper function for get_user_keybinds() that loads the specified keybind from the config file and binds it to the specified function
void Mod::get_single_user_keybind(const char *keybind_name, int(*function)())
{
    // Variable that holds the Virtual-key code of the keybind when read from settings file
    uint8_t key;

    if ((key = (uint8_t)get_vk_hotkey(_DS1_OVERHAUL_SETTINGS_FILE_, _DS1_OVERHAUL_KEYBINDS_SECTION_, keybind_name)) // Obtain user's preferred key
        && register_hotkey_function(key, function))	// Register the keybind
    {
        // Successfully loaded and registered keybind; now print feedback to console
        std::string output = std::string("    Registered ").append(keybind_name).append(" keybind: ");
        output.append(VK_NAME[key]).append(" (0x");   // Get the key name as a string
        if (key < 0x10)
            output += '0';
        std::stringstream hex_stream;
        hex_stream << std::hex << (int)key; // Convert Virtual-key code to hex string
        output.append(hex_stream.str());
        output += ')';
        print_console(output.c_str());
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
    errno_t return_error = 0;
    if (return_error = string_mb_to_wide(custom_file_name_buff, Mod::custom_game_archive_path)) {
        // Conversion error
        Mod::custom_game_archive_path = L"";
    }
    else if (std::string(custom_file_name_buff).length() > 0)
    {
        Mod::startup_messages.push_back(std::string("    Found custom game archive file definition: \"").append(custom_file_name_buff).append("\""));
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
    return_error = 0;
    if (return_error = string_mb_to_wide(custom_file_name_buff, Mod::custom_save_file_path)) {
        // Conversion error
        Mod::custom_save_file_path = L"";
    }
    else if (std::string(custom_file_name_buff).length() > 0)
    {
        Mod::startup_messages.push_back(std::string("    Found custom game save file definition: \"").append(custom_file_name_buff).append("\""));
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
    return_error = 0;
    if ((return_error = string_mb_to_wide(custom_file_name_buff, Mod::custom_config_file_path))) {
        // Conversion error
        Mod::custom_config_file_path = L"";
    }
    else if (std::string(custom_file_name_buff).length() > 0)
    {
        Mod::startup_messages.push_back(std::string("    Found custom game config file definition: \"").append(custom_file_name_buff).append("\""));
    }
}

