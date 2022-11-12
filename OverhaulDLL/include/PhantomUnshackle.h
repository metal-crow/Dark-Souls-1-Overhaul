/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Metal-Crow  -  Reverse engineering, C++

*/
#pragma once

#ifndef DS1_OVERHAUL_PHANTOM_UNSHACKLE_H_
#define DS1_OVERHAUL_PHANTOM_UNSHACKLE_H_

#include <cstdint>

class PhantomUnshackle {
public:
    static void start();
private:
    static const uint64_t mp_zone_changing_injection_offset = 0x326cbc;
    static const uint64_t mp_zone_neg2_force_offset = 0x326d47;
    static const uint64_t mp_zone_neg2_force_offset_part2 = 0x327640;
};

#endif
