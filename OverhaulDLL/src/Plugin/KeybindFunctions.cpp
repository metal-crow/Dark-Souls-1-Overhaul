/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Sean Pesce  -  C++, DirectX9 overlay


    This file defines functions that will be associated
    with keybinds at runtime.
*/

#include "DllMain.h"
#include "AntiCheat.h"
#include "Challenge/AggressiveAi.h"
#include "Challenge/BlackPhantomEnemies.h"
#include "Challenge/GravelordPhantoms.h"
#include "LadderFix.h"
#include "DurabilityBars.h"
#include "Menu/Dialog.h"
#include "Menu/SavedCharacters.h"
#include "MultiTribute.h"
#include "MultiConsume.h"


// Toggles mouse input
int kf_toggle_mouse_input()
{
    Mod::mouse_input = !Mod::mouse_input;
    if (Mod::mouse_input) {
        print_console("Mouse input enabled");
    } else {
        print_console("Mouse input disabled");
    }
    Sleep(_DS1_OVERHAUL_KEYPRESS_DELAY_); // Sleep to avoid keypress being detected multiple times
    return ERROR_SUCCESS;
}

// Toggles camera lock when console is open
int kf_toggle_console_lock_cam()
{
    Mod::console_lock_camera = !Mod::console_lock_camera;
    if (Mod::console_lock_camera) {
        print_console("Camera lock when console is open = enabled");
    } else {
        print_console("Camera lock when console is open = disabled");
    }
    Sleep(_DS1_OVERHAUL_KEYPRESS_DELAY_);
    return ERROR_SUCCESS;
}

// Fixes input bug that causes players to be stuck at a bonfire (usually after turning human with framerate unlocked)
int kf_fix_bonfire_input()
{
    // Apply fix and print message to text feed
    int return_value = Game::fix_bonfire_input(true);
    Sleep(_DS1_OVERHAUL_KEYPRESS_DELAY_);
    return return_value;
}


// Toggles node count element of overlay text feed
int kf_toggle_node_count()
{
    Mod::show_node_count = !Mod::show_node_count;
    if (Mod::show_node_count)
        print_console("Displaying multiplayer node count");
    else
        print_console("Hiding multiplayer node count");
    Sleep(_DS1_OVERHAUL_KEYPRESS_DELAY_);
    return ERROR_SUCCESS;
}


// Toggles dimmed lava visual effects
int kf_toggle_dim_lava()
{
    if (Game::lava_luminosity == NULL)
    {
        if (!Game::characters_loaded)
            print_console("ERROR: Unable to change lava brightness (try loading a character first)");
        else
            print_console("ERROR: Unable to change lava brightness (failed to find lava effects in memory)");
        Sleep(_DS1_OVERHAUL_KEYPRESS_DELAY_);
        return ERROR_FILE_NOT_FOUND;
    }
    Game::enable_dim_lava(!Game::dim_lava_enabled());
    Mod::dim_lava_pref = Game::dim_lava_enabled();
    if (Game::dim_lava_enabled())
        print_console("Lowered brightness of lava effects");
    else
        print_console("Restored default lava effects");
    Sleep(_DS1_OVERHAUL_KEYPRESS_DELAY_);
    return ERROR_SUCCESS;
}


// Toggles ladder fix
int kf_toggle_ladder_fix()
{
    if (LadderFix::is_active())
    {
        LadderFix::unpatch();
    }
    else
    {
        LadderFix::apply();
    }
    Sleep(_DS1_OVERHAUL_KEYPRESS_DELAY_);
    return ERROR_SUCCESS;
}

// Toggles item auto-equip
int kf_toggle_item_auto_equip()
{
    long ret = ERROR_SUCCESS;
    if (!Params::all_loaded())
    {
        ret = D2DERR_NOT_INITIALIZED;
    }
    else if (Game::item_auto_equip_enabled())
    {
        Game::set_item_auto_equip(false, false);
    }
    else
    {
        Game::set_item_auto_equip(true, false);
    }
    Sleep(_DS1_OVERHAUL_KEYPRESS_DELAY_);
    return ret;
}


// Toggles armor sound effects
int kf_toggle_armor_sfx()
{
    if (Params::Armor().base == NULL)
    {
        if (!Game::characters_loaded)
            print_console("ERROR: Unable to toggle armor sounds (try loading a character first)");
        else
            print_console("ERROR: Unable to toggle armor sounds (failed to find armor data in memory)");
        Sleep(_DS1_OVERHAUL_KEYPRESS_DELAY_);
        return ERROR_FILE_NOT_FOUND;
    }
    Game::enable_armor_sfx(!Game::armor_sfx_enabled());
    Mod::disable_armor_sfx_pref = !Game::armor_sfx_enabled();
    if (Game::armor_sfx_enabled())
        print_console("Enabled armor sounds");
    else
        print_console("Disabled armor sounds");
    Sleep(_DS1_OVERHAUL_KEYPRESS_DELAY_);
    return ERROR_SUCCESS;
}


// De-spawns existing Gravelord phantoms, but only if current character is in NG+0
int kf_gravelord_phantoms_despawn() {
    Challenge::GravelordPhantoms::despawn_gravelord_phantoms();
    Sleep(_DS1_OVERHAUL_KEYPRESS_DELAY_);
    return ERROR_SUCCESS;
}


// Toggles Multi-Tribute (multiple simultaneous covenant tributes patch)
int kf_toggle_multi_tribute() {
    if (MultiTribute::is_active())
    {
        MultiTribute::unpatch();
    }
    else
    {
        MultiTribute::apply();
    }
    Sleep(_DS1_OVERHAUL_KEYPRESS_DELAY_);
    return ERROR_SUCCESS;
}


// Toggles Multi-Consume (multiple simultaneous item consumption)
int kf_toggle_multi_consume() {
    if (MultiConsume::is_active())
    {
        MultiConsume::unpatch();
    }
    else
    {
        MultiConsume::apply();
    }
    Sleep(_DS1_OVERHAUL_KEYPRESS_DELAY_);
    return ERROR_SUCCESS;
}


// Next/previous save file
int kf_save_file_next()
{
    if (Files::saves_menu_is_open() && !Menu::Dlg::showing())
    {
        Files::set_save_file_next(false);
        if (GetLastError() == ERROR_SUCCESS)
            Sleep(_DS1_OVERHAUL_KEYPRESS_DELAY_);
    }
    return ERROR_SUCCESS;
}

int kf_save_file_prev()
{
    if (Files::saves_menu_is_open() && !Menu::Dlg::showing())
    {
        Files::set_save_file_prev(false);
        if (GetLastError() == ERROR_SUCCESS)
            Sleep(_DS1_OVERHAUL_KEYPRESS_DELAY_);
    }
    return ERROR_SUCCESS;
}

// Opens save file picker dialog
int kf_save_file_choose()
{
    if (Files::saves_menu_is_open() && !Menu::Dlg::showing())
    {
        Menu::Saves::open_dialog();
        Sleep(_DS1_OVERHAUL_KEYPRESS_DELAY_);
    }
    return ERROR_SUCCESS;
}


// Toggles for additional HUD elements
int kf_toggle_hud_compass_radial()
{
    Hud::set_show_compass_radial(!Hud::get_show_compass_radial());
    if (Hud::get_show_compass_radial())
        print_console("Enabled radial compass HUD element");
    else
        print_console("Disabled radial compass HUD element");
    Sleep(_DS1_OVERHAUL_KEYPRESS_DELAY_);
    return ERROR_SUCCESS;
}

int kf_toggle_hud_compass_bar()
{
    Hud::set_show_compass_bar(!Hud::get_show_compass_bar());
    if (Hud::get_show_compass_bar())
        print_console("Enabled bar compass HUD element");
    else
        print_console("Disabled bar compass HUD element");
    Sleep(_DS1_OVERHAUL_KEYPRESS_DELAY_);
    return ERROR_SUCCESS;
}

int kf_toggle_hud_elevation_meter()
{
    Hud::set_show_elevation_meter(!Hud::get_show_elevation_meter());
    if (Hud::get_show_elevation_meter())
        print_console("Enabled elevation meter HUD element");
    else
        print_console("Disabled elevation meter HUD element");
    Sleep(_DS1_OVERHAUL_KEYPRESS_DELAY_);
    return ERROR_SUCCESS;
}

int kf_toggle_hud_node_graph()
{
    Hud::set_show_node_graph(!Hud::get_show_node_graph(), false);
    if (Hud::get_show_node_graph())
        print_console("Enabled multiplayer node graph HUD element");
    else
        print_console("Disabled multiplayer node graph HUD element");
    Sleep(_DS1_OVERHAUL_KEYPRESS_DELAY_);
    return ERROR_SUCCESS;
}

int kf_toggle_hud_durability()
{
    DurabilityBars::enable_pref = !DurabilityBars::enable_pref;
    if (DurabilityBars::enable_pref)
        print_console("Enabled weapon durability HUD elements");
    else
        print_console("Disabled weapon durability HUD elements");
    Sleep(_DS1_OVERHAUL_KEYPRESS_DELAY_);
    return ERROR_SUCCESS;
}


// Toggles for anti-cheat services
int kf_toggle_ac_binocs_trigger_block()
{
    AntiCheat::BinocsTriggerBlock::toggle();
    Sleep(_DS1_OVERHAUL_KEYPRESS_DELAY_);
    return ERROR_SUCCESS;
}

int kf_toggle_ac_dragon_trigger_block()
{
    AntiCheat::DragonTriggerBlock::toggle();
    Sleep(_DS1_OVERHAUL_KEYPRESS_DELAY_);
    return ERROR_SUCCESS;
}


// Toggles for challenge mods
int kf_toggle_cm_aggressive_ai()
{
    Challenge::AggressiveAi::toggle();
    Sleep(_DS1_OVERHAUL_KEYPRESS_DELAY_);
    return ERROR_SUCCESS;
}

int kf_toggle_cm_bp_enemies()
{
    Challenge::BlackPhantomEnemies::toggle();
    Sleep(_DS1_OVERHAUL_KEYPRESS_DELAY_);
    return ERROR_SUCCESS;
}

int kf_toggle_cm_gravelord_phantoms() {
    Challenge::GravelordPhantoms::toggle();
    Sleep(_DS1_OVERHAUL_KEYPRESS_DELAY_);
    return ERROR_SUCCESS;
}