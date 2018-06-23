/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Sean Pesce  -  C++

*/

#include "dllmain.h"
#include "SP/preferences/user_setting.h"
#include "SP/io/keybinds.h"
#include "AntiCheat.h"
//#include "Challenge/AggressiveAI.h"
//#include "Challenge/BlackPhantomEnemies.h"
//#include "Challenge/GravelordPhantoms.h"
#include "AnimationEdits.h"

#define _SP_DEFINE_VK_NAME_STRINGS_  // Must be defined to use Virtual-key code name strings from SP_IO_Strings.hpp (opt-in by default because it increases filesize by a few KB)

#ifdef DS1_OVERHAUL_QOL_PREVIEW
    #define DS1_OVERHAUL_LEGACY_MODE_DEFAULT_VAL true
#else
    #define DS1_OVERHAUL_LEGACY_MODE_DEFAULT_VAL false
#endif // DS1_OVERHAUL_QOL_PREVIEW


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
std::vector<uint8_t> Mod::supported_game_versions = {   DS1_VERSION_RELEASE,
                                                        DS1_VERSION_OVERHAUL,
                                                        DS1_VERSION_OVERHAUL_CHEATS,
                                                        DS1_VERSION_REKINDLED,
                                                        DS1_VERSION_REKINDLED_CHEATS };

// Enable/disable mouse input
bool Mod::mouse_input = true;

// Lock camera when console is open
bool Mod::console_lock_camera = true;

// Determines whether to start in legacy mode (only applies fixes, no gameplay changes)
bool Mod::legacy_mode = DS1_OVERHAUL_LEGACY_MODE_DEFAULT_VAL;

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

#ifndef DS1_OVERHAUL_QOL_PREVIEW
    // Check if legacy mode is enabled
    Mod::legacy_mode = ((int)GetPrivateProfileInt(_DS1_OVERHAUL_PREFS_SECTION_, _DS1_OVERHAUL_PREF_LEGACY_MODE_, (int)Mod::legacy_mode, _DS1_OVERHAUL_SETTINGS_FILE_) != 0);
    if (Mod::legacy_mode)
        Mod::startup_messages.push_back("    Legacy mode enabled. Gameplay changes will not be applied.");
#endif // DS1_OVERHAUL_QOL_PREVIEW

    // Check for custom game files
    Mod::get_custom_game_files();

    // Anti-cheat
    //AntiCheat::BossGuard::active = ((int)GetPrivateProfileInt(_DS1_OVERHAUL_ANTICHEAT_SECTION_, _DS1_OVERHAUL_PREF_AC_BOSS_GUARD_, (int)AntiCheat::BossGuard::active, _DS1_OVERHAUL_SETTINGS_FILE_) != 0);
    //AntiCheat::NpcGuard::active = ((int)GetPrivateProfileInt(_DS1_OVERHAUL_ANTICHEAT_SECTION_, _DS1_OVERHAUL_PREF_AC_NPC_GUARD_, (int)AntiCheat::NpcGuard::active, _DS1_OVERHAUL_SETTINGS_FILE_) != 0);
    //AntiCheat::TeleBackstabProtect::active = ((int)GetPrivateProfileInt(_DS1_OVERHAUL_ANTICHEAT_SECTION_, _DS1_OVERHAUL_PREF_AC_TELEBACKSTAB_PROT_, (int)AntiCheat::TeleBackstabProtect::active, _DS1_OVERHAUL_SETTINGS_FILE_) != 0);
    //AntiCheat::BinocsTriggerBlock::active = ((int)GetPrivateProfileInt(_DS1_OVERHAUL_ANTICHEAT_SECTION_, _DS1_OVERHAUL_PREF_AC_BINOCS_TRIGGER_BLOCK_, (int)AntiCheat::BinocsTriggerBlock::active, _DS1_OVERHAUL_SETTINGS_FILE_) != 0);
    //AntiCheat::DragonTriggerBlock::active = ((int)GetPrivateProfileInt(_DS1_OVERHAUL_ANTICHEAT_SECTION_, _DS1_OVERHAUL_PREF_AC_DRAGON_TRIGGER_BLOCK_, (int)AntiCheat::DragonTriggerBlock::active, _DS1_OVERHAUL_SETTINGS_FILE_) != 0);


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
    global::cmd_out << (Mod::output_prefix + "Loading user preferences...");

    // Check if camera should be locked when console is open
    Mod::console_lock_camera = ((int)GetPrivateProfileInt(_DS1_OVERHAUL_INPUT_SECTION_, _DS1_OVERHAUL_PREF_CONSOLE_LOCK_CAM_, (int)Mod::console_lock_camera, _DS1_OVERHAUL_SETTINGS_FILE_) != 0);

    Mod::cheats_warning = ((int)GetPrivateProfileInt(_DS1_OVERHAUL_PREFS_SECTION_, _DS1_OVERHAUL_PREF_CHEATS_WARNING_, (int)Mod::cheats_warning, _DS1_OVERHAUL_SETTINGS_FILE_) != 0);

    Mod::disable_low_fps_disconnect = ((int)GetPrivateProfileInt(_DS1_OVERHAUL_PREFS_SECTION_, _DS1_OVERHAUL_PREF_DISABLE_LOW_FPS_DISCONNECT_, (int)Mod::disable_low_fps_disconnect, _DS1_OVERHAUL_SETTINGS_FILE_) != 0);

    //AnimationEdits::gesture_cancelling = ((int)GetPrivateProfileInt(_DS1_OVERHAUL_PREFS_SECTION_, _DS1_OVERHAUL_PREF_DISABLE_GESTURE_CANCELLING_, (int)AnimationEdits::gesture_cancelling, _DS1_OVERHAUL_SETTINGS_FILE_) != 0);
    //if (AnimationEdits::gesture_cancelling) {
    //    global::cmd_out << (msg.append("    Gesture cancelling enabled"));
    //}

    // Display multiplayer node count in text feed info header
    Mod::show_node_count = ((int)GetPrivateProfileInt(_DS1_OVERHAUL_PREFS_SECTION_, _DS1_OVERHAUL_PREF_SHOW_NODE_COUNT_, (int)Mod::show_node_count, _DS1_OVERHAUL_SETTINGS_FILE_) != 0);
    msg = "    Display multiplayer node count = ";
    if (Mod::show_node_count)
        global::cmd_out << (msg.append("enabled"));
    else
        global::cmd_out << (msg.append("disabled"));


    // Check whether to lower the brightness of lava effects
    Mod::dim_lava_pref = ((int)GetPrivateProfileInt(_DS1_OVERHAUL_PREFS_SECTION_, _DS1_OVERHAUL_PREF_DIM_LAVA_, 0, _DS1_OVERHAUL_SETTINGS_FILE_) != 0);
    if (Mod::dim_lava_pref)
        global::cmd_out << ("    Lava visual effects will be dimmed");


    // Check whether to disable armor sound effects
#ifndef DS1_OVERHAUL_QOL_PREVIEW
    /*Mod::disable_armor_sfx_pref = ((int)GetPrivateProfileInt(_DS1_OVERHAUL_PREFS_SECTION_, _DS1_OVERHAUL_PREF_DISABLE_ARMOR_SFX_, 0, _DS1_OVERHAUL_SETTINGS_FILE_) != 0);
    if (Mod::disable_armor_sfx_pref)
        global::cmd_out << ("    Armor sound effects will be disabled");
    */
#endif // DS1_OVERHAUL_QOL_PREVIEW


    // Check if additional HUD elements should be displayed
    /*if ((int)GetPrivateProfileInt(_DS1_OVERHAUL_HUD_SECTION_, _DS1_OVERHAUL_PREF_COMPASS_RADIAL_, 0, _DS1_OVERHAUL_SETTINGS_FILE_) != 0) {
        Hud::set_show_compass_radial(true);
        global::cmd_out << ("    HUD: Radial compass enabled");
    }*/

    /*if ((int)GetPrivateProfileInt(_DS1_OVERHAUL_HUD_SECTION_, _DS1_OVERHAUL_PREF_COMPASS_BAR_, 0, _DS1_OVERHAUL_SETTINGS_FILE_) != 0) {
        Hud::set_show_compass_bar(true);
        global::cmd_out << ("    HUD: Bar compass enabled");
    }*/

    /*if ((int)GetPrivateProfileInt(_DS1_OVERHAUL_HUD_SECTION_, _DS1_OVERHAUL_PREF_ELEVATION_, 0, _DS1_OVERHAUL_SETTINGS_FILE_) != 0) {
        Hud::set_show_elevation_meter(true);
        global::cmd_out << ("    HUD: Elevation meter enabled");
    }*/

    /*if ((int)GetPrivateProfileInt(_DS1_OVERHAUL_HUD_SECTION_, _DS1_OVERHAUL_PREF_NODE_GRAPH_, Hud::get_show_node_graph(), _DS1_OVERHAUL_SETTINGS_FILE_) != 0) {
        Hud::set_show_node_graph(true, false);
        global::cmd_out << ("    HUD: Multiplayer node graph enabled");
    }*/

    // Check if challenge mods should be enabled
#if 0
    if ((int)GetPrivateProfileInt(_DS1_OVERHAUL_CHALLENGE_SECTION_, _DS1_OVERHAUL_PREF_CM_AGGRO_AI_, Challenge::AggressiveAi::active(), _DS1_OVERHAUL_SETTINGS_FILE_) != 0) {
        global::cmd_out << ("    Challenge: Aggressive AI will be enabled");
    }
    if ((int)GetPrivateProfileInt(_DS1_OVERHAUL_CHALLENGE_SECTION_, _DS1_OVERHAUL_PREF_CM_GL_PHANTOMS_, Challenge::GravelordPhantoms::active, _DS1_OVERHAUL_SETTINGS_FILE_) != 0) {
        global::cmd_out << ("    Challenge: Auto-spawning Gravelord Phantoms will be enabled");
    }
    if ((int)GetPrivateProfileInt(_DS1_OVERHAUL_CHALLENGE_SECTION_, _DS1_OVERHAUL_PREF_CM_BP_ENEMIES_, Challenge::BlackPhantomEnemies::active, _DS1_OVERHAUL_SETTINGS_FILE_) != 0) {
        global::cmd_out << ("    Challenge: Black Phantom Enemies will be enabled");
    }
    Challenge::AggressiveAi::ear_distance  = (uint16_t)GetPrivateProfileInt(_DS1_OVERHAUL_CHALLENGE_SETTINGS_SECTION_, _DS1_OVERHAUL_PREF_CM_AGGRO_AI_EAR_DIST_,  Challenge::AggressiveAi::DEFAULT_EAR_DISTANCE,  _DS1_OVERHAUL_SETTINGS_FILE_);
    Challenge::AggressiveAi::nose_distance = (uint16_t)GetPrivateProfileInt(_DS1_OVERHAUL_CHALLENGE_SETTINGS_SECTION_, _DS1_OVERHAUL_PREF_CM_AGGRO_AI_NOSE_DIST_, Challenge::AggressiveAi::DEFAULT_NOSE_DISTANCE, _DS1_OVERHAUL_SETTINGS_FILE_);
    Challenge::BlackPhantomEnemies::DRAW_TYPE = (uint8_t)GetPrivateProfileInt(_DS1_OVERHAUL_CHALLENGE_SETTINGS_SECTION_, _DS1_OVERHAUL_PREF_CM_BP_ENEMY_DRAW_TYPE_, Challenge::BlackPhantomEnemies::DRAW_TYPE_DEFAULT, _DS1_OVERHAUL_SETTINGS_FILE_);
#endif
    // @TODO Load additional user preferences here



}



// Get user-defined keybinds from the settings file
void Mod::get_user_keybinds()
{
    // Begin loading keybinds
    global::cmd_out << (Mod::output_prefix + "Loading keybinds...");


    // Toggle mouse input keybind
    //get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_TOGGLE_MOUSE_INPUT_, kf_toggle_mouse_input);

    // Toggle camera lock when console is open
    //get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_TOGGLE_CONSOLE_LOCK_CAM_, kf_toggle_console_lock_cam);

    // Bonfire input fix keybind
    //get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_BONFIRE_INPUT_FIX_, kf_fix_bonfire_input);

    // Toggle multiplayer node count in text feed info header
    //get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_TOGGLE_NODE_COUNT_, kf_toggle_node_count);


    // Toggle dimmed lava visual effects
    //get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_TOGGLE_DIM_LAVA_, kf_toggle_dim_lava);

#ifndef DS1_OVERHAUL_QOL_PREVIEW
    // Toggle armor sound effects
    //get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_TOGGLE_ARMOR_SFX_, kf_toggle_armor_sfx);
#endif // DS1_OVERHAUL_QOL_PREVIEW

    // De-spawn existing Gravelord phantoms
    //get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_DESPAWN_GL_PHANTOMS_, kf_gravelord_phantoms_despawn);

    // Next/previous save file
    //get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_SAVE_FILE_NEXT_, kf_save_file_next);
    //get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_SAVE_FILE_PREV_, kf_save_file_prev);

    // Toggle additional HUD elements
    //get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_TOGGLE_HUD_COMPASS_RADIAL_, kf_toggle_hud_compass_radial);
    //get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_TOGGLE_HUD_COMPASS_BAR_, kf_toggle_hud_compass_bar);
    //get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_TOGGLE_HUD_ELEVATION_METER_, kf_toggle_hud_elevation_meter);
    //get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_TOGGLE_HUD_NODE_GRAPH_, kf_toggle_hud_node_graph);
    
    // Toggle anti-cheats (not all are togglable)
    //get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_TOGGLE_AC_BINOCS_TRIG_BLOCK_, kf_toggle_ac_binocs_trigger_block);
    //get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_TOGGLE_AC_DRAGON_TRIG_BLOCK_, kf_toggle_ac_dragon_trigger_block);

    // Toggle challenge mods
    //get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_TOGGLE_CM_AGGRO_AI_, kf_toggle_cm_aggressive_ai);
    //get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_TOGGLE_CM_BP_ENEMIES_, kf_toggle_cm_bp_enemies);
    //get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_TOGGLE_CM_GL_PHANTOMS_, kf_toggle_cm_gravelord_phantoms);

    // @TODO Load additional keybinds here


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
        global::cmd_out << (output.c_str());
    }
}


static void string_mb_to_wide(char *in_string, std::wstring &out_string) {
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &in_string[0], -1, NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &in_string[0], -1, &wstrTo[0], size_needed);
    out_string.append(wstrTo);
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
    string_mb_to_wide(custom_file_name_buff, Mod::custom_game_archive_path);
    if (std::string(custom_file_name_buff).length() > 0)
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
    string_mb_to_wide(custom_file_name_buff, Mod::custom_save_file_path);
    if (std::string(custom_file_name_buff).length() > 0)
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
    string_mb_to_wide(custom_file_name_buff, Mod::custom_config_file_path);
    if (std::string(custom_file_name_buff).length() > 0)
    {
        Mod::startup_messages.push_back(std::string("    Found custom game config file definition: \"").append(custom_file_name_buff).append("\""));
    }

    // Get I/O output filter
    /*Files::io_output_filter;
    custom_file_name_buff[MAX_PATH] = '\0';
    GetPrivateProfileString(_DS1_OVERHAUL_DEBUG_SECTION_,
                            _DS1_OVERHAUL_PREF_IO_OUT_FILTER_,
                            NULL,
                            custom_file_name_buff,
                            MAX_PATH + 1,
                            _DS1_OVERHAUL_SETTINGS_FILE_);
    Files::io_output_filter = std::string(custom_file_name_buff);*/
}

