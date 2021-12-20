#pragma once
#ifndef CustomInvasionTypes_H
#define CustomInvasionTypes_H

#include <cstdint>

static const uint32_t MultiPlayerRegionIDs[] =
{
    //Depths
    100001,
    100000,
    100011,
    100010,
    //Undead burg / parish
    101200,
    101210,
    101000,
    101010,
    101011,
    101100,
    101101,
    //Painted world
    110000,
    110001,
    110011,
    110010,
    //Darkroot garden / basin
    120000,
    120010,
    120001,
    120011,
    120100,
    120101,
    //Artorias of the Abyss areas
    121000,
    121100,
    121101,
    121102,
    121200,
    121201,
    121210,
    121300,
    121310,
    121320,
    121330,
    121340,
    121350,
    121370,
    121380,
    121390,
    //the catacombs
    130000,
    130010,
    130011,
    //tomb of the giants
    131000,
    131001,
    131010,
    131011,
    131012,
    131013,
    //blighttown
    140000,
    140001,
    140002,
    140010,
    140011,
    140012,
    140013,
    //demon ruins / lost izaleth
    141000,
    141100,
    141200,
    141300,
    141301,
    //Sens fortress
    150000,
    150001,
    150010,
    150011,
    150012,
    150020,
    150021,
    //Anor Londo
    151000,
    151010,
    151011,
    151020,
    151021,
    151022,
    //New Londo / vally of drakes
    160100,
    160101,
    160110,
    160111,
    160112,
    160120,
    160121,
    //Dukes archive / crystal caves
    170000,
    170001,
    170010,
    170011,
    //Kiln
    180000,
    180001,
    180002,
};

//This handles WexDust invasions and Infinite Upwards SL search invasions
class CustomInvasionTypes
{
public:
    static void start();

    static const uint32_t AllAreasInvadingOrbSpEffect = 18;
    static const uint32_t InfiniteUpwardsInvadingOrbSpEffect = 19;
private:
    static const uint64_t Send_Type17_GeneralRequestTask_offset = 0xbcce19;
};

#endif
