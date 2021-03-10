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
#include "AnimationEdits.h"
#include "SP/memory/injection/asm/x64.h"


/*
    Initialize static variables:
*/

// Base address of Dark Souls game process
uint64_t Game::ds1_base = NULL;

// Base address of fmod_event64 dll process
uint64_t Game::fmod_event64_base = NULL;

// Base address for player character data
uint64_t Game::player_char_base = NULL;

// Base address for character class data
uint64_t Game::char_class_base = NULL;

uint64_t Game::frpg_net_base = NULL;

uint64_t Game::game_data_man = NULL;

uint64_t Game::world_chr_man_imp = NULL;

uint64_t Game::param_man = NULL;

uint64_t Game::solo_param_man = NULL;

uint64_t Game::file_man = NULL;

uint64_t Game::session_man_imp = NULL;

// Player character status (loading, human, co-op, invader, hollow)
sp::mem::pointer<int32_t> Game::player_char_status;

// Marker for if we're currently in a loading screen
sp::mem::pointer<int32_t> Game::is_loading;

uint64_t Game::player_animation_mediator = NULL;

extern "C" {
    uint64_t* player_animation_mediator_cptr;
    uint64_t player_animation_mediator_loading_injection_return;
    void player_animation_mediator_loading_injection();

    uint32_t last_attack_weaponid;
    uint64_t calculate_attack_damage_injection_return;
    void calculate_attack_damage_injection();
}

// Flag to determine if any characters have been loaded since the game was launched (useful if player had a character loaded but returned to main menu)
bool Game::characters_loaded = false;

// Address of lava brightness effect (used for dimming lava)
uint8_t *Game::lava_luminosity = NULL;

// Game saving on/off
sp::mem::pointer<uint8_t> Game::saves_enabled;

// Multiplayer node count
int Game::node_count = -1;


// Initializes pointers and base addresses required for most other functions
void Game::init()
{
    global::cmd_out << "Initializing pointers...\n";

    Game::ds1_base = (uint64_t)sp::mem::get_process_base();

    Game::fmod_event64_base = (uint64_t)GetModuleHandle("fmod_event64.dll");

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

    // Note this is not actually the real pointer to the loading screen byte. Just a best guess
    Game::is_loading = sp::mem::pointer<int32_t>((void*)(Game::ds1_base+0x1ACD758), { 0x28, 0x250, 0x2F8 });

    //hook the code that init's the player's animation mediator so we know it's location later
    player_animation_mediator_cptr = &Game::player_animation_mediator;
    //uint8_t* write_address = (uint8_t*)(Game::player_animation_mediator_loading + Game::ds1_base);
    //sp::mem::code::x64::inject_jmp_14b(write_address, &player_animation_mediator_loading_injection_return, 1, &player_animation_mediator_loading_injection);

    Game::game_data_man = Game::ds1_base + 0x1D278F0;

    Game::world_chr_man_imp = Game::ds1_base + 0x1d151b0;

    Game::param_man = Game::ds1_base + 0x1d1b098;

    Game::solo_param_man = Game::ds1_base + 0x1d1b360;

    Game::file_man = Game::ds1_base + 0x1d1e4f8;

    Game::session_man_imp = Game::ds1_base + 0x1d1a370;

    //hook the code that calculates attack damage and save off the weapon id used for the attack
    last_attack_weaponid = -1;
    uint8_t* write_address = (uint8_t*)(Game::calculate_attack_damage_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &calculate_attack_damage_injection_return, 1, &calculate_attack_damage_injection);
}

// Performs tasks that were deferred until a character was loaded
void Game::on_first_character_loaded()
{
    Game::characters_loaded = true;

    Game::preload_function_caches();

    // Enable rally system vfx
    BloodborneRally::on_char_load();

    global::cmd_out << Mod::output_prefix + "All character loading finished!\n";
}


static bool resolve_current_player_animation_speed();

/*
 * Help speedup some functions by, whenever we're loaded into an area,
 * preload/preresolve some pointers and values so they can be much more quickly read when we need them
 * This function should be called whenever an area is loaded (after player status changes from loading)
 * This also refreshes the cache on area load so it doesn't get stale
*/

static uint64_t* pc_entity_ptr = NULL;
static float* pc_position_ptr = NULL;
static uint32_t* time_address = NULL;
static uint32_t* left_hand_weapon_ptr_cache = NULL;
static uint32_t* right_hand_weapon_ptr_cache = NULL;
static uint32_t* player_char_max_hp_cache = NULL;
static int32_t* player_body_anim_id_cache = NULL;
static int32_t* player_upper_body_anim_id_cache = NULL;
static int32_t* player_lower_body_anim_id_cache = NULL;
static float* set_current_player_animation_speed_cache = NULL;
static int32_t* area_id_cache = NULL;
static int32_t* mp_id_cache = NULL;
static int32_t* saved_chars_menu_flag_cache = NULL;
static uint8_t* saved_chars_preview_data_cache = NULL;
static uint32_t* pc_playernum_cache = NULL;
static uint64_t connected_players_array_cache = NULL;
static void** pc_EzStateMachineImpl_cache = NULL;

void Game::preload_function_caches() {
    global::cmd_out << "Cache loading\n";

    pc_entity_ptr = NULL;
    Game::get_pc_entity_pointer();
    time_address = NULL;
    Game::get_game_time_ms();
    pc_position_ptr = NULL;
    Game::get_pc_position();
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
    area_id_cache = NULL;
    Game::get_area_id();
    mp_id_cache = NULL;
    Game::get_mp_id_ptr();
    saved_chars_menu_flag_cache = NULL;
    Game::get_saved_chars_menu_flag();
    saved_chars_preview_data_cache = NULL;
    Game::get_saved_chars_preview_data();
    pc_playernum_cache = NULL;
    Game::get_pc_playernum();
    connected_players_array_cache = NULL;
    Game::get_connected_player(0);
    pc_EzStateMachineImpl_cache = NULL;
    Game::get_pc_ActiveState_EzStateMachineImpl();


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

static const uint64_t disable_low_fps_disconnect_offset = 0x778B29;

// Disables automatic game disconnection when low framerate is detected
void Game::disable_low_fps_disconnect(bool enable)
{
    uint8_t *fps_warn = (uint8_t *)(Game::ds1_base + disable_low_fps_disconnect_offset);

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
    global::cmd_out << Mod::output_prefix << "Increasing game memory allocation size.\n";

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
bool Game::set_current_player_animation_speed(float speed) {
    if (set_current_player_animation_speed_cache) {
        *set_current_player_animation_speed_cache = speed;
    }

    if (!resolve_current_player_animation_speed()) {
        return false;
    }
    *set_current_player_animation_speed_cache = speed;
    return true;
}

// Returns current player character body animation ID (attacking, rolling, gestures, etc)
std::optional<int32_t> Game::get_player_body_anim_id()
{
    if (player_body_anim_id_cache) {
        return *player_body_anim_id_cache;
    }

    sp::mem::pointer anim_id = sp::mem::pointer<int32_t>((void*)Game::world_char_base, { 0x68, 0x68, 0x48, 0x80 });
    if (anim_id.resolve() == NULL) {
        return std::nullopt;
    } else {
        player_body_anim_id_cache = (int32_t*)anim_id.resolve();
        return *player_body_anim_id_cache;
    }
}


std::optional<int32_t> Game::get_player_upper_body_anim_id()
{
    if (player_upper_body_anim_id_cache) {
        return *player_upper_body_anim_id_cache;
    }

    sp::mem::pointer anim_id = sp::mem::pointer<int32_t>((void*)Game::world_char_base, { 0x68, 0x30, 0x5D0, 0x690 });
    if (anim_id.resolve() == NULL) {
        return std::nullopt;
    }
    else {
        player_upper_body_anim_id_cache = (int32_t*)anim_id.resolve();
        return *player_upper_body_anim_id_cache;
    }
}


std::optional<int32_t> Game::get_player_lower_body_anim_id()
{
    if (player_lower_body_anim_id_cache) {
        return *player_lower_body_anim_id_cache;
    }

    sp::mem::pointer anim_id = sp::mem::pointer<int32_t>((void*)Game::world_char_base, { 0x68, 0x30, 0x5D0, 0x13B0 });
    if (anim_id.resolve() == NULL) {
        return std::nullopt;
    }
    else {
        player_lower_body_anim_id_cache = (int32_t*)anim_id.resolve();
        return *player_lower_body_anim_id_cache;
    }
}


std::optional<int32_t> Game::get_animation_mediator_state_animation(void* animationMediator, AnimationStateTypesEnum state_id) {
    void* state_entry = (void*)((uint64_t)animationMediator + 168 * state_id);
    return *(int32_t*)((uint64_t)state_entry + 0);
}

void Game::set_animation_mediator_state_entry(void* animationMediator, AnimationStateTypesEnum state_id, int32_t new_aid, float new_progressTime) {
    void* state_entry = (void*)((uint64_t)animationMediator + 168 * state_id);
    *(int32_t*)((uint64_t)state_entry + 0) = new_aid;
    *(float*)((uint64_t)state_entry + 0xa4) = new_progressTime;
}

// Return pointer to current game time in milliseconds since the game has started
std::optional<uint32_t*> Game::get_game_time_ms()
{
    //quick resolve
    if (time_address) {
        return time_address;
    }
    //first time resolve
    sp::mem::pointer timer = sp::mem::pointer<uint32_t>((void*)((uint64_t)Game::fmod_event64_base + 0x00077278), { 0x470, 0x40, 0x8C });
    if (timer.resolve() == NULL) {
        return std::nullopt;
    }
    else {
        time_address = timer.resolve();
        return time_address;
    }
}

std::optional<uint64_t> Game::get_pc_entity_pointer() {
    //quick resolve
    if (pc_entity_ptr) {
        return *pc_entity_ptr;
    }

    sp::mem::pointer entity_ptr = sp::mem::pointer<uint64_t>((void*)(Game::world_char_base), { 0x68 });
    if (entity_ptr.resolve() == NULL) {
        return std::nullopt;
    }
    else {
        pc_entity_ptr = entity_ptr.resolve();
        return *pc_entity_ptr;
    }
}

std::optional<float*> Game::get_pc_position() {
    if (pc_position_ptr) {
        return pc_position_ptr;
    }

    sp::mem::pointer position_ptr = sp::mem::pointer<float>((void*)(Game::world_char_base), { 0x68, 0x68, 0x28, 0x10 });
    if (position_ptr.resolve() == NULL) {
        return std::nullopt;
    }
    else {
        pc_position_ptr = position_ptr.resolve();
        return pc_position_ptr;
    }
}

static const uint64_t unrestrict_network_synced_effectids_offset = 0x36FBDD;

// Allow effect IDs to be transferred between clients without bounds restrictions
void Game::unrestrict_network_synced_effectids()
{
    global::cmd_out << (Mod::output_prefix + "Unrestricting effectIDs sent over network.\n");

    uint8_t nop_patch[3] = { 0x90, 0x90, 0x90 };

    //Instructions that shift left and right to limit effect id range
    void *write_address = (void*)(Game::ds1_base + unrestrict_network_synced_effectids_offset);

    sp::mem::patch_bytes(write_address, nop_patch, 3);
    sp::mem::patch_bytes((void*)((uint64_t)write_address+0xA), nop_patch, 3);
}

float Game::new_hpbar_max = 2633.0;
static const uint64_t gui_hpbar_value_offset = 0x676ECD;

// Fix the bug where the player HP could be greater than the displayed GUI bar
void Game::increase_gui_hpbar_max()
{
    global::cmd_out << (Mod::output_prefix + "Fixing hp bar.\n");

    //Instruction that loads the immediate float (+6 for immediate location in opcode)
    void *write_address = (void*)(Game::ds1_base + gui_hpbar_value_offset);
    sp::mem::patch_bytes((void*)((uint64_t)write_address+6), (uint8_t*)&new_hpbar_max, 4);
}


std::optional<uint32_t> Game::left_hand_weapon() {
    if (left_hand_weapon_ptr_cache) {
        return *left_hand_weapon_ptr_cache;
    }

    sp::mem::pointer weapon = sp::mem::pointer<uint32_t>((void*)(Game::ds1_base + 0x1ACD758), { 0x28, 0x250, 0x2F8, 0x18, 0x0 });
    if (weapon.resolve() == NULL) {
        return std::nullopt;
    }
    else {
        left_hand_weapon_ptr_cache = (uint32_t*)weapon.resolve();
        return *left_hand_weapon_ptr_cache;
    }
}


std::optional<uint32_t> Game::right_hand_weapon() {
    if (right_hand_weapon_ptr_cache) {
        return *right_hand_weapon_ptr_cache;
    }

    sp::mem::pointer weapon = sp::mem::pointer<uint32_t>((void*)(Game::ds1_base+0x1ACD758), { 0x28, 0x250, 0x2F8, 0x18, 0x4 });
    if (weapon.resolve() == NULL) {
        return std::nullopt;
    }
    else {
        right_hand_weapon_ptr_cache = (uint32_t*)weapon.resolve();
        return *right_hand_weapon_ptr_cache;
    }
}

// Note we can't cache this because we rely on it to check cache staleness
int32_t Game::get_player_char_status() {
    int32_t* char_status_ptr = Game::player_char_status.resolve();
    int32_t* is_loading = Game::is_loading.resolve();

    if (char_status_ptr == NULL || is_loading == NULL) {
        return DS1_PLAYER_STATUS_LOADING;
    }
    else {
        return *char_status_ptr;
    }
}


std::optional<uint32_t> Game::get_player_char_max_hp() {
    if (player_char_max_hp_cache) {
        return *player_char_max_hp_cache;
    }

    sp::mem::pointer maxhp = sp::mem::pointer<uint32_t>((void*)(Game::world_char_base), { 0x68, 0x3EC });
    if (maxhp.resolve() == NULL) {
        return std::nullopt;
    }
    else {
        player_char_max_hp_cache = (uint32_t*)maxhp.resolve();
        return *player_char_max_hp_cache;
    }
}

//Returns a value between -PI and PI
std::optional<float> Game::get_entity_rotation(void* entity_ptr) {
    sp::mem::pointer rotation = sp::mem::pointer<float>((void*)((uint64_t)entity_ptr + 0x68), { 0x28, 0x4 });
    if (rotation.fast_resolve() == NULL) {
        return std::nullopt;
    }
    else {
        return *(float*)rotation.fast_resolve();
    }
}

std::optional<int32_t> Game::get_area_id() {
    if (area_id_cache) {
        return (*area_id_cache & 0x0000ffff);
    }

    sp::mem::pointer area_id = sp::mem::pointer<int32_t>((void*)(Game::frpg_net_base), { 0xA22 });
    if (area_id.resolve() == NULL) {
        return std::nullopt;
    }
    else {
        area_id_cache = area_id.resolve();
        return (*area_id_cache & 0x0000ffff);
    }
}

std::optional<int32_t*> Game::get_mp_id_ptr() {
    if (mp_id_cache) {
        return mp_id_cache;
    }

    sp::mem::pointer mp_id = sp::mem::pointer<int32_t>((void*)(Game::world_char_base), { 0x68, 0x354 });
    if (mp_id.resolve() == NULL) {
        return std::nullopt;
    }
    else {
        mp_id_cache = mp_id.resolve();
        return mp_id_cache;
    }
}

std::optional<int32_t*> Game::get_saved_chars_menu_flag() {
    if (saved_chars_menu_flag_cache) {
        return saved_chars_menu_flag_cache;
    }

    sp::mem::pointer saved_chars_menu_flag = sp::mem::pointer<int32_t>((void*)(Game::ds1_base + 0x1D26168), { 0xA4 });
    if (saved_chars_menu_flag.resolve() == NULL) {
        return std::nullopt;
    }
    else {
        saved_chars_menu_flag_cache = saved_chars_menu_flag.resolve();
        return saved_chars_menu_flag_cache;
    }
}

std::optional<uint8_t*> Game::get_saved_chars_preview_data() {
    if (saved_chars_preview_data_cache) {
        return saved_chars_preview_data_cache;
    }

    sp::mem::pointer saved_chars_preview_data = sp::mem::pointer<uint8_t>((void*)(Game::ds1_base + 0x1D278F0), { 0x60, 0x10 });
    if (saved_chars_preview_data.resolve() == NULL) {
        return std::nullopt;
    }
    else {
        saved_chars_preview_data_cache = saved_chars_preview_data.resolve();
        return saved_chars_preview_data_cache;
    }
}

std::optional<uint32_t> Game::get_pc_playernum() {
    if (pc_playernum_cache) {
        return *pc_playernum_cache;
    }

    sp::mem::pointer pc_playernum = sp::mem::pointer<uint32_t>((void*)(Game::game_data_man), { 0x10, 0x10 });
    if (pc_playernum.resolve() == NULL) {
        return std::nullopt;
    }
    else {
        pc_playernum_cache = pc_playernum.resolve();
        return *pc_playernum_cache;
    }
}

std::optional<uint64_t> Game::get_connected_player(uint32_t i) {
    if (connected_players_array_cache) {
        return *(uint64_t*)(connected_players_array_cache + (0x38 * (i + 1)));
    }

    sp::mem::pointer connected_players_array = sp::mem::pointer<uint64_t>((void*)(Game::world_chr_man_imp), { 0x68, 0x18 });
    if (connected_players_array.resolve() == NULL) {
        return std::nullopt;
    }
    else {
        connected_players_array_cache = *(uint64_t*)connected_players_array.resolve();
        return *(uint64_t*)(connected_players_array_cache + (0x38 * (i + 1)));
    }
}

std::optional<int32_t> Game::convert_handle_to_playernum(uint32_t handle) {
    //PC specific handle
    if (handle == PC_Handle) {
        return Game::get_pc_playernum();
    }
    else {
        //loop through the conncted players and look for matching handle
        for (int i = 0; i < 5; i++) {
            std::optional<uint64_t> guest_o = Game::get_connected_player(i);
            if (guest_o.has_value()) {
                uint64_t guest = guest_o.value();
                uint32_t guestHandle = *(uint32_t*)(guest + 8);
                if (guestHandle == handle) {
                    return *(uint32_t*)((*(uint64_t*)(guest + 0x578)) + 0x10);
                }
            }
            else
            {
                return std::nullopt;
            }
        }
        return -1;
    }
}

std::optional<uint32_t> Game::convert_playernum_to_handle(uint32_t playernum) {
    if (playernum == Game::get_pc_playernum()) {
        return PC_Handle; //const handle for PC
    }
    else {
        //loop through the conncted players and look for matching playernum
        for (int i = 0; i < 5; i++) {
            std::optional<uint64_t> guest_o = Game::get_connected_player(i);
            if (guest_o.has_value()) {
                uint64_t guest = guest_o.value();
                uint32_t guestNo = *(uint32_t*)((*(uint64_t*)(guest + 0x578)) + 0x10);
                if (guestNo == playernum) {
                    return *(uint32_t*)(guest + 8);
                }
            }
            else
            {
                return std::nullopt;
            }
        }
        return 0;
    }
}

uint32_t Game::get_last_attack_weapon_id() {
    return last_attack_weaponid;
}

std::optional<void*> Game::get_pc_ActiveState_EzStateMachineImpl() {
    if (pc_EzStateMachineImpl_cache) {
        return *pc_EzStateMachineImpl_cache;
    }

    //WorldChrManImp -> PlayerIns -> ChrIns -> PlayerCtrl -> ChrCtrl -> ActionCtrl -> ActiveState -> EzStateMachineImpl
    sp::mem::pointer pc_EzStateMachineImpl = sp::mem::pointer<void*>((void*)(Game::world_chr_man_imp), { 0x68, 8+0x60, 0x48, 0x30+(0x20*1) });
    if (pc_EzStateMachineImpl.resolve() == NULL) {
        return std::nullopt;
    }
    else {
        pc_EzStateMachineImpl_cache = pc_EzStateMachineImpl.resolve();
        return *pc_EzStateMachineImpl_cache;
    }
}

std::optional<uint64_t> Game::get_EzStateMachineImpl_curstate_id(void* EzStateMachineImpl) {
    void* ezstate_state = *(void**)((uint64_t)EzStateMachineImpl + 0x20);
    return *(uint64_t*)((uint64_t)ezstate_state + 0);
}

std::optional<SessionActionResultEnum> Game::get_SessionManagerImp_session_action_result()
{
    return static_cast<SessionActionResultEnum>(*(*(uint32_t**)Game::session_man_imp) + 0xf8);
}
