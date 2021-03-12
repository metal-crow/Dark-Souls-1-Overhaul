/*
    DARK SOULS OVERHAUL

    Contributors to this file:
    Metal-Crow    -    Reverse engineering, C++

*/

#include "PhantomUnshackle.h"
#include "DarkSoulsOverhaulMod.h"
#include "SP/memory/injection/asm/x64.h"

extern "C" {
    uint64_t mp_zone_changing_injection_return;
    void mp_zone_changing_injection();
    int32_t vanilla_mp_zone;
    uint64_t world_char_base_asm;
}


static DWORD WINAPI change_mp_zone(void* unused);

void PhantomUnshackle::start() {
    global::cmd_out << Mod::output_prefix << "Enabling Phantom Unshackle patch...\n";

    world_char_base_asm = Game::world_char_base;

    // Injection to prevent MP zone ID from being changed by the game, and to grab the expected zone ID
    uint8_t *write_address = (uint8_t*)(PhantomUnshackle::mp_zone_changing_injection_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &mp_zone_changing_injection_return, 4, &mp_zone_changing_injection);

    // Injection to prevent MP zone ID from being set to -2
    uint8_t nop_patch[10] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
    write_address = (uint8_t*)(PhantomUnshackle::mp_zone_neg2_force_offset + Game::ds1_base);
    sp::mem::patch_bytes(write_address, nop_patch, 10);
    write_address = (uint8_t*)(PhantomUnshackle::mp_zone_neg2_force_offset_part2 + Game::ds1_base);
    sp::mem::patch_bytes(write_address, nop_patch, 10);

    // Start thread for controlling mp zone
    CreateThread(NULL, 0, change_mp_zone, NULL, 0, NULL);
}

/*
// Mp zone works as long as it is any positive number
//
// This makes it so if you're in an area but not a chunk w/ an MP zone
// It sticks you in an "extra" mp zone for that area
// This isn't super pretty, as it could be annoying if two "extra" chunks are far apart in the same map
*/
static DWORD WINAPI change_mp_zone(void* unused) {
    int32_t char_status = DS1_PLAYER_STATUS_LOADING;

    while (true) {
        //only alter the mp zone if character is loaded
        char_status = Game::get_player_char_status();

        while (char_status != DS1_PLAYER_STATUS_LOADING) {
            if (Game::get_mp_id_ptr().has_value())
            {
                // normal zone id (don't alter, we want to connect with vanilla players)
                if (vanilla_mp_zone > 0)
                {
                    *Game::get_mp_id_ptr().value() = vanilla_mp_zone;
                }
                //area normally without a zone (anywhere without multiplayer)
                else
                {
                    //set mp id to area id + 100000 (since the server apparently validates it or something?)
                    *Game::get_mp_id_ptr().value() = Game::get_area_id().value_or(0) + 100000;
                }
            }

            Sleep(25);
            char_status = Game::get_player_char_status();
        }

        Sleep(500);
    }

    return 0;
}
