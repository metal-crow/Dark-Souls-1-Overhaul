#pragma once
#ifndef CustomInvasionTypes_H
#define CustomInvasionTypes_H

#include <cstdint>

//This handles WexDust invasions and Infinite Upwards SL search invasions
class CustomInvasionTypes
{
public:
    static void start();
    static void ReadSearchFile();

    static const uint32_t AllAreasInvadingOrbSpEffect_Red = 18;
    static const uint32_t AllAreasInvadingOrbSpEffect_Blue = 91;
    static const uint32_t InfiniteUpwardsInvadingOrbSpEffect_Red = 19;
    static const uint32_t InfiniteUpwardsInvadingOrbSpEffect_Blue = 92;
    static const uint32_t AllAreasAndInfiniteUpwardsInvadingOrbSpEffect_Red = 90;
    static const uint32_t AllAreasAndInfiniteUpwardsInvadingOrbSpEffect_Blue = 93;
private:
    static const uint64_t Send_Type17_GeneralRequestTask_offset = 0xbd1730;
};

#endif
