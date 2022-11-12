#pragma once

#ifndef _DS1_OVERHAUL_FIX_ANKLES_H_
#define _DS1_OVERHAUL_FIX_ANKLES_H_

#include <cstdint>


class FixAnkles
{
public:
    static void start();
private:
    static const uint64_t set_foot_ikvalue_injection_offset = 0x2a0409;
};

#endif
