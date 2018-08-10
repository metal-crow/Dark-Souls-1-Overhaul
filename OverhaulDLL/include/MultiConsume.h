/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Sean Pesce  -  C++, Multi-Consume technique
*/


#pragma once

#ifndef DS1_OVERHAUL_MULTI_CONSUME_H_
    #define DS1_OVERHAUL_MULTI_CONSUME_H_

#include <cstdint>
#include <string>


namespace MultiConsume {

    void apply(const std::string& output_prefix = "");
    void unpatch(const std::string& output_prefix = "");
    bool is_active();

} // namespace MultiConsume

#endif // DS1_OVERHAUL_MULTI_CONSUME_H_