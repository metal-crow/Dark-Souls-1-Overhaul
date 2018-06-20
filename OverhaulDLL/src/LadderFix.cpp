/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Sean Pesce  -  C++ & Ladder glitch fix technique
*/


#include "LadderFix.h"
#include "DllMain.h"

namespace LadderFix {

bool enable_pref = true;

constexpr const size_t patch_byte_count = 7;
const static uint8_t*  injection_point  = (uint8_t*)0xD5FBE5; // DARKSOULS.exe+0x95FBE5
const static uint8_t   original_bytes[patch_byte_count] = { 0x83, 0x40, 0x04, 0xFF, 0x8B, 0x50, 0x04 };

uint32_t return_address = 0;
constexpr const uint32_t exit_ladder_return_address = 0xD5FBFD; // DARKSOULS.exe+0x95FBFD

bool is_active()
{
    return (*reinterpret_cast<const uint8_t*>(injection_point)) != original_bytes[0];
}

void apply()
{
    if (!is_active())
    {
        print_console("Applying experimental ladder fix...");
        set_mem_protection(const_cast<uint8_t*>(injection_point), patch_byte_count, MEM_PROTECT_RWX);
        inject_jmp_5b(const_cast<uint8_t*>(injection_point), &return_address, 2, &ladder_fix);
    }
}

void unpatch()
{
    if (is_active())
    {
        print_console("Disabling experimental ladder fix...");
        forget_injection((uint32_t)injection_point);
        apply_byte_patch(const_cast<uint8_t*>(injection_point), original_bytes, patch_byte_count);
    }
}

// Injected ladder glitch fix assembly function
void __declspec(naked) __stdcall ladder_fix()
{
    __asm
    {
        // Original code:
        add dword ptr [eax+0x04],-0x01
        mov edx,[eax+0x04] // Current ladder rung now stored in edx

        // Save registers
        push eax

        // Get pointer to player animation ID
        mov eax, 0x12E29E8
        mov eax, [eax]
        mov eax, [eax]
        mov eax, [eax+0xFF0]

        // Check if player is sliding down ladder (animation ID = 7000 || 7001 || 7005)
        cmp eax, 0x1B58 // 7000
        je is_sliding
        cmp eax, 0x1B59 // 7001
        je is_sliding
        cmp eax, 0x1B5D // 7005
        je is_sliding

        // Not sliding down, leave code cave
        jmp cleanup

        is_sliding:
        // Check if player is on rung 0, 1, or 2
        cmp edx, 2
        jg cleanup // Not on low rung

        // More original code:
        mov ecx, [ecx+0x28]
        mov edx, [ecx+0x2C]

        // Prep ecx with value to exit ladder
        mov ecx, 5

        // Restore eax and "return" to instruction for exiting ladder
        pop eax
        jmp exit_ladder_return_address

        cleanup:
        // Restore registers & fix sign flag
        pop eax
        test eax, eax

        jmp return_address;
    }
}

} // namespace LadderFix

