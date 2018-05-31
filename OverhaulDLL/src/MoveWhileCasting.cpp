/*
    DARK SOULS OVERHAUL

    Contributors to this file:
    Metal-Crow    -    Reverse engineering, C++

*/

#include "MoveWhileCasting.h"
#include "DllMain_Legacy.h"
#include "GameData.h"

static uint32_t endaid_arg1;
static uint32_t endaid_arg4;
static DWORD WINAPI CallSetAnimationIdFunc(void* delay_arg);

static const std::tuple<uint32_t, float> SPELL_AIDS_TO_ALLOW_MOVEMENT[] = {
    {6200, 0.5f}, {6202, 1.0f}, {6204, 1.0f}, {6205, 1.5f}, {6206, 1.833333f}, {6208, 1.333333f}, {6210, 1.5f}, {6215, 1.833333f}, {6217, 1.333333f}, {6218, 1.666667f}, {6219, 1.333333f}, {6224, 1.666667f},
    {6300, 0.5f}, {6302, 1.0f}, {6304, 1.0f}, {6305, 1.5f}, {6306, 1.833333f}, {6308, 1.333333f}, {6310, 1.5f}, {6315, 1.833333f}, {6317, 1.333333f}, {6318, 1.666667f}, {6319, 1.333333f}, {6324, 1.666667f},
    {6400, 1.5f}, {6402, 1.666667f}, {6404, 1.833333f}, {6406, 1.633333f}, {6408, 1.833333f}, {6410, 2.333333f}, {6411, 1.666667f}, {6417, 2.0f}, {6418, 1.9f}, {6419, 1.666667f}, {6424, 1.833333f}, {6426, 2.333333f},
    {6500, 1.5f}, {6502, 1.666667f}, {6504, 1.833333f}, {6506, 1.633333f}, {6508, 1.833333f}, {6510, 2.333333f}, {6511, 1.666667f}, {6517, 2.0f}, {6518, 1.9f}, {6519, 1.666667f}, {6524, 1.833333f}, {6526, 2.333333f},
};

static uint32_t set_check2_var_to_zero_injection_return;
static uint32_t force_animation_to_attack_category_instead_of_spattack_injection_return;
static uint32_t return_animation_category_injection_return;
static uint32_t bullet_genetation_notify_return;

void CastingMovement::start() {
    if (!print_console("    Enabling casting while moving patch...")) {
        Mod::startup_messages.push_back("    Enabling casting while moving patch...");
    }

    //Fix one of the checks that prevents WalkFB aid from being set
    uint8_t nop_patch[] = { 0x90, 0x90 };
    uint8_t *write_address = (uint8_t*)(CastingMovement::walkfb_check_1 + ((uint32_t)Game::ds1_base));
    apply_byte_patch(write_address, nop_patch, 2);

    //Fix one of the checks that blocks WalkFb by setting a variable to 0
    write_address = (uint8_t*)(CastingMovement::walkfb_check_2 + ((uint32_t)Game::ds1_base));
    set_mem_protection(write_address, 7, MEM_PROTECT_RWX);
    inject_jmp_5b(write_address, &set_check2_var_to_zero_injection_return, 2, &set_check2_var_to_zero_injection);

    //Fix one of the checks that blocks WalkFb by setting a variable to 0
    //Do via inline XOR and patch to uncond jump, instead of making code cave
    uint8_t patch3[] = { 0x31, 0xC0, 0xEB, 0x22 };
    write_address = (uint8_t*)(CastingMovement::walkfb_check_3 + ((uint32_t)Game::ds1_base));
    apply_byte_patch(write_address, patch3, 4);

    //Fix a write of -1 to WalkFB from happening in some cases
    write_address = (uint8_t*)(CastingMovement::walkfb_check_4 + ((uint32_t)Game::ds1_base));
    apply_byte_patch(write_address, nop_patch, 2);

    //Get notified upon a bullet being generated
    //Need to prevent glitch
    write_address = (uint8_t*)(CastingMovement::bullet_generation + ((uint32_t)Game::ds1_base));
    set_mem_protection(write_address, 6, MEM_PROTECT_RWX);
    inject_jmp_5b(write_address, &bullet_genetation_notify_return, 1, &bullet_genetation_notify);

    //Force the casting animation to use the attack category instead of special attack category for the specified spell animation ids
    write_address = (uint8_t*)(CastingMovement::set_animation_category + ((uint32_t)Game::ds1_base));
    set_mem_protection(write_address, 5, MEM_PROTECT_RWX);
    inject_jmp_5b(write_address, &force_animation_to_attack_category_instead_of_spattack_injection_return, 0, &force_animation_to_attack_category_instead_of_spattack_injection);

    //Disable the TAE events for disabling rotation during spell casting
    int aids_changed = 0;
    if (Game::characters_loaded && Game::player_tae.is_initialized())
    {
        for (int i = 0; i < sizeof(SPELL_AIDS_TO_ALLOW_MOVEMENT) / sizeof(std::tuple<uint32_t, float>); i++)
        {
            uint32_t aid = std::get<0>(SPELL_AIDS_TO_ALLOW_MOVEMENT[i]);

            int n_events = Game::player_tae.get_event_count_by_id(aid);
            for (int i = 0; i < n_events; i++) {
                if (Game::player_tae.get_event_type_by_id(aid, i) == 0 && Game::player_tae.get_event_param_by_id(aid, i, 0) == TAE_type0_param_values::lock_rotation) {
                    Game::player_tae.set_event_start_by_id(aid, i, 0.0f);
                    Game::player_tae.set_event_end_by_id(aid, i, 0.0f);
                    //print_console("Updated gesture " + std::to_string(aid));
                }
            }
        }
    }
}

void __declspec(naked) __stdcall CastingMovement::set_check2_var_to_zero_injection() {
    //Disable:
    //"DARKSOULS.exe" + A0F57E:
    //cmp dword ptr[ebx + 0000013C], -01
    //Alt: db 83 BB 3C 01 00 00 FF
    __asm {
        mov dword ptr[ebx + 0x13C], 0
        cmp dword ptr[ebx + 0x13C], -01
        jmp set_check2_var_to_zero_injection_return
    }
}

static uint8_t triggered_bullet = 0;

void __declspec(naked) __stdcall CastingMovement::bullet_genetation_notify() {
    //"DARKSOULS.exe" + C06178:
    //mov eax, [ecx + 04]
    //add esp, 04
    //Alt: db 8B 41 04 83 C4 04
    _asm {
        mov eax, [ecx + 4]
        //check if this is a PC generated bullet (0x10044000 corresponds to [[[BaseA]+0]+4] for the player character)
        cmp [edi+0x8C], 0x10044000
        jne exit_bullet_genetation_notify
        mov triggered_bullet, 1
        exit_bullet_genetation_notify:
        add esp, 4
        jmp bullet_genetation_notify_return
    }
}

//Dunno what this does, but needed to not destroy the ending animation
const uint32_t ATTACK_EVENT_STRUCT_DATA[] = { 0x2, 0x3DCCCCCD, 0x1, 0x0, 0x2, 0x2, 0x2, 0x0, 0x2, 0x0, 0x2 };

typedef struct {
    uint32_t VTable;
    uint32_t category;
    uint32_t attack_aid;
    uint32_t dword1;
    uint32_t spattack_aid;
    uint32_t data[sizeof(ATTACK_EVENT_STRUCT_DATA)/4]; //off 20
    uint8_t unknown[188]; //off 64
    uint32_t dword2; //off 252
} AnimationEventStruct;

//set this so drop the following SpAttack -1
static bool ignore_unset_spattack = false;

//this has to be stdcall so it trashes as few registers as possible
static void __stdcall helper_function_for_force_animation(AnimationEventStruct* animation_struct) {
    //check that this is an SpAttack category
    if (animation_struct->category != 3) {
        return;
    }

    //TODO disable setting aid to -1 by the previous attack animation so to allow continuous walking
    //The attack set to -1 happens before the Spattack we overwrite

    //check if this spattack is in the list
    for (int i = 0; i < sizeof(SPELL_AIDS_TO_ALLOW_MOVEMENT) / sizeof(std::tuple<uint32_t, float>);i++) {
        std::tuple<uint32_t, float> allow_aid = SPELL_AIDS_TO_ALLOW_MOVEMENT[i];
        if (animation_struct->spattack_aid == std::get<0>(allow_aid)) {
            //change the Event struct to be an attack type
            animation_struct->category = 1;
            animation_struct->attack_aid = animation_struct->spattack_aid;
            animation_struct->dword1 = 2;
            animation_struct->spattack_aid = 5;
            memcpy(&animation_struct->data, ATTACK_EVENT_STRUCT_DATA, sizeof(ATTACK_EVENT_STRUCT_DATA));

            ignore_unset_spattack = true;

            CreateThread(NULL, 0, CallSetAnimationIdFunc, (void*)&SPELL_AIDS_TO_ALLOW_MOVEMENT[i], 0, NULL);

            return;
        }
    }

    //The SpAttack unset that noramlly follows an animation is bugged to trigger the frame after we change the SpAttack to attack
    //so drop it by setting its category to an invalid number
    if (ignore_unset_spattack && animation_struct->spattack_aid == -1) {
        animation_struct->category = 0xffff;
        ignore_unset_spattack = false;
        return;
    }
}

void __declspec(naked) __stdcall CastingMovement::force_animation_to_attack_category_instead_of_spattack_injection() {
    //Disable:
    //"DARKSOULS.exe" + BDD623:
    //mov eax, [edi]
    //mov edx, [eax + 04]
    //Alt: db 8B 07 8B 50 04

    _asm {
        //save args so we can call the function later
        mov endaid_arg1, ecx
        push ecx
        mov ecx, [ebp + 0xC]
        mov endaid_arg4, ecx
        pop ecx

        //these registers will be trashed by stdcall
        push eax
        push ecx
        push edx

        push edi //arg
        call helper_function_for_force_animation

        pop edx
        pop ecx
        pop eax
        //original code
        mov eax, [edi]
        mov edx, [eax + 04]
        //return
        jmp force_animation_to_attack_category_instead_of_spattack_injection_return
    }
}

static const uint32_t SetAnimationIdFunc = 0x0FDD5E0; //function which properly sets an animation for a given category

#pragma warning( disable : 4244 )  //disable conversion from DWORD warning
static DWORD WINAPI CallSetAnimationIdFunc(void* delay_arg) {
    uint32_t start = Game::get_game_time_ms();
    uint32_t cur = 0;

    //Wait till animation is finished
    std::tuple<uint32_t, float> delay = *(std::tuple<uint32_t, float>*)delay_arg;

    while (cur < (start+(std::get<1>(delay)*1000)) && !triggered_bullet) {
        cur = Game::get_game_time_ms();
        Sleep(1);
    }
    triggered_bullet = 0;
    
    AnimationEventStruct animation_struct;
    animation_struct.VTable = 0x1104CB0;
    animation_struct.category = 1;
    animation_struct.attack_aid = -1;
    animation_struct.dword1 = 2;
    animation_struct.spattack_aid = 5;
    memcpy(&animation_struct.data, ATTACK_EVENT_STRUCT_DATA, sizeof(ATTACK_EVENT_STRUCT_DATA));
    animation_struct.dword2 = 5;
    uint32_t animation_struct_address = (uint32_t)&animation_struct;

    _asm {
        push endaid_arg4 //a4: unknown comparitor
        push animation_struct_address //a3: animation structure pointer
        mov  ecx, endaid_arg1//a1: EzStateActionEvent
        call[SetAnimationIdFunc]
    }
}
