#include "CustomInvasionTypes.h"
#include "DarkSoulsOverhaulMod.h"
#include "SP/memory/injection/asm/x64.h"
#include "MainLoop.h"

extern "C" {
    uint64_t Send_Type17_GeneralRequestTask_injection_return;
    void Send_Type17_GeneralRequestTask_injection();
    void Send_Type17_GeneralRequestTask_injection_helper(uint64_t RequestGetBreakInTargetList_Data);
}

void CustomInvasionTypes::start()
{
    ConsoleWrite("Enabling Custom Invasions...");

    uint8_t *write_address = (uint8_t*)(CustomInvasionTypes::Send_Type17_GeneralRequestTask_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &Send_Type17_GeneralRequestTask_injection_return, 1, &Send_Type17_GeneralRequestTask_injection);
}

static uint32_t last_send_count = 0;
static size_t current_mpregionid_offset = 0;
static bool UsingAllAreasInvadeOrb = false;
static uint32_t current_soullevel_offset = 0;
static bool UsingInfiniteUpwardsInvadeOrb = false;

void Send_Type17_GeneralRequestTask_injection_helper(uint64_t RequestGetBreakInTargetList_Data)
{
    auto playerins_o = Game::get_PlayerIns();
    if (!playerins_o.has_value())
    {
        return;
    }

    //If we have the special speffect on, start the custom invasion state
    //This speffect only stays for the 1st frame/send. So we have to remember it
    if (Game::player_has_speffect((uint64_t)(playerins_o.value()), { CustomInvasionTypes::AllAreasInvadingOrbSpEffect }))
    {
        UsingAllAreasInvadeOrb = true;
        UsingInfiniteUpwardsInvadeOrb = false;
        last_send_count = 0;
    }
    else if (Game::player_has_speffect((uint64_t)(playerins_o.value()), { CustomInvasionTypes::InfiniteUpwardsInvadingOrbSpEffect }))
    {
        UsingAllAreasInvadeOrb = false;
        UsingInfiniteUpwardsInvadeOrb = true;
        last_send_count = 0;
    }
    //If the player is doing any other normal multiplayer types, don't do this custom code
    else if (Game::player_has_speffect((uint64_t)(playerins_o.value()), { 4, 10, 11, 16, 26, 27, 15 }))
    {
        UsingAllAreasInvadeOrb = false;
        UsingInfiniteUpwardsInvadeOrb = false;
        last_send_count = 0;
    }

    if (UsingAllAreasInvadeOrb)
    {
        //use the current area id in the list
        *(uint32_t*)(RequestGetBreakInTargetList_Data) = MultiPlayerRegionIDs[current_mpregionid_offset];

        ConsoleWrite("Searching orb: area %d", MultiPlayerRegionIDs[current_mpregionid_offset]);
        last_send_count++;
        if (last_send_count >= 2)
        {
            last_send_count = 0;
        }

        //for some reason, the game calls this function twice for each request. Not sure if one is bad or good or what
        //so to be safe, we don't change from the 1st request
        //(the two requests are not on the same frame)
        if (last_send_count == 0)
        {
            current_mpregionid_offset++;
        }

        if (current_mpregionid_offset >= (sizeof(MultiPlayerRegionIDs) / sizeof(MultiPlayerRegionIDs[0])))
        {
            current_mpregionid_offset = 0;
        }

        //set the timer to be closer to the refresh time (30 seconds)
        Game::set_invasion_refresh_timer(28.0f);
    }
    else
    {
        //reset the area id list to the start
        current_mpregionid_offset = 0;
    }

    if (UsingInfiniteUpwardsInvadeOrb)
    {
        //Add an offset to the SL we tell the server, so it returns a new range of connection results
        uint32_t pc_sl = *(uint32_t*)(RequestGetBreakInTargetList_Data + 28);
        *(uint32_t*)(RequestGetBreakInTargetList_Data + 28) += current_soullevel_offset;
        uint32_t pc_searched_sl = pc_sl + current_soullevel_offset;

        ConsoleWrite("Unbound orb: SL %d", pc_searched_sl);
        last_send_count++;
        if (last_send_count >= 2)
        {
            last_send_count = 0;
        }

        //for some reason, the game calls this function twice for each request. Not sure if one is bad or good or what
        //so to be safe, we don't change from the 1st request
        //(the two requests are not on the same frame)
        if (last_send_count == 0)
        {
            current_soullevel_offset += 20 + (uint32_t)(0.1f*(pc_searched_sl)); //increase by the search's upper bound: 20+0.1*SL
        }

        if (pc_sl + current_soullevel_offset > 713)
        {
            current_soullevel_offset = 0;
        }

        //set the timer to be closer to the refresh time (30 seconds)
        Game::set_invasion_refresh_timer(28.0f);
    }
    else
    {
        //reset the offset
        current_soullevel_offset = 0;
    }
}
