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

enum InvasionOrb
{
    AllAreas,
    InfiniteUp,
    AllAreasAndInfiniteUp,
    NoCustomOrb,
};

static uint32_t last_send_count = 0;
static size_t current_mpregionid_offset = 0;
static uint32_t current_soullevel_offset = 0;
static InvasionOrb current_InvasionOrb = InvasionOrb::NoCustomOrb;

void Send_Type17_GeneralRequestTask_injection_helper(uint64_t RequestGetBreakInTargetList_Data)
{
    auto playerins_o = Game::get_PlayerIns();
    if (!playerins_o.has_value())
    {
        return;
    }

    //If we have the special speffect on, start the custom invasion state
    //This speffect only stays for the 1st frame/send. So we have to remember it
    //Support save and continue. So if we invade and search again it doesn't go back to the start
    if (Game::player_has_speffect((uint64_t)(playerins_o.value()), { CustomInvasionTypes::AllAreasInvadingOrbSpEffect }))
    {
        current_InvasionOrb = InvasionOrb::AllAreas;
        last_send_count = 0;
    }
    else if (Game::player_has_speffect((uint64_t)(playerins_o.value()), { CustomInvasionTypes::InfiniteUpwardsInvadingOrbSpEffect }))
    {
        current_InvasionOrb = InvasionOrb::InfiniteUp;
        last_send_count = 0;
    }
    else if (Game::player_has_speffect((uint64_t)(playerins_o.value()), { CustomInvasionTypes::AllAreasAndInfiniteUpwardsInvadingOrbSpEffect }))
    {
        current_InvasionOrb = InvasionOrb::AllAreasAndInfiniteUp;
        last_send_count = 0;
    }
    //If the player is doing any other normal multiplayer types, don't do this custom code
    else if (Game::player_has_speffect((uint64_t)(playerins_o.value()), { 4, 10, 11, 16, 26, 27, 15 }))
    {
        current_InvasionOrb = InvasionOrb::NoCustomOrb;
        last_send_count = 0;
        //reset the area id and SL offset
        current_mpregionid_offset = 0;
        current_soullevel_offset = 0;
    }

    if (current_InvasionOrb == InvasionOrb::AllAreas)
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
        Game::set_invasion_refresh_timer(26.0f);
    }

    if (current_InvasionOrb == InvasionOrb::InfiniteUp)
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
            current_soullevel_offset += (uint32_t)(1.22222*pc_searched_sl + 22.2222) - pc_searched_sl; //increase by the search's upper and lower bounds
            //clamp the last search value to 713 if needed
            if (pc_sl + current_soullevel_offset > 713)
            {
                current_soullevel_offset = 713 - pc_sl;
            }
            if (pc_searched_sl >= 713)
            {
                current_soullevel_offset = 0;
            }
        }

        //set the timer to be closer to the refresh time (30 seconds)
        Game::set_invasion_refresh_timer(26.0f);
    }

    if (current_InvasionOrb == InvasionOrb::AllAreasAndInfiniteUp)
    {
        *(uint32_t*)(RequestGetBreakInTargetList_Data) = MultiPlayerRegionIDs[current_mpregionid_offset];

        uint32_t pc_sl = *(uint32_t*)(RequestGetBreakInTargetList_Data + 28);
        *(uint32_t*)(RequestGetBreakInTargetList_Data + 28) += current_soullevel_offset;
        uint32_t pc_searched_sl = pc_sl + current_soullevel_offset;

        ConsoleWrite("Twin eye orb: area %d SL %d", MultiPlayerRegionIDs[current_mpregionid_offset], pc_searched_sl);
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
            //increase the SL offset first
            current_soullevel_offset += (uint32_t)(1.22222*pc_searched_sl + 22.2222) - pc_searched_sl; //increase by the search's upper and lower bounds
            //clamp the last search value to 713 if needed
            if (pc_sl + current_soullevel_offset > 713)
            {
                current_soullevel_offset = 713 - pc_sl;
            }
            if (pc_searched_sl >= 713)
            {
                current_soullevel_offset = 0;
            }

            //once the SL offset has looped around, go to next area
            if (current_soullevel_offset == 0)
            {
                current_mpregionid_offset++;
            }
            if (current_mpregionid_offset >= (sizeof(MultiPlayerRegionIDs) / sizeof(MultiPlayerRegionIDs[0])))
            {
                current_mpregionid_offset = 0;
            }
        }

        //set the timer to be closer to the refresh time (30 seconds)
        Game::set_invasion_refresh_timer(26.0f);
    }
}
