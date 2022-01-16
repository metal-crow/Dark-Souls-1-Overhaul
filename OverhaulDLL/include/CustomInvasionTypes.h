#pragma once
#ifndef CustomInvasionTypes_H
#define CustomInvasionTypes_H

#include <cstdint>

//These only need to include 1 area id from each set that match all but the last digit
//i.e 100000 can invade all areas matching 10000X
//http://darksouls.wikidot.com/invasion-spawn-locations
static const uint32_t MultiPlayerRegionIDs[] =
{
    //Depths
    100000,
    100010,
    //Undead burg / parish
    101200,
    101210,
    101000,
    101010,
    101100,
    //Painted world
    110000,
    110010,
    //Darkroot garden / basin
    120000,
    120010,
    120100,
    //Artorias of the Abyss areas
    121000,
    121100,
    121200,
    121210,
    //the catacombs
    130000,
    130010,
    //tomb of the giants
    131000,
    131010,
    //blighttown
    140000,
    140010,
    //demon ruins / lost izaleth
    141000,
    141100,
    141200,
    141300,
    //Sens fortress
    150000,
    150010,
    150020,
    //Anor Londo
    151000,
    151010,
    151020,
    //New Londo / vally of drakes
    160100,
    160110,
    160120,
    //Dukes archive / crystal caves
    170000,
    170010,
    //Kiln
    180000,
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
