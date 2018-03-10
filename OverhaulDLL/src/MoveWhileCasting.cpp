/*
DARK SOULS OVERHAUL

Contributors to this file:
Metal-Crow    -    Reverse engineering, C++

*/

#include "MoveWhileCasting.h"
#include "DllMain.h"

static const uint32_t spell_aids_to_allow_movement[] = {
    6200, 6202, 6204, 6205, 6206, 6208, 6210, 6215, 6217, 6218, 6219, 6224,
    6300, 6302, 6304, 6305, 6306, 6308, 6310, 6315, 6317, 6318, 6319, 6324,
    6400, 6402, 6404, 6406, 6408, 6410, 6411, 6417, 6418, 6419, 6424, 6426,
    6500, 6502, 6504, 6506, 6508, 6510, 6511, 6517, 6518, 6519, 6524, 6526
};

static uint32_t set_check2_var_to_zero_injection_return;
static uint32_t force_animation_to_attack_category_instead_of_spattack_injection_return;
static uint32_t return_animation_category_injection_return;

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

    //Force the casting animation to use the attack category instead of special attack category for the specified spell animation ids
    write_address = (uint8_t*)(CastingMovement::set_animation_category + ((uint32_t)Game::ds1_base));
    set_mem_protection(write_address, 11, MEM_PROTECT_RWX);
    inject_jmp_5b(write_address, &force_animation_to_attack_category_instead_of_spattack_injection_return, 6, &force_animation_to_attack_category_instead_of_spattack_injection);

    //Disable the TAE event for disabling rotation during spell casting

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


//this has to be stdcall so it trashes as few registers as possible
static void __stdcall helper_function_for_force_animation(uint32_t* animation_id_ptr, uint32_t subanimation_array_length, uint32_t animation_category) {
    //check that this is an SpAttack category
    if (animation_category != 3) {
        return;
    }

    //check if each subanimation is in the list
    for (uint32_t i = 0; i < subanimation_array_length; i++) {
        if (*animation_id_ptr == 6500) {
            print_console("found it!");
        }
    }
}

void __declspec(naked) __stdcall CastingMovement::force_animation_to_attack_category_instead_of_spattack_injection() {
    //Disable:
    //"DARKSOULS.exe" + 36A672:
    //mov eax, [esi + 04]
    //push eax
    //lea ecx, [esp + 0000011C]
    //Alt: db 8B 46 04 50 8D 8C 24 1C 01 00 00

    _asm {
        //these registers will be trashed by stdcall
        push eax
        push ecx
        push edx
        push [esi + 0x4] //animation category
        push [esi + 0xc] //length of subanimation array
        push [esp + 0x22c + 20] //ptr to animation struct array (+20 to acount for the pushs above)
        call helper_function_for_force_animation
        pop edx
        pop ecx
        pop eax
        //original code
        mov eax, [esi + 0x4]
        push eax
        lea ecx, [esp + 0x11C]
        //return
        jmp force_animation_to_attack_category_instead_of_spattack_injection_return
    }
}
