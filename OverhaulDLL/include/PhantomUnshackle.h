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
};

#endif
