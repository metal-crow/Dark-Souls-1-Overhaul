/*
DARK SOULS OVERHAUL

Contributors to this file:
Metal-Crow    -    Reverse engineering, C++

*/

#include "PhantomUnshackle.h"
#include "DllMain.h"
#include <atomic>

static uint32_t mp_zone_changing_injection_return;

static std::atomic<int32_t> vanilla_mp_zone;

static DWORD WINAPI change_mp_zone(void* unused);

void PhantomUnshackle::start() {
    if (!print_console("    Enabling Phantom Unshackle patch...")) {
        Mod::startup_messages.push_back("    Enabling Phantom Unshackle patch...");
    }

    // Injection to prevent MP zone ID from being changed
    uint8_t *write_address = (uint8_t*)(PhantomUnshackle::mp_zone_changing_injection_offset + ((uint32_t)Game::ds1_base));
    set_mem_protection(write_address, 6, MEM_PROTECT_RWX);
    inject_jmp_5b(write_address, &mp_zone_changing_injection_return, 1, &mp_zone_changing_injection);

    // Injection to prevent MP zone ID from being set to -2
    uint8_t nop_patch[10] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
    write_address = (uint8_t*)(PhantomUnshackle::mp_zone_neg2_force_offset + ((uint32_t)Game::ds1_base));
    apply_byte_patch(write_address, nop_patch, 10);
    write_address = (uint8_t*)(PhantomUnshackle::mp_zone_neg2_force_offset_part2 + ((uint32_t)Game::ds1_base));
    apply_byte_patch(write_address, nop_patch, 10);

    // Start thread for controlling mp zone
    CreateThread(NULL, 0, change_mp_zone, NULL, 0, NULL);
}

void __declspec(naked) __stdcall PhantomUnshackle::mp_zone_changing_injection() {
    // Assembly code below is injected into the function that changes the the current mp zone
    // This edit prevents the mp zone from being change to whatever it was before
    // This allows the thread to have full control of the mp zone
    // Also saves the intended mp zone

    //Disable:
    //"DARKSOULS.exe" + A6E697:
    //mov   [edi + 00000284], eax
    //Alt: db 89 87 84 02 00 00
    __asm {
        push    ebx
        mov     ebx, DWORD PTR ds: 0x0137D644
        mov     ebx, [ebx + 0x28]
        mov     ebx, [ebx + 0]
        cmp     edi, ebx //check if the ptr being written to is for player
        jne     immediate_ret
        mov     vanilla_mp_zone, eax
        immediate_ret:
        pop     ebx
        jmp     mp_zone_changing_injection_return
    }
}

/*
// Mp zone works as long as it is any positive number
//
// This makes it so if you're in an area but not a chunk w/ an MP zone
// It sticks you in an "extra" mp zone for that area
// This isn't super pretty, as it could be annoying if two "extra" chunks are far apart in the same map
//
// Also makes it so any standard mp zone is edited
// This allows MP even with boss dead
*/
static DWORD WINAPI change_mp_zone(void* unused) {
    int char_status = DS1_PLAYER_STATUS_LOADING;
    uint32_t* mp_id;

    while (true) {
        //only alter the mp zone if character is loaded
        Game::player_char_status.read(&char_status);

        if (char_status != DS1_PLAYER_STATUS_LOADING) {
            mp_id = (uint32_t*)(*(uint32_t*)((*(uint32_t*)((*(uint32_t*)0x0137D644) + 0x28)) + 0) + 0x284);
        }

        while (char_status != DS1_PLAYER_STATUS_LOADING) {
            if (vanilla_mp_zone > 0) {
                //set mp id to mp_id+5
                *mp_id = vanilla_mp_zone + 5;
            }
            else {
                //set mp id to area id
                uint32_t area_id = *(uint32_t*)((*((uint32_t*)0x0137E204)) + 0xA12);
                *mp_id = area_id & 0x0000ffff;
            }

            Sleep(25);
            Game::player_char_status.read(&char_status);
        }

        Sleep(500);
    }

    return 0;
}
