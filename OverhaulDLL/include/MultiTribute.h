/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Sean Pesce  -  C++, Multi-Tribute technique
*/


#pragma once

#ifndef DS1_OVERHAUL_MULTI_TRIBUTE_H_
    #define DS1_OVERHAUL_MULTI_TRIBUTE_H_

#include <cstdint>
#include <string>


namespace MultiTribute {

    void apply(const std::string& output_prefix = "");
    void unpatch(const std::string& output_prefix = "");
    bool is_active();

} // namespace MultiTribute

#endif // DS1_OVERHAUL_MULTI_TRIBUTE_H_