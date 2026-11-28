#include "CustomInvasionTypes.h"
#include "DarkSoulsOverhaulMod.h"
#include "SP/memory/injection/asm/x64.h"
#include "MainLoop.h"
#include "ModNetworking.h"
#include <tuple>
#include <vector>
#include <string>
#include <atomic>
#include <mutex>
#include <optional>

extern "C" {
    uint64_t Send_Type17_GeneralRequestTask_injection_return;
    void Send_Type17_GeneralRequestTask_injection();
    void Send_Type17_GeneralRequestTask_injection_helper(uint64_t RequestGetBreakInTargetList_Data);
    uint64_t BreakInPushDispatch_injection_return;
    void BreakInPushDispatch_injection();
    void BreakInPushDispatch_original(void* breakin_impl, uint32_t type, uint8_t* data, uint64_t size);
    bool BreakInPushDispatch_injection_helper(void* breakin_impl, uint32_t type, uint8_t* data, uint64_t size);
}
static bool CheckHeldBreakInAllow(void* unused);

void CustomInvasionTypes::start()
{
    ConsoleWrite("Enabling Custom Invasions...");

    uint8_t *write_address = (uint8_t*)(CustomInvasionTypes::Send_Type17_GeneralRequestTask_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &Send_Type17_GeneralRequestTask_injection_return, 2, &Send_Type17_GeneralRequestTask_injection);
    //Hold the host's break in allow for the Jolly eye orb until we know their lobby size
    write_address = (uint8_t*)(CustomInvasionTypes::BreakInPushDispatch_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &BreakInPushDispatch_injection_return, 6, &BreakInPushDispatch_injection);
    MainLoop::setup_mainloop_callback(CheckHeldBreakInAllow, NULL, "CheckHeldBreakInAllow");
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
    AllAreasJolly,
    NoCustomOrb,
};

static uint32_t last_send_count = 0;
static size_t current_mpregionid_offset = 0;
static uint32_t current_soullevel_offset = 0;
static InvasionOrb current_InvasionOrb = InvasionOrb::NoCustomOrb;
static std::atomic<uint64_t> last_jolly_search_time = 0;

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
    if (Game::player_has_speffect((uint64_t)(playerins_o.value()), { CustomInvasionTypes::AllAreasJollyInvadingOrbSpEffect_Red, CustomInvasionTypes::AllAreasJollyInvadingOrbSpEffect_Blue }))
    {
        current_InvasionOrb = InvasionOrb::AllAreasJolly;
        last_send_count = 0;
    }
    else if (Game::player_has_speffect((uint64_t)(playerins_o.value()), { CustomInvasionTypes::AllAreasInvadingOrbSpEffect_Red, CustomInvasionTypes::AllAreasInvadingOrbSpEffect_Blue }))
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

    if (current_InvasionOrb == InvasionOrb::AllAreas || current_InvasionOrb == InvasionOrb::AllAreasJolly)
    {
        //use the current area id in the list
        if (MultiPlayerRegionIDs.size() > 0)
        {
            *(uint32_t*)(RequestGetBreakInTargetList_Data) = std::get<0>(MultiPlayerRegionIDs[current_mpregionid_offset]);

            if (current_InvasionOrb == InvasionOrb::AllAreasJolly)
            {
                last_jolly_search_time = GetTickCount64();
                swprintf(banner_msg, sizeof(banner_msg) / sizeof(wchar_t), L"Searching area for co-op: %s", std::get<1>(MultiPlayerRegionIDs[current_mpregionid_offset]).c_str());
                ConsoleWrite("Jolly orb: area %d", std::get<0>(MultiPlayerRegionIDs[current_mpregionid_offset]));
            }
            else
            {
                swprintf(banner_msg, sizeof(banner_msg) / sizeof(wchar_t), L"Searching area: %s", std::get<1>(MultiPlayerRegionIDs[current_mpregionid_offset]).c_str());
                ConsoleWrite("Searching orb: area %d", std::get<0>(MultiPlayerRegionIDs[current_mpregionid_offset]));
            }
            Game::show_banner_message(banner_msg);

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

/*
 * ===========JOLLY EYE ORB SECTION
 * When a host allows our break in, their lobby id is sent to us before the game starts joining.
 * Hold that message, and only pass it on to the game if the host's lobby has another player in it.
 */
//If no search request has been sent in this time, the search has ended
static const uint64_t JOLLY_SEARCH_TIMEOUT_MS = 60000;
static const uint64_t JOLLY_LOBBY_DATA_TIMEOUT_MS = 3000;

static const uint32_t PushRequestAllowBreakInTarget_Red = 5;
static const uint32_t PushRequestAllowBreakInTarget_Blue = 9;

typedef void* FlatbufferGetRoot_FUNC(uint8_t* data, uint64_t size);
static FlatbufferGetRoot_FUNC* BreakInPush_GetRoot = (FlatbufferGetRoot_FUNC*)0x14112fe00;
typedef void* FlatbufferGetField_FUNC(void* table);
static FlatbufferGetField_FUNC* BreakInPush_GetBody = (FlatbufferGetField_FUNC*)0x140bd27e0;
static FlatbufferGetField_FUNC* AllowBreakIn_GetJoinData = (FlatbufferGetField_FUNC*)0x140bd2aa0;

bool CustomInvasionTypes::jolly_search_active()
{
    uint64_t last_time = last_jolly_search_time.load();
    return current_InvasionOrb == InvasionOrb::AllAreasJolly && last_time != 0 && GetTickCount64() - last_time < JOLLY_SEARCH_TIMEOUT_MS;
}

struct HeldBreakInAllow
{
    void* breakin_impl;
    uint32_t type;
    std::vector<uint8_t> data;
    uint64_t lobby;
    uint64_t time;
};
static std::mutex HeldBreakInAllow_Mtx;
static std::optional<HeldBreakInAllow> held_allow;

//The join data is a serialized blob. Find the steam lobby id in it by its universe/type/instance bits.
static uint64_t FindLobbyIdInJoinData(uint8_t* join_data, uint32_t len)
{
    for (uint32_t i = 0; i + sizeof(uint64_t) <= len; i++)
    {
        uint64_t val = *(uint64_t*)(join_data + i);
        if ((val >> 48) == 0x0186)
        {
            return val;
        }
    }
    return 0;
}

//Returns true if this push should be held instead of dispatched
bool BreakInPushDispatch_injection_helper(void* breakin_impl, uint32_t type, uint8_t* data, uint64_t size)
{
    if (type != PushRequestAllowBreakInTarget_Red && type != PushRequestAllowBreakInTarget_Blue)
    {
        return false;
    }
    if (!CustomInvasionTypes::jolly_search_active() || ModNetworking::SteamMatchmaking == nullptr)
    {
        return false;
    }

    uint64_t lobby = 0;
    void* root = BreakInPush_GetRoot(data, size);
    void* body = (root != nullptr) ? BreakInPush_GetBody(root) : nullptr;
    uint8_t* join_data = (body != nullptr) ? (uint8_t*)AllowBreakIn_GetJoinData(body) : nullptr;
    if (join_data != nullptr)
    {
        uint32_t len = *(uint32_t*)join_data;
        lobby = FindLobbyIdInJoinData(join_data + 4, len);
    }
    //Never let the game join a host we can't check
    if (lobby == 0)
    {
        return true;
    }

    ConsoleWrite("Jolly orb: host allowed break in, checking lobby %llx", lobby);
    ModNetworking::SteamMatchmaking->RequestLobbyData(lobby);
    HeldBreakInAllow_Mtx.lock();
    held_allow = HeldBreakInAllow{ breakin_impl, type, std::vector<uint8_t>(data, data + size), lobby, GetTickCount64() };
    HeldBreakInAllow_Mtx.unlock();
    return true;
}

//Once we know how many players are in the host's lobby, either pass the held allow to the game or drop it
static bool CheckHeldBreakInAllow(void* unused)
{
    HeldBreakInAllow_Mtx.lock();
    if (!held_allow.has_value())
    {
        HeldBreakInAllow_Mtx.unlock();
        return true;
    }
    //returns 0 until we have the lobby data
    int members = ModNetworking::SteamMatchmaking->GetNumLobbyMembers(held_allow->lobby);
    if (members == 0 && GetTickCount64() - held_allow->time < JOLLY_LOBBY_DATA_TIMEOUT_MS)
    {
        HeldBreakInAllow_Mtx.unlock();
        return true;
    }
    HeldBreakInAllow allow = std::move(held_allow.value());
    held_allow.reset();
    HeldBreakInAllow_Mtx.unlock();

    ConsoleWrite("Jolly orb: lobby %llx has %d members", allow.lobby, members);
    if (members >= 2 && CustomInvasionTypes::jolly_search_active())
    {
        BreakInPushDispatch_original(allow.breakin_impl, allow.type, allow.data.data(), allow.data.size());
    }
    return true;
}
