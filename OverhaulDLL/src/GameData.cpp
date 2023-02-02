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
#include "FileReloading.h"
#include "ModNetworking.h"
#include "CustomInvasionTypes.h"


/*
    Initialize static variables:
*/

// Base address of Dark Souls game process
uint64_t Game::ds1_base = NULL;

// Base address of fmod_event64 dll process
uint64_t Game::fmod_event64_base = NULL;

// Base address for character class data
uint64_t Game::char_class_base = NULL;

uint64_t Game::frpg_net_base = NULL;

uint64_t Game::game_data_man = NULL;

uint64_t Game::world_chr_man_imp = NULL;

uint64_t Game::param_man = NULL;

uint64_t Game::solo_param_man = NULL;

uint64_t Game::file_man = NULL;

uint64_t Game::session_man_imp = NULL;

uint64_t Game::menu_man = NULL;

uint64_t Game::bullet_man = NULL;

uint64_t Game::unknown_global_struct_141d283a8 = NULL;

uint64_t Game::frpg_net_man = NULL;

uint64_t Game::pad_man = NULL;

uint64_t Game::frpg_system = NULL;

uint64_t Game::damage_man = NULL;

uint64_t Game::frpg_havok_man_imp = NULL;

uint64_t Game::sfx_man = NULL;

uint64_t Game::throw_man = NULL;

uint64_t Game::delay_delete_man = NULL;

// Player character status (loading, human, co-op, invader, hollow)
sp::mem::pointer<int32_t> Game::player_char_status;

extern "C" {
    bool char_loaded;
    uint64_t char_loaded_injection_return;
    void char_loaded_injection();
    uint64_t char_loading_injection_return;
    void char_loading_injection();

    uint64_t gui_hpbar_max_injection_return;
    void gui_hpbar_max_injection();

    uint64_t grab_movemapstep_return;
    void grab_movemapstep_injection();
    uint64_t grab_movemapstep_value;

    uint64_t grab_thread_handle_return;
    void grab_thread_handle_injection();
    void grab_thread_handle_helper(HANDLE);

    uint64_t grab_destruct_thread_handle_return;
    void grab_destruct_thread_handle_injection();
    void grab_destruct_thread_handle_helper(HANDLE);

    uint64_t Step_PadMan_ReadInputs_return;
    bool Step_PadMan_ReadInputs_allowed = true;
    void Step_PadMan_ReadInputs_injection();
}

// Flag to determine if a character have been loaded since the game was launched (useful if player had a character loaded but returned to main menu)
bool Game::first_character_loaded = false;

// Address of lava brightness effect (used for dimming lava)
uint8_t *Game::lava_luminosity = NULL;

// Game saving on/off
sp::mem::pointer<uint8_t> Game::saves_enabled;

// Multiplayer node count
int Game::node_count = -1;


// Initializes pointers and base addresses required for most other functions
void Game::init()
{
    ConsoleWrite("Initializing pointers...");

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

    Game::game_data_man = Game::ds1_base + 0x1c8a530;

    Game::world_chr_man_imp = Game::ds1_base + 0x1c77e50;

    Game::player_char_status = sp::mem::pointer<int32_t>((void*)(Game::world_chr_man_imp), { 0x68, 0xD4 });

    Game::param_man = Game::ds1_base + 0x1c7dd38;

    Game::solo_param_man = Game::ds1_base + 0x1c7e000;

    Game::file_man = Game::ds1_base + 0x1c81198;

    Game::session_man_imp = Game::ds1_base + 0x1c7d010;

    Game::menu_man = Game::ds1_base + 0x1c88d98;

    Game::bullet_man = Game::ds1_base + 0x1c7a488;

    Game::unknown_global_struct_141d283a8 = Game::ds1_base + 0x1c8b038;

    Game::frpg_net_man = Game::ds1_base + 0x1c8a9a0;

    Game::pad_man = Game::ds1_base + 0x1c6aea0;

    Game::frpg_system = Game::ds1_base + 0x1b68e18;

    Game::damage_man = Game::ds1_base + 0x1c7a050;

    Game::frpg_havok_man_imp = Game::ds1_base + 0x1c74870;

    Game::sfx_man = Game::ds1_base + 0x1c7c758;

    Game::throw_man = Game::ds1_base + 0x1c79ad0;

    Game::delay_delete_man = Game::ds1_base + 0x1c6a8e8;
}

void Game::injections_init()
{
    uint8_t* write_address;

    //inject the code that detects if the character is loaded in or not
    char_loaded = false;
    write_address = (uint8_t*)(Game::char_loaded_injection_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &char_loaded_injection_return, 6, &char_loaded_injection);
    write_address = (uint8_t*)(Game::char_loading_injection_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &char_loading_injection_return, 1, &char_loading_injection);

    //inject the code that saves the HP Bar UI element pointer
    write_address = (uint8_t*)(Game::gui_hpbar_max_injection_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &gui_hpbar_max_injection_return, 7, &gui_hpbar_max_injection);

    //inject the code to save the movemapstep pointer (using the pointer chain from frpg_system is sometimes unreliable)
    write_address = (uint8_t*)(Game::MoveMapStep_New_injection_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &grab_movemapstep_return, 1, &grab_movemapstep_injection);

    //inject code to save the handles of DLThreads dark souls starts, so they can be paused/resumed
    write_address = (uint8_t*)(Game::InitAndStart_DLThread_injection_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &grab_thread_handle_return, 3, &grab_thread_handle_injection);
    //remove the handles when they get destroyed
    write_address = (uint8_t*)(Game::Destruct_DLThread_injection_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &grab_destruct_thread_handle_return, 1, &grab_destruct_thread_handle_injection);

    //inject code to skip the logos
    //set the state of the logo class to always be "next menu"
    uint8_t pat[] = { 0xb9, 0x03, 0x0, 0x0, 0x0, 0x90 };
    write_address = (uint8_t*)(Game::LogoSkip_offset + Game::ds1_base);
    sp::mem::patch_bytes(write_address, pat, 6);

    //inject code to skip validating the player face/hair/body data
    uint8_t Validate_Type6FaceData_patch[] = { 0xB0, 0x1, 0x90, 0x90, 0x90 }; //mov al, 1
    write_address = (uint8_t*)(Game::Validate_Type6FaceData_offset + Game::ds1_base);
    sp::mem::patch_bytes(write_address, Validate_Type6FaceData_patch, 5);

    //inject code to control if Step_PadMan can read inputs or not
    write_address = (uint8_t*)(Game::Step_PadMan_ReadInputs_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &Step_PadMan_ReadInputs_return, 2, &Step_PadMan_ReadInputs_injection);
}

static bool character_reload_run = false;
static bool character_initial_load_run = false;


// Performs tasks that were deferred until a character was loaded/reloaded
bool Game::on_character_load(void* unused)
{
    // Triggered on event: first character loaded in this instance of the game
    if (character_initial_load_run == false && Game::first_character_loaded == false && Game::playerchar_is_loaded())
    {
        Game::first_character_loaded = true;

        Game::set_display_name(Mod::use_steam_names);

        //only now, on first load, do we load the overhaul files
        //This way we know they're the 2nd thing loaded
        //(also only load them once since they get saved once loaded)
        FileReloading::LoadGameParam();

        CustomInvasionTypes::ReadSearchFile();

        ConsoleWrite("First character loading finished");

        character_initial_load_run = true;
    }

    //Triggered on event: a character has been loaded from the save menu, but not the first character loaded
    if (character_initial_load_run == false && Game::first_character_loaded == true && Game::playerchar_is_loaded())
    {
        CustomInvasionTypes::ReadSearchFile();

        ConsoleWrite("Another character loading finished");

        character_initial_load_run = true;
    }

    //Triggered on event: the game has gone through a loading screen
    if (character_reload_run == false && Game::playerchar_is_loaded())
    {
        Game::preload_function_caches();

        //need to force refresh the character in case the legacy mod changed while the game was off (restarting the game doesn't do this for some reason)
        //need to force refresh the character in case this character was in a different mode then the current when it was previous loaded
        FileReloading::RefreshPlayerStats();

        ConsoleWrite("Character reload finished");

        character_reload_run = true;
    }

    //set the flags to determine what to run
    if (character_reload_run == true && !Game::playerchar_is_loaded())
    {
        character_reload_run = false;
    }
    if (character_initial_load_run == true && Game::get_main_menu_flag().has_value() && *Game::get_main_menu_flag().value() == 1)
    {
        character_initial_load_run = false;
    }

    return true;
}

std::unordered_set<HANDLE> thread_handles;

void grab_thread_handle_helper(HANDLE h)
{
    if (h != NULL)
    {
        thread_handles.emplace(h);
    }
}

void grab_destruct_thread_handle_helper(HANDLE h)
{
    if (h != NULL)
    {
        thread_handles.erase(h);
    }
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
static uint8_t* area_num_cache = NULL;
static uint8_t* world_num_cache = NULL;
static int32_t* mp_id_cache = NULL;
static int32_t* area_id_cache = NULL;
static int32_t* saved_chars_menu_flag_cache = NULL;
static int32_t* main_menu_flag_cache = NULL;
static uint8_t* saved_chars_preview_data_cache = NULL;
static uint32_t* pc_playernum_cache = NULL;
static uint64_t connected_players_array_cache = NULL;
static void** pc_EzStateMachineImpl_cache = NULL;
static uint32_t* SessionManagerImp_session_action_result_cache = NULL;
static void** SteamSessionLight_cache = NULL;
static uint32_t* NextPlayerNum_cache = NULL;
static void** PlayerIns_cache = NULL;
static void** player_animationMediator_cache = NULL;
static void** host_player_gamedata_cache = NULL;
static bool* display_name_cache = NULL;
static int32_t** MP_AreaID_cache = NULL;

void Game::preload_function_caches() {
    ConsoleWrite("Cache loading");

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
    area_num_cache = NULL;
    Game::get_area_number();
    world_num_cache = NULL;
    Game::get_world_number();
    mp_id_cache = NULL;
    Game::get_online_area_id_ptr();
    area_id_cache = NULL;
    Game::get_area_id_ptr();
    saved_chars_menu_flag_cache = NULL;
    Game::get_saved_chars_menu_flag();
    main_menu_flag_cache = NULL;
    Game::get_main_menu_flag();
    saved_chars_preview_data_cache = NULL;
    Game::get_saved_chars_preview_data();
    pc_playernum_cache = NULL;
    Game::get_pc_playernum();
    connected_players_array_cache = NULL;
    Game::get_connected_player(0);
    pc_EzStateMachineImpl_cache = NULL;
    Game::get_pc_ActiveState_EzStateMachineImpl();
    SessionManagerImp_session_action_result_cache = NULL;
    Game::get_SessionManagerImp_session_action_result();
    SteamSessionLight_cache = NULL;
    Game::get_SessionManagerImp_SteamSessionLight();
    NextPlayerNum_cache = NULL;
    Game::get_SessionManagerImp_Next_Player_Num();
    PlayerIns_cache = NULL;
    Game::get_PlayerIns();
    player_animationMediator_cache = NULL;
    Game::get_player_animationMediator();
    host_player_gamedata_cache = NULL;
    Game::get_host_player_gamedata();
    MP_AreaID_cache = NULL;
    Game::get_MP_AreaID_ptr();

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

// Stop Durability Damage
extern "C" {
    uint64_t stop_durability_damage_injection_return;
    uint64_t stop_durability_damage_original_jump;
    void stop_durability_damage_hook();
}

void Game::stopDurabilityDamage(bool enable) {

    if (enable) {

        ConsoleWrite("Disabling durability damage...");

        uint64_t InfDur1AOB = Game::ds1_base + 0x74e7e7;

        // A conditional (and therefore relative) jump was overwritten by inject_jmp_14b. Calculate absolute address of the jump target
        // This allows the stop_durability_damage_hook procedure to correctly replicate the instructions overwritten by the trampoline
        stop_durability_damage_original_jump = InfDur1AOB + 0x1F;

        // Apply the injection
        sp::mem::code::x64::inject_jmp_14b((void*)InfDur1AOB, &stop_durability_damage_injection_return, 0, &stop_durability_damage_hook);

        /*
        Before injection:
        InfDura1AOB:
        .text:000000014074BB07 45 89 4B 14                                mov     [r11+14h], r9d
        .text:000000014074BB0B 44 3B 51 20                                cmp     r10d, [rcx+20h]
        .text:000000014074BB0F 7D 15                                      jge     short loc_14074BB26
        .text:000000014074BB11 48 8B 41 30                                mov     rax, [rcx+30h]
                               ^^^^^^^^^^^ these 14 bytes are patched with a long jump to stop_durability_damage_hook
        After injections:
        .text:000000014074BB07 ff 25 00 00 00 00 dd f3 1c 4a fc 7f 00 00  jmp stop_durability_damage_hook
        Injection Code:
            cmp [r11+14h], r9d
            jle originalcode
            mov r9d, dword ptr[r11+14h]                                   ; set the new durability value to be the current value
            originalcode:
                mov dword ptr[r11+14h], r9d
                cmp r10d, dword ptr[rcx+20h]
                jge hopper
                mov rax, qword ptr[rcx+20h]
                jmp stop_durability_damage_injection_return
            hopper:
                jmp stop_durability_damage_original_jump                  ; another function somewhere
        Returning from injection:
        .text:000000014074BB15 44 89 44 02 18                             mov     [rdx+rax+18h], r8d
        .text:000000014074BB1A 48 8D 0C 02                                lea     rcx, [rdx+rax]
        .text:000000014074BB1E B0 01                                      mov     al, 1
        .text:000000014074BB20 48 8B 5C 24 08                             mov     rbx, [rsp+arg_0]
        .text:000000014074BB25 C3                                         retn

        */
    }
}

// Returns multiplayer node count as an int (or -1 if player is not online)
int Game::get_node_count()
{
    sp::mem::pointer node_count_ptr = sp::mem::pointer<int>((void*)Game::frpg_net_base, { 0xAF0 });

    if (node_count_ptr.resolve() == NULL) {
        return -1;
    }

    return *node_count_ptr.resolve();
}

static const uint64_t disable_low_fps_disconnect_offset = 0x77c269;

// Disables automatic game disconnection when low framerate is detected
void Game::disable_low_fps_disconnect(bool enable)
{
    uint8_t *fps_warn = (uint8_t *)(Game::ds1_base + disable_low_fps_disconnect_offset);

    sp::mem::set_protection(fps_warn, 1, MEM_PROTECT_RWX);

    if (enable)
    {
        ConsoleWrite("Enabling low FPS disconnect...");
        *fps_warn = 0xEB;
        *(fps_warn+1) = 0x5;
    }
    else
    {
        ConsoleWrite("Disabling low FPS disconnect...");
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
    ConsoleWrite("Increasing game memory allocation size");

    set_memory_limit_chunk(0x1C200000,
                           0x7C200000);
    set_memory_limit_chunk(0x500000,
                           0xA00000);
    set_memory_limit_chunk(0x2000000,
                           0x7800000);
    set_memory_limit_chunk(0x4000000,
                           0xB000000);
    set_memory_limit_chunk(0x390000,
                           0xB00000);
    set_memory_limit_chunk(0x1320000,
                           0x7320000);
    set_memory_limit_chunk(0x400000,
                           0xB00000);
    set_memory_limit_chunk(0x800000,
                           0xF00000);
    set_memory_limit_chunk(0x380000,
                           0xA80000);
    set_memory_limit_chunk(0x5280000,
                           0xB280000);
    set_memory_limit_chunk(0xE0000,
                          0x400000);
    set_memory_limit_chunk(0x200000,
                           0x800000);
}


static bool resolve_current_player_animation_speed() {
    sp::mem::pointer speed_ptr = sp::mem::pointer<float>((void*)Game::world_chr_man_imp, { 0x68, 0x68, 0x18, 0xA8 });
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

    sp::mem::pointer anim_id = sp::mem::pointer<int32_t>((void*)Game::world_chr_man_imp, { 0x68, 0x68, 0x48, 0x80 });
    if (anim_id.resolve() == NULL || !character_reload_run) {
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

    sp::mem::pointer anim_id = sp::mem::pointer<int32_t>((void*)Game::world_chr_man_imp, { 0x68, 0x30, 0x5D0, 0x690 });
    if (anim_id.resolve() == NULL || !character_reload_run) {
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

    sp::mem::pointer anim_id = sp::mem::pointer<int32_t>((void*)Game::world_chr_man_imp, { 0x68, 0x30, 0x5D0, 0x13B0 });
    if (anim_id.resolve() == NULL || !character_reload_run) {
        return std::nullopt;
    }
    else {
        player_lower_body_anim_id_cache = (int32_t*)anim_id.resolve();
        return *player_lower_body_anim_id_cache;
    }
}

std::optional<void*> Game::get_player_animationMediator()
{
    if (player_animationMediator_cache)
    {
        return *player_animationMediator_cache;
    }

    sp::mem::pointer animationMediator = sp::mem::pointer<void*>((void*)Game::world_chr_man_imp, { 0x68, 0x68, 0x20 });
    if (animationMediator.resolve() == NULL || !character_reload_run)
    {
        return std::nullopt;
    }
    else
    {
        player_animationMediator_cache = animationMediator.resolve();
        return *player_animationMediator_cache;
    }
}

void* Game::get_PlayerIns_AnimationMediator(uint64_t playerIns)
{
    sp::mem::pointer animationMediator = sp::mem::pointer<void*>((void*)(playerIns + 0x68), { 0x20 });
    if (animationMediator.resolve() == NULL)
    {
        return NULL;
    }
    else
    {
        return *animationMediator.resolve();
    }
}

int32_t Game::get_animation_mediator_state_animation(void* animationMediator, AnimationStateTypesEnum state_id) {
    void* state_entry = (void*)((uint64_t)animationMediator + 168 * state_id);
    return *(int32_t*)((uint64_t)state_entry + 0);
}

bool Game::set_animation_currentProgress(void* animationMediator, AnimationStateTypesEnum state_id, float new_progressTime) {
    uint64_t state_entry = ((uint64_t)animationMediator + 168 * state_id);

    //sets the currentProgress that is read by the renderer this frame
    *(float*)(state_entry + 0xa4) = new_progressTime;

    //sets the currentProgress that is used to overwrite the above currentProgress in all later frames
    uint32_t animationController_index = -1;

    uint64_t linked_animation1 = *(uint64_t*)(state_entry + 0x90);
    if (linked_animation1 != NULL)
    {
        animationController_index = *(uint32_t*)(linked_animation1 + 4);
    }
    else
    {
        animationController_index = *(uint32_t*)(state_entry + 4);
    }
    animationController_index = (animationController_index & 0xffff);
    if (animationController_index >= 6)
    {
        return false;
    }

    uint64_t AnimationMediator_field0x1460 = *(uint64_t*)((uint64_t)(animationMediator)+0x1460);
    uint64_t AnimationMediator_field0x1460_field0x8_elem = *(uint64_t*)(AnimationMediator_field0x1460 + 0x8 + animationController_index*8);
    if (AnimationMediator_field0x1460_field0x8_elem == NULL)
    {
        return false;
    }
    uint32_t ChrCtrl_Animation_index = *(uint32_t*)(AnimationMediator_field0x1460_field0x8_elem + 0x140) & 0xffff;

    uint64_t ChrCtrl_AnimationQueue = *(uint64_t*)(AnimationMediator_field0x1460+0);
    uint64_t ChrCtrl_AnimationQueue_arrayLength = *(uint32_t*)(ChrCtrl_AnimationQueue + 0);
    if (ChrCtrl_Animation_index >= ChrCtrl_AnimationQueue_arrayLength)
    {
        return false;
    }
    uint64_t ChrCtrl_AnimationQueue_field0x8 = *(uint64_t*)(ChrCtrl_AnimationQueue + 8);
    uint64_t ChrCtrl_AnimationQueue_field0x8_elem = (ChrCtrl_AnimationQueue_field0x8 + ChrCtrl_Animation_index*0x78);
    uint16_t ChrCtrl_AnimationQueue_field0x8_elem_field0x0 = *(uint16_t*)(ChrCtrl_AnimationQueue_field0x8_elem + 0);
    if (ChrCtrl_AnimationQueue_field0x8_elem_field0x0 != ChrCtrl_Animation_index)
    {
        return false;
    }
    uint64_t FrpgDefaultAnimationControl = *(uint64_t*)(ChrCtrl_AnimationQueue_field0x8_elem + 8);
    float* curTimeInAnimation = (float*)(FrpgDefaultAnimationControl + 0x10);

    *curTimeInAnimation = new_progressTime;

    return true;
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
    if (timer.resolve() == NULL || !character_reload_run) {
        return std::nullopt;
    }
    else {
        time_address = timer.resolve();
        return time_address;
    }
}

uint32_t Game::get_frame_count()
{
    uint32_t frame_count = *(uint32_t*)((*(uint64_t*)Game::file_man) + 0x88);
    return frame_count;
}

std::optional<uint64_t> Game::get_pc_entity_pointer() {
    //quick resolve
    if (pc_entity_ptr) {
        return *pc_entity_ptr;
    }

    sp::mem::pointer entity_ptr = sp::mem::pointer<uint64_t>((void*)(Game::world_chr_man_imp), { 0x68 });
    if (entity_ptr.resolve() == NULL || !character_reload_run) {
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

    sp::mem::pointer position_ptr = sp::mem::pointer<float>((void*)(Game::world_chr_man_imp), { 0x68, 0x68, 0x28, 0x10 });
    if (position_ptr.resolve() == NULL || !character_reload_run) {
        return std::nullopt;
    }
    else {
        pc_position_ptr = position_ptr.resolve();
        return pc_position_ptr;
    }
}

float Game::current_hpbar_max = 0;
const float overhaul_hpbar_max = 5267.0;
const float corrected_vanilla_hpbar_max = 2633.0;
const float original_vanilla_hpbar_max = 1900.0;
static const uint64_t gui_hpbar_value_offset = 0x6797cd;

extern "C" {
    uint64_t Gui_HP_bar_UI_ptr = NULL;
}

// Fix the bug where the player HP could be greater than the displayed GUI bar
void Game::set_gui_hpbar_max()
{
    if (!Mod::fix_hp_bar_size)
    {
        current_hpbar_max = original_vanilla_hpbar_max;
    }
    else if (Mod::fix_hp_bar_size && Mod::get_mode() != ModMode::Overhaul)
    {
        current_hpbar_max = corrected_vanilla_hpbar_max;
    }
    else if (Mod::fix_hp_bar_size && Mod::get_mode() == ModMode::Overhaul)
    {
        current_hpbar_max = overhaul_hpbar_max;
    }

    ConsoleWrite("Setting hp bar to %f", current_hpbar_max);

    //Instruction that loads the immediate float (+6 for immediate location in opcode)
    void *write_address = (void*)(Game::ds1_base + gui_hpbar_value_offset);
    sp::mem::patch_bytes((void*)((uint64_t)write_address+6), (uint8_t*)&current_hpbar_max, 4);

    //Also update the FrpgMenuDlgPCGauge variable that is set based on the above, so the change is immediate
    //If we're calling this before the variable is init'd, it's fine since the above injection will set the value and we don't need this
    //If we're calling it after the above injection has already been exec'd, this pointer will be saved and current
    if (Gui_HP_bar_UI_ptr != NULL)
    {
        *(float*)Gui_HP_bar_UI_ptr = current_hpbar_max;
    }
}


std::optional<uint32_t> Game::left_hand_weapon() {
    if (left_hand_weapon_ptr_cache) {
        return *left_hand_weapon_ptr_cache;
    }

    sp::mem::pointer weapon = sp::mem::pointer<uint32_t>((void*)(Game::ds1_base + 0x1a31768), { 0x28, 0x250, 0x2F8, 0x18, 0x0 });
    if (weapon.resolve() == NULL || !character_reload_run) {
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

    sp::mem::pointer weapon = sp::mem::pointer<uint32_t>((void*)(Game::ds1_base + 0x1a31768), { 0x28, 0x250, 0x2F8, 0x18, 0x4 });
    if (weapon.resolve() == NULL || !character_reload_run) {
        return std::nullopt;
    }
    else {
        right_hand_weapon_ptr_cache = (uint32_t*)weapon.resolve();
        return *right_hand_weapon_ptr_cache;
    }
}

// Note we can't cache this because we rely on it to check cache staleness
// And it must be done as an injection since otherwise mem derefs are too slow for how much we use this
bool Game::playerchar_is_loaded()
{
    return char_loaded;
}

std::optional<uint32_t> Game::get_player_char_max_hp() {
    if (player_char_max_hp_cache) {
        return *player_char_max_hp_cache;
    }

    sp::mem::pointer maxhp = sp::mem::pointer<uint32_t>((void*)(Game::world_chr_man_imp), { 0x68, 0x3EC });
    if (maxhp.resolve() == NULL || !character_reload_run) {
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
    if (entity_ptr == NULL || rotation.fast_resolve() == NULL) {
        return std::nullopt;
    }
    else {
        return *(float*)rotation.fast_resolve();
    }
}

std::optional<uint8_t> Game::get_area_number() {
    if (area_num_cache) {
        return *area_num_cache;
    }

    sp::mem::pointer area_num = sp::mem::pointer<uint8_t>((void*)(Game::frpg_net_base), { 0xA22 });
    if (area_num.resolve() == NULL || !character_reload_run) {
        return std::nullopt;
    }
    else {
        area_num_cache = area_num.resolve();
        return *area_num_cache;
    }
}

std::optional<uint8_t> Game::get_world_number()
{
    if (world_num_cache)
    {
        return *world_num_cache;
    }

    sp::mem::pointer world_num = sp::mem::pointer<uint8_t>((void*)(Game::frpg_net_base), { 0xA23 });
    if (world_num.resolve() == NULL || !character_reload_run)
    {
        return std::nullopt;
    }
    else
    {
        world_num_cache = world_num.resolve();
        return *world_num_cache;
    }
}

std::optional<int32_t*> Game::get_online_area_id_ptr() {
    if (mp_id_cache) {
        return mp_id_cache;
    }

    sp::mem::pointer mp_id = sp::mem::pointer<int32_t>((void*)(Game::world_chr_man_imp), { 0x68, 0x354 });
    if (mp_id.resolve() == NULL || !character_reload_run) {
        return std::nullopt;
    }
    else {
        mp_id_cache = mp_id.resolve();
        return mp_id_cache;
    }
}

std::optional<int32_t*> Game::get_area_id_ptr()
{
    if (area_id_cache)
    {
        return area_id_cache;
    }

    sp::mem::pointer area_id = sp::mem::pointer<int32_t>((void*)(Game::world_chr_man_imp), { 0x68, 0x358 });
    if (area_id.resolve() == NULL || !character_reload_run)
    {
        return std::nullopt;
    }
    else
    {
        area_id_cache = area_id.resolve();
        return area_id_cache;
    }
}

std::optional<int32_t*> Game::get_saved_chars_menu_flag() {
    if (saved_chars_menu_flag_cache) {
        return saved_chars_menu_flag_cache;
    }

    sp::mem::pointer saved_chars_menu_flag = sp::mem::pointer<int32_t>((void*)(Game::menu_man), { 0xA4 });
    //don't check for character_reload_run here, since this is called before chr load
    if (saved_chars_menu_flag.resolve() == NULL) {
        return std::nullopt;
    }
    else {
        saved_chars_menu_flag_cache = saved_chars_menu_flag.resolve();
        return saved_chars_menu_flag_cache;
    }
}

std::optional<int32_t*> Game::get_main_menu_flag()
{
    if (main_menu_flag_cache)
    {
        return main_menu_flag_cache;
    }

    sp::mem::pointer main_menu_flag = sp::mem::pointer<int32_t>((void*)(Game::menu_man), { 0x80 });
    //don't check for character_reload_run here, since this is called before chr load
    if (main_menu_flag.resolve() == NULL)
    {
        return std::nullopt;
    }
    else
    {
        main_menu_flag_cache = main_menu_flag.resolve();
        return main_menu_flag_cache;
    }
}

std::optional<uint8_t*> Game::get_saved_chars_preview_data() {
    if (saved_chars_preview_data_cache) {
        return saved_chars_preview_data_cache;
    }

    sp::mem::pointer saved_chars_preview_data = sp::mem::pointer<uint8_t>((void*)(Game::game_data_man), { 0x60, 0x10 });
    //don't check for character_reload_run here, since this is called before chr load
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
    if (pc_playernum.resolve() == NULL || !character_reload_run) {
        return std::nullopt;
    }
    else {
        pc_playernum_cache = pc_playernum.resolve();
        return *pc_playernum_cache;
    }
}

// returns PlayerIns
std::optional<uint64_t> Game::get_connected_player(uint32_t i) {
    //go to the given index in the connectedPlayers_ChrSlotArray, grab the first value (pointer to PlayerIns) and return it
    if (connected_players_array_cache) {
        return *(uint64_t*)(connected_players_array_cache + (0x38 * (i + 1)));
    }

    //gets a pointer to the connectedPlayers_ChrSlotArray stored in the player's PlayerIns
    sp::mem::pointer connected_players_array = sp::mem::pointer<uint64_t>((void*)(Game::world_chr_man_imp), { 0x68, 0x18 });
    if (connected_players_array.resolve() == NULL || !character_reload_run) {
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
                if (guest != 0)
                {
                    uint32_t guestHandle = *(uint32_t*)(guest + 8);
                    if (guestHandle == handle)
                    {
                        return *(uint32_t*)((*(uint64_t*)(guest + 0x578)) + 0x10);
                    }
                }
            }
            else
            {
                return std::nullopt;
            }
        }
        return std::nullopt;
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

std::optional<void*> Game::get_pc_ActiveState_EzStateMachineImpl() {
    if (pc_EzStateMachineImpl_cache) {
        return *pc_EzStateMachineImpl_cache;
    }

    //WorldChrManImp -> PlayerIns -> ChrIns -> PlayerCtrl -> ChrCtrl -> ActionCtrl -> ActiveState -> EzStateMachineImpl
    sp::mem::pointer pc_EzStateMachineImpl = sp::mem::pointer<void*>((void*)(Game::world_chr_man_imp), { 0x68, 8+0x60, 0x48, 0x30+(0x20*1) });
    if (pc_EzStateMachineImpl.resolve() == NULL || !character_reload_run) {
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
    if (SessionManagerImp_session_action_result_cache)
    {
        return static_cast<SessionActionResultEnum>(*SessionManagerImp_session_action_result_cache);
    }

    sp::mem::pointer session_action_result = sp::mem::pointer<uint32_t>((void*)(Game::session_man_imp), { 0xf8 });
    if (session_action_result.resolve() == NULL || !character_reload_run)
    {
        return std::nullopt;
    }
    else
    {
        SessionManagerImp_session_action_result_cache = session_action_result.resolve();
        return static_cast<SessionActionResultEnum>(*SessionManagerImp_session_action_result_cache);
    }
}

std::optional<void*> Game::get_SessionManagerImp_SteamSessionLight()
{
    if (SteamSessionLight_cache)
    {
        return *SteamSessionLight_cache;
    }

    sp::mem::pointer SteamSessionLight = sp::mem::pointer<void*>((void*)(Game::session_man_imp), { 0x08 });
    if (SteamSessionLight.resolve() == NULL || !character_reload_run)
    {
        return std::nullopt;
    }
    else
    {
        SteamSessionLight_cache = SteamSessionLight.resolve();
        return *SteamSessionLight_cache;
    }
}

std::optional<uint32_t> Game::get_SessionManagerImp_Next_Player_Num()
{
    if (NextPlayerNum_cache)
    {
        return *NextPlayerNum_cache;
    }

    sp::mem::pointer NextPlayerNum = sp::mem::pointer<uint32_t>((void*)(Game::session_man_imp), { 0x108 });
    if (NextPlayerNum.resolve() == NULL || !character_reload_run)
    {
        return std::nullopt;
    }
    else
    {
        NextPlayerNum_cache = NextPlayerNum.resolve();
        return *NextPlayerNum_cache;
    }
}

std::optional<void*> Game::get_PlayerIns()
{
    if (PlayerIns_cache)
    {
        return *PlayerIns_cache;
    }

    sp::mem::pointer playerIns = sp::mem::pointer<void*>((void*)(Game::world_chr_man_imp), { 0x68 });
    if (playerIns.resolve() == NULL || !character_reload_run)
    {
        return std::nullopt;
    }
    else
    {
        PlayerIns_cache = playerIns.resolve();
        return *PlayerIns_cache;
    }
}

uint32_t Game::get_equipped_inventory(uint64_t playerins, InventorySlots index)
{
    uint64_t playerGameData = *(uint64_t*)(playerins + 0x578);
    uint64_t equipGameData = (uint64_t)(playerGameData + 0x280);
    uint64_t charAsm = (uint64_t)(equipGameData + 0x80);
    uint32_t* equip_items = (uint32_t*)(charAsm + 0x24);
    return equip_items[index];
}

InventorySlots Game::get_equipped_right_weapon_inhand(uint64_t playerins)
{
    uint64_t playerGameData = *(uint64_t*)(playerins + 0x578);
    uint64_t equipGameData = (uint64_t)(playerGameData + 0x280);
    uint64_t charAsm = (uint64_t)(equipGameData + 0x80);
    uint32_t right_weapon_slot_index = *(uint32_t*)(charAsm + 0x10);
    if (right_weapon_slot_index == 0)
    {
        return RightHand1;
    }
    else if (right_weapon_slot_index == 1)
    {
        return RightHand2;
    }
}

//given a list of speffects, check if the given player has any of them active
bool Game::player_has_speffect(uint64_t playerins, std::unordered_set<uint32_t> speffects)
{
    uint64_t chrIns = (uint64_t)(playerins + 0x8);
    uint64_t specialEffects = *(uint64_t*)(chrIns + 0x270);
    uint64_t specialEffectInfo = *(uint64_t*)(specialEffects + 0x8 + 0x0);
    while (true)
    {
        if (specialEffectInfo == 0x0)
        {
            return false;
        }
        uint32_t specialEffectInfo_id = *(uint32_t*)(specialEffectInfo + 0x30);
        if (speffects.count(specialEffectInfo_id))
        {
            return true;
        }
        //get next in linked list
        specialEffectInfo = *(uint64_t*)(specialEffectInfo + 0x40);
    }
    return false;
}

std::optional<SpecialEffect_Info*> Game::player_get_speffect(uint64_t playerins, uint32_t speffectId)
{
    if (speffectId == -1)
    {
        return std::nullopt;
    }
    uint64_t chrIns = (uint64_t)(playerins + 0x8);
    uint64_t specialEffects = *(uint64_t*)(chrIns + 0x270);
    uint64_t specialEffectInfo = *(uint64_t*)(specialEffects + 0x8 + 0x0);
    while (true)
    {
        if (specialEffectInfo == 0x0)
        {
            return std::nullopt;
        }
        uint32_t specialEffectInfo_id = *(uint32_t*)(specialEffectInfo + 0x30);
        if (specialEffectInfo_id == speffectId)
        {
            return (SpecialEffect_Info*)specialEffectInfo;
        }
        //get next in linked list
        specialEffectInfo = *(uint64_t*)(specialEffectInfo + 0x40);
    }
    return std::nullopt;
}


void Game::player_add_speffect(uint32_t speffectId)
{
    //0x2710 is hardcoded to the PC
    return lua_SetEventSpecialEffect_2_function(NULL, 0x2710, speffectId);
}

std::optional<void*> Game::get_host_player_gamedata()
{
    if (host_player_gamedata_cache)
    {
        return *host_player_gamedata_cache;
    }

    sp::mem::pointer host_player_gamedata = sp::mem::pointer<void*>((void*)(Game::game_data_man), { 0x10 });
    if (host_player_gamedata.resolve() == NULL || !character_reload_run)
    {
        return std::nullopt;
    }
    else
    {
        host_player_gamedata_cache = host_player_gamedata.resolve();
        return *host_player_gamedata_cache;
    }
}

bool Game::set_display_name(bool useSteam)
{
    ConsoleWrite("Setting display name setting...");

    if (display_name_cache)
    {
        *display_name_cache = useSteam;
        return true;
    }

    sp::mem::pointer display_name = sp::mem::pointer<bool>((void*)(Game::game_data_man), { 0x58, 0x3e });
    if (display_name.resolve() == NULL)
    {
        return false;
    }
    else
    {
        display_name_cache = display_name.resolve();
        *display_name_cache = useSteam;
        return true;
    }
}

uint64_t Game::get_accurate_time()
{
    uint64_t time;
    //returns time in units of 100 nanoseconds since system started
    QueryUnbiasedInterruptTime(&time);
    return time;
}

//convert the time we get from accurate/synced time (unit of 100ns) to the amount required for the animation entry offset value (unit of seconds)
float Game::convert_time_to_offset(uint64_t time)
{
    return time / 10000000.0f;
}

typedef void create_popup_window_msg_Typedef(uint64_t unk, const wchar_t* str);
create_popup_window_msg_Typedef* create_popup_window_msg = (create_popup_window_msg_Typedef*)0x1406eb370;

//This is mostly copied from the debug code that creates a test message. Function 1406e9840
void Game::show_popup_message(const wchar_t* msg)
{
    uint64_t pcVar3 = *((uint64_t*)Game::menu_man) + 0xa48;
    int i = 0;
    uint64_t pcVar7 = pcVar3;
    do
    {
        pcVar7 = pcVar7 + 0x40;
        if (*(byte*)pcVar7 == 0)
        {
            *(uint32_t*)(pcVar3 + 0x150) = *(uint32_t*)(pcVar3 + 0x8);
            *(uint32_t*)(pcVar3 + 0x8) = i;
            pcVar7 = pcVar3 + i * 0x40 + 0x10;
            if (pcVar7 != NULL)
            {
                *(uint64_t*)(pcVar7 + 0x8) = 0xffffffffffffffff;
                *(uint32_t*)(pcVar7 + 0x4) = 0xffffffff;
                *(uint32_t*)(pcVar7 + 0x18) = 1;
                *(uint32_t*)(pcVar7 + 0x34) = 0x1;
                *(uint32_t*)(pcVar7 + 0x38) = 0x7;
                *(uint8_t*)(pcVar7 + 0x30) = 1;
            }
            create_popup_window_msg(pcVar3, msg);
            *(uint16_t*)(pcVar7 + 0x30) = 0x101;
            *(uint32_t*)(pcVar3 + 0x8) = *(uint32_t*)(pcVar3 + 0x150);
            break;
        }
        i = i + 1;
    } while (i < 5);
}

typedef bool Create_BannerMessage_Func_Typedef(uint64_t menuman_banner, uint32_t banner_type, const wchar_t* str);
Create_BannerMessage_Func_Typedef* Create_BannerMessage_Func = (Create_BannerMessage_Func_Typedef*)0x14071fc60;

void Game::show_banner_message(const wchar_t* msg)
{
    uint64_t MenuMan_BannerMessages = *((uint64_t*)Game::menu_man) + 0xf80;
    Create_BannerMessage_Func(MenuMan_BannerMessages, 1, msg); //the other banner type, 0, just shows the banner slightly higher
}

std::optional<void*> Game::find_bullet(uint32_t owner_handle, uint32_t bullet_num)
{
    //this points to a linked list of the bullets in use. Follow backwards till there isn't another node in the list
    uint64_t current_bullet_in_use = *(uint64_t*)((*(uint64_t*)Game::bullet_man) + 8);
    while(current_bullet_in_use != NULL)
    {
        uint8_t cur_bullet_num = *(uint8_t*)(current_bullet_in_use + 8);
        uint32_t cur_bullet_owner = *(uint32_t*)(current_bullet_in_use + 156);
        if (cur_bullet_num == bullet_num && owner_handle == cur_bullet_owner)
        {
            return (void*)(current_bullet_in_use);
        }
        current_bullet_in_use = *(uint64_t*)(current_bullet_in_use + 0x348);
    }

    return std::nullopt;
}

// You'd think we'd only ever want to check the bullet_num value, at +8
// However, this value isn't reliably synced online, so it can't be used for other players and the SpellDesync fix
// Instead, we have to do it the ugly way, and find the closest by position.
// Unreliable and ugly, but i don't have a better way
// we need to include the ones in our current queue, to disallow the heuristic from selecting the same bullet twice
std::optional<void*> Game::find_unfired_bullet(uint32_t owner_handle, float x_pos, float y_pos, float z_pos, std::unordered_set<uint8_t> formidden_nums)
{
    //this points to a linked list of the bullets in use. Follow backwards till there isn't another node in the list
    uint64_t current_bullet_in_use = *(uint64_t*)((*(uint64_t*)Game::bullet_man) + 8);
    uint64_t closest_bullet = NULL;
    float closest_bullet_distance = 99999.0f;

    while (current_bullet_in_use != NULL)
    {
        float cur_bullet_x = *(float*)(current_bullet_in_use + 16 + 0);
        float cur_bullet_y = *(float*)(current_bullet_in_use + 16 + 4);
        float cur_bullet_z = *(float*)(current_bullet_in_use + 16 + 8);
        uint8_t cur_bullet_num = *(uint8_t*)(current_bullet_in_use + 8);
        uint32_t cur_bullet_owner = *(uint32_t*)(current_bullet_in_use + 156);
        uint32_t cur_bullet_target = *(uint32_t*)(current_bullet_in_use + 656);

        //check the owner, and that the bullet has not yet been fired
        if (owner_handle == cur_bullet_owner && cur_bullet_target == cur_bullet_owner)
        {
            // distance = sqrt(a^2+b^2+c^2)
            float x_dist = abs(cur_bullet_x - x_pos);
            float y_dist = abs(cur_bullet_y - y_pos);
            float z_dist = abs(cur_bullet_z - z_pos);
            float this_bullet_distance = sqrt(x_dist * x_dist + y_dist * y_dist + z_dist * z_dist);
            if (this_bullet_distance < closest_bullet_distance && formidden_nums.count(cur_bullet_num) == 0)
            {
                closest_bullet = current_bullet_in_use;
                closest_bullet_distance = this_bullet_distance;
            }
        }
        current_bullet_in_use = *(uint64_t*)(current_bullet_in_use + 0x348);
    }

    if (closest_bullet == NULL)
    {
        return std::nullopt;
    }
    else
    {
        return (void*)(closest_bullet);
    }
}

bool Game::set_invasion_refresh_timer(float newtime)
{
    sp::mem::pointer refresh_timer = sp::mem::pointer<float>((void*)(Game::unknown_global_struct_141d283a8), { 0x8, 0x8, 0x1e4 });
    if (refresh_timer.resolve() == NULL)
    {
        return false;
    }
    else
    {
        *(refresh_timer.resolve()) = newtime;
        return true;
    }
}

bool Game::set_blue_invasion_refresh_timer(float newtime)
{
    sp::mem::pointer refresh_timer = sp::mem::pointer<float>((void*)(Game::unknown_global_struct_141d283a8), { 0x8, 0x0, 0x1e4 });
    if (refresh_timer.resolve() == NULL)
    {
        return false;
    }
    else
    {
        *(refresh_timer.resolve()) = newtime;
        return true;
    }
}

uint32_t Game::get_player_chr_type(uint64_t playerIns)
{
    return *(uint32_t*)(playerIns + 0xD4);
}

std::optional<int32_t*> Game::get_MP_AreaID_ptr()
{
    if (MP_AreaID_cache)
    {
        return *MP_AreaID_cache;
    }

    sp::mem::pointer MP_AreaID = sp::mem::pointer<int32_t*>((void*)(Game::frpg_net_man), { 0xA8C });
    if (MP_AreaID.resolve() == NULL || !character_reload_run)
    {
        return std::nullopt;
    }
    else
    {
        MP_AreaID_cache = MP_AreaID.resolve();
        return *MP_AreaID_cache;
    }
}

void* Game::game_malloc(size_t size, size_t alignment, void* heap)
{
    return InGame_Malloc(size, alignment, heap);
}

void Game::game_free(void* p, size_t size)
{
    return InGame_Free(p, size);
}

typedef void heapObjFreeFunc(void* heapObj, void* p);

void Game::game_free_alt(void* p)
{
    uint64_t* heapObj = FUN_140cbede0(p);
    uint64_t heapObjVtable = *heapObj;
    heapObjFreeFunc* freeFunc = (heapObjFreeFunc*)*(uint64_t*)(heapObjVtable + 0x68);
    freeFunc(heapObj, p);
}

void* Game::get_MoveMapStep()
{
    //this pointer breaks when debug is enabled, so it's unreliable
    //sp::mem::pointer MoveMapStep = sp::mem::pointer<void*>((void*)(Game::frpg_system), { 0x8, 0x20, 0x58, 0x20, 0xa0, 0x38, 0x20 });

    return (void*)grab_movemapstep_value;
}

void Game::Step_GameSimulation(bool renderFrame)
{
    void* MoveMapStep = Game::get_MoveMapStep();
    if (!renderFrame)
    {
        void* FieldArea = *(void**)(((uint64_t)MoveMapStep) + 0x60);
        Step_MapArea_MapAreaObjects_and_NearbyMapAreas(FieldArea, FRAMETIME, 1, 1, 0);
        Step_Chr(MoveMapStep, FRAMETIME, 1);
        Step_Bullet(*(void**)Game::bullet_man, FRAMETIME);
        Step_DamageMan(*(void**)Game::damage_man, FRAMETIME);
    }
    else
    {
        MoveMapStep_Step_13(MoveMapStep, FRAMETIME);
    }
    //need to prevent the game from reading inputs directly here
    Step_PadMan_ReadInputs_allowed = false;
    Step_PadMan(FRAMETIME);
    Step_PadMan_ReadInputs_allowed = true;
    Step_Havok(*(void**)Game::frpg_havok_man_imp, FRAMETIME);
    FinishStep_Havok(*(void**)Game::frpg_havok_man_imp);
}

void Game::SuspendThreads()
{
    for (const auto& handle : thread_handles)
    {
        DWORD out;
        do
        {
            out = SuspendThread(handle);
        } while (out != (DWORD)-1 && out > 0);
        if (out == (DWORD)-1)
        {
            FATALERROR("Unable to suspend thread correctly. Error=%x", GetLastError());
        }
    }
}

void Game::ResumeThreads()
{
    for (const auto& handle : thread_handles)
    {
        DWORD out;
        do
        {
            out = ResumeThread(handle);
        } while (out != (DWORD)-1 && out > 0);
        if (out == (DWORD)-1)
        {
            FATALERROR("Unable to resume thread correctly. Error=%x", GetLastError());
        }
    }
}

uint8_t* ConnectedPlayerData_Get_SteamId(uint64_t connectedplayerdata)
{
    uint64_t steam_player_data = *(uint64_t*)(connectedplayerdata + 0);
    uint64_t online_id = *(uint64_t*)(steam_player_data + 0x18);
    return (uint8_t*)(online_id + 0x50);
}

int32_t Game::get_SessionPlayerNumber_For_ConnectedPlayerData(uint64_t connectedplayerdata)
{
    uint64_t session_man = *(uint64_t*)Game::session_man_imp;
    uint64_t connected_players = *(uint64_t*)(session_man + 0x248);
    uint64_t connected_players_prev = *(uint64_t*)(connected_players + 0);
    void* looking_steam_id = ConnectedPlayerData_Get_SteamId(connectedplayerdata);

    while (connected_players_prev != connected_players)
    {
        void* cur_steam_id = ConnectedPlayerData_Get_SteamId(connected_players_prev + 0x10);
        int playerEql = memcmp(looking_steam_id, cur_steam_id, 0x20);
        if (playerEql == 0)
        {
            return *(int32_t*)(connected_players_prev + 0x50);
        }
        connected_players_prev = *(uint64_t*)(connected_players_prev + 0);
    }

    return -1;
}
