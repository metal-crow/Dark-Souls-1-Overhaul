/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Ashley                      -   Reverse engineering, Low FPS Disconnect patch technique
        Jellybaby34                 -   Game version number patch technique
        Metal-Crow                  -   Reverse engineering, Phantom Limit patch, special effect ID sync, C++
        RavagerChris                -   Reverse engineering of game files, gesture cancelling technique
        Sean Pesce                  -   C++, automataed Bonfire Input Fix (FPSFix+), various C++ conversions of other contributors' work
        Youri "NullBy7e" de Mooij   -   Original Bonfire Input Fix technique (FPSFix)
        Zullie The Witch            -   Toggle armor sounds, Dim Lava Effects

*/

#include "DllMain.h"
#include "AntiCheat.h"
#include "BloodborneRallySystem.h"
#include "Challenge/AggressiveAi.h"
#include "Challenge/BlackPhantomEnemies.h"
#include "PhantomUnshackle.h"
#include "MoveWhileCasting.h"
#include "Menu/Dialog.h"
#include "Param/Params.h"
#include "Param/Item.h"
#include "XInputUtil.h"


/*
    Initialize static variables:
*/

// Base address of Dark Souls game process
void *Game::ds1_base = NULL;

// Base address of fmodex dll Dark Souls loads
void *Game::fmodex_base = NULL;

// Base address for player character data
void *Game::player_char_base = NULL;

// Base address for world character data
void *Game::world_char_base = NULL;

// Player character status (loading, human, co-op, invader, hollow)
SpPointer Game::player_char_status;

// Current character name
SpPointer Game::player_char_name(reinterpret_cast<void*>(0x1378700), { 0x8, 0xA0 });

// Time Action Events for the player character's animations
Tae Game::player_tae = Tae();

// Flag to determine if any characters have been loaded since the game was launched (useful if player had a character loaded but returned to main menu)
bool Game::characters_loaded = false;

// Address of lava brightness effect (used for dimming lava)
uint8_t *Game::lava_luminosity = NULL;

// Game saving on/off
SpPointer Game::saves_enabled;

// Indicates whether any invaders are present
uint32_t Game::invaders_present_in_world = 0;

// Multiplayer node count
int Game::node_count = -1;

// In-game time (IGT) as a printable string
std::string Game::play_time_str = "[IGT: --:--:--:--]   ";

// Size of the pool of memory the game allocates for itself
uint32_t Game::memory_limit = 0;


// Strutures for tracking file I/O data for the game's BDT, BHD5, and SL2 files
Files::IoMonitor Files::io_monitors[9];





// Initializes pointers that depend on the game's base address
void Game::init_pointers()
{
    print_console(std::string(Mod::output_prefix + "Initializing pointers...").c_str());

    // Obtain base address for player character data
    Game::player_char_base = (void*)((unsigned int)Game::ds1_base + 0xF7E204);

    // Base addr for world character data
    Game::world_char_base = (void*)((unsigned int)Game::ds1_base + 0xF7D644);
        
    // Player character status (loading, human, co-op, invader, hollow)
    Game::player_char_status = SpPointer(Game::player_char_base, { 0xA28 });

    // Game saving on/off
    Game::saves_enabled = SpPointer((void*)0x13784A0, { 0xB40 });

}


// Performs tasks that were deferred until all game parameter files were loaded
void Game::on_all_params_loaded()
{
    // Initialize param data manipulators
    Params::init();
    if (!print_console("[DARK SOULS] Finished loading .param files."))
    {
        Mod::startup_messages.push_back("[DARK SOULS] Finished loading .param files.");
    }

    // Disable armor sounds if it was specified in the config file
    if (Mod::disable_armor_sfx_pref)
        Game::enable_armor_sfx(false);

    // Disable automatic equipping of items on pickup
    if (Mod::disable_auto_equip_pref) {
        Game::set_item_auto_equip(false);
    }

    // Enable challenge mods
    if ((int)GetPrivateProfileInt(_DS1_OVERHAUL_CHALLENGE_SECTION_, _DS1_OVERHAUL_PREF_CM_AGGRO_AI_, Challenge::AggressiveAi::active(), _DS1_OVERHAUL_SETTINGS_FILE_) != 0) {
        Challenge::AggressiveAi::enable();
    }
    if ((int)GetPrivateProfileInt(_DS1_OVERHAUL_CHALLENGE_SECTION_, _DS1_OVERHAUL_PREF_CM_BP_ENEMIES_, Challenge::BlackPhantomEnemies::active, _DS1_OVERHAUL_SETTINGS_FILE_) != 0) {
        Challenge::BlackPhantomEnemies::enable();
    }
}


// Performs tasks that were deferred until a character was loaded
void Game::on_first_character_loaded()
{
    Game::characters_loaded = true;

    // Obtain lava effect address
    if (Game::lava_luminosity == NULL)
        Game::lava_luminosity = (uint8_t*)aob_scan("66 66 26 40 00 00 80 3F 00 00 80 3F 00 00 80 3F 00 00 80 3F 00 00 80 40");

    if (Mod::dim_lava_pref)
        Game::enable_dim_lava(true);

    // Enable forced binoculars/dragonification PvP protections
    if (AntiCheat::BinocsTriggerBlock::active)
        AntiCheat::BinocsTriggerBlock::enable();
    if (AntiCheat::DragonTriggerBlock::active)
        AntiCheat::DragonTriggerBlock::enable();
}

// Performs tasks that must be rerun after any loading screen
void Game::on_reloaded() {

    /*//refresh the animation table pointers
    for (int i = 0; i < sizeof(pc_animation_table) / sizeof(void**); i++) {
        Game::pc_animation_table[i] = (AnimationEntry**)SpPointer(Game::world_char_base, { 0x28, 0x0, 0x28, 0x14, 0x4, 0x10 + (0x60 * i) }).resolve();
        if (Game::pc_animation_table[i] == NULL) {
            print_console("Error getting Animation Table Entry address");
        }
    }*/
}


// Obtains the current game version number
uint8_t Game::get_game_version()
{
    // @TODO: Fix this function to work on different builds of DARKSOULS.exe
    return *((uint8_t*)Game::ds1_base + 0x7E719F);
}


// Changes the game version number to avoid compatibility issues with different game builds
void Game::set_game_version(uint8_t version_number)
{
    std::stringstream hex_stream;
    hex_stream << std::hex << (int)version_number;
    std::string new_version_str = hex_stream.str();
    hex_stream.str(""); // Clear string stream
    hex_stream << std::hex << (int)Game::get_game_version();

    if (version_number != Game::get_game_version()) {
        Mod::startup_messages.push_back(std::string(Mod::output_prefix + "Changing game version number from 0x").append(hex_stream.str()).append(" to 0x").append(new_version_str).append("..."));
    }

    uint8_t patch1[5] = { 0xC6, 0x44, 0x24, 0x1C, version_number }; // mov byte ptr [esp+0x1C],0x3C
    uint8_t patch2[3] = { 0x80, 0x38, version_number }; // cmp byte ptr [eax],0x3C

    void *write_address = (uint8_t*)Game::ds1_base + 0x7E73FA;
    apply_byte_patch(write_address, patch1, 5);

    write_address = (uint8_t*)Game::ds1_base + 0x7E719D;
    apply_byte_patch(write_address, patch2, 3);

    write_address = (uint8_t*)Game::ds1_base + 0x7E7229;
    apply_byte_patch(write_address, patch2, 3);
}


// Checks if the player is stuck at the bonfire, and if so, automatically applies the bonfire input fix
void Game::check_bonfire_input_bug()
{
    // Check that the mod is fully loaded
    if (!Mod::initialized)
        return;

    /*
        If glitch conditions do not exist, this variable is set to 0.
        When bonfire input glitch is detected and first_detected is 0, the time is logged in first_detected.

        If the glitch is detected, and the time in first_detected is more than 1 second ago, the bonfire input fix is applied.
    */
    static DWORD first_detected; // (Static variable persists between function calls)

    // Get current player status
    int status = DS1_PLAYER_STATUS_LOADING;
    if (player_char_status.read(&status) != ERROR_SUCCESS)
    {
        // Error reading memory location
        first_detected = 0;
        return;
    }


    // Check for bonfire input glitch conditions
    if (status == DS1_PLAYER_STATUS_HOLLOW || status == DS1_PLAYER_STATUS_HUMAN) // Check if player is hollow/human
    {
        // Nested if-statements are necessary to avoid crashes related to resolving multi-level pointers

        // Get pointer to current character animation
        SpPointer bonfire_anim_fix = SpPointer((void*)0x12E29E8, { 0x0, 0xFC });

        // Read current character animation
        uint32_t current_anim;
        if (bonfire_anim_fix.read(&current_anim) != ERROR_SUCCESS)
        {
            // Error reading memory location
            first_detected = 0;
            return;
        }


        // Read bonfire menu flag
        SpPointer bonfire_menu_flag = SpPointer((void*)((uint32_t)GetModuleHandle(NULL) + 0xF786D0), { 0x40 });
        uint8_t bonfire_menu_is_open;
        if (bonfire_menu_flag.read(&bonfire_menu_is_open) != ERROR_SUCCESS)
        {
            first_detected = 0;
            return;
        }

        // Read repair menu flag
        SpPointer repair_menu_flag = SpPointer((void*)((uint32_t)GetModuleHandle(NULL) + 0xF786D0), { 0x4C });
        uint8_t repair_menu_is_open;
        if (repair_menu_flag.read(&repair_menu_is_open) != ERROR_SUCCESS)
        {
            first_detected = 0;
            return;
        }

        // Read level up menu flag
        SpPointer level_menu_flag = SpPointer((void*)((uint32_t)GetModuleHandle(NULL) + 0xF786D0), { 0x78 });
        uint8_t level_menu_is_open;
        if (level_menu_flag.read(&level_menu_is_open) != ERROR_SUCCESS)
        {
            first_detected = 0;
            return;
        }

        // Read bottomless box menu flag
        SpPointer bottomless_menu_flag = SpPointer((void*)((uint32_t)GetModuleHandle(NULL) + 0xF786D0), { 0x84 });
        uint8_t bottomless_menu_is_open;
        if (bottomless_menu_flag.read(&bottomless_menu_is_open) != ERROR_SUCCESS)
        {
            first_detected = 0;
            return;
        }

        // Read attune magic menu flag
        SpPointer attune_menu_flag = SpPointer((void*)((uint32_t)GetModuleHandle(NULL) + 0xF786D0), { 0x80 });
        uint8_t attune_menu_is_open;
        if (attune_menu_flag.read(&attune_menu_is_open) != ERROR_SUCCESS)
        {
            first_detected = 0;
            return;
        }

        // Read reinforce weapon/armor menu flag
        SpPointer reinforce_menu_flag = SpPointer((void*)((uint32_t)GetModuleHandle(NULL) + 0xF786D0), { 0x50 });
        uint8_t reinforce_menu_is_open;
        if (reinforce_menu_flag.read(&reinforce_menu_is_open) != ERROR_SUCCESS)
        {
            first_detected = 0;
            return;
        }

        // Read warp menu flag
        SpPointer warp_menu_flag = SpPointer((void*)((uint32_t)GetModuleHandle(NULL) + 0xF786D0), { 0xAC });
        uint8_t warp_menu_is_open;
        if (warp_menu_flag.read(&warp_menu_is_open) != ERROR_SUCCESS)
        {
            first_detected = 0;
            return;
        }

        // Read "reverse hollowing?"/"can't kindle"/"can't reverse hollowing"/"insufficient attunement slots" dialog flag
        SpPointer dialog_menu_flag = SpPointer((void*)((uint32_t)GetModuleHandle(NULL) + 0xF786D0), { 0x60 });
        uint8_t dialog_menu_is_open;
        if (dialog_menu_flag.read(&dialog_menu_is_open) != ERROR_SUCCESS)
        {
            first_detected = 0;
            return;
        }


        // Check bonfire input fix conditions
        if ((current_anim == 7701 || current_anim == 7711 || current_anim == 7721) // 3 different bonfire resting animation IDs
            && !bonfire_menu_is_open && !repair_menu_is_open && !bottomless_menu_is_open && !reinforce_menu_is_open
            && !level_menu_is_open && !attune_menu_is_open && !dialog_menu_is_open && !warp_menu_is_open) // Make sure no bonfire menus are open
        {
            // Bonfire input glitch conditions currently exist

            if (first_detected == 0)
            {
                // First detection; log time
                first_detected = GetTickCount();
                return;
            }
            else if ((GetTickCount() - first_detected) >= 1000)
            {
                // Too much time has elapsed; bonfire input glitch is definitely present
                fix_bonfire_input();
                print_console(_DS1_MOD_MSG_BONFIRE_INPUT_FIX_AUTOMATIC_);
            }
            else
            {
                // Not enough time has elapsed; return and check again later
                return;
            }
        }
    }

    first_detected = 0;
}


// Fixes input bug that causes players to be stuck at a bonfire (usually after turning human with framerate unlocked)
int Game::fix_bonfire_input(bool print_to_text_feed, bool print_to_console)
{
    // Get current player status
    int status = DS1_PLAYER_STATUS_LOADING;
    if (player_char_status.read(&status) != ERROR_SUCCESS)
    {
        // Error reading memory location
        if (print_to_text_feed)
            print(_DS1_MOD_MSG_BONFIRE_INPUT_FIX_NO_INIT_FAIL_);

        if (print_to_console)
            print_console(_DS1_MOD_MSG_BONFIRE_INPUT_FIX_NO_INIT_FAIL_);
        return ERROR_INVALID_ADDRESS;
    }


    if (status == DS1_PLAYER_STATUS_HOLLOW || status == DS1_PLAYER_STATUS_HUMAN) // Check if player is hollow/human
    {
        // Get pointer to current character animation
        SpPointer bonfire_anim_fix = SpPointer((void*)0x12E29E8, { 0x0, 0xFC });

        // Read current character animation
        uint32_t current_anim;
        bonfire_anim_fix.read(&current_anim);

        // Check that player is currently sitting at a bonfire
        if (current_anim == 7701 || current_anim == 7711 || current_anim == 7721) // 3 different bonfire resting animation IDs
        {
            // Write zero to bonfire animation status address
            bonfire_anim_fix.write((uint32_t)0);

            if (print_to_text_feed)
                print(_DS1_MOD_MSG_BONFIRE_INPUT_FIX_SUCCESS_);

            if (print_to_console)
                print_console(_DS1_MOD_MSG_BONFIRE_INPUT_FIX_SUCCESS_);

            return ERROR_SUCCESS;
        }
        else
        {
            // Player is not at a bonfire
            if (print_to_text_feed)
                print(_DS1_MOD_MSG_BONFIRE_INPUT_FIX_NOT_AT_BONFIRE_FAIL_);

            if (print_to_console)
                print_console(_DS1_MOD_MSG_BONFIRE_INPUT_FIX_NOT_AT_BONFIRE_FAIL_);
        }
    }
    else
    {
        // Player is not hollow/human, so can't be at a bonfire
        if (status == DS1_PLAYER_STATUS_LOADING)
        {
            if (print_to_text_feed)
                print(_DS1_MOD_MSG_BONFIRE_INPUT_FIX_NO_CHAR_FAIL_);

            if (print_to_console)
                print_console(_DS1_MOD_MSG_BONFIRE_INPUT_FIX_NO_CHAR_FAIL_);
        }
        else
        {
            if (print_to_text_feed)
                print(_DS1_MOD_MSG_BONFIRE_INPUT_FIX_NOT_AT_BONFIRE_FAIL_);

            if (print_to_console)
                print_console(_DS1_MOD_MSG_BONFIRE_INPUT_FIX_NOT_AT_BONFIRE_FAIL_);
        }
    }

    return ERROR_BAD_ENVIRONMENT;
}


// Returns true if item auto-equip is currently enabled/unmodified
bool Game::item_auto_equip_enabled()
{
    if (Params::all_loaded())
    {
        return (Params::Goods().get(0)->isAutoEquip && Params::Goods().get(1)->isAutoEquip && Params::Goods().get(2)->isAutoEquip);
    }
    return true;
}

// Enable/disable item auto-equip
void Game::set_item_auto_equip(bool enabled, bool silent)
{
    static std::vector<uint8_t> default_values;
    if (default_values.empty())
    {
        // Initialize default values container
        for (size_t i = 0; i < Params::Goods().param_count; i++)
        {
            default_values.push_back(Params::Goods().get(i)->isAutoEquip);
        }
    }
    if (enabled)
    {
        for (size_t i = 0; i < Params::Goods().param_count; i++)
        {
            Params::Goods().get(i)->isAutoEquip = default_values[i];
        }
    }
    else
    {
        // Disable auto-pickup
        for (size_t i = 0; i < Params::Goods().param_count; i++)
        {
            if (Params::Goods().get_id(i) < 200 || Params::Goods().get_id(i) > 215) // Don't change auto-pickup settings for Estus (IDs 200-215)
            {
                Params::Goods().get(i)->isAutoEquip = false;
            }
        }
    }
    if (!silent)
    {
        print_console(std::string(enabled ? "Disabled" : "Enabled") + " item auto-equip on pickup");
    }
}


// Check if dim lava mod is currently active
bool Game::dim_lava_enabled()
{
    if (Game::lava_luminosity == NULL)
        Game::lava_luminosity = (uint8_t*)aob_scan("66 66 26 40 00 00 80 3F 00 00 80 3F 00 00 80 3F 00 00 80 3F 00 00 80 40");

    if (Game::lava_luminosity == NULL)
    {
        // Unable to find lava brightness effects in memory
        set_error(ERROR_FILE_NOT_FOUND);
        return false;
    }

    uint8_t check_lava[4] = { 0x66, 0x66, 0x26, 0x40 };
    for (int i = 0; i < 4; i++)
        if (check_lava[i] != Game::lava_luminosity[i])
            return true;

    return false;
}

// Dim lava effects or restore default lava visuals
void Game::enable_dim_lava(bool dim)
{
    if (dim != Game::dim_lava_enabled() && Game::lava_luminosity != NULL)
    {
        uint8_t patch_bytes[4];

        if (dim)
        {
            // Dim lava effects
            patch_bytes[0] = 0xD6, patch_bytes[1] = 0xCC, patch_bytes[2] = 0x4C, patch_bytes[3] = 0x3E;
            apply_byte_patch(Game::lava_luminosity, patch_bytes, 4);

            patch_bytes[0] = 0x00, patch_bytes[0] = 0x00, patch_bytes[0] = 0x80, patch_bytes[0] = 0x3F;
            apply_byte_patch(Game::lava_luminosity + 0xAE0, patch_bytes, 4);
            apply_byte_patch(Game::lava_luminosity + 0x1050, patch_bytes, 4);
        }
        else
        {
            // Restore default lava effects
            patch_bytes[0] = 0x66, patch_bytes[1] = 0x66, patch_bytes[2] = 0x26, patch_bytes[3] = 0x40;
            apply_byte_patch(Game::lava_luminosity, patch_bytes, 4);

            patch_bytes[0] = 0x00, patch_bytes[0] = 0x00, patch_bytes[0] = 0x30, patch_bytes[0] = 0x40;
            apply_byte_patch(Game::lava_luminosity + 0xAE0, patch_bytes, 4);

            patch_bytes[0] = 0x00, patch_bytes[0] = 0x00, patch_bytes[0] = 0x80, patch_bytes[0] = 0x40;
            apply_byte_patch(Game::lava_luminosity + 0x1050, patch_bytes, 4);
        }
    }
    else if (Game::lava_luminosity == NULL)
        set_error(ERROR_FILE_NOT_FOUND);
}


// Checks if armor sound effects are enabled
bool Game::armor_sfx_enabled()
{
    if (Params::Armor().base == NULL)
    {
        print_console("ERROR: Waiting for Armor params to load");
        return true;
    }

    ArmorParam *first_param = Params::Armor().data();

    return (first_param->defenseMaterial == 59 && first_param->defenseMaterial_Weak == 29);
}


// Toggles armor sound effecs
void Game::enable_armor_sfx(bool enable)
{
#ifndef DS1_OVERHAUL_QOL_PREVIEW
    if (Params::Armor().base == NULL)
    {
        print_console("ERROR: Waiting for Armor params to load");
        return;
    }

    // Static variable persists between function calls
    static std::vector<std::vector<uint8_t>> default_armor_sfx_values;

    // Check if default armor sound effects have been stored yet
    bool backup_defaults = default_armor_sfx_values.empty();


    for (int i = 0; i < (int)Params::Armor().param_count; i++)
    {
        // First time, store default armor sound effects
        if (backup_defaults)
            default_armor_sfx_values.push_back({
                        Params::Armor().get(i)->defenseMaterial,
                        Params::Armor().get(i)->defenseMaterial_Weak });

        if (enable)
        {
            Params::Armor().get(i)->defenseMaterial = default_armor_sfx_values.at(i).at(0);
            Params::Armor().get(i)->defenseMaterial_Weak = default_armor_sfx_values.at(i).at(1);
        }
        else
        {
            Params::Armor().get(i)->defenseMaterial = 0;
            //Params::Armor().get(i)->defenseMaterial_Weak = 0;
        }
    }
#endif // DS1_OVERHAUL_QOL_PREVIEW
}


// Returns multiplayer node count as an int (or -1 if player is not online)
int Game::get_node_count()
{
    SpPointer node_count_ptr = SpPointer((uint8_t*)ds1_base + 0xF7F77C, { 0x2C, 0x778, 0x80 });

    // Get node count
    int nodes = -1;
    node_count_ptr.read(&nodes);
    return nodes;
}


// Additional HUD elements
bool Hud::get_show_compass_radial()
{
    return !!*(uint8_t*)((uint32_t)Game::ds1_base + 0xF7851B);
}

void Hud::set_show_compass_radial(bool enable)
{
    *(uint8_t*)((uint32_t)Game::ds1_base + 0xF7851B) = enable;
}

bool Hud::get_show_compass_bar()
{
    return !!*(uint8_t*)((uint32_t)Game::ds1_base + 0xF78525);
}

void Hud::set_show_compass_bar(bool enable)
{
    *(uint8_t*)((uint32_t)Game::ds1_base + 0xF78525) = enable;
}

bool Hud::get_show_elevation_meter()
{
    return !!*(uint8_t*)((uint32_t)Game::ds1_base + 0xF78524);
}

void Hud::set_show_elevation_meter(bool enable)
{
    *(uint8_t*)((uint32_t)Game::ds1_base + 0xF78524) = enable;
}

bool Hud::get_show_node_graph()
{
    return Mod::hud_node_graph_pref;
    // return !!*(uint8_t*)((uint32_t)Game::ds1_base + 0x??);
}

void Hud::set_show_node_graph(bool enable, bool game_flag_only)
{
    if (!game_flag_only) {
        Mod::hud_node_graph_pref = enable;
    }
    SpPointer node_graph_ptr = SpPointer((uint8_t*)ds1_base + 0xF7F77C, { 0x2C, 0x778, 0x90 });
    node_graph_ptr.write((uint8_t)enable);
}


                /////////////////////////////////////////
                //////////////// PATCHES ////////////////
                /////////////////////////////////////////


// Toggles automatic game disconnection when low framerate is detected
void Game::enable_low_fps_disconnect(bool enable, const std::string& output_prefix)
{
    static uint8_t *fps_warn = NULL;
    if (!fps_warn)
    {
        // Save address for future calls to this function
        fps_warn = (uint8_t*)aob_scan("74 17 B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B"); // WARNING: this pattern returns 2 results
    }
    

    if (fps_warn)
    {
        // AoB Scan was successful
        set_mem_protection(fps_warn, 1, MEM_PROTECT_RWX);
        fps_warn -= 0x1E;

        if (enable)
        {
            print_console(output_prefix + "Enabling low FPS disconnect...");
            *fps_warn = 0x51; // Enable low FPS disconnect
        }
        else
        {
            print_console(output_prefix + "Disabling low FPS disconnect...");
            *fps_warn = 0xC3; // Disable low FPS disconnect
        }

    }
    else
    {
        print_console(output_prefix + "Failed to " + (enable ? "enable" : "disable") + " low FPS disconnect (AoB scan returned NULL)");
    }
}

// Set available pool of memory that Dark Souls allocates for itself
void Game::set_memory_limit(uint32_t mem_limit)
{
    if (mem_limit <= 0xA20000) // 0xA20000 = game default memory limit
        return;

    if (mem_limit > 0x0FFFFFFF) // Game crashes if allocation is too large (not sure of the exact upper limit, but pretty sure 0x1FFFFFFF causes a crash)
    {
        Mod::startup_messages.push_back(Mod::output_prefix + "WARNING: Specified memory pool size is too large. Allocating maximum allowed memory instead: 268435455 (0x0FFFFFFF)");
        mem_limit = 0x0FFFFFFF;
    }

    std::stringstream hex_stream;
    hex_stream << std::hex << (int)mem_limit;
    Mod::startup_messages.push_back(Mod::output_prefix + "Setting game memory allocation size to " + std::to_string(mem_limit) + " (0x" + hex_stream.str() + ")...");

    uint8_t *mem_limit_bytes = (uint8_t*)&mem_limit;
    uint8_t patch[5] = { 0x68, mem_limit_bytes[0], mem_limit_bytes[1], mem_limit_bytes[2], mem_limit_bytes[3] }; // push 0x0DA0000. The constant can be increased as desired, and represents dark souls total memory pool

    void *write_address;
    if (Game::get_game_version() == DS1_VERSION_RELEASE) {
        write_address = (uint8_t*)Game::ds1_base + 0xB8B7E5;
        apply_byte_patch(write_address, patch, 5);

        write_address = (uint8_t*)Game::ds1_base + 0x9E20;
        apply_byte_patch(write_address, patch, 5);

        write_address = (uint8_t*)Game::ds1_base + 0x9E41;
        apply_byte_patch(write_address, patch, 5);
    } else if (Game::get_game_version() == 139) {
        // Debug build
        write_address = (uint8_t*)Game::ds1_base + 0xB8F0A5;
        apply_byte_patch(write_address, patch, 5);

        write_address = (uint8_t*)Game::ds1_base + 0xA1E0;
        apply_byte_patch(write_address, patch, 5);

        write_address = (uint8_t*)Game::ds1_base + 0xA201;
        apply_byte_patch(write_address, patch, 5);
    }
}

// Checks if player is currently locked onto an enemy
bool Game::player_is_locked_on()
{
    //SpPointer lock_on_flag = SpPointer((void*)((uint32_t)Game::ds1_base + 0xEE29E8), { 0x0, 0x128 }); // Read-only
    SpPointer lock_on_flag = SpPointer((void*)((uint32_t)Game::ds1_base + 0xF7D6C8), { 0x48C, 0x32C, 0x56C, 0x2C1 });
    return !(lock_on_flag.resolve() == NULL || *((uint8_t*)lock_on_flag.resolve()) == 0);
}

// Set the current animation speed for the player character
void Game::set_current_player_animation_speed(float speed) {
    SpPointer speed_ptr = SpPointer(Game::world_char_base, { 0x28, 0x0, 0x28, 0x14, 0x64 });
    if (speed_ptr.resolve() != NULL) {
        *(float*)speed_ptr.resolve() = speed;
    }
}

// Returns current player character body animation ID (attacking, rolling, gestures, etc)
int32_t Game::get_player_body_anim_id()
{
    //SpPointer anim_id = SpPointer((void*)((uint32_t)Game::ds1_base + 0xEE29E8), { 0x0, 0xFF0 });
    SpPointer anim_id = SpPointer((void*)((uint32_t)Game::ds1_base + 0xEE29E8), { 0x10, 0x38, 0x46C, 0x60 });
    if (anim_id.resolve() == NULL) {
        return -1;
    } else {
        return *(int32_t*)anim_id.resolve();
    }
}

int32_t Game::get_player_upper_body_anim_id()
{
    SpPointer anim_id = SpPointer(Game::world_char_base, { 0x28, 0x0, 0x28, 0x2C, 0x58 });
    if (anim_id.resolve() == NULL) {
        return -1;
    }
    else {
        return *(int32_t*)anim_id.resolve();
    }
}

int32_t Game::get_player_lower_body_anim_id()
{
    SpPointer anim_id = SpPointer(Game::world_char_base, { 0x28, 0x0, 0x28, 0x2C, 0x60 });
    if (anim_id.resolve() == NULL) {
        return -1;
    }
    else {
        return *(int32_t*)anim_id.resolve();
    }
}

// Return current game time in milliseconds since the game has started
uint32_t Game::get_game_time_ms()
{
    SpPointer timer = SpPointer((void*)((uint32_t)Game::fmodex_base + 0xC42AC));
    if (timer.resolve() == NULL) {
        return 0;
    }
    else {
        return *(uint32_t*)timer.resolve();
    }
}

// Sets whether player character will automatically turn toward enemies when locked on
bool Game::allow_rotation_when_locked_on(bool allow)
{
    uint8_t new_value = allow ? 0 : 1;
    static SpPointer flag = SpPointer((void*)((uint32_t)Game::ds1_base + 0xF7D6C8), { 0x48C, 0x32C, 0x56C, 0x3F0 });
    uint8_t* flag_ptr = (uint8_t*)flag.resolve();
    if (flag_ptr == NULL) {
        return false;
    }
    *flag_ptr = new_value;
    // Return true if successful
    return true;
}


// Checks if any invaders are present
bool Game::check_invaders_present_in_world()
{
    static SpPointer player_char_type[3] = { SpPointer((void*)((uint32_t)0x12E29E8),{ 0x20, 0x70 }), SpPointer((void*)((uint32_t)0x12E29E8),{ 0x40, 0x70 }), SpPointer((void*)((uint32_t)0x12E29E8),{ 0x60, 0x70 }) };
    //static SpPointer player_team_type[3] = { SpPointer((void*)((uint32_t)0x12E29E8),{ 0x20, 0x74 }), SpPointer((void*)((uint32_t)0x12E29E8),{ 0x40, 0x74 }), SpPointer((void*)((uint32_t)0x12E29E8),{ 0x60, 0x74 }) };
    
    // Check if player is an invader
    uint32_t* pc_status = NULL;
    pc_status = reinterpret_cast<uint32_t*>(Game::player_char_status.resolve());
    if (pc_status && !(*pc_status == DS1_PLAYER_STATUS_HUMAN || *pc_status == DS1_PLAYER_STATUS_COOP || *pc_status == DS1_PLAYER_STATUS_HOLLOW))
    {
        Game::invaders_present_in_world = 1;
        return true;
    }

    // Check if other players are invaders
    for (SpPointer c : player_char_type)
    {
        uint32_t* char_type = (uint32_t*)c.resolve();
        if (char_type && (*char_type != 0 && *char_type != 1)) // Player is neither host or white phantom
        {
            Game::invaders_present_in_world = 1;
            return true;
        }
    }

    Game::invaders_present_in_world = 0;
    return false;
}


// Player's current world area ID
uint32_t Game::get_online_area_id()
{
    static SpPointer online_area(reinterpret_cast<void*>(0x12E29E8), { 0x0, 0x284 });
    uint32_t* area = static_cast<uint32_t*>(online_area.resolve());
    if (area)
    {
        return *area;
    }
    return 0;
}


// Updates stored IGT
void Game::update_play_time_str()
{
    static SpPointer igt(reinterpret_cast<void*>(0x1378700), { 0x68 });
    // Time stored in milliseconds
    uint32_t* ms = static_cast<uint32_t*>(igt.resolve());

    std::string tmp_time_str = "[IGT: ";

    if (Mod::hud_play_time_pref && ms)
    {
        unsigned long millisecs = *ms;
        // 1 hour = (1000 * 60 * 60) milliseconds
        unsigned long hours = millisecs / 3600000;
        millisecs -= (hours * 3600000);
        // 1 minute = (1000 * 60) milliseconds
        unsigned int  mins = millisecs / 60000;
        millisecs -= (mins * 60000);
        unsigned int  secs = millisecs / 1000;
        millisecs -= (secs * 1000);
        // Hundreths of seconds
        unsigned int centisecs = millisecs / 10;

        tmp_time_str += (hours < 10 ? "0" : "") + std::to_string(hours) + ":";
        tmp_time_str += (mins < 10 ? "0" : "") + std::to_string(mins) + ":" + (secs < 10 ? "0" : "") + std::to_string(secs) + ":" + (centisecs < 10 ? "0" : "") + std::to_string(centisecs) + "]  ";
    }
    else
    {
        // No game time available
        tmp_time_str += "--:--:--:--]  ";
    }
    Game::play_time_str = tmp_time_str;
}


// Returns the name of the current character
std::string Game::get_character_name()
{
    uint32_t* player_status = static_cast<uint32_t*>(Game::player_char_status.resolve());
    wchar_t* name_buff = static_cast<wchar_t*>(player_char_name.resolve());
    if (player_status && (*player_status != DS1_PLAYER_STATUS_LOADING) && name_buff)
    {
        name_buff[Game::player_char_name_max_len+1] = L'\0';
        errno_t return_error = 0;
        std::wstring w_name = name_buff;
        std::string  name;
        if (return_error = string_wide_to_mb(const_cast<wchar_t*>(w_name.c_str()), name))
        {
            // Conversion error
            print_console("ERROR: Failed to read character name (Conversion error)");
        }
        else
        {
            return name;
        }
    }
    else
    {
        print_console("ERROR: Failed to read character name (No character loaded)");
    }
    return "";
}


// Sets the name of the current character
void Game::set_character_name(std::string& name)
{
    uint32_t* player_status = static_cast<uint32_t*>(Game::player_char_status.resolve());
    wchar_t*  name_buff = static_cast<wchar_t*>(player_char_name.resolve());
    if (!(player_status && (*player_status != DS1_PLAYER_STATUS_LOADING) && name_buff))
    {
        print_console("ERROR: Failed to write character name (No character loaded)");
    }
    errno_t return_error = 0;
    std::wstring w_name;
    if (return_error = string_mb_to_wide(const_cast<char*>(name.c_str()), w_name))
    {
        // Conversion error
        print_console("ERROR: Failed to write character name (Conversion error)");
    }
    else if (!w_name.empty())
    {
        if (w_name.length() > Game::player_char_name_max_len)
        {
            print_console("WARNING: Name string too long (truncating)");
            w_name = w_name.substr(0, Game::player_char_name_max_len);
        }
        size_t illegal_char_count = 0;
        for (size_t i = 0; i <= w_name.length(); i++)
        {
            if (w_name[i] == L'#')
            {
                w_name[i] = L'_';
                illegal_char_count++;
            }
        }
        if (illegal_char_count)
        {
            print_console("WARNING: " + std::to_string(illegal_char_count)
                          + " invalid character" + (illegal_char_count > 1 ? "s were" : " was")
                          + " removed and replaced with exclamation point" + (illegal_char_count > 1 ? "s" : ""));
        }
        
        if (name_buff)
        {
            name_buff[0] = w_name[0];
            name_buff[Game::player_char_name_max_len] = L'\0';
            memset(name_buff+1, 0, sizeof(wchar_t)*Game::player_char_name_max_len);
            memcpy_s(name_buff, sizeof(wchar_t)*(Game::player_char_name_max_len+1), w_name.c_str(), sizeof(wchar_t)*w_name.length());
        }
    }
    else
    {
        print_console("ERROR: Name cannot be empty");
    }
}


// Allow effect IDs to be transferred between clients without bounds restrictions
void Game::unrestrict_network_synced_effectids()
{
    Mod::startup_messages.push_back(Mod::output_prefix + "Unrestricting effectIDs sent over network.");

    uint8_t nop_patch[3] = { 0x90, 0x90, 0x90 };
    void *write_address = (uint8_t*)Game::ds1_base + 0xA3EC84;
    apply_byte_patch(write_address, nop_patch, 3);

    write_address = (uint8_t*)Game::ds1_base + 0xA3EC8D;
    apply_byte_patch(write_address, nop_patch, 3);
}

static const uint32_t HEALTHBAR_SIZE = 0x011E89E0;
const float new_hpbar_max = 2300;

// Fix the bug where the player HP could be greater than the displayed GUI bar
void Game::increase_gui_hpbar_max()
{
    apply_byte_patch((void*)HEALTHBAR_SIZE, &new_hpbar_max, 4);
}

// Runs code specific to the Debug build of Dark Souls
void Game::run_debug_tasks()
{
    Game::set_memory_limit(Game::memory_limit);
    XInput::apply_function_intercepts();
}
