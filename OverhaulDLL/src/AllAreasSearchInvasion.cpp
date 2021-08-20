#include "AllAreasSearchInvasion.h"
#include "DarkSoulsOverhaulMod.h"
#include "SP/memory/injection/asm/x64.h"
#include "MainLoop.h"

extern "C" {
    uint64_t Send_Type17_GeneralRequestTask_injection_return;
    void Send_Type17_GeneralRequestTask_injection();
    void Send_Type17_GeneralRequestTask_injection_helper(uint64_t RequestGetBreakInTargetList_Data);
}

void AllAreasSearchInvasion::start()
{
    ConsoleWrite("Enabling All Areas Search Invasion...");

    uint8_t *write_address = (uint8_t*)(AllAreasSearchInvasion::Send_Type17_GeneralRequestTask_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &Send_Type17_GeneralRequestTask_injection_return, 1, &Send_Type17_GeneralRequestTask_injection);
}

static size_t current_mpregionid_offset = 0;
static bool UsingAllAreasInvadeOrb = false;

void Send_Type17_GeneralRequestTask_injection_helper(uint64_t RequestGetBreakInTargetList_Data)
{
    auto playerins_o = Game::get_PlayerIns();
    if (!playerins_o.has_value())
    {
        return;
    }

    //If we have the special speffect on, start the custom invasion state
    //This speffect only stays for the 1st frame/send. So we have to remember it
    if (Game::player_has_speffect((uint64_t)(playerins_o.value()), { AllAreasSearchInvasion::AllAreasInvadingOrbSpEffect }))
    {
        UsingAllAreasInvadeOrb = true;
    }
    //If the player is doing any other normal multiplayer types, don't do this custom code
    else if (Game::player_has_speffect((uint64_t)(playerins_o.value()), { 4, 10, 11, 16, 26, 27, 15 }))
    {
        UsingAllAreasInvadeOrb = false;
    }

    if (UsingAllAreasInvadeOrb)
    {
        //use the current area id in the list
        *(uint32_t*)(RequestGetBreakInTargetList_Data) = MultiPlayerRegionIDs[current_mpregionid_offset];
        current_mpregionid_offset++;
        if (current_mpregionid_offset >= (sizeof(MultiPlayerRegionIDs) / sizeof(MultiPlayerRegionIDs[0])))
        {
            current_mpregionid_offset = 0;
        }

        //set the timer to be closer to the refresh time (30 seconds)
        Game::set_invasion_refresh_timer(25.0f);
    }
    else
    {
        //reset the area id list to the start
        current_mpregionid_offset = 0;
    }
}
