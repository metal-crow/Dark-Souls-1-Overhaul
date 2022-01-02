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
    void mp_zone_changing_helper(int32_t vanilla_zone, uint64_t entity);
}


bool change_mp_zone(void* unused);

void PhantomUnshackle::start() {
    ConsoleWrite("Enabling Phantom Unshackle patch...");

    // Injection to control the MP zone ID
    uint8_t *write_address = (uint8_t*)(PhantomUnshackle::mp_zone_changing_injection_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &mp_zone_changing_injection_return, 4, &mp_zone_changing_injection);

    // Injection to prevent MP zone ID from being set to -2
    uint8_t nop_patch[10] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
    write_address = (uint8_t*)(PhantomUnshackle::mp_zone_neg2_force_offset + Game::ds1_base);
    sp::mem::patch_bytes(write_address, nop_patch, 10);
    write_address = (uint8_t*)(PhantomUnshackle::mp_zone_neg2_force_offset_part2 + Game::ds1_base);
    sp::mem::patch_bytes(write_address, nop_patch, 10);
}

/*
// Mp zone works as long as it is any positive number
//
// This makes it so if you're in an area but not a chunk w/ an MP zone
// It sticks you in an "extra" mp zone for that area
// This isn't super pretty, as it could be annoying if two "extra" chunks are far apart in the same map
*/
void mp_zone_changing_helper(int32_t vanilla_zone, uint64_t entity)
{
    auto playerins = Game::get_PlayerIns();
    if (playerins.has_value() && (uint64_t)playerins.value() == entity)
    {
        //area normally without a zone (anywhere without multiplayer)
        if (vanilla_zone <= 0)
        {
            //apparently this value can't be set to whatever, the server may validate it or something
            //a value of 100XXX seems to work reliably
            int32_t new_zone = 100000 + Game::get_area_number().value_or(0) * 100 + Game::get_world_number().value_or(0);

            *Game::get_online_area_id_ptr().value() = new_zone;
            *Game::get_area_id_ptr().value() = new_zone;
        }
    }
}
