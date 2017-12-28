#include "BloodborneRallySystem.h"
#include "DllMain.h"


static uint32_t weapon_toggle_injection_return;
static uint32_t current_selected_bar_injection_return;
static uint32_t control_timer_injection_return;
static uint32_t main_rally_injection_return;

//time (in milliseonds) player was last hit
static uint32_t  beforehit_time;
static uint32_t *beforehit_time_ptr = &beforehit_time;

//hp of the player before last hit
static uint32_t  beforehit_hp;
static uint32_t *beforehit_hp_ptr = &beforehit_hp;

//the Ui bar currently in focus for the game to alter
static uint32_t  current_selected_bar;
static uint32_t *current_selected_bar_ptr = &current_selected_bar;

//tracker for the number of hits the user has received
static uint32_t  gothit;
static uint32_t *gothit_ptr = &gothit;

void BloodborneRally::start() {
    if (!print_console("    Enabling Bloodborne Rally System...")) {
        Mod::startup_messages.push_back("    Enabling Bloodborne Rally System...");
    }

    // Inject function to clear rally on weapon toggle
    uint8_t *write_address = (uint8_t*)(BloodborneRally::weapon_toggle_injection_offset + ((uint32_t)Game::ds1_base));
    set_mem_protection(write_address, 5, MEM_PROTECT_RWX);
    inject_jmp_5b(write_address, &weapon_toggle_injection_return, 0, &BloodborneRally::weapon_toggle_injection);

    //Inject function to track which ui bar is being smoothed
    write_address = (uint8_t*)(BloodborneRally::current_selected_bar_injection_offset + ((uint32_t)Game::ds1_base));
    set_mem_protection(write_address, 8, MEM_PROTECT_RWX);
    inject_jmp_5b(write_address, &current_selected_bar_injection_return, 3, &BloodborneRally::selected_bar_injection);

    //Inject function to control the timer for the current ui bar
    write_address = (uint8_t*)(BloodborneRally::control_timer_injection_offset + ((uint32_t)Game::ds1_base));
    set_mem_protection(write_address, 5, MEM_PROTECT_RWX);
    inject_jmp_5b(write_address, &control_timer_injection_return, 0, &BloodborneRally::control_timer_injection);

    //Inject function to perform the main rally code
    write_address = (uint8_t*)(BloodborneRally::main_rally_injection_offset + ((uint32_t)Game::ds1_base));
    set_mem_protection(write_address, 6, MEM_PROTECT_RWX);
    inject_jmp_5b(write_address, &main_rally_injection_return, 1, &BloodborneRally::main_rally_injection);
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
        mov [beforehit_time_ptr], 0 //fake we got hit infinite time ago, to drop it
        mov [ebx - 0x8], eax
        mov eax, [ebx]
        jmp weapon_toggle_injection_return
    }
}

void __declspec(naked) __stdcall BloodborneRally::selected_bar_injection() {
    // Injected into the function which loops over all Ui bars to be edited

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
#define MAX_DISPLAYABLE_HP 1900 //amount of health GUI can actually display

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
        //fix for coming into section with too many hits
        //can happen if pc gets hit but doesn't take hp _bar_ damage (since hp can extend past visual)
        //causing this code not to get run immediatly after hit
        cmp [gothit_ptr], 2
        jle timer_check_continue
        mov [gothit_ptr], 1

        timer_check_continue:
        //check timer
        push eax
        mov  eax, [beforehit_time_ptr]
        add  eax, MAX_RALLY_RECOVERY_TIME_MS
        cmp  DWORD PTR ds:0x100C42AC, eax //compare current time to hittime+MAX RECOVERY TIME
        pop  eax
        jge  execute_orange_drop

        //if we got hit before the timer went down
        cmp [gothit_ptr], 2
        je  execute_partial_orange_drop
        jmp control_timer_injection_return

        //if timer is at desired point, drop orange
        execute_orange_drop:
        mov [gothit_ptr], 0
        mov DWORD PTR [eax + 0x3C], 0
        jmp control_timer_injection_return

        //if we got interrupted before previous drop, drop orange to the _previous_ hp value
        execute_partial_orange_drop:
        mov     [gothit_ptr], 1
        //save sse's
        sub     esp, 0x10
        movdqu  [esp], xmm0
        sub     esp, 0x10
        movdqu  [esp], xmm1
        //calculate previous_hp/max_hp. This should be location of orange
        //NOTE: max hp must be capped at _displayable_ max hp (1900 in gui bar), otherwise get problems
        push    eax
        mov     eax, DWORD PTR ds:(0x00CE0FEB + 5)
        mov     eax, [eax]
        mov     eax, [eax + 0]
        mov     eax, [eax + 0x2D8] //max hp
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
        //restore sse's
        movdqu  xmm1, [esp]
        add     esp, 0x10
        movdqu  xmm0, [esp]
        add     esp, 0x10
        jmp     control_timer_injection_return

        //if timer not interested, normal instruction
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
    //Inject into funciton that is called on hit

    //Disable:
    //"DARKSOULS.exe"+A6BFE6:
    //xorps xmm0, xmm0
    //sub esp, 14
    //Alt: db 0F 57 C0 83 EC 14
    __asm
    {
        //if (target entity is host)
        // {
        push ebx
        mov  ebx, DWORD PTR ds:0x137DC70
        mov  ebx, [ebx + 4] //ebx is entityPointer to local player
        cmp  [ebx], eax //if entityPointer for local player == target (eax)
        pop  ebx
        jne  check_if_player_attacking
        //save current time and player hp
        push ebx
        mov  ebx, [eax + 0x2D4] //player hp
        mov  [beforehit_hp_ptr], ebx
        mov  ebx, DWORD PTR ds:0x100C42AC //current time ms
        mov  [beforehit_time_ptr], ebx
        inc  [gothit_ptr] //marker for getting hit (used for ui)
        cmp  [gothit_ptr], 2
        jle  gothit_bounds_check
        mov  [gothit_ptr], 2
        gothit_bounds_check:
        pop  ebx
        jmp  track_onhit_data_exit
        // }

        //if (attacking entity is host)
        // {
        check_if_player_attacking:
        push ebx
        mov  ebx, DWORD PTR ds:0x137DC70
        mov  ebx, [ebx + 4]
        cmp  [ebx], esi //attacker (esi)
        pop  ebx
        jne  track_onhit_data_exit
        //rally code
        push eax
        push ebx
        push edx
        push ecx
        //if (currenttime-storedtime < MAX RECOVERY TIME
        mov  eax, [beforehit_time_ptr]
        add  eax, MAX_RALLY_RECOVERY_TIME_MS
        cmp  eax, DWORD PTR ds:0x100C42AC
        jl   track_onhit_data_exit_cleanup
        //&& weapon_is_occult)
        //need to load weapon used data
        mov  eax, DWORD PTR ds:0x0137D644
        mov  eax, [eax + 0x3C]
        mov  eax, [eax + 0x30]
        mov  eax, [eax + 0xC]
        mov  eax, [eax + 0x654]
        mov  ebx, [ebp + 0xC] //grab attack_data ptr
        cmp  DWORD PTR [ebx + 0x44], -1 //indicates which hand used in attack
        je   right_hand_wep_atk
        cmp  DWORD PTR [ebx + 0x44], -2
        je   left_hand_wep_atk
        jmp  track_onhit_data_exit_cleanup
        right_hand_wep_atk:
        mov  eax, [eax + 0x1F8] //grabs the weapon's id
        jmp  check_is_weapon_occult
        left_hand_wep_atk:
        mov  eax, [eax + 0x1B4]
        check_is_weapon_occult:
        mov  ecx, eax //save weaponid since we'll trash it but need it again
        cmp  eax, DARKHAND_ID
        je   weapon_is_occult
        //Priscilla's Dagger +1,+2,etc
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
        //Velka's Rapier +1,+2,etc
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
        mov  edx, 0 //clear dividend
        mov  ebx, 100 //Occult upgrade (check if hundreds place == 7 via weaponid/100 % 10)
        div  ebx //eax is set to div 100
        mov  edx, 0
        mov  ebx, 10
        div  ebx //edx is set to modulo 10 (weapon path)
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
        mov     edx, 0x3d4ccccd //0.05
        movd    xmm1, edx
        addss   xmm0, xmm1 //xmm0 is our scaler
        //calculate scaled recovery
        mov     ebx, [ebp + 0xC] //re-grab attack_data ptr
        mov     eax, [ebx + 0x16C] //damage is attack_data+16C
        movd    xmm1, eax
        mulss   xmm0, xmm1
        movd    eax, xmm0 //scaled recovery hp (eax is scaled dmg)
        //restore sse's
        movdqu  xmm1, [esp]
        add     esp, 0x10
        movdqu  xmm0, [esp]
        add     esp, 0x10

        add  eax, [esi + 0x2D4] //new possible hp after rally
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
        //exit normally
        track_onhit_data_exit:
        xorps xmm0, xmm0
        sub   esp, 0x14
        jmp main_rally_injection_return
    }
}