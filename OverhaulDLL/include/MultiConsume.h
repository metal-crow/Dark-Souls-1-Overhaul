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

    extern uint32_t skip_next_multiconsume_dialog;
    extern uint32_t force_consume_next_item;
    extern uint32_t last_multiconsume_item;
    extern uint32_t last_consumption_quantity;
    //extern uint32_t skip_next_whiff_anim;
    extern uint32_t item_quantity;
    extern uint32_t capture_next_dlg_result;
    extern uint32_t consumed_from_inventory;

    void apply(const std::string& output_prefix = "");
    void unpatch(const std::string& output_prefix = "");
    bool is_active();

} // namespace MultiConsume

#endif // DS1_OVERHAUL_MULTI_CONSUME_H_