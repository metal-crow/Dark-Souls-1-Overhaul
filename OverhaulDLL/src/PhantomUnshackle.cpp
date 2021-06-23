/*
    DARK SOULS OVERHAUL

    Contributors to this file:
    Metal-Crow    -    Reverse engineering, C++

*/

#include "PhantomUnshackle.h"
#include "DarkSoulsOverhaulMod.h"
#include "SP/memory/injection/asm/x64.h"
#include "MainLoop.h"

extern "C" {
    uint64_t mp_zone_changing_injection_return;
    void mp_zone_changing_injection();
    int32_t vanilla_mp_zone;
    uint64_t world_char_base_asm;
}


bool change_mp_zone(void* unused);

void PhantomUnshackle::start() {
    ConsoleWrite("Enabling Phantom Unshackle patch...");

    world_char_base_asm = Game::world_chr_man_imp;

    // Injection to prevent MP zone ID from being changed by the game, and to grab the expected zone ID
    uint8_t *write_address = (uint8_t*)(PhantomUnshackle::mp_zone_changing_injection_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &mp_zone_changing_injection_return, 4, &mp_zone_changing_injection);

    // Injection to prevent MP zone ID from being set to -2
    uint8_t nop_patch[10] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
    write_address = (uint8_t*)(PhantomUnshackle::mp_zone_neg2_force_offset + Game::ds1_base);
    sp::mem::patch_bytes(write_address, nop_patch, 10);
    write_address = (uint8_t*)(PhantomUnshackle::mp_zone_neg2_force_offset_part2 + Game::ds1_base);
    sp::mem::patch_bytes(write_address, nop_patch, 10);

    // Start callback for controlling mp zone
    MainLoop::setup_mainloop_callback(change_mp_zone, NULL, "change_mp_zone");
}

/*
// Mp zone works as long as it is any positive number
//
// This makes it so if you're in an area but not a chunk w/ an MP zone
// It sticks you in an "extra" mp zone for that area
// This isn't super pretty, as it could be annoying if two "extra" chunks are far apart in the same map
*/
bool change_mp_zone(void* unused) {
    //only alter the mp zone if character is loaded
    bool loaded = Game::playerchar_is_loaded();

    if (loaded)
    {
        auto get_online_area_id_ptr_o = Game::get_online_area_id_ptr();
        if (get_online_area_id_ptr_o.has_value())
        {
            auto get_online_area_id_ptr = get_online_area_id_ptr_o.value();
            // normal zone id (don't alter, we want to connect with vanilla players)
            if (vanilla_mp_zone > 0)
            {
                *get_online_area_id_ptr = vanilla_mp_zone;
            }
            //area normally without a zone (anywhere without multiplayer)
            else
            {
                //apparently this value can't be set to whatever, the server may validate it or something
                //a value of 100XXX seems to work reliably
                *get_online_area_id_ptr = 100000 + Game::get_area_number().value_or(0)*100 + Game::get_world_number().value_or(0);
            }
        }
    }

    return true;
}
