#pragma once

#ifndef _DS1_OVERHAUL_DEAD_ANGLES_H_
#define _DS1_OVERHAUL_DEAD_ANGLES_H_

#include <cstdint>


class DeadAngles
{
public:
    static void start();
private:
    static const uint64_t main_dead_angle_injection_offset = 0x3206C0;
};

#endif
