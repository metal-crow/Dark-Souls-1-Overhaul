/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Sean Pesce  -  C++, L3 jump technique
*/


#pragma once

#ifndef DS1_OVERHAUL_L3_JUMP_H_
    #define DS1_OVERHAUL_L3_JUMP_H_

#include <cstdint>
#include <string>


namespace L3Jump {

    void apply(const std::string& output_prefix = "");
    void unpatch(const std::string& output_prefix = "");
    bool is_active();
    void force_jump(bool force);

} // namespace L3Jump

#endif // DS1_OVERHAUL_L3_JUMP_H_