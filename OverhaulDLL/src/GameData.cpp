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

static uint64_t BaseBOffset = NULL;

static uint64_t BaseB = NULL;

// Player character status (loading, human, co-op, invader, hollow)
sp::mem::pointer<int32_t> Game::player_char_status;

extern "C" {
    uint32_t last_attack_weaponid;
    uint64_t calculate_attack_damage_injection_return;
    void calculate_attack_damage_injection();

    bool char_loaded;
    uint64_t char_loaded_injection_return;
    void char_loaded_injection();
    uint64_t char_loading_injection_return;
    void char_loading_injection();

    uint64_t gui_hpbar_max_injection_return;
    void gui_hpbar_max_injection();
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
    uint8_t* write_address;

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

    Game::game_data_man = Game::ds1_base + 0x1D278F0;

    Game::world_chr_man_imp = Game::ds1_base + 0x1d151b0;

    Game::player_char_status = sp::mem::pointer<int32_t>((void*)(Game::world_chr_man_imp), { 0x68, 0xD4 });

    Game::param_man = Game::ds1_base + 0x1d1b098;

    Game::solo_param_man = Game::ds1_base + 0x1d1b360;

    Game::file_man = Game::ds1_base + 0x1d1e4f8;

    Game::session_man_imp = Game::ds1_base + 0x1d1a370;

    Game::menu_man = Game::ds1_base + 0x1d26168;

    BaseBOffset = Game::ds1_base + 0x728E50;

    //hook the code that calculates attack damage and save off the weapon id used for the attack
    last_attack_weaponid = -1;
    write_address = (uint8_t*)(Game::calculate_attack_damage_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &calculate_attack_damage_injection_return, 1, &calculate_attack_damage_injection);

    //inject the code that detects if the character is loaded in or not
    char_loaded = false;
    write_address = (uint8_t*)(Game::char_loaded_injection_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &char_loaded_injection_return, 6, &char_loaded_injection);
    write_address = (uint8_t*)(Game::char_loading_injection_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &char_loading_injection_return, 1, &char_loading_injection);

    //inject the code that saves the HP Bar UI element pointer
    write_address = (uint8_t*)(Game::gui_hpbar_max_injection_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &gui_hpbar_max_injection_return, 7, &gui_hpbar_max_injection);
}

static bool character_reload_run = false;

// Performs tasks that were deferred until a character was loaded/reloaded
bool Game::on_character_load(void* unused)
{
    // Wait for event: first character loaded in this instance of the game
    if (Game::characters_loaded == false && Game::playerchar_is_loaded())
    {
        Game::characters_loaded = true;

        Game::preload_function_caches();

        Game::set_display_name(Mod::use_steam_names);

        character_reload_run = true;

        //only now, on first load, do we load the overhaul files
        //This way we know they're the 2nd thing loaded
        //(also only load them once since they get saved once loaded)
        FileReloading::LoadGameParam();

        //need to force refresh the character in case the legacy mod changed while the game was off (restarting the game doesn't do this for some reason)
        FileReloading::RefreshPlayerStats();

        ConsoleWrite("All character loading finished!");

        return true;
    }

    if (character_reload_run == false && Game::playerchar_is_loaded())
    {
        Game::preload_function_caches();

        /*//refresh the animation table pointers
        for (int i = 0; i < sizeof(pc_animation_table) / sizeof(void**); i++) {
            Game::pc_animation_table[i] = (AnimationEntry**)SpPointer(Game::world_char_base, { 0x28, 0x0, 0x28, 0x14, 0x4, 0x10 + (0x60 * i) }).resolve();
            if (Game::pc_animation_table[i] == NULL) {
                print_console("Error getting Animation Table Entry address");
            }
        }*/

        //need to force refresh the character in case this character was in a different mode then the current when it was previous loaded
        FileReloading::RefreshPlayerStats();

        character_reload_run = true;

        return true;
    }

    if (character_reload_run == true && !Game::playerchar_is_loaded())
    {
        character_reload_run = false;
        return true;
    }

    return true;
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
static int32_t* saved_chars_menu_flag_cache = NULL;
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

        uint64_t InfDur1AOB = Game::ds1_base + 0x74bb07;

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

// Replenish spell casts
void Game::replenishSpells() {

    if (!BaseB) {
        BaseB = *((uint64_t*)(BaseBOffset + *(uint32_t*)((uint64_t)BaseBOffset + 3) + 7));
    }

    if (BaseB) {

        uint32_t spells[] = {
            90,          // 3000 - Sorcery: Soul Arrow
            60,          // 3010 - Sorcery: Great Soul Arrow
            36,          // 3020 - Sorcery: Heavy Soul Arrow
            24,          // 3030 - Sorcery: Great Heavy Soul Arrow
            30,          // 3040 - Sorcery: Homing Soulmass
            30,          // 3050 - Sorcery: Homing Crystal Soulmass
            12,          // 3060 - Sorcery: Soul Spear
            12,          // 3070 - Sorcery: Crystal Soul Spear
            0,           // 3080
            0,           // 3090
            15,          // 3100 - Sorcery: Magic Weapon
            9,           // 3110 - Sorcery: Great Magic Weapon
            9,           // 3120 - Sorcery: Crystal Magic Weapon
            0,           // 3130
            0,           // 3140
            0,           // 3150
            0,           // 3160
            0,           // 3170
            0,           // 3180
            0,           // 3190
            0,           // 3200
            0,           // 3210
            0,           // 3220
            0,           // 3230
            0,           // 3240
            0,           // 3250
            0,           // 3260
            0,           // 3270
            0,           // 3280
            0,           // 3290
            15,          // 3300 - Sorcery: Magic Shield
            9,           // 3310 - Sorcery: Strong Magic Shield
            0,           // 3320
            0,           // 3330
            0,           // 3340
            0,           // 3350
            0,           // 3360
            0,           // 3370
            0,           // 3380
            0,           // 3390
            9,           // 3400 - Sorcery: Hidden Weapon
            9,           // 3410 - Sorcery: Hidden Body
            0,           // 3420
            0,           // 3430
            0,           // 3440
            0,           // 3450
            0,           // 3460
            0,           // 3470
            0,           // 3480
            0,           // 3490
            9,           // 3500 - Sorcery: Cast Light
            18,          // 3510 - Sorcery: Hush
            60,          // 3520 - Sorcery: Aural Decoy
            3,           // 3530 - Sorcery: Repair
            30,          // 3540 - Sorcery: Fall Control
            33,          // 3550 - Sorcery: Chameleon
            0,           // 3560
            0,           // 3570
            0,           // 3580
            0,           // 3590
            12,          // 3600 - Sorcery: Resist Curse
            12,          // 3610 - Sorcery: Remedy
            0,           // 3620
            0,           // 3630
            0,           // 3640
            0,           // 3650
            0,           // 3660
            0,           // 3670
            0,           // 3680
            0,           // 3690
            60,          // 3700 - Sorcery: White Dragon Breath
            36,          // 3710 - Sorcery: Dark Orb
            18,          // 3720 - Sorcery: Dark Bead
            6,           // 3730 - Sorcery: Dark Fog
            9,           // 3740 - Sorcery: Pursuers
            0,           // 3750
            0,           // 3760
            0,           // 3770
            0,           // 3780
            0,           // 3790
            0,           // 3800
            0,           // 3810
            0,           // 3820
            0,           // 3830
            0,           // 3840
            0,           // 3850
            0,           // 3860
            0,           // 3870
            0,           // 3880
            0,           // 3890
            0,           // 3900
            0,           // 3910
            0,           // 3920
            0,           // 3930
            0,           // 3940
            0,           // 3950
            0,           // 3960
            0,           // 3970
            0,           // 3980
            0,           // 3990
            24,          // 4000 - Pyromancy: Fireball
            18,          // 4010 - Pyromancy: Fire Orb
            12,          // 4020 - Pyromancy: Great Fireball
            60,          // 4030 - Pyromancy: Firestorm
            60,          // 4040 - Pyromancy: Fire Tempest
            240,         // 4050 - Pyromancy: Fire Surge
            240,         // 4060 - Pyromancy: Fire Whip
            0,           // 4070
            0,           // 4080
            0,           // 4090
            48,          // 4100 - Pyromancy: Combustion
            24,          // 4110 - Pyromancy: Great Combustion
            0,           // 4120
            0,           // 4130
            0,           // 4140
            0,           // 4150
            0,           // 4160
            0,           // 4170
            0,           // 4180
            0,           // 4190
            9,           // 4200 - Pyromancy: Poison Mist
            3,           // 4210 - Pyromancy: Toxic Mist
            6,           // 4220 - Pyromancy: Acid Surge
            0,           // 4230
            0,           // 4240
            0,           // 4250
            0,           // 4260
            0,           // 4270
            0,           // 4280
            0,           // 4290
            9,           // 4300 - Pyromancy: Iron Flesh
            9,           // 4310 - Pyromancy: Flash Sweat
            0,           // 4320
            0,           // 4330
            0,           // 4340
            0,           // 4350
            21,          // 4360 - Pyromancy: Undead Rapport
            0,           // 4370
            0,           // 4380
            0,           // 4390
            3,           // 4400 - Pyromancy: Power Within
            0,           // 4410
            0,           // 4420
            0,           // 4430
            0,           // 4440
            0,           // 4450
            0,           // 4460
            0,           // 4470
            0,           // 4480
            0,           // 4490
            12,          // 4500 - Pyromancy: Great Chaos Fireball
            60,          // 4510 - Pyromancy: Chaos Storm
            240,         // 4520 - Pyromancy: Chaos Fire Whip
            24,          // 4530 - Pyromancy: Black Flame
            0,           // 4540
            0,           // 4550
            0,           // 4560
            0,           // 4570
            0,           // 4580
            0,           // 4590
            0,           // 4600
            0,           // 4610
            0,           // 4620
            0,           // 4630
            0,           // 4640
            0,           // 4650
            0,           // 4660
            0,           // 4670
            0,           // 4680
            0,           // 4690
            0,           // 4700
            0,           // 4710
            0,           // 4720
            0,           // 4730
            0,           // 4740
            0,           // 4750
            0,           // 4760
            0,           // 4770
            0,           // 4780
            0,           // 4790
            0,           // 4800
            0,           // 4810
            0,           // 4820
            0,           // 4830
            0,           // 4840
            0,           // 4850
            0,           // 4860
            0,           // 4870
            0,           // 4880
            0,           // 4890
            0,           // 4900
            0,           // 4910
            0,           // 4920
            0,           // 4930
            0,           // 4940
            0,           // 4950
            0,           // 4960
            0,           // 4970
            0,           // 4980
            0,           // 4990
            15,          // 5000 - Miracle: Heal
            9,           // 5010 - Miracle: Great Heal
            3,           // 5020 - Miracle: Great Heal Excerpt
            9,           // 5030 - Miracle: Soothing Sunlight
            6,           // 5040 - Miracle: Replenishment
            6,           // 5050 - Miracle: Bountiful Sunlight
            0,           // 5060
            0,           // 5070
            0,           // 5080
            0,           // 5090
            120,         // 5100 - Miracle: Gravelord Sword Dance
            120,         // 5110 - Miracle: Gravelord Greatsword Dance
            0,           // 5120
            0,           // 5130
            0,           // 5140
            0,           // 5150
            0,           // 5160
            0,           // 5170
            0,           // 5180
            0,           // 5190
            3,           // 5200 - Miracle: Escape Death
            3,           // 5210 - Miracle: Homeward
            0,           // 5220
            0,           // 5230
            0,           // 5240
            0,           // 5250
            0,           // 5260
            0,           // 5270
            0,           // 5280
            0,           // 5290
            63,          // 5300 - Miracle: Force
            9,           // 5310 - Miracle: Wrath of the Gods
            18,          // 5320 - Miracle: Emit Force
            0,           // 5330
            0,           // 5340
            0,           // 5350
            0,           // 5360
            0,           // 5370
            0,           // 5380
            0,           // 5390
            15,          // 5400 - Miracle: Seek Guidance
            0,           // 5410
            0,           // 5420
            0,           // 5430
            0,           // 5440
            0,           // 5450
            0,           // 5460
            0,           // 5470
            0,           // 5480
            0,           // 5490
            30,          // 5500 - Miracle: Lightning Spear
            30,          // 5510 - Miracle: Great Lightning Spear
            15,          // 5520 - Miracle: Sunlight Spear
            0,           // 5530
            0,           // 5540
            0,           // 5550
            0,           // 5560
            0,           // 5570
            0,           // 5580
            0,           // 5590
            12,          // 5600 - Miracle: Magic Barrier
            6,           // 5610 - Miracle: Great Magic Barrier
            0,           // 5620
            0,           // 5630
            0,           // 5640
            0,           // 5650
            0,           // 5660
            0,           // 5670
            0,           // 5680
            0,           // 5690
            12,          // 5700 - Miracle: Karmic Justice
            0,           // 5710
            0,           // 5720
            0,           // 5730
            0,           // 5740
            0,           // 5750
            0,           // 5760
            0,           // 5770
            0,           // 5780
            0,           // 5790
            15,          // 5800 - Miracle: Tranquil Walk of Peace
            6,           // 5810 - Miracle: Vow of Silence
            0,           // 5820
            0,           // 5830
            0,           // 5840
            0,           // 5850
            0,           // 5860
            0,           // 5870
            0,           // 5880
            0,           // 5890
            3,           // 5900 - Miracle: Sunlight Blade
            3,           // 5910 - Miracle: Darkmoon Blade

        };

        uint64_t SlotBase = *((uint64_t*)(BaseB + 0x10));
        SlotBase = *((uint64_t*)(SlotBase + 0x418));
        SlotBase = SlotBase + 0x18;

        // For each of the ten attunement slots...
        for (int i = 0; i < 11; i++) {

            // Read the spell ID
            uint32_t spellID = *(uint32_t*)SlotBase;

            // Check for unused slot
            if (spellID == -1 || spellID == 0) {
                SlotBase += 8;
                continue;
            }

            // Lookup spell quantity
            uint32_t spellQuantity = spells[(spellID / 10) - 300];

            // Replenish spells
            *(uint32_t*)(SlotBase + 4) = spellQuantity;

            // Next slot
            SlotBase += 8;

        }
    }
}

static const uint64_t disable_low_fps_disconnect_offset = 0x778B29;

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

    sp::mem::pointer anim_id = sp::mem::pointer<int32_t>((void*)Game::world_chr_man_imp, { 0x68, 0x30, 0x5D0, 0x690 });
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

    sp::mem::pointer anim_id = sp::mem::pointer<int32_t>((void*)Game::world_chr_man_imp, { 0x68, 0x30, 0x5D0, 0x13B0 });
    if (anim_id.resolve() == NULL) {
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
    if (animationMediator.resolve() == NULL)
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
    if (timer.resolve() == NULL) {
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

    sp::mem::pointer position_ptr = sp::mem::pointer<float>((void*)(Game::world_chr_man_imp), { 0x68, 0x68, 0x28, 0x10 });
    if (position_ptr.resolve() == NULL) {
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
static const uint64_t gui_hpbar_value_offset = 0x676ECD;

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
    else if (Mod::fix_hp_bar_size && Mod::legacy_mode)
    {
        current_hpbar_max = corrected_vanilla_hpbar_max;
    }
    else if (Mod::fix_hp_bar_size && !Mod::legacy_mode)
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

std::optional<uint8_t> Game::get_area_number() {
    if (area_num_cache) {
        return *area_num_cache;
    }

    sp::mem::pointer area_num = sp::mem::pointer<uint8_t>((void*)(Game::frpg_net_base), { 0xA22 });
    if (area_num.resolve() == NULL) {
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
    if (world_num.resolve() == NULL)
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

// returns PlayerIns
std::optional<uint64_t> Game::get_connected_player(uint32_t i) {
    //go to the given index in the connectedPlayers_ChrSlotArray, grab the first value (pointer to PlayerIns) and return it
    if (connected_players_array_cache) {
        return *(uint64_t*)(connected_players_array_cache + (0x38 * (i + 1)));
    }

    //gets a pointer to the connectedPlayers_ChrSlotArray stored in the player's PlayerIns
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
    if (SessionManagerImp_session_action_result_cache)
    {
        return static_cast<SessionActionResultEnum>(*SessionManagerImp_session_action_result_cache);
    }

    sp::mem::pointer session_action_result = sp::mem::pointer<uint32_t>((void*)(Game::session_man_imp), { 0xf8 });
    if (session_action_result.resolve() == NULL)
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
    if (SteamSessionLight.resolve() == NULL)
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
    if (NextPlayerNum.resolve() == NULL)
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
    if (playerIns.resolve() == NULL)
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

std::optional<void*> Game::get_host_player_gamedata()
{
    if (host_player_gamedata_cache)
    {
        return *host_player_gamedata_cache;
    }

    sp::mem::pointer host_player_gamedata = sp::mem::pointer<void*>((void*)(Game::game_data_man), { 0x10 });
    if (host_player_gamedata.resolve() == NULL)
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

uint64_t Game::get_synced_time()
{
    return Game::get_accurate_time() + ModNetworking::timer_offset;
}

//convert the time we get from accurate/synced time (unit of 100ns) to the amount required for the animation entry offset value (unit of seconds)
float Game::convert_time_to_offset(uint64_t time)
{
    return time / 10000000.0f;
}

typedef void FUN_1406e8a60_Typedef(uint64_t unk, const wchar_t* str);
FUN_1406e8a60_Typedef* FUN_1406e8a60 = (FUN_1406e8a60_Typedef*)0x1406e8a60;

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
            uint64_t pcVar7 = pcVar3 + i * 0x40 + 0x10;
            if (pcVar7 != NULL)
            {
                *(uint64_t*)(pcVar7 + 0x8) = 0xffffffffffffffff;
                *(uint32_t*)(pcVar7 + 0x4) = 0xffffffff;
                *(uint32_t*)(pcVar7 + 0x18) = 1;
                *(uint32_t*)(pcVar7 + 0x34) = 0x1;
                *(uint32_t*)(pcVar7 + 0x38) = 0x7;
                *(uint8_t*)(pcVar7 + 0x30) = 1;
            }
            FUN_1406e8a60(pcVar3, msg);
            *(uint16_t*)(pcVar7 + 0x30) = 0x101;
            *(uint32_t*)(pcVar3 + 0x8) = *(uint32_t*)(pcVar3 + 0x150);
            break;
        }
        i = i + 1;
    } while (i < 5);
}
