/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Sean Pesce  -  C++

*/

#include "DllMain.h"
#include "AntiCheat.h"
#include "Challenge/AggressiveAI.h"
#include "Challenge/BlackPhantomEnemies.h"
#include "Challenge/GravelordPhantoms.h"
#include "AnimationEdits.h"
#include "LadderFix.h"
#include "Updates.h"
#include "DurabilityBars.h"
#include "MultiTribute.h"
#include "MultiConsume.h"
#include "L3Jump.h"

#include <regex>

#define _SP_DEFINE_VK_NAME_STRINGS_		// Must be defined to use Virtual-key code name strings from SP_IO_Strings.hpp (opt-in by default because it increases filesize by a few KB)

#include "SP_IO.hpp"
#include "SP_IO_Strings.hpp"

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

// User preference setting; determines whether in-game time (IGT) HUD element will be shown when available
bool Mod::hud_play_time_pref = false;

// User preference setting; determines whether to automatically equip items on pickup
bool Mod::disable_auto_equip_pref = true;

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
    memset(buffer, 0, sizeof(char)*_DS1_OVERHAUL_SETTINGS_STRING_BUFF_LEN_);

    // Temporary string for printing console messages
    std::string msg;


    // Begin loading setting preferences
    print_console(Mod::output_prefix + "Loading user preferences...");

    // Check if camera should be locked when console is open
    Mod::console_lock_camera = ((int)GetPrivateProfileInt(_DS1_OVERHAUL_INPUT_SECTION_, _DS1_OVERHAUL_PREF_CONSOLE_LOCK_CAM_, (int)Mod::console_lock_camera, _DS1_OVERHAUL_SETTINGS_FILE_) != 0);

    Mod::cheats_warning = ((int)GetPrivateProfileInt(_DS1_OVERHAUL_PREFS_SECTION_, _DS1_OVERHAUL_PREF_CHEATS_WARNING_, (int)Mod::cheats_warning, _DS1_OVERHAUL_SETTINGS_FILE_) != 0);

    Mod::disable_low_fps_disconnect = ((int)GetPrivateProfileInt(_DS1_OVERHAUL_PREFS_SECTION_, _DS1_OVERHAUL_PREF_DISABLE_LOW_FPS_DISCONNECT_, (int)Mod::disable_low_fps_disconnect, _DS1_OVERHAUL_SETTINGS_FILE_) != 0);

    AnimationEdits::gesture_cancelling = ((int)GetPrivateProfileInt(_DS1_OVERHAUL_PREFS_SECTION_, _DS1_OVERHAUL_PREF_GESTURE_CANCELLING_, (int)AnimationEdits::gesture_cancelling, _DS1_OVERHAUL_SETTINGS_FILE_) != 0);
    if (AnimationEdits::gesture_cancelling) {
        print_console(msg.append("    Gesture cancelling enabled"));
    }
    
    Mod::disable_auto_equip_pref = ((int)GetPrivateProfileInt(_DS1_OVERHAUL_PREFS_SECTION_, _DS1_OVERHAUL_PREF_DISABLE_AUTO_EQUIP_, (int)Mod::disable_auto_equip_pref, _DS1_OVERHAUL_SETTINGS_FILE_) != 0);
    if (Mod::disable_auto_equip_pref) {
        print_console("    Automatic item equip disabled");
    }

    LadderFix::enable_pref = ((int)GetPrivateProfileInt(_DS1_OVERHAUL_PREFS_SECTION_, _DS1_OVERHAUL_PREF_LADDER_FIX_, (int)LadderFix::enable_pref, _DS1_OVERHAUL_SETTINGS_FILE_) != 0);
    if (LadderFix::enable_pref) {
        print_console("    Ladder fix enabled (EXPERIMENTAL)");
    }

    uint32_t ladder_rung = (uint32_t)GetPrivateProfileInt(_DS1_OVERHAUL_PREFS_SECTION_, _DS1_OVERHAUL_PREF_LADDER_FIX_RUNG_INDEX_, (int)LadderFix::exit_rung(), _DS1_OVERHAUL_SETTINGS_FILE_);
    if (ladder_rung > 0 && ladder_rung <= 5 && ladder_rung != DS1_DEFAULT_LADDER_FIX_EXIT_RUNG_) {
        print_console("    Overriding Ladder Fix exit rung to " + std::to_string(ladder_rung));
        LadderFix::set_exit_rung(ladder_rung);
    }

    // Display multiplayer node count in text feed info header
    Mod::show_node_count = ((int)GetPrivateProfileInt(_DS1_OVERHAUL_PREFS_SECTION_, _DS1_OVERHAUL_PREF_SHOW_NODE_COUNT_, (int)Mod::show_node_count, _DS1_OVERHAUL_SETTINGS_FILE_) != 0);
    msg = "    Display multiplayer node count = ";
    if (Mod::show_node_count)
        print_console(msg.append("enabled"));
    else
        print_console(msg.append("disabled"));

    // Display IGT in text feed info header
    Mod::hud_play_time_pref = ((int)GetPrivateProfileInt(_DS1_OVERHAUL_PREFS_SECTION_, _DS1_OVERHAUL_PREF_PLAY_TIME_, (int)Mod::hud_play_time_pref, _DS1_OVERHAUL_SETTINGS_FILE_) != 0);
    msg = "    Display IGT = ";
    if (Mod::hud_play_time_pref)
        print_console(msg.append("enabled"));
    else
        print_console(msg.append("disabled"));


    // Check whether to lower the brightness of lava effects
    Mod::dim_lava_pref = ((int)GetPrivateProfileInt(_DS1_OVERHAUL_PREFS_SECTION_, _DS1_OVERHAUL_PREF_DIM_LAVA_, 0, _DS1_OVERHAUL_SETTINGS_FILE_) != 0);
    if (Mod::dim_lava_pref)
        print_console("    Lava visual effects will be dimmed");

    // Check whether to display weapon durability bars on HUD
    DurabilityBars::enable_pref = ((int)GetPrivateProfileInt(_DS1_OVERHAUL_HUD_SECTION_, _DS1_OVERHAUL_PREF_DURABILITY_BARS_, DurabilityBars::render_data.display, _DS1_OVERHAUL_SETTINGS_FILE_) != 0);
    if (DurabilityBars::enable_pref)
    {
        DurabilityBars::apply(true);
        print_console("    Weapon durability bars = enabled");
    }
    else
    {
        print_console("    Weapon durability bars = disabled");
    }

    // Durability bar offsets (in pixels)
    DurabilityBars::render_data.offset_left_x = (int)GetPrivateProfileInt(_DS1_OVERHAUL_HUD_SECTION_, _DS1_OVERHAUL_PREF_DURABILITY_OFFSET_L_X_, DurabilityBars::render_data.offset_left_x,  _DS1_OVERHAUL_SETTINGS_FILE_);
    if (DurabilityBars::render_data.offset_left_x)
    {
        print_console("    Horizontal offset for  left-side durability bar = " + std::to_string(DurabilityBars::render_data.offset_left_x) + " pixels");
    }
    DurabilityBars::render_data.offset_right_x = (int)GetPrivateProfileInt(_DS1_OVERHAUL_HUD_SECTION_, _DS1_OVERHAUL_PREF_DURABILITY_OFFSET_R_X_, DurabilityBars::render_data.offset_right_x, _DS1_OVERHAUL_SETTINGS_FILE_);
    if (DurabilityBars::render_data.offset_right_x)
    {
        print_console("    Horizontal offset for right-side durability bar = " + std::to_string(DurabilityBars::render_data.offset_right_x) + " pixels");
    }
    DurabilityBars::render_data.offset_y = (int)GetPrivateProfileInt(_DS1_OVERHAUL_HUD_SECTION_, _DS1_OVERHAUL_PREF_DURABILITY_OFFSET_Y_,   DurabilityBars::render_data.offset_y,       _DS1_OVERHAUL_SETTINGS_FILE_);
    if (DurabilityBars::render_data.offset_y)
    {
        print_console("    Vertical offset for durability bars = " + std::to_string(DurabilityBars::render_data.offset_y) + " pixels");
    }

    // Durability bar scale
    if (GetPrivateProfileString(_DS1_OVERHAUL_HUD_SECTION_, _DS1_OVERHAUL_PREF_DURABILITY_BAR_SCALE_, NULL, buffer, _DS1_OVERHAUL_SETTINGS_STRING_BUFF_LEN_, _DS1_OVERHAUL_SETTINGS_FILE_))
    {
        try
        {
            float dur_bar_scale = std::stof(std::string(buffer));
            if (dur_bar_scale >= 0.0f)
            {
                DurabilityBars::render_data.scale = dur_bar_scale;
                print_console("    Durability bar scale = " + std::to_string(DurabilityBars::render_data.scale));
            }
        }
        catch (const std::invalid_argument&)
        {
            print_console(std::string("    ERROR: Invalid entry was ignored for ") + _DS1_OVERHAUL_PREF_DURABILITY_BAR_SCALE_);
        }
        catch (const std::out_of_range&)
        {
            print_console(std::string("    ERROR: Invalid entry was ignored for ") + _DS1_OVERHAUL_PREF_DURABILITY_BAR_SCALE_);
        }
    }
    memset(buffer, 0, sizeof(char)*_DS1_OVERHAUL_SETTINGS_STRING_BUFF_LEN_);


    // Check whether to disable armor sound effects
#ifndef DS1_OVERHAUL_QOL_PREVIEW
    Mod::disable_armor_sfx_pref = ((int)GetPrivateProfileInt(_DS1_OVERHAUL_PREFS_SECTION_, _DS1_OVERHAUL_PREF_DISABLE_ARMOR_SFX_, 0, _DS1_OVERHAUL_SETTINGS_FILE_) != 0);
    if (Mod::disable_armor_sfx_pref)
        print_console("    Armor sound effects will be disabled");
#endif // DS1_OVERHAUL_QOL_PREVIEW


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
    if ((int)GetPrivateProfileInt(_DS1_OVERHAUL_CHALLENGE_SECTION_, _DS1_OVERHAUL_PREF_CM_AGGRO_AI_, Challenge::AggressiveAi::active(), _DS1_OVERHAUL_SETTINGS_FILE_) != 0) {
        print_console("    Challenge: Aggressive AI will be enabled");
    }
    if ((int)GetPrivateProfileInt(_DS1_OVERHAUL_CHALLENGE_SECTION_, _DS1_OVERHAUL_PREF_CM_GL_PHANTOMS_, Challenge::GravelordPhantoms::active, _DS1_OVERHAUL_SETTINGS_FILE_) != 0) {
        print_console("    Challenge: Auto-spawning Gravelord Phantoms will be enabled");
    }
    if ((int)GetPrivateProfileInt(_DS1_OVERHAUL_CHALLENGE_SECTION_, _DS1_OVERHAUL_PREF_CM_BP_ENEMIES_, Challenge::BlackPhantomEnemies::active, _DS1_OVERHAUL_SETTINGS_FILE_) != 0) {
        print_console("    Challenge: Black Phantom Enemies will be enabled");
    }
    Challenge::AggressiveAi::ear_distance  = (uint16_t)GetPrivateProfileInt(_DS1_OVERHAUL_CHALLENGE_SETTINGS_SECTION_, _DS1_OVERHAUL_PREF_CM_AGGRO_AI_EAR_DIST_,  Challenge::AggressiveAi::DEFAULT_EAR_DISTANCE,  _DS1_OVERHAUL_SETTINGS_FILE_);
    Challenge::AggressiveAi::nose_distance = (uint16_t)GetPrivateProfileInt(_DS1_OVERHAUL_CHALLENGE_SETTINGS_SECTION_, _DS1_OVERHAUL_PREF_CM_AGGRO_AI_NOSE_DIST_, Challenge::AggressiveAi::DEFAULT_NOSE_DISTANCE, _DS1_OVERHAUL_SETTINGS_FILE_);
    Challenge::BlackPhantomEnemies::DRAW_TYPE = (uint8_t)GetPrivateProfileInt(_DS1_OVERHAUL_CHALLENGE_SETTINGS_SECTION_, _DS1_OVERHAUL_PREF_CM_BP_ENEMY_DRAW_TYPE_, Challenge::BlackPhantomEnemies::DRAW_TYPE_DEFAULT, _DS1_OVERHAUL_SETTINGS_FILE_);

    GetPrivateProfileString(_DS1_OVERHAUL_PREFS_SECTION_,
                            _DS1_OVERHAUL_PREF_CUSTOM_UPDATE_SOURCE_URL_,
                            NULL,
                            buffer,
                            _DS1_OVERHAUL_SETTINGS_STRING_BUFF_LEN_,
                            _DS1_OVERHAUL_SETTINGS_FILE_);
    if (buffer[0] != '\0' && std::regex_match(buffer, std::regex("^(?:http(s)?:\\/\\/)?[\\w.-]+(?:\\.[\\w\\.-]+)+[\\w\\-\\._~:/?#[\\]@!\\$&'\\(\\)\\*\\+,;=.]+$"))) {
        if (std::find(Updates::sources().begin(), Updates::sources().end(), buffer) == Updates::sources().end())
        {
            Updates::sources().push_back(buffer);
            print_console("    Using additional custom remote host for updates: " + Updates::sources().back());
        }
        else
        {
            print_console("    Custom remote host is already being used: " + std::string(buffer));
        }
    }
    buffer[0] = '\0';
    while (Updates::skip_source.size() < Updates::sources().size())
    {
        Updates::skip_source.push_back(false);
    }

    for (unsigned int i = 0; i < Updates::sources().size(); i++) {
        Updates::skip_source[i] = ((int)GetPrivateProfileInt(_DS1_OVERHAUL_PREFS_SECTION_, (_DS1_OVERHAUL_PREF_SKIP_UPDATE_SOURCE_+std::to_string(i)).c_str(), 0, _DS1_OVERHAUL_SETTINGS_FILE_) != 0);
    }

    Updates::keep_temp_files = ((int)GetPrivateProfileInt(_DS1_OVERHAUL_PREFS_SECTION_, _DS1_OVERHAUL_PREF_KEEP_TEMP_UPDATE_CHECK_FILES_, Updates::keep_temp_files, _DS1_OVERHAUL_SETTINGS_FILE_) != 0);

    if ((int)GetPrivateProfileInt(_DS1_OVERHAUL_PREFS_SECTION_, _DS1_OVERHAUL_PREF_MULTI_TRIBUTE_, 1, _DS1_OVERHAUL_SETTINGS_FILE_) != 0) {
        MultiTribute::apply("    ");
    }

    if ((int)GetPrivateProfileInt(_DS1_OVERHAUL_PREFS_SECTION_, _DS1_OVERHAUL_PREF_MULTI_CONSUME_, 1, _DS1_OVERHAUL_SETTINGS_FILE_) != 0) {
        MultiConsume::apply("    ");
    }

    if ((int)GetPrivateProfileInt(_DS1_OVERHAUL_PREFS_SECTION_, _DS1_OVERHAUL_PREF_L3_JUMP_, L3Jump::is_active(), _DS1_OVERHAUL_SETTINGS_FILE_) != 0) {
        L3Jump::apply("    ");
    }
    
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

    // Toggle IGT in text feed info header
    get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_TOGGLE_PLAY_TIME_, kf_toggle_play_time);


    // Toggle dimmed lava visual effects
    get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_TOGGLE_DIM_LAVA_, kf_toggle_dim_lava);

    // Toggle ladder fix
    get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_TOGGLE_LADDER_FIX_, kf_toggle_ladder_fix);

    // Toggle item auto equip
    get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_TOGGLE_ITEM_AUTO_EQUIP_, kf_toggle_item_auto_equip);

#ifndef DS1_OVERHAUL_QOL_PREVIEW
    // Toggle armor sound effects
    get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_TOGGLE_ARMOR_SFX_, kf_toggle_armor_sfx);
#endif // DS1_OVERHAUL_QOL_PREVIEW

    // De-spawn existing Gravelord phantoms
    get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_DESPAWN_GL_PHANTOMS_, kf_gravelord_phantoms_despawn);

    // Toggle Multi-tribute
    get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_TOGGLE_MULTI_TRIBUTE_, kf_toggle_multi_tribute);

    // Toggle Multi-consumption
    get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_TOGGLE_MULTI_CONSUME_, kf_toggle_multi_consume);

    // Toggle omni-directional rolling
    get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_TOGGLE_OMNI_ROLL_, kf_toggle_omni_roll);

    // Toggle jumping with L3 instead of B
    get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_TOGGLE_L3_JUMP_, kf_toggle_l3_jump);

    // Next/previous save file
    get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_SAVE_FILE_NEXT_, kf_save_file_next);
    get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_SAVE_FILE_PREV_, kf_save_file_prev);
    get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_SAVE_FILE_CHOOSE_, kf_save_file_choose);

    // Toggle additional HUD elements
    get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_TOGGLE_HUD_COMPASS_RADIAL_, kf_toggle_hud_compass_radial);
    get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_TOGGLE_HUD_COMPASS_BAR_, kf_toggle_hud_compass_bar);
    get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_TOGGLE_HUD_ELEVATION_METER_, kf_toggle_hud_elevation_meter);
    get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_TOGGLE_HUD_NODE_GRAPH_, kf_toggle_hud_node_graph);
    get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_TOGGLE_HUD_DURABILITY_BARS_, kf_toggle_hud_durability);
    
    // Toggle anti-cheats (not all are togglable)
    get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_TOGGLE_AC_BINOCS_TRIG_BLOCK_, kf_toggle_ac_binocs_trigger_block);
    get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_TOGGLE_AC_DRAGON_TRIG_BLOCK_, kf_toggle_ac_dragon_trigger_block);

    // Toggle challenge mods
    get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_TOGGLE_CM_AGGRO_AI_, kf_toggle_cm_aggressive_ai);
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

    // Get I/O output filter
    Files::io_output_filter;
    custom_file_name_buff[MAX_PATH] = '\0';
    GetPrivateProfileString(_DS1_OVERHAUL_DEBUG_SECTION_,
                            _DS1_OVERHAUL_PREF_IO_OUT_FILTER_,
                            NULL,
                            custom_file_name_buff,
                            MAX_PATH + 1,
                            _DS1_OVERHAUL_SETTINGS_FILE_);
    Files::io_output_filter = std::string(custom_file_name_buff);
}

