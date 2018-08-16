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

    extern uint32_t change_next_dlg_to_number_picker;
    extern uint32_t tmp_last_talk;
    extern uint32_t adjust_tribute_dec;
    extern uint32_t sunlight_medal_count;
    extern uint32_t dragon_scale_count;
    extern uint32_t souvenir_count;
    extern uint32_t eye_of_death_count;

    void apply(const std::string& output_prefix = "");
    void unpatch(const std::string& output_prefix = "");
    bool is_active();

} // namespace MultiTribute

#endif // DS1_OVERHAUL_MULTI_TRIBUTE_H_