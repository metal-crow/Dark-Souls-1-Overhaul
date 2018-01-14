#pragma once

#ifndef DS1_OVERHAUL_PHANTOM_UNSHACKLE_H_
#define DS1_OVERHAUL_PHANTOM_UNSHACKLE_H_

#include <cstdint>

class PhantomUnshackle {
public:
    static void start();
private:
    static const uint32_t mp_zone_changing_injection_offset = 0xA6E697;
    static void __stdcall mp_zone_changing_injection();
    static const uint32_t mp_zone_neg2_force_offset = 0xA6E6DF;
    static const uint32_t mp_zone_neg2_force_offset_part2 = 0xA834F2;
};

#endif
