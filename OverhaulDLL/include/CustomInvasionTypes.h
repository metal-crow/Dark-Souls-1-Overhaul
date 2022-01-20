#pragma once
#ifndef CustomInvasionTypes_H
#define CustomInvasionTypes_H

#include <cstdint>
#include <tuple>

//These only need to include 1 area id from each set that match all but the last digit
//i.e 100000 can invade all areas matching 10000X
//http://darksouls.wikidot.com/invasion-spawn-locations
static const std::tuple<uint32_t, const wchar_t*> MultiPlayerRegionIDs[] =
{
    //Depths
    {100000, L"The Depths"},
    {100010, L"Lower Depths"},
    //Undead parish
    {101200, L"Undead Parish Bridge"},
    {101210, L"Upper Undead Parish"},
    //Undead burg
    {101000, L"Undead Burg Start"},
    {101010, L"Undead Burg"},
    {101100, L"Lower Undead Burg"},
    //Painted world
    {110000, L"Painted World"},
    {110010, L"Painted World"},
    //Darkroot garden / basin
    {120000, L"Darkroot Garden and Basin"},
    {120010, L"Darkroot Garden"},
    {120100, L"Darkroot Garden Forest"},
    //Artorias of the Abyss areas
    {121000, L"Sanctuary Garden"},
    {121100, L"Royal Wood"},
    {121200, L"Oolacile Township"},
    {121210, L"Chasm of the Abyss"},
    //the catacombs
    {130000, L"The Catacombs Start"},
    {130010, L"The Catacombs"},
    //tomb of the giants
    {131000, L"Tomb of the Giants Upper"},
    {131010, L"Tomb of the Giants Lower"},
    //blighttown
    {140000, L"Upper Blighttown"},
    {140010, L"Blighttown Swamp"},
    //demon ruins / lost izaleth
    {141000, L"Ceaseless Discharge Bridge"},
    {141100, L"Demon Ruins"},
    {141200, L"Centipede Demon Hallway"},
    {141300, L"Lost Izalith"},
    //Sens fortress
    {150000, L"Sen's Fortress"},
    {150010, L"Sen's Fortress"},
    {150020, L"Sen's Fortress Roof"},
    //Anor Londo
    {151000, L"Anor Londo Start"},
    {151010, L"Anor Londo"},
    {151020, L"Anor Londo Interior"},
    //New Londo / vally of drakes
    {160100, L"New Londo Start"},
    {160110, L"New Londo"},
    {160120, L"New Londo Ground"},
    //Dukes archive / crystal caves
    {170000, L"Duke's Archives"},
    {170010, L"Outside Duke's Archives"},
    //Kiln
    {180000, L"Kiln of the First Flame"},
};

//This handles WexDust invasions and Infinite Upwards SL search invasions
class CustomInvasionTypes
{
public:
    static void start();

    static const uint32_t AllAreasInvadingOrbSpEffect_Red = 18;
    static const uint32_t AllAreasInvadingOrbSpEffect_Blue = 91;
    static const uint32_t InfiniteUpwardsInvadingOrbSpEffect_Red = 19;
    static const uint32_t InfiniteUpwardsInvadingOrbSpEffect_Blue = 92;
    static const uint32_t AllAreasAndInfiniteUpwardsInvadingOrbSpEffect_Red = 90;
    static const uint32_t AllAreasAndInfiniteUpwardsInvadingOrbSpEffect_Blue = 93;
private:
    static const uint64_t Send_Type17_GeneralRequestTask_offset = 0xbcce19;
};

#endif
