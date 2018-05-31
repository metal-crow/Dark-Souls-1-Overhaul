/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Metal-Crow    -    Reverse engineering, Bloodborne rally system, C++

*/

#include "BloodborneRallySystem.h"
#include "DllMain_Legacy.h"

#define DS1_BB_RALLY_SYS_OCCULT_MAT_ID 700

static uint32_t weapon_toggle_injection_return;
static uint32_t current_selected_bar_injection_return;
static uint32_t control_timer_injection_return;
static uint32_t main_rally_injection_return;

//Time in ms from game start
static uint32_t  time_ms_ptr;

// Time (in milliseonds) player was last hit
static uint32_t  beforehit_time;
static uint32_t *beforehit_time_ptr = &beforehit_time;

// HP of the player before last hit
static uint32_t  beforehit_hp;
static uint32_t *beforehit_hp_ptr = &beforehit_hp;

// UI bar currently in focus for the game to alter
static uint32_t  current_selected_bar;
static uint32_t *current_selected_bar_ptr = &current_selected_bar;

// Tracker for the number of hits the user has received
static uint32_t  gothit;
static uint32_t *gothit_ptr = &gothit;

void BloodborneRally::start() {
    if (!print_console("    Enabling Bloodborne Rally System...")) {
        Mod::startup_messages.push_back("    Enabling Bloodborne Rally System...");
    }

    time_ms_ptr = ((uint32_t)Game::fmodex_base + 0xC42AC); //offset from Game::fmodex_base that keeps track of ms from game start

    // Inject function to clear rally on weapon toggle
    uint8_t *write_address = (uint8_t*)(BloodborneRally::weapon_toggle_injection_offset + ((uint32_t)Game::ds1_base));
    set_mem_protection(write_address, 5, MEM_PROTECT_RWX);
    inject_jmp_5b(write_address, &weapon_toggle_injection_return, 0, &BloodborneRally::weapon_toggle_injection);

    // Inject function to track which ui bar is being smoothed
    write_address = (uint8_t*)(BloodborneRally::current_selected_bar_injection_offset + ((uint32_t)Game::ds1_base));
    set_mem_protection(write_address, 8, MEM_PROTECT_RWX);
    inject_jmp_5b(write_address, &current_selected_bar_injection_return, 3, &BloodborneRally::selected_bar_injection);

    // Inject function to control the timer for the current ui bar
    write_address = (uint8_t*)(BloodborneRally::control_timer_injection_offset + ((uint32_t)Game::ds1_base));
    set_mem_protection(write_address, 5, MEM_PROTECT_RWX);
    inject_jmp_5b(write_address, &control_timer_injection_return, 0, &BloodborneRally::control_timer_injection);

    // Inject function to perform the main rally code
    write_address = (uint8_t*)(BloodborneRally::main_rally_injection_offset + ((uint32_t)Game::ds1_base));
    set_mem_protection(write_address, 6, MEM_PROTECT_RWX);
    inject_jmp_5b(write_address, &main_rally_injection_return, 1, &BloodborneRally::main_rally_injection);
}

static DWORD WINAPI Apply_rally_capable_sfx_and_starting_hp(void*);

void BloodborneRally::on_char_load() {
    // Start new thread dedicated to applying rally-capable weapon sfx and setting the starting HP
    CreateThread(NULL, 0, Apply_rally_capable_sfx_and_starting_hp, NULL, 0, NULL);
    BloodborneRally::set_weapon_faith_requirements();
}

void BloodborneRally::set_weapon_faith_requirements() {
    SetLastError(ERROR_SUCCESS);
    if (Params::Weapon().base == NULL) {
        // Weapon parameters not loaded yet
        print_console("ERROR: Failed to set faith requirements for Occult weapons (wait for Weapon params to load)");
        SetLastError(ERROR_FILE_NOT_FOUND);
        return;
    }
    // @TODO: Un-comment next line when final faith requirement values are determined
    //BloodborneRally::set_default_weapon_faith_requirements();
    for (int i = 0; i < (int)Params::Weapon().param_count; i++) {
        if (Params::Weapon().get(i)->materialSetId == DS1_BB_RALLY_SYS_OCCULT_MAT_ID) {
            // Set faith requirement for possible custom weapons (in case player is using custom game files)
            if (Params::Weapon().get(i)->properFaith <= 0) {
                // Sum of all base damage types (physical, lightning, fire, magic)
                int base_damage_sum = Params::Weapon().get(i)->attackBasePhysics + Params::Weapon().get(i)->attackBaseThunder + Params::Weapon().get(i)->attackBaseFire + Params::Weapon().get(i)->attackBaseMagic;
                // Faith requirement = (base_damage_sum / 12) + 3
                // @TODO: Un-comment next line when final faith requirement values are determined
                //Params::Weapon().get(i)->properFaith = 3 + (int)(base_damage_sum / 12);  // @TODO: Implement better algorithm to calculate a fair faith requirement for each weapon (should take weapon attack speed into consideration)
            }
        }
    }
}

// Sets faith requirements for the default weapons in the base (un-modded) game
void BloodborneRally::set_default_weapon_faith_requirements() {
    SetLastError(ERROR_SUCCESS);
    if (Params::Weapon().base == NULL) {
        // Weapon parameters not loaded yet
        print_console("ERROR: Failed to set faith requirements for Occult weapons (wait for Weapon params to load)");
        SetLastError(ERROR_FILE_NOT_FOUND);
        return;
    }
    // Values have been hand-tweaked for balance purposes
    ((WeaponParam*)Params::Weapon().get_by_id(100700))->properFaith = 10;  // Occult Dagger
    ((WeaponParam*)Params::Weapon().get_by_id(101700))->properFaith = 10;  // Occult Parrying Dagger
    ((WeaponParam*)Params::Weapon().get_by_id(103700))->properFaith = 12;  // Occult Knife
    ((WeaponParam*)Params::Weapon().get_by_id(200700))->properFaith = 13;  // Occult Shortsword
    ((WeaponParam*)Params::Weapon().get_by_id(201700))->properFaith = 13;  // Occult Longsword
    ((WeaponParam*)Params::Weapon().get_by_id(202700))->properFaith = 13;  // Occult Broadsword
    ((WeaponParam*)Params::Weapon().get_by_id(204700))->properFaith = 16;  // Occult Balder Side Sword
    ((WeaponParam*)Params::Weapon().get_by_id(210700))->properFaith = 16;  // Occult Darksword
    ((WeaponParam*)Params::Weapon().get_by_id(212700))->properFaith = 6;   // Occult Straight Sword Hilt
    ((WeaponParam*)Params::Weapon().get_by_id(300700))->properFaith = 14;  // Occult Bastard Sword
    ((WeaponParam*)Params::Weapon().get_by_id(301700))->properFaith = 16;  // Occult Claymore
    ((WeaponParam*)Params::Weapon().get_by_id(303700))->properFaith = 15;  // Occult Flamberge
    ((WeaponParam*)Params::Weapon().get_by_id(350700))->properFaith = 13;  // Occult Zweihander
    ((WeaponParam*)Params::Weapon().get_by_id(351700))->properFaith = 13;  // Occult Greatsword
    ((WeaponParam*)Params::Weapon().get_by_id(352700))->properFaith = 12;  // Occult Demon Great Machete
    ((WeaponParam*)Params::Weapon().get_by_id(400700))->properFaith = 18;  // Occult Scimitar
    ((WeaponParam*)Params::Weapon().get_by_id(401700))->properFaith = 19;  // Occult Falchion
    ((WeaponParam*)Params::Weapon().get_by_id(402700))->properFaith = 16;  // Occult Shotel
    ((WeaponParam*)Params::Weapon().get_by_id(450700))->properFaith = 1;   // Occult Server
    ((WeaponParam*)Params::Weapon().get_by_id(451700))->properFaith = 17;  // Occult Murakumo
    ((WeaponParam*)Params::Weapon().get_by_id(500700))->properFaith = 18;  // Occult Uchigatana
    ((WeaponParam*)Params::Weapon().get_by_id(501700))->properFaith = 17;  // Occult Washing Pole
    ((WeaponParam*)Params::Weapon().get_by_id(502700))->properFaith = 18;  // Occult Iaito
    ((WeaponParam*)Params::Weapon().get_by_id(600700))->properFaith = 10;  // Occult Mail Breaker
    ((WeaponParam*)Params::Weapon().get_by_id(601700))->properFaith = 13;  // Occult Rapier
    ((WeaponParam*)Params::Weapon().get_by_id(602700))->properFaith = 18;  // Occult Estoc
    ((WeaponParam*)Params::Weapon().get_by_id(604700))->properFaith = 14;  // Occult Ricard's Rapier
    ((WeaponParam*)Params::Weapon().get_by_id(700700))->properFaith = 10;  // Occult Hand Axe
    ((WeaponParam*)Params::Weapon().get_by_id(701700))->properFaith = 11;  // Occult Battle Axe
    ((WeaponParam*)Params::Weapon().get_by_id(703700))->properFaith = 1;   // Occult Butcher Knife
    ((WeaponParam*)Params::Weapon().get_by_id(705700))->properFaith = 11;  // Occult Gargoyle Tail Axe
    ((WeaponParam*)Params::Weapon().get_by_id(750700))->properFaith = 10;  // Occult Greataxe
    ((WeaponParam*)Params::Weapon().get_by_id(751700))->properFaith = 10;  // Occult Demon's Greataxe
    ((WeaponParam*)Params::Weapon().get_by_id(800700))->properFaith = 10;  // Occult Club
    ((WeaponParam*)Params::Weapon().get_by_id(801700))->properFaith = 11;  // Occult Mace
    ((WeaponParam*)Params::Weapon().get_by_id(802700))->properFaith = 11;  // Occult Morning Star
    ((WeaponParam*)Params::Weapon().get_by_id(803700))->properFaith = 11;  // Occult Warpick
    ((WeaponParam*)Params::Weapon().get_by_id(804700))->properFaith = 10;  // Occult Pickaxe
    ((WeaponParam*)Params::Weapon().get_by_id(809700))->properFaith = 10;  // Occult Reinforced Club
    ((WeaponParam*)Params::Weapon().get_by_id(810700))->properFaith = 10;  // Occult Blacksmith Hammer
    ((WeaponParam*)Params::Weapon().get_by_id(850700))->properFaith = 13;  // Occult Great Club
    ((WeaponParam*)Params::Weapon().get_by_id(855700))->properFaith = 13;  // Occult Large Club
    ((WeaponParam*)Params::Weapon().get_by_id(901700))->properFaith = 10;  // Occult Caestus
    ((WeaponParam*)Params::Weapon().get_by_id(902700))->properFaith = 8;   // Occult Claw
    ((WeaponParam*)Params::Weapon().get_by_id(1000700))->properFaith = 18; // Occult Spear
    ((WeaponParam*)Params::Weapon().get_by_id(1001700))->properFaith = 18; // Occult Winged Spear
    ((WeaponParam*)Params::Weapon().get_by_id(1002700))->properFaith = 18; // Occult Partizan
    ((WeaponParam*)Params::Weapon().get_by_id(1050700))->properFaith = 16; // Occult Pike
    ((WeaponParam*)Params::Weapon().get_by_id(1100700))->properFaith = 12; // Occult Halberd
    ((WeaponParam*)Params::Weapon().get_by_id(1103700))->properFaith = 12; // Occult Gargoyle's Halberd
    ((WeaponParam*)Params::Weapon().get_by_id(1106700))->properFaith = 12; // Occult Lucerne 
    ((WeaponParam*)Params::Weapon().get_by_id(1107700))->properFaith = 1;  // Occult Scythe
    ((WeaponParam*)Params::Weapon().get_by_id(1150700))->properFaith = 1;  // Occult Great Scythe
    ((WeaponParam*)Params::Weapon().get_by_id(1200700))->properFaith = 10; // Occult Short Bow
    ((WeaponParam*)Params::Weapon().get_by_id(1201700))->properFaith = 12; // Occult Longbow
    ((WeaponParam*)Params::Weapon().get_by_id(1202700))->properFaith = 12; // Occult Black Bow of Pharis
    ((WeaponParam*)Params::Weapon().get_by_id(1204700))->properFaith = 12; // Occult Composite Bow
    ((WeaponParam*)Params::Weapon().get_by_id(1600700))->properFaith = 12; // Occult Whip
    ((WeaponParam*)Params::Weapon().get_by_id(1601700))->properFaith = 12; // Occult Notched Whip
}

void __declspec(naked) __stdcall BloodborneRally::weapon_toggle_injection() {
    // Assembly code below is injected into the function that is triggered on weapon toggle

    //Disable:
    //"DARKSOULS.exe"+B37F31:
    //mov[ebx - 08], eax
    //mov eax, [ebx]
    //Alt: db 89 43 F8 8B 03
    __asm
    {
        mov [beforehit_time_ptr], 0 // Fake we got hit infinite time ago, to drop it
        mov [ebx - 0x8], eax
        mov eax, [ebx]
        jmp weapon_toggle_injection_return
    }
}

void __declspec(naked) __stdcall BloodborneRally::selected_bar_injection() {
    // Injected into the function which loops over all UI bars to be edited

    //Disable:
    //"DARKSOULS.exe" + 386D04:
    //inc eax
    //cmp eax, [esi + 10]
    //mov[esp + 28], eax
    //Alt: db 40 3B 46 10 89 44 24 28
    __asm
    {
        mov [current_selected_bar_ptr], eax
        inc eax
        cmp eax, [esi + 0x10]
        mov [esp + 0x28], eax
        jmp current_selected_bar_injection_return
    }
}

#define MAX_RALLY_RECOVERY_TIME_MS 5000 
#define MAX_DISPLAYABLE_HP 1900 // Amount of health GUI can actually display

void __declspec(naked) __stdcall BloodborneRally::control_timer_injection() {
    // Injected into the function which sets the smoothing/orange aspects of Ui bars

    //Disable:
    //"DARKSOULS.exe"+888CED:
    //movss[eax + 3C], xmm1
    //Alt: db F3 0F 11 48 3C
    __asm
    {
        //if this is the timer of interest
        cmp [current_selected_bar_ptr], 0x23
        je  start_controlling_timer
        cmp [current_selected_bar_ptr], 0x24
        je  start_controlling_timer
        cmp [current_selected_bar_ptr], 0x25
        je  start_controlling_timer
        cmp [current_selected_bar_ptr], 0x26
        je  start_controlling_timer
        jmp dont_control_timer

        start_controlling_timer:
        // Fix for coming into section with too many hits;
        //     can happen if pc gets hit but doesn't take hp _bar_ damage (since HP can extend past visual)
        //     causing this code not to get run immediatly after hit
        cmp [gothit_ptr], 2
        jle timer_check_continue
        mov [gothit_ptr], 1

        timer_check_continue:
        // Check timer
        push eax
        mov  eax, [beforehit_time_ptr]
        add  eax, MAX_RALLY_RECOVERY_TIME_MS
        push ebx
        mov  ebx, [time_ms_ptr]
        cmp  [ebx], eax // Compare current time to hittime+MAX RECOVERY TIME
        pop  ebx
        pop  eax
        jge  execute_orange_drop

        // If we got hit before the timer went down
        cmp [gothit_ptr], 2
        je  execute_partial_orange_drop
        jmp control_timer_injection_return

        // If timer is at desired point, drop orange
        execute_orange_drop:
        mov [gothit_ptr], 0
        mov DWORD PTR [eax + 0x3C], 0
        jmp control_timer_injection_return

        // If we got interrupted before previous drop, drop orange to the _previous_ hp value
        execute_partial_orange_drop:
        mov     [gothit_ptr], 1
        // Save sse's
        sub     esp, 0x10
        movdqu  [esp], xmm0
        sub     esp, 0x10
        movdqu  [esp], xmm1
        // Calculate previous_hp/max_hp. This should be location of orange
        //   NOTE: Max HP must be capped at _displayable_ max hp (1900 in GUI bar), otherwise get problems
        push    eax
        mov     eax, DWORD PTR ds:(0x00CE0FEB + 5)
        mov     eax, [eax]
        mov     eax, [eax + 0]
        mov     eax, [eax + 0x2D8] // Max HP
        cmp     eax, MAX_DISPLAYABLE_HP
        jg      cap_display_max_hp
        jmp     dont_cap_max_hp
        cap_display_max_hp:
        mov     eax, MAX_DISPLAYABLE_HP
        dont_cap_max_hp:
        movd    xmm0, eax
        pop     eax
        movss   xmm1, [beforehit_hp_ptr]
        divss   xmm1, xmm0
        movss   [eax + 0x3C], xmm1
        // Restore sse's
        movdqu  xmm1, [esp]
        add     esp, 0x10
        movdqu  xmm0, [esp]
        add     esp, 0x10
        jmp     control_timer_injection_return

        // If timer not interested, normal instruction
        dont_control_timer:
        movss   [eax + 0x3C], xmm1
        jmp     control_timer_injection_return
    }
}

#define DARKHAND_ID 0xDCB40
#define PRISCILLADAGGER_ID 0x19640
#define VELKASRAPIER_ID 0x93378
#define RALLY_EFFECT_ID 92000

static uint32_t lua_SetEventSpecialEffect_2 = 0xD611C0;

void __declspec(naked) __stdcall BloodborneRally::main_rally_injection() {
    // Inject into funciton that is called on hit

    //Disable:
    //"DARKSOULS.exe"+A6BFE6:
    //xorps xmm0, xmm0
    //sub esp, 14
    //Alt: db 0F 57 C0 83 EC 14
    __asm
    {
        // if (target entity is host)
        // {
        push ebx
        mov  ebx, DWORD PTR ds:0x137DC70
        mov  ebx, [ebx + 4] // ebx is entityPointer to local player
        cmp  [ebx], eax // if entityPointer for local player == target (eax)
        pop  ebx
        jne  check_if_player_attacking
        // Save current time and player HP
        push ebx
        mov  ebx, [eax + 0x2D4] // Player HP
        mov  [beforehit_hp_ptr], ebx
        push eax
        mov  eax, [time_ms_ptr]
        mov  ebx, [eax] // Current time (ms)
        pop  eax
        mov  [beforehit_time_ptr], ebx
        inc  [gothit_ptr] // Marker for getting hit (used for UI)
        cmp  [gothit_ptr], 2
        jle  gothit_bounds_check
        mov  [gothit_ptr], 2
        gothit_bounds_check:
        pop  ebx
        jmp  track_onhit_data_exit
        // }

        // if (attacking entity is host)
        // {
        check_if_player_attacking:
        push ebx
        mov  ebx, DWORD PTR ds:0x137DC70
        mov  ebx, [ebx + 4]
        cmp  [ebx], esi // Attacker (esi)
        pop  ebx
        jne  track_onhit_data_exit
        // Rally code
        push eax
        push ebx
        push edx
        push ecx
        // if (currenttime-storedtime < MAX RECOVERY TIME
        mov  eax, [beforehit_time_ptr]
        add  eax, MAX_RALLY_RECOVERY_TIME_MS
        push ebx
        mov  ebx, [time_ms_ptr]
        cmp  eax, [ebx]
        pop  ebx
        jl   track_onhit_data_exit_cleanup
        // && weapon_is_occult)
        // Need to load weapon used data
        mov  eax, DWORD PTR ds:0x0137D644
        mov  eax, [eax + 0x3C]
        mov  eax, [eax + 0x30]
        mov  eax, [eax + 0xC]
        mov  eax, [eax + 0x654]
        mov  ebx, [ebp + 0xC] // Grab attack_data ptr
        cmp  DWORD PTR [ebx + 0x44], -1 // Indicates which hand used in attack
        je   right_hand_wep_atk
        cmp  DWORD PTR [ebx + 0x44], -2
        je   left_hand_wep_atk
        jmp  track_onhit_data_exit_cleanup
        right_hand_wep_atk:
        mov  eax, [eax + 0x1F8] // Grabs the weapon's ID
        jmp  check_is_weapon_occult
        left_hand_wep_atk:
        mov  eax, [eax + 0x1B4]
        check_is_weapon_occult:
        mov  ecx, eax // Save weaponID since we'll trash it but need it again
        cmp  eax, DARKHAND_ID
        je   weapon_is_occult
        // Priscilla's Dagger +1,+2,etc
        cmp  eax, PRISCILLADAGGER_ID
        je   weapon_is_occult
        cmp  eax, (PRISCILLADAGGER_ID + 1)
        je   weapon_is_occult
        cmp  eax, (PRISCILLADAGGER_ID + 2)
        je   weapon_is_occult
        cmp  eax, (PRISCILLADAGGER_ID + 3)
        je   weapon_is_occult
        cmp  eax, (PRISCILLADAGGER_ID + 4)
        je   weapon_is_occult
        cmp  eax, (PRISCILLADAGGER_ID + 5)
        je   weapon_is_occult
        // Velka's Rapier +1,+2,etc
        cmp  eax, VELKASRAPIER_ID
        je   weapon_is_occult
        cmp  eax, (VELKASRAPIER_ID + 1)
        je   weapon_is_occult
        cmp  eax, (VELKASRAPIER_ID + 2)
        je   weapon_is_occult
        cmp  eax, (VELKASRAPIER_ID + 3)
        je   weapon_is_occult
        cmp  eax, (VELKASRAPIER_ID + 4)
        je   weapon_is_occult
        cmp  eax, (VELKASRAPIER_ID + 5)
        je   weapon_is_occult
        mov  edx, 0 // Clear dividend
        mov  ebx, 100 // Occult upgrade (check if hundreds place == 7 via weaponid/100 % 10)
        div  ebx // eax is set to div 100
        mov  edx, 0
        mov  ebx, 10
        div  ebx // edx is set to modulo 10 (weapon path)
        cmp  edx, 7
        je   weapon_is_occult
        jmp  track_onhit_data_exit_cleanup

        weapon_is_occult:
        // Get weapon scaling
        mov     eax, ecx
        // Save sse's
        sub     esp, 0x10
        movdqu  [esp], xmm0
        sub     esp, 0x10
        movdqu  [esp], xmm1
        mov     edx, 0
        mov     ebx, 10
        div     ebx // edx is set to modulo 10 (weapon upgrade)
        // Calculate (0.05+upgrade/10)
        movd    xmm0, edx
        mov     edx, 10
        movd    xmm1, edx
        divss   xmm0, xmm1
        mov     edx, 0x3d4ccccd // 0.05
        movd    xmm1, edx
        addss   xmm0, xmm1 // xmm0 is our scaler
        // Calculate scaled recovery
        mov     ebx, [ebp + 0xC] // Re-grab attack_data ptr
        mov     eax, [ebx + 0x16C] // Damage is attack_data+0x16C
        movd    xmm1, eax
        mulss   xmm0, xmm1
        movd    eax, xmm0 // Scaled recovery HP (eax is scaled dmg)
        // Restore sse's
        movdqu  xmm1, [esp]
        add     esp, 0x10
        movdqu  xmm0, [esp]
        add     esp, 0x10

        add  eax, [esi + 0x2D4] // New possible HP after rally
        // Playerhp = min(prehithp, playerhp+Recoveryhp)
        cmp  eax, [beforehit_hp_ptr]
        jle  set_new_rally_hp
        mov  eax, [beforehit_hp_ptr]
        set_new_rally_hp:
        mov  [esi + 0x2D4], eax

        // Apply the rally sfx
        push  RALLY_EFFECT_ID // effect ID
        push  10000 // character ID
        call  [lua_SetEventSpecialEffect_2]
        // }

        track_onhit_data_exit_cleanup:
        pop ecx
        pop edx
        pop ebx
        pop eax
        // Exit normally
        track_onhit_data_exit:
        xorps xmm0, xmm0
        sub   esp, 0x14
        jmp main_rally_injection_return
    }
}

#define RALLY_CAPABLE_WEAPON_EFFECT_ID_RHAND 92001
#define RALLY_CAPABLE_WEAPON_EFFECT_ID_LHAND 92002

static DWORD WINAPI Apply_rally_capable_sfx_and_starting_hp(void* unused) {
    SpPointer rh_weapon_id ((void*)((uint32_t)Game::ds1_base + 0xF7D644), { 0x3c, 0x30, 0xc, 0x654, 0x1f8 });
    SpPointer lh_weapon_id ((void*)((uint32_t)Game::ds1_base + 0xF7D644), { 0x3c, 0x30, 0xc, 0x654, 0x1b4 });

    int char_status = DS1_PLAYER_STATUS_LOADING;

    uint32_t weaponid_R = 0;
    uint32_t weaponid_L = 0;

    while (true) {
        //prevent first hit having a beforehit hp of zero
        beforehit_hp_ptr = (uint32_t*)UINT16_MAX;

        //only apply the rally sfx if character is loaded
        Game::player_char_status.read(&char_status);

        while (char_status != DS1_PLAYER_STATUS_LOADING) {
            rh_weapon_id.read(&weaponid_R);
            lh_weapon_id.read(&weaponid_L);

            //There appears to be some bug here where both the effects cannot be applied simultaneously
            //But it just alternates which is applied, so it works ok
            if ((weaponid_R >= PRISCILLADAGGER_ID && weaponid_R <= (PRISCILLADAGGER_ID + 5)) ||
                (weaponid_R >= VELKASRAPIER_ID && weaponid_R <= (VELKASRAPIER_ID + 5)) ||
                (weaponid_R / 100 % 10 == 7)
                )
            {
                __asm {
                    push  RALLY_CAPABLE_WEAPON_EFFECT_ID_RHAND
                    push  10000
                    call[lua_SetEventSpecialEffect_2]
                }
            }

            if ((weaponid_L >= PRISCILLADAGGER_ID && weaponid_L <= (PRISCILLADAGGER_ID + 5)) ||
                (weaponid_L >= VELKASRAPIER_ID && weaponid_L <= (VELKASRAPIER_ID + 5)) ||
                (weaponid_L / 100 % 10 == 7)
                )
            {
                __asm {
                    push  RALLY_CAPABLE_WEAPON_EFFECT_ID_LHAND
                    push  10000
                    call[lua_SetEventSpecialEffect_2]
                }
            }

            Game::player_char_status.read(&char_status);
        }

        Sleep(500);
    }

    return 0;
}

#undef DS1_BB_RALLY_SYS_OCCULT_MAT_ID
