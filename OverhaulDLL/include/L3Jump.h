/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        B3LYP       -  L3 jump technique
        Sean Pesce  -  C++
*/


#pragma once

#ifndef DS1_OVERHAUL_L3_JUMP_H_
    #define DS1_OVERHAUL_L3_JUMP_H_

#include <cstdint>


namespace L3Jump {

    void apply();
    void unpatch();
    bool is_active();

} // namespace L3Jump

#endif // DS1_OVERHAUL_L3_JUMP_H_