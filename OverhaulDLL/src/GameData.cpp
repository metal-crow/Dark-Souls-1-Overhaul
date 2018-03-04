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


/*
    Initialize static variables:
*/

// Base address of Dark Souls game process
void *Game::ds1_base = NULL;

// Base address for player character data
void *Game::player_char_base = NULL;

// Player character status (loading, human, co-op, invader, hollow)
SpPointer Game::player_char_status;

// Time Action Events for the player character's animations
Tae Game::player_tae = Tae();

// Flag to determine if any characters have been loaded since the game was launched (useful if player had a character loaded but returned to main menu)
bool Game::characters_loaded = false;

// Address of lava brightness effect (used for dimming lava)
uint8_t *Game::lava_luminosity = NULL;

// Game saving on/off
SpPointer Game::saves_enabled;

// Multiplayer node count
int Game::node_count = -1;

// Size of the pool of memory the game allocates for itself
uint32_t Game::memory_limit = 0;

// Gesture cancelling enabled/disabled
bool Game::gesture_cancelling = true;

// Animation IDs for the default set of gesture animations in the game
const uint32_t Game::gesture_anim_ids[15] = { 6800, 6801, 6802, 6803, 6804, 6805, 6806, 6807, 6808, 6809, 6810, 6815, 6820, 6825, 6830 };

enum TAE_type0_param_values {
    cancel_by_atk = 4,
    lock_rotation = 7,
    activate_iframes = 8,
    cancel_by_moving = 11,
    cancel_by_left_atk = 16,
    cancel_by_left_block = 22,
    cancel_by_rolling_or_backstepping = 26,
    cancel_by_using_consumable = 31,
    cancel_by_two_handing = 32,
    allow_animation_cancel_events = 87,
    cap_movement_to_walk_speed = 90,
};


// Strutures for tracking file I/O data for the game's BDT, BHD5, and SL2 files
Files::IoMonitor Files::io_monitors[9];





// Initializes pointers that depend on the game's base address
void Game::init_pointers()
{
    print_console(std::string(Mod::output_prefix + "Initializing pointers...").c_str());

    // Obtain base address for player character data
    Game::player_char_base = (void*)((unsigned int)Game::ds1_base + 0xF7E204);

    // Player character status (loading, human, co-op, invader, hollow)
    Game::player_char_status = SpPointer(Game::player_char_base, { 0xA28 });

    // Game saving on/off
    Game::saves_enabled = SpPointer((void*)0x13784A0, { 0xB40 });

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

    print_console(Mod::output_prefix + "Searching memory for files...");
    // Initialize param files
    Params::init();

    // Disable armor sounds if it was specified in the config file
    if (Mod::disable_armor_sfx_pref)
        Game::enable_armor_sfx(false);

    if (Game::gesture_cancelling) {
        // Perform TAE edits to player animations to enable gesture cancelling
        void *ret_val = (void*)Game::player_tae.init_from_aob_scan("54 41 45 20 00 00 00 00 0B 00 01 00 B4 AE 09 00");
        std::string temp;
        print_console(std::string("    Found Time Action Event file for player character animations at 0x") + FileUtil::to_hex_string((int)ret_val, temp) + ". Enabling gesture cancelling...");
        Game::enable_gesture_cancelling();
    }

    if (!Mod::legacy_mode)
    {
        // Enable rally system vfx
        BloodborneRally::on_char_load();

        // Apply phantom unshackle patch
        PhantomUnshackle::start();

        //Allow movement during spells
        CastingMovement::start();
    }

    // Enable forced binoculars/dragonification PvP protections
    if (AntiCheat::BinocsTriggerBlock::active)
        AntiCheat::BinocsTriggerBlock::enable();
    if (AntiCheat::DragonTriggerBlock::active)
        AntiCheat::DragonTriggerBlock::enable();

    // Enable challenge mods
    if ((int)GetPrivateProfileInt(_DS1_OVERHAUL_CHALLENGE_SECTION_, _DS1_OVERHAUL_PREF_CM_AGGRO_AI_, Challenge::AggressiveAi::active(), _DS1_OVERHAUL_SETTINGS_FILE_) != 0) {
        Challenge::AggressiveAi::enable();
    }
    if ((int)GetPrivateProfileInt(_DS1_OVERHAUL_CHALLENGE_SECTION_, _DS1_OVERHAUL_PREF_CM_BP_ENEMIES_, Challenge::BlackPhantomEnemies::active, _DS1_OVERHAUL_SETTINGS_FILE_) != 0) {
        Challenge::BlackPhantomEnemies::enable();
    }
}


// Obtains the current game version number
uint8_t Game::get_game_version()
{
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


// Disables automatic game disconnection when low framerate is detected
void Game::enable_low_fps_disconnect(bool enable)
{
    uint8_t *fps_warn = NULL;
    fps_warn = (uint8_t*)aob_scan("74 17 B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B"); // WARNING: this pattern returns 2 results

    if (fps_warn)
    {
        // AoB Scan was successful
        set_mem_protection(fps_warn, 1, MEM_PROTECT_RWX);
        fps_warn -= 0x1E;

        if (enable)
        {
            print_console(Mod::output_prefix + "Enabling low FPS disconnect...");
            *fps_warn = 0x51; // Enable low FPS disconnect
        }
        else
        {
            print_console(Mod::output_prefix + "Disabling low FPS disconnect...");
            *fps_warn = 0xC3; // Disable low FPS disconnect
        }

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

    void *write_address = (uint8_t*)Game::ds1_base + 0xB8B7E5;
    apply_byte_patch(write_address, patch, 5);

    write_address = (uint8_t*)Game::ds1_base + 0x9E20;
    apply_byte_patch(write_address, patch, 5);

    write_address = (uint8_t*)Game::ds1_base + 0x9E41;
    apply_byte_patch(write_address, patch, 5);
}


// Enables gesture cancelling via rolling
bool Game::enable_gesture_cancelling()
{
    int gestures_changed = 0;
    if (Game::characters_loaded && Game::player_tae.is_initialized())
    {
        for (uint32_t id : gesture_anim_ids) {
            int n_events = Game::player_tae.get_event_count_by_id(id);
            for (int i = 0; i < n_events; i++) {
                bool anim_updated = false;
                if (Game::player_tae.get_event_type_by_id(id, i) == 0 &&
                    (
                        Game::player_tae.get_event_param_by_id(id, i, 0) == TAE_type0_param_values::allow_animation_cancel_events ||
                        Game::player_tae.get_event_param_by_id(id, i, 0) == TAE_type0_param_values::cancel_by_rolling_or_backstepping
                    )
                   )
                {
                    Game::player_tae.set_event_start_by_id(id, i, 0.0f);
                    //if (!print_console("Updated gesture " + std::to_string(id) + ", event " + std::to_string(i) + " to allow cancelling"))
                    //    Mod::startup_messages.push_back("Updated gesture " + std::to_string(id) + ", event " + std::to_string(i) + " to allow cancelling");
                    if (!anim_updated) {
                        anim_updated = true;
                        gestures_changed++;
                    }
                }
            }
        }
        return (gestures_changed >= 15);
    } else {
        return false;
    }
}


// Checks if player is currently locked onto an enemy
bool Game::player_is_locked_on()
{
    //SpPointer lock_on_flag = SpPointer((void*)((uint32_t)Game::ds1_base + 0xEE29E8), { 0x0, 0x128 }); // Read-only
    SpPointer lock_on_flag = SpPointer((void*)((uint32_t)Game::ds1_base + 0xF7D6C8), { 0x48C, 0x32C, 0x56C, 0x2C1 });
    return !(lock_on_flag.resolve() == NULL || *((uint8_t*)lock_on_flag.resolve()) == 0);
}


// Returns current player character body animation ID (attacking, rolling, gestures, etc)
int32_t Game::get_player_body_anim_id()
{
    SpPointer anim_id = SpPointer((void*)((uint32_t)Game::ds1_base + 0xEE29E8), { 0x0, 0xFF0 });
    if (anim_id.resolve() == NULL) {
        return -1;
    } else {
        return *(int32_t*)anim_id.resolve();
    }
}


// Sets whether player character will automatically turn toward enemies when locked on
bool Game::allow_rotation_when_locked_on(bool allow)
{
    uint8_t new_value = allow ? 0 : 1;
    SpPointer flag = SpPointer((void*)((uint32_t)Game::ds1_base + 0xF7D6C8), { 0x48C, 0x32C, 0x56C, 0x3F0 });
    flag.write(new_value);
    // Return true if successful
    return !(flag.resolve() == NULL);
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
