/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Ashley            -  Anti-Tele-Backstab
        Metal-crow        -  in-line ASM fixes
        Sean Pesce        -  C++
*/


#include "AntiCheat.h"
#include "DllMain.h"

#define DS1_AC_TBP_STORE_ANIM_INJECTION_OFFSET 0xA06A49
#define      DS1_AC_TBP_CHECK_INJECTION_OFFSET 0xABDBB9

namespace AntiCheat {
namespace TeleBackstabProtect {

bool      active = true;
uint32_t  check_return;
uint32_t  store_anim_id_return;
uint32_t  anim_id;
uint32_t *anim_id_ptr = &anim_id;
uint32_t  no_move_start;
float     math_storage;
float    *math_storage_ptr = &math_storage;


void start() {
    no_move_start = ((uint32_t)Game::ds1_base) + 0xABDBE5;
    if (active) {
        if (!print_console("    Enabling TeleBackstabProtect..."))
            Mod::startup_messages.push_back("    Enabling TeleBackstabProtect...");

        // Inject function to store new animation ID
        uint8_t *write_address = (uint8_t*)(((uint32_t)Game::ds1_base) + DS1_AC_TBP_STORE_ANIM_INJECTION_OFFSET);
        set_mem_protection(write_address, 5, MEM_PROTECT_RWX);
        inject_jmp_5b(write_address, &store_anim_id_return, 0, &asm_store_new_animation_id);

        // Inject main anti-cheat function
        write_address = (uint8_t*)(((uint32_t)Game::ds1_base) + DS1_AC_TBP_CHECK_INJECTION_OFFSET);
        set_mem_protection(write_address, 6, MEM_PROTECT_RWX);
        inject_jmp_5b(write_address, &check_return, 1, &asm_check);
        SetLastError(ERROR_SUCCESS);
    } else {
        if (!print_console("    WARNING: TeleBackstabProtect is disabled"))
            Mod::startup_messages.push_back("    WARNING: TeleBackstabProtect is disabled");
        SetLastError(ERROR_BAD_ENVIRONMENT);
    }
}


void __declspec(naked) __stdcall asm_check() {
    // Assembly code below is injected into the function that moves the player
    __asm
    {
        // Check if movement is due to a backstab, and if so, check validity of movement
        //anti_tele_bs_check:
        // If in backstab animation
        cmp DWORD PTR[anim_id_ptr], 0x2328
        je  anti_tele_bs_check_is_backstab
        cmp DWORD PTR[anim_id_ptr], 0x24CC
        je  anti_tele_bs_check_is_backstab
        jmp no_tele_bs_detected
        anti_tele_bs_check_is_backstab:
        // If this is the player's position
        push eax
        push ebx
        mov eax, DWORD PTR ds:0x137D644
        mov eax, [eax + 0x3C]
        mov eax, [eax + 0x28]
        mov eax, [eax + 0x1C]
        mov ebx, [eax + 0x10]
        cmp[esi + 0x10], ebx
        jne no_tele_bs_detected_pre
        mov ebx, [eax + 0x18]
        cmp[esi + 0x18], ebx
        jne no_tele_bs_detected_pre
        pop ebx
        pop eax
        //anti_tele_bs_check__is_player:
        // If the distance to move is reasonable
        //save xmm0[0] and xmm1[0]
        movss DWORD PTR[math_storage_ptr], xmm1
        movss DWORD PTR[math_storage_ptr + 4], xmm0
        // Calculate distance of movement
        // old_x is [esi+0x10]; new_x is in xmm1
        subss xmm1, DWORD PTR[esi + 0x10] // new_x - old_x
        movss DWORD PTR[math_storage_ptr + 8], xmm1
        and DWORD PTR[math_storage_ptr + 8], 0x7FFFFFFF // Make result abs
        movss xmm1, DWORD PTR[math_storage_ptr + 8] // xmm1 = abs(new_x-old_x)
        mulss xmm1, xmm1 // xmm1 = abs(new_x-old_x)^2
        // old_y is [esi+0x18], new_y is in xmm0
        subss xmm0, DWORD PTR[esi + 0x18]
        movss DWORD PTR[math_storage_ptr + 8], xmm0
        and DWORD PTR[math_storage_ptr + 8], 0x7FFFFFFF
        movss xmm0, DWORD PTR[math_storage_ptr + 8]
        mulss xmm0, xmm0 // xmm0 = abs(new_y-old_y)^2
        // int(abs(new_x-old_x)^2 + abs(new_y-old_y)^2)
        addss xmm1, xmm0
        push eax
        cvttss2si eax, xmm1
        mov DWORD PTR[math_storage_ptr + 8], eax
        pop eax
        // Restore xmm0 and xmm1
        movss xmm1, DWORD PTR[math_storage_ptr]
        movss xmm0, DWORD PTR[math_storage_ptr + 4]
        // Reset our global
        mov DWORD PTR[anim_id_ptr], 0
        // int(abs(new_x-old_x)^2 + abs(new_y-old_y)^2) > 8^2
        cmp DWORD PTR[math_storage_ptr + 8], 0x40
        //jge DARKSOULS.exe + 0xABDBE5
        jge no_move // Too great a distance; don't move the player
        jmp no_tele_bs_detected
        // else continue normally
        no_tele_bs_detected_pre:
        pop ebx
        pop eax
        no_tele_bs_detected:
        movss[esp + 8], xmm0
        jmp check_return

        no_move:
        jmp no_move_start
    }
}


void __declspec(naked) __stdcall asm_store_new_animation_id() {
    // Assembly code below is injected into the function that sets the player's animation ID to a backstab
    __asm
    {
        mov esi, [eax + 0x28]
        mov DWORD PTR[anim_id_ptr], esi // Store the animation id
        xor ebp, ebp
        jmp store_anim_id_return
    }
}

} // namespace TeleBackstabProtect
} // namespace AntiCheat

#undef DS1_AC_TBP_STORE_ANIM_INJECTION_OFFSET
#undef DS1_AC_TBP_CHECK_INJECTION_OFFSET