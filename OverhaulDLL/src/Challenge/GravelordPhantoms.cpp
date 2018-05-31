/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        IAmV3nom     -  Original CE script
        Jellybaby34  -  Original CE script
        Sean Pesce   -  C++
        Xiang        -  Original CE script
*/

#include "DllMain_Legacy.h"
#include "Challenge/GravelordPhantoms.h"
#include "SP_AoB_Scan.hpp"

namespace Challenge {
namespace GravelordPhantoms {

bool     active = false;
void    *injection_address     = NULL;
void    *despawn_write_address = NULL;
uint32_t asm_return_address    = 0;

// Spawns additional Gravelord phantom enemies (even if player hasn't been Gravelorded)
void enable()
{
    if (injection_address == NULL) {
        // First time enabling mod, scan for injection point
        injection_address = aob_scan("89 4B 30 3B C6 74 11 8B 54 24 28");
        if (injection_address == NULL) {
            print_console("ERROR: Failed to activate Gravelord Phantoms challenge mod (Injection point scan returned NULL)");
            SetLastError(ERROR_FILE_NOT_FOUND);
            return;
        }
    }
    if (!active) {
        print_console("Activating Gravelord phantoms challenge mod");
        set_mem_protection(injection_address, 5, MEM_PROTECT_RWX);
        inject_jmp_5b((uint8_t*)injection_address, &asm_return_address, 0, asm_enable_gravelord_phantoms);
        active = true;
    } else {
        SetLastError(ERROR_BAD_ENVIRONMENT);
    }
}

// Disables auto-spawning of Gravelord phantoms. Also de-spawns additional Gravelord phantom enemies, but only if player is in NG+0.
//   If player is in NG+1 or higher, they have to wait 10 minutes for the phantoms
//   to disappear on their own. This is to prevent the user from de-spawning the
//   additional enemies in the event that they were legitimately Gravelorded by
//   another player (which can only happen in NG+1 or higher).
void disable()
{
    // Note: The last byte in each "original bytes" array isn't part of the original data,
    //       just a backup measure to make sure they don't get picked up when pattern scanning
    //       for the injection/despawn addresses
    uint8_t original_injection_address_bytes[6] = { 0x89, 0x4B, 0x30, 0x3B, 0xC6, 0x00 };
    if (active) {
        print_console("Deactivating Gravelord phantoms auto-spawning...");
        apply_byte_patch(injection_address, original_injection_address_bytes, 5);
        active = false;
        despawn_gravelord_phantoms();
    } else {
        SetLastError(ERROR_BAD_ENVIRONMENT);
    }
}

// De-spawns existing Gravelord Phantoms (only if player is in NG+0)
void despawn_gravelord_phantoms()
{
    uint8_t original_despawn_bytes[3] = { 0x72, 0x0F, 0x00 };
    uint8_t          despawn_bytes[2] = { 0x90, 0x90 };
    SpPointer newgame_level = SpPointer((void*)((uint32_t)Game::ds1_base + 0xF78700), { 0x3C  });
    SpPointer player_status = SpPointer((void*)((uint32_t)Game::ds1_base + 0xF7E204), { 0xA28 });
    if (*(uint32_t*)newgame_level.resolve() == 0 || Mod::cheats) {
        if (player_status.resolve() != NULL && (*(uint32_t*)player_status.resolve() == 0 || *(uint32_t*)player_status.resolve() == 8)) {
            print_console("De-spawning existing Gravelord phantoms...");
            if (despawn_write_address == NULL) {
                // First time despawning Gravelord phantoms; scan for write address
                despawn_write_address = aob_scan("72 0F 8A 51 08 B0 01 32 D0 22 D0 30 51 08 C2 08 00");
                if (despawn_write_address == NULL) {
                    print_console("ERROR: Failed to de-spawn Gravelord Phantoms (Write address scan returned NULL)");
                    SetLastError(ERROR_FILE_NOT_FOUND);
                    return;
                }
            }
            apply_byte_patch(despawn_write_address, despawn_bytes, 2);
            Sleep(50); // Wait for phantoms to de-spawn
            apply_byte_patch(despawn_write_address, original_despawn_bytes, 2);
        }
    } else {
        print_console("NG+ detected; existing Gravelord phantoms will de-spawn in 10 minutes.");
    }
}

// Toggles Gravelord phantoms challenge mod
void toggle()
{
    if (active) {
        disable();
    } else {
        enable();
    }
}

// Injected assembly function that enables Gravelord phantoms challenge mod
void __declspec(naked) __stdcall asm_enable_gravelord_phantoms() {
    __asm
    {
        mov byte ptr[ebx + 0x30], 0xFF
        cmp eax, esi
        jmp asm_return_address
    }
}

} // namespace BlackPhantomEnemies
} // namespace Challenge
