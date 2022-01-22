#include "CustomInvasionTypes.h"
#include "DarkSoulsOverhaulMod.h"
#include "SP/memory/injection/asm/x64.h"
#include "MainLoop.h"
#include <tuple>
#include <vector>
#include <string>

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

//These only need to include 1 area id from each set that match all but the last digit
//i.e 100000 can invade all areas matching 10000X
//http://darksouls.wikidot.com/invasion-spawn-locations
static std::vector<std::tuple<uint32_t, std::wstring>> MultiPlayerRegionIDs;

//read the data from the text file that specifics the areas to search, and their names
//run when a character is loaded/reloaded from the save file
void CustomInvasionTypes::ReadSearchFile()
{
    FILE* fp = NULL;
    char line[80];
    uint32_t mpid;
    char area_name[80];

    fopen_s(&fp, "./searchlist.txt", "r");
    if (fp == NULL)
    {
        ConsoleWrite("WARNING: Searchlist.txt not found. Unable to use searching eye orb.");
        return;
    }

    MultiPlayerRegionIDs.clear();

    while (fgets(line, sizeof(line), fp) != NULL)
    {
        //ignore comments
        if (line[0] != '#')
        {
            int matched = sscanf_s(line, "%lu:%79[^\r\n].*", &mpid, area_name, (unsigned)_countof(area_name));
            if (matched != 2)
            {
                ConsoleWrite("WARNING: Searchlist.txt has malformed line \"%s\"", line);
            }
            else
            {
                //convert area name to wstring
                const size_t size = std::strlen(area_name);
                std::wstring wstr;
                wstr.resize(size);
                mbstowcs_s(NULL, &wstr[0], wstr.capacity(), area_name, _TRUNCATE);

                //insert this entry
                auto entry = std::make_tuple(mpid, wstr);
                MultiPlayerRegionIDs.push_back(entry); //this is fine since a vector makes a copy of the string
            }
        }
    }

    fclose(fp);

    ConsoleWrite("Invasion search areas reloaded");
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
    wchar_t banner_msg[75];

    auto playerins_o = Game::get_PlayerIns();
    if (!playerins_o.has_value())
    {
        return;
    }

    //If we have the special speffect on, start the custom invasion state
    //This speffect only stays for the 1st frame/send. So we have to remember it
    //Support save and continue. So if we invade and search again it doesn't go back to the start
    if (Game::player_has_speffect((uint64_t)(playerins_o.value()), { CustomInvasionTypes::AllAreasInvadingOrbSpEffect_Red, CustomInvasionTypes::AllAreasInvadingOrbSpEffect_Blue }))
    {
        current_InvasionOrb = InvasionOrb::AllAreas;
        last_send_count = 0;
    }
    else if (Game::player_has_speffect((uint64_t)(playerins_o.value()), { CustomInvasionTypes::InfiniteUpwardsInvadingOrbSpEffect_Red, CustomInvasionTypes::InfiniteUpwardsInvadingOrbSpEffect_Blue }))
    {
        current_InvasionOrb = InvasionOrb::InfiniteUp;
        last_send_count = 0;
    }
    else if (Game::player_has_speffect((uint64_t)(playerins_o.value()), { CustomInvasionTypes::AllAreasAndInfiniteUpwardsInvadingOrbSpEffect_Red, CustomInvasionTypes::AllAreasAndInfiniteUpwardsInvadingOrbSpEffect_Blue }))
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
        if (MultiPlayerRegionIDs.size() > 0)
        {
            *(uint32_t*)(RequestGetBreakInTargetList_Data) = std::get<0>(MultiPlayerRegionIDs[current_mpregionid_offset]);

            swprintf(banner_msg, sizeof(banner_msg) / sizeof(wchar_t), L"Searching area: %s", std::get<1>(MultiPlayerRegionIDs[current_mpregionid_offset]).c_str());
            Game::show_banner_message(banner_msg);

            ConsoleWrite("Searching orb: area %d", std::get<0>(MultiPlayerRegionIDs[current_mpregionid_offset]));
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
                if (current_mpregionid_offset >= MultiPlayerRegionIDs.size())
                {
                    current_mpregionid_offset = 0;
                }
            }

            //set the timer to be closer to the refresh time (30 seconds)
            Game::set_invasion_refresh_timer(26.0f);
            Game::set_blue_invasion_refresh_timer(26.0f);
        }
    }

    if (current_InvasionOrb == InvasionOrb::InfiniteUp)
    {
        //Add an offset to the SL we tell the server, so it returns a new range of connection results
        uint32_t pc_sl = *(uint32_t*)(RequestGetBreakInTargetList_Data + 28);
        *(uint32_t*)(RequestGetBreakInTargetList_Data + 28) += current_soullevel_offset;
        uint32_t pc_searched_sl = pc_sl + current_soullevel_offset;

        swprintf(banner_msg, sizeof(banner_msg) / sizeof(wchar_t), L"Using Soul Level: %d", pc_searched_sl);
        Game::show_banner_message(banner_msg);

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
        Game::set_blue_invasion_refresh_timer(26.0f);
    }

    if (current_InvasionOrb == InvasionOrb::AllAreasAndInfiniteUp)
    {
        if (MultiPlayerRegionIDs.size() > 0)
        {
            *(uint32_t*)(RequestGetBreakInTargetList_Data) = std::get<0>(MultiPlayerRegionIDs[current_mpregionid_offset]);

            uint32_t pc_sl = *(uint32_t*)(RequestGetBreakInTargetList_Data + 28);
            *(uint32_t*)(RequestGetBreakInTargetList_Data + 28) += current_soullevel_offset;
            uint32_t pc_searched_sl = pc_sl + current_soullevel_offset;

            swprintf(banner_msg, sizeof(banner_msg) / sizeof(wchar_t), L"Searching area: %s at SL: %d", std::get<1>(MultiPlayerRegionIDs[current_mpregionid_offset]).c_str(), pc_searched_sl);
            Game::show_banner_message(banner_msg);

            ConsoleWrite("Twin eye orb: area %d SL %d", std::get<0>(MultiPlayerRegionIDs[current_mpregionid_offset]), pc_searched_sl);
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
                if (current_mpregionid_offset >= MultiPlayerRegionIDs.size())
                {
                    current_mpregionid_offset = 0;
                }
            }

            //set the timer to be closer to the refresh time (30 seconds)
            Game::set_invasion_refresh_timer(26.0f);
            Game::set_blue_invasion_refresh_timer(26.0f);
        }
    }
}
