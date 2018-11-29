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

#include "GameData.h"
#include "AntiCheat.h"
#include "BloodborneRallySystem.h"
#include "Challenge/AggressiveAi.h"
#include "Challenge/BlackPhantomEnemies.h"
#include "PhantomUnshackle.h"
#include "MoveWhileCasting.h"
#include "AnimationEdits.h"


/*
    Initialize static variables:
*/

// Base address of Dark Souls game process
uint64_t Game::ds1_base = NULL;

// Base address of fmod_event64 dll process
uint64_t Game::fmod_event64_base = NULL;

// Base address for player character data
uint64_t Game::player_char_base = NULL;

// Base address for world character data
uint64_t Game::world_char_base = NULL;

// Base address for character class data
uint64_t Game::char_class_base = NULL;

uint64_t Game::frpg_net_base = NULL;

// Player character status (loading, human, co-op, invader, hollow)
sp::mem::pointer<int32_t> Game::player_char_status;

// Time Action Events for the player character's animations
void* Game::player_tae = NULL;

// Flag to determine if any characters have been loaded since the game was launched (useful if player had a character loaded but returned to main menu)
bool Game::characters_loaded = false;

// Address of lava brightness effect (used for dimming lava)
uint8_t *Game::lava_luminosity = NULL;

// Game saving on/off
sp::mem::pointer<uint8_t> Game::saves_enabled;

// Multiplayer node count
int Game::node_count = -1;

// Strutures for tracking file I/O data for the game's BDT, BHD5, and SL2 files
Files::IoMonitor Files::io_monitors[9];


// Initializes pointers and base addreWsses required for most other functions
void Game::init()
{
    global::cmd_out << "Initializing pointers...\n";

    Game::ds1_base = (uint64_t)sp::mem::get_process_base();

    Game::fmod_event64_base = (uint64_t)GetModuleHandle("fmod_event64.dll");

    // Base addr for world character data
    void* world_char_base_sp = sp::mem::aob_scan("48 8B 05 ? ? ? ? 48 8B 48 68 48 85 C9 0F 84 ? ? ? ? 48 39 5E 10 0F 84 ? ? ? ? 48");
    if (world_char_base_sp == NULL) {
        FATALERROR("world_char_base_sp is null");
    }
    Game::world_char_base = ((uint64_t)world_char_base_sp + *(uint32_t*)((uint64_t)world_char_base_sp + 3) + 7);
    //XXX: crappy heuristic for checking if we got a valid ptr (game main has been entered)
    if (Game::world_char_base > Game::ds1_base*1.5) {
        FATALERROR("world_char_base_sp is an invalid pointer");
    }

    // Base addr for character class data
    void* char_class_base_sp = sp::mem::aob_scan("48 8B 05 ? ? ? ? 48 85 C0 ? ? F3 0F 58 80 AC 00 00 00");
    if (char_class_base_sp == NULL) {
        FATALERROR("char_class_base_sp is null");
    }
    Game::char_class_base = ((uint64_t)char_class_base_sp + *(uint32_t*)((uint64_t)char_class_base_sp + 3) + 7);
    if (Game::char_class_base > Game::ds1_base*1.5) {
        FATALERROR("char_class_base_sp is an invalid pointer");
    }

    void* frpg_net_base_sp = sp::mem::aob_scan("48 8B 1D ? ? ? ? 48 8D 94 24 ? ? ? ? 4C 8B F1 0F 29 7C 24 40");
    if (char_class_base_sp == NULL) {
        FATALERROR("frpg_net_base_sp is null");
    }
    Game::frpg_net_base = ((uint64_t)frpg_net_base_sp + *(uint32_t*)((uint64_t)frpg_net_base_sp + 3) + 7);
    if (Game::frpg_net_base > Game::ds1_base*1.5) {
        FATALERROR("frpg_net_base_sp is an invalid pointer");
    }

    // Game saving on/off
    void* saves_enabled_sp = sp::mem::aob_scan("48 8B 05 xx xx xx xx 0F 28 01 66 0F 7F 80 xx xx 00 00 C6 80");
    if (saves_enabled_sp == NULL) {
        FATALERROR("saves_enabled_sp is null");
    }

    Game::saves_enabled = sp::mem::pointer<uint8_t>((void*)((uint64_t)saves_enabled_sp + *(uint32_t*)((uint64_t)saves_enabled_sp + 3) + 7), { 0xB70 });

    Game::player_char_status = sp::mem::pointer<int32_t>((void*)(Game::world_char_base), { 0x68, 0xD4 });
}


// Performs tasks that were deferred until a character was loaded
void Game::on_first_character_loaded()
{
    Game::characters_loaded = true;

    Game::preload_function_caches();

    global::cmd_out << Mod::output_prefix + "Searching memory for files...\n";
    // Initialize param files
    //TODO Params::init();

    // Disable armor sounds if it was specified in the config file
    //if (Mod::disable_armor_sfx_pref)
        //Game::enable_armor_sfx(false);

    //if (AnimationEdits::gesture_cancelling) {
        // Perform TAE edits to player animations to enable gesture cancelling
        //TODO Game::player_tae.init_from_aob_scan("54 41 45 20 00 00 00 00 0B 00 01 00 AC AE 09 00");
        //global::cmd_out << "    Found Time Action Event file for player character animations. Enabling gesture cancelling...\n";
        //AnimationEdits::enable_gesture_cancelling();
    //}

    //if (!Mod::legacy_mode)
    {
        // Enable rally system vfx
        BloodborneRally::on_char_load();

        // Apply phantom unshackle patch
        //PhantomUnshackle::start();

        //Allow movement during spells
        CastingMovement::on_char_load();
    }

    // Enable forced binoculars/dragonification PvP protections
    //if (AntiCheat::BinocsTriggerBlock::active)
        //AntiCheat::BinocsTriggerBlock::enable();
    //if (AntiCheat::DragonTriggerBlock::active)
        //AntiCheat::DragonTriggerBlock::enable();

    // Enable challenge mods
    //if ((int)GetPrivateProfileInt(_DS1_OVERHAUL_CHALLENGE_SECTION_, _DS1_OVERHAUL_PREF_CM_AGGRO_AI_, Challenge::AggressiveAi::active(), _DS1_OVERHAUL_SETTINGS_FILE_) != 0) {
        //Challenge::AggressiveAi::enable();
    //}
    //if ((int)GetPrivateProfileInt(_DS1_OVERHAUL_CHALLENGE_SECTION_, _DS1_OVERHAUL_PREF_CM_BP_ENEMIES_, Challenge::BlackPhantomEnemies::active, _DS1_OVERHAUL_SETTINGS_FILE_) != 0) {
        //Challenge::BlackPhantomEnemies::enable();
    //}
}


static bool resolve_current_player_animation_speed();

/*
 * Help speedup some functions by, whenever we're loaded into an area,
 * preload/preresolve some pointers and values so they can be much more quickly read when we need them
 * This function should be called whenever an area is loaded (after player status changes from loading)
 * This also refreshes the cache on area load so it doesn't get stale
*/
static bool one_time_only_caches = false; //some caches only need to be preloaded once per game start

static uint64_t* pc_entity_ptr = NULL;
static uint32_t* time_address = NULL;
static uint32_t* left_hand_weapon_ptr_cache = NULL;
static uint32_t* right_hand_weapon_ptr_cache = NULL;
static uint32_t* player_char_max_hp_cache = NULL;
static int32_t* player_body_anim_id_cache = NULL;
static int32_t* player_upper_body_anim_id_cache = NULL;
static int32_t* player_lower_body_anim_id_cache = NULL;
static float* set_current_player_animation_speed_cache = NULL;

void Game::preload_function_caches() {

    if (!one_time_only_caches) {
        pc_entity_ptr = NULL;
        Game::get_pc_entity_pointer();
        time_address = NULL;
        Game::get_game_time_ms();
        one_time_only_caches = true;
    }

    left_hand_weapon_ptr_cache = NULL;
    Game::left_hand_weapon();
    right_hand_weapon_ptr_cache = NULL;
    Game::right_hand_weapon();
    player_char_max_hp_cache = NULL;
    Game::get_player_char_max_hp();
    player_body_anim_id_cache = NULL;
    Game::get_player_body_anim_id();
    player_upper_body_anim_id_cache = NULL;
    Game::get_player_upper_body_anim_id();
    player_lower_body_anim_id_cache = NULL;
    Game::get_player_lower_body_anim_id();
    Sleep(10);
    //this pointer is a bit late to resolve on load
    set_current_player_animation_speed_cache = NULL;
    uint_fast8_t i;
    for(i=0;i<16;i++) {
        if (resolve_current_player_animation_speed()) break;
        Sleep(1);
    }
    if (i>=16) FATALERROR("Unable to set_current_player_animation_speed.");
}

// Performs tasks that must be rerun after any loading screen
void Game::on_reloaded() {
    preload_function_caches();

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
    //TODO
    return 0;
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

    //TODO
}

// Check if dim lava mod is currently active
#if 0
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
#endif

// Checks if armor sound effects are enabled
#if 0
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
#endif

// Returns multiplayer node count as an int (or -1 if player is not online)
int Game::get_node_count()
{
    sp::mem::pointer node_count_ptr = sp::mem::pointer<int>((void*)Game::frpg_net_base, { 0xAF0 });

    if (node_count_ptr.resolve() == NULL) {
        return -1;
    }

    return *node_count_ptr.resolve();
}

// Additional HUD elements
#if 0
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
    sp::mem::pointer node_graph_ptr = sp::mem::pointer<uint8_t>((uint8_t*)ds1_base + 0xF7F77C, { 0x2C, 0x778, 0x90 });
    node_graph_ptr.write((uint8_t)enable);
}
#endif

// Disables automatic game disconnection when low framerate is detected
void Game::enable_low_fps_disconnect(bool enable)
{
    uint8_t *fps_warn = NULL;
    fps_warn = (uint8_t *)sp::mem::aob_scan("75 0D 84 C0 74 05");

    if (fps_warn)
    {
        // AoB Scan was successful
        sp::mem::set_protection(fps_warn, 1, MEM_PROTECT_RWX);

        if (enable)
        {
            global::cmd_out << Mod::output_prefix + "Enabling low FPS disconnect...\n";
            *fps_warn = 0xEB;
            *(fps_warn+1) = 0x5;
        }
        else
        {
            global::cmd_out << Mod::output_prefix + "Disabling low FPS disconnect...\n";
            *fps_warn = 0x75;
            *(fps_warn + 1) = 0xD;
        }
    }
    else {
        global::cmd_out << Mod::output_prefix + "!!ERROR!! Enabling low FPS disconnect...\n";
    }
}

static void set_memory_limit_chunk(uint64_t default_chunk_size, uint64_t new_chunk_size)
{
    //what to scan for
    uint8_t *orig_mem_limit_bytes = (uint8_t*)&default_chunk_size;
    uint8_t aob1[] = { 0xB9, orig_mem_limit_bytes[0], orig_mem_limit_bytes[1], orig_mem_limit_bytes[2], orig_mem_limit_bytes[3], 0xE8 };
    uint8_t aob2[] = { 0x41, 0xB8, orig_mem_limit_bytes[0], orig_mem_limit_bytes[1], orig_mem_limit_bytes[2], orig_mem_limit_bytes[3], 0x48, 0x8B, 0xD3 };

    //what to replace with
    uint8_t *mem_limit_bytes = (uint8_t*)&new_chunk_size;
    uint8_t patch1[] = { 0xB9, mem_limit_bytes[0], mem_limit_bytes[1], mem_limit_bytes[2], mem_limit_bytes[3] };
    uint8_t patch2[] = { 0x41, 0xB8, mem_limit_bytes[0], mem_limit_bytes[1], mem_limit_bytes[2], mem_limit_bytes[3] };

    //immediate 1
    void *write_address = sp::mem::aob_scan(aob1, sizeof(aob1));
    if(write_address == NULL)
        FATALERROR((Mod::output_prefix + "!!ERROR!! Unable to increase game memory allocation size for aob1 of size %x").c_str(), default_chunk_size);
    if (write_address)
        sp::mem::patch_bytes(write_address, patch1, sizeof(patch1));

    //immediate 2 and 3
    write_address = sp::mem::aob_scan(aob2, sizeof(aob2));
    if (write_address == NULL)
        FATALERROR((Mod::output_prefix + "!!ERROR!! Unable to increase game memory allocation size for aob2 of size %x").c_str(), default_chunk_size);
    if (write_address)
        sp::mem::patch_bytes(write_address, patch2, sizeof(patch2));

    write_address = sp::mem::aob_scan(aob2, sizeof(aob2), 0, write_address);
    if (write_address == NULL)
        FATALERROR((Mod::output_prefix + "!!ERROR!! Unable to increase game memory allocation size for aob2 of size %x").c_str(), default_chunk_size);
    if (write_address)
        sp::mem::patch_bytes(write_address, patch2, sizeof(patch2));
}

// Set available pool of memory that Dark Souls allocates for itself
void Game::set_memory_limit()
{
    set_memory_limit_chunk(0x1C200000, 0x7C200000);
    set_memory_limit_chunk(0x500000  , 0xA00000);
    set_memory_limit_chunk(0x2000000 , 0x7800000);
    set_memory_limit_chunk(0x4000000 , 0xB000000);
    set_memory_limit_chunk(0x390000  , 0xB00000);
    set_memory_limit_chunk(0x1320000 , 0x7320000);
    set_memory_limit_chunk(0x400000  , 0xB00000);
    set_memory_limit_chunk(0x800000  , 0xF00000);
    set_memory_limit_chunk(0x380000  , 0xA80000);
    set_memory_limit_chunk(0x5280000 , 0xB280000);
    set_memory_limit_chunk(0xE0000   , 0x400000);
    set_memory_limit_chunk(0x200000  , 0x800000);

    Mod::startup_messages.push_back(Mod::output_prefix + "Increasing game memory allocation size.");
}


static bool resolve_current_player_animation_speed() {
    sp::mem::pointer speed_ptr = sp::mem::pointer<float>((void*)Game::world_char_base, { 0x68, 0x68, 0x18, 0xA8 });
    if (speed_ptr.resolve() == NULL) {
        return false;
    }

    set_current_player_animation_speed_cache = (float*)speed_ptr.resolve();
    return true;
}

// Set the current animation speed for the player character
void Game::set_current_player_animation_speed(float speed) {
    if (set_current_player_animation_speed_cache) {
        *set_current_player_animation_speed_cache = speed;
    }

    if (!resolve_current_player_animation_speed()) {
        FATALERROR("Unable to set_current_player_animation_speed.");
    }
    *set_current_player_animation_speed_cache = speed;
}

// Returns current player character body animation ID (attacking, rolling, gestures, etc)
int32_t Game::get_player_body_anim_id()
{
    if (player_body_anim_id_cache) {
        return *player_body_anim_id_cache;
    }

    sp::mem::pointer anim_id = sp::mem::pointer<int32_t>((void*)Game::world_char_base, { 0x68, 0x68, 0x48, 0x80 });
    if (anim_id.resolve() == NULL) {
        FATALERROR("Unable to get_player_body_anim_id.");
    } else {
        player_body_anim_id_cache = (int32_t*)anim_id.resolve();
        return *player_body_anim_id_cache;
    }
}


int32_t Game::get_player_upper_body_anim_id()
{
    if (player_upper_body_anim_id_cache) {
        return *player_upper_body_anim_id_cache;
    }

    sp::mem::pointer anim_id = sp::mem::pointer<int32_t>((void*)Game::world_char_base, { 0x68, 0x30, 0x5D0, 0x690 });
    if (anim_id.resolve() == NULL) {
        FATALERROR("Unable to get_player_upper_body_anim_id.");
    }
    else {
        player_upper_body_anim_id_cache = (int32_t*)anim_id.resolve();;
        return *player_upper_body_anim_id_cache;
    }
}


int32_t Game::get_player_lower_body_anim_id()
{
    if (player_lower_body_anim_id_cache) {
        return *player_lower_body_anim_id_cache;
    }

    sp::mem::pointer anim_id = sp::mem::pointer<int32_t>((void*)Game::world_char_base, { 0x68, 0x30, 0x5D0, 0x13B0 });
    if (anim_id.resolve() == NULL) {
        FATALERROR("Unable to get_player_lower_body_anim_id.");
    }
    else {
        player_lower_body_anim_id_cache = (int32_t*)anim_id.resolve();
        return *player_lower_body_anim_id_cache;
    }
}


// Return pointer to current game time in milliseconds since the game has started
uint32_t* Game::get_game_time_ms()
{
    //quick resolve
    if (time_address) {
        return time_address;
    }
    //first time resolve
    sp::mem::pointer timer = sp::mem::pointer<uint32_t>((void*)((uint64_t)Game::fmod_event64_base + 0x00077278), { 0x470, 0x40, 0x8C });
    if (timer.resolve() == NULL) {
        FATALERROR("Unable to get pointer to current time.");
        return NULL;
    }
    else {
        time_address = timer.resolve();
        return time_address;
    }
}

uint64_t Game::get_pc_entity_pointer() {
    //quick resolve
    if (pc_entity_ptr) {
        return *pc_entity_ptr;
    }

    sp::mem::pointer entity_ptr = sp::mem::pointer<uint64_t>((void*)(Game::world_char_base), { 0x68 });
    if (entity_ptr.resolve() == NULL) {
        FATALERROR("Unable to get_pc_entity_pointer.");
    }
    else {
        pc_entity_ptr = entity_ptr.resolve();
        return *pc_entity_ptr;
    }
}

// Allow effect IDs to be transferred between clients without bounds restrictions
void Game::unrestrict_network_synced_effectids()
{
    uint8_t nop_patch[3] = { 0x90, 0x90, 0x90 };

    //Instructions that shift left and right to limit effect id range
    void *write_address = sp::mem::aob_scan("c1 e2 0f 41 81 e0 ff ff ff 3f c1 fa 0f");

    if(write_address){
        global::cmd_out << (Mod::output_prefix + "Unrestricting effectIDs sent over network.\n");

        sp::mem::patch_bytes(write_address, nop_patch, 3);
        sp::mem::patch_bytes((void*)((uint64_t)write_address+0xA), nop_patch, 3);
    }
    else {
        FATALERROR((Mod::output_prefix + "!!ERROR!! Unrestricting effectIDs sent over network.").c_str());
    }
}

float Game::new_hpbar_max = 2633.0;

// Fix the bug where the player HP could be greater than the displayed GUI bar
void Game::increase_gui_hpbar_max()
{
    //Instruction that loads the immediate float (+6 for immediate location in opcode)
    void *write_address = sp::mem::aob_scan("c7 83 04 05 00 00 00 80 ed 44 c7 83 08 05 00 00 00 00 22 44");

    if (write_address) {
        global::cmd_out << (Mod::output_prefix + "Fixing hp bar.\n");

        sp::mem::patch_bytes((void*)((uint64_t)write_address+6), (uint8_t*)&new_hpbar_max, 4);
    }
    else {
        FATALERROR((Mod::output_prefix + "!!ERROR!! Fixing hp bar.\n").c_str());
    }
}


uint32_t Game::left_hand_weapon() {
    if (left_hand_weapon_ptr_cache) {
        return *left_hand_weapon_ptr_cache;
    }

    sp::mem::pointer weapon = sp::mem::pointer<uint32_t>((void*)(Game::char_class_base), { 0x10, 0x324 });
    if (weapon.resolve() == NULL) {
        FATALERROR("Unable to get left_hand_weapon.");
    }
    else {
        left_hand_weapon_ptr_cache = (uint32_t*)weapon.resolve();
        return *left_hand_weapon_ptr_cache;
    }
}


uint32_t Game::right_hand_weapon() {
    if (right_hand_weapon_ptr_cache) {
        return *right_hand_weapon_ptr_cache;
    }

    sp::mem::pointer weapon = sp::mem::pointer<uint32_t>((void*)(Game::char_class_base), { 0x10, 0x328 });
    if (weapon.resolve() == NULL) {
        FATALERROR("Unable to get right_hand_weapon.");
    }
    else {
        right_hand_weapon_ptr_cache = (uint32_t*)weapon.resolve();
        return *right_hand_weapon_ptr_cache;
    }
}

// Note we can't cache this because we rely on it to check cache staleness
int32_t Game::get_player_char_status() {
    int32_t* char_status_ptr = Game::player_char_status.resolve();

    if (char_status_ptr == NULL) {
        return DS1_PLAYER_STATUS_LOADING;
    }
    else {
        return *char_status_ptr;
    }
}


uint32_t Game::get_player_char_max_hp() {
    if (player_char_max_hp_cache) {
        return *player_char_max_hp_cache;
    }

    sp::mem::pointer maxhp = sp::mem::pointer<uint32_t>((void*)(Game::world_char_base), { 0x68, 0x3EC });
    if (maxhp.resolve() == NULL) {
        FATALERROR("Unable to get_player_char_max_hp.");
    }
    else {
        player_char_max_hp_cache = (uint32_t*)maxhp.resolve();
        return *player_char_max_hp_cache;
    }
}

//Returns a value between -PI and PI
float Game::get_entity_rotation(void* entity_ptr) {
    sp::mem::pointer rotation = sp::mem::pointer<float>((void*)((uint64_t)entity_ptr + 0x68), { 0x28, 0x4 });
    if (rotation.fast_resolve() == NULL) {
        FATALERROR("Unable to get_entity_rotation.");
    }
    else {
        return *(float*)rotation.fast_resolve();
    }
}
