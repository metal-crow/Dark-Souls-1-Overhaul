/*
    DARK SOULS OVERHAUL

    Contributors to this file:
    Metal-Crow    -    Reverse engineering, C++

*/

#pragma once

#ifndef _DS1_OVERHAUL_MOVE_WHILE_CASTING_H_
#define _DS1_OVERHAUL_MOVE_WHILE_CASTING_H_

#include <cstdint>

class CastingMovement
{
public:
    static void start();
    static void on_char_load();
private:
    static const uint64_t walkfb_check_1 = 0x382CD9;
    static const uint64_t walkfb_check_2 = 0x3A6114;
    static const uint64_t walkfb_check_3 = 0x3A5ECE;
    static const uint64_t c0000_esd_reader = 0x1153C55;
};
#endif
