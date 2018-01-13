/*
DARK SOULS OVERHAUL

Contributors to this file:
Metal-Crow    -    Reverse engineering, C++

*/

#include "PhantomUnshackle.h"
#include "DllMain.h"

static uint32_t mp_zone_changing_injection_return;


void PhantomUnshackle::start() {
    if (!print_console("    Enabling Phantom Unshackle patch...")) {
        Mod::startup_messages.push_back("    Enabling Phantom Unshackle patch...");
    }

    // Inject function to prevent MP zone ID from being set to -2
    uint8_t *write_address = (uint8_t*)(PhantomUnshackle::mp_zone_changing_injection_offset + ((uint32_t)Game::ds1_base));
    set_mem_protection(write_address, 6, MEM_PROTECT_RWX);
    inject_jmp_5b(write_address, &mp_zone_changing_injection_return, 1, &mp_zone_changing_injection);
}

void __declspec(naked) __stdcall PhantomUnshackle::mp_zone_changing_injection() {
    // Assembly code below is injected into the function that changes the the current MP zone
    // Force the current MP zone to always be 1, which allows multiplayer everywhere
    // It seems MP zone is not very important

    //Disable:
    //"DARKSOULS.exe" + A6E697:
    //mov   [edi + 00000284], eax
    //Alt: db 89 87 84 02 00 00
    __asm {
        mov     DWORD PTR [edi + 0x284], 00000001
        jmp     mp_zone_changing_injection_return
    }
}
