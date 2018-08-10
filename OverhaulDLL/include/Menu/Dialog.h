/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Sean Pesce  -  Reverse engineering, C++
*/


#pragma once

#ifndef DS1_OVERHAUL_MENU_DIALOGS_MAIN_H_
    #define DS1_OVERHAUL_MENU_DIALOGS_MAIN_H_

#include <cstdint>
#include <string>

#include "SP_AoB_Scan.hpp"
#include "FileUtil.h"


// Unused message IDs for custom strings
#define DS1_OVERHAUL_CUSTOM_DIALOG_MSG_DEFAULT_ID_  44001  // Default offset in FSB = 0x42C
#define DS1_OVERHAUL_CUSTOM_DIALOG_BT_L_DEFAULT_ID_ 44002  // Default offset in FSB = 0x430
#define DS1_OVERHAUL_CUSTOM_DIALOG_BT_R_DEFAULT_ID_ 44003  // Default offset in FSB = 0x434

#define DS1_OVERHAUL_CUSTOM_DIALOG_MSG_MAX_LEN_ 1024
#define DS1_OVERHAUL_CUSTOM_DIALOG_BT_MAX_LEN_  128


namespace Menu {
namespace Dlg  {


enum class dlg_type {
    no_dlg                =  0,
    message_dlg           =  1, // 1 button
    prompt_dlg            =  2, // 2 buttons
    number_picker_dlg     =  3, // 2 buttons
    message_large_dlg     =  4, // 1 button
    prompt_large_dlg      =  5, // 2 buttons
    loading_dlg           =  6, // 0 buttons
    interact_1bt_dlg      =  7, // 1 button
    interact_2bt_dlg      =  8, // 2 buttons
    interact_0bt_dlg      =  9, // 0 buttons
    message_large_0bt_dlg = 10  // 0 buttons
};


enum class dlg_item_use {
    no_dlg = 0,
    prompt_dlg = 1, // 2 buttons
    unknown_soft_humanity_menu = 4,
    write_soapstone_msg = 5,
    soapstone_msg_options = 6,
    book_of_guilty = 7,
    servant_roster = 8
};


// Button pressed on the last shown dialog
enum class dlg_button_result {
    no_button    = 0, // Dialog closed with B button
    left_button  = 1,
    right_button = 2,
    unpressed    = 0x7FFFFFFF // Dialog is still open
};





static SpPointer menu_flags((void*)0x13786D0 /* DARKSOULS.EXE+0xF786D0 */, { 0x0 });

static uint32_t& msg_id      = *(uint32_t*)0x012E33E0; // DARKSOULS.exe+0xEE33E0
static uint32_t& bt_left_id  = *(uint32_t*)0x012E33E4; // DARKSOULS.exe+0xEE33E4
static uint32_t& bt_right_id = *(uint32_t*)0x012E33E8; // DARKSOULS.exe+0xEE33E8

// Value selected on the last shown number picker dialog
static uint32_t& number_picker_result = *(uint32_t*)0x012E3400; // DARKSOULS.exe+0xEE3400

static wchar_t msg_buffer[DS1_OVERHAUL_CUSTOM_DIALOG_MSG_MAX_LEN_];
static wchar_t bt_l_buffer[DS1_OVERHAUL_CUSTOM_DIALOG_BT_MAX_LEN_];
static wchar_t bt_r_buffer[DS1_OVERHAUL_CUSTOM_DIALOG_BT_MAX_LEN_];

// Shows a dialog box for the specified duration (in milliseconds), then closes it
void show_async(dlg_type dialog_type, unsigned long duration);


inline void* menu_fsb(bool silent = false)
{
    static void* addr = NULL;
    if (!addr)
    {
        std::string out_msg;
        addr = aob_scan("00 00 01 00 ?? ?? ?? ?? 01 00 00 00 2E 00 00 00 F4 00 00 00");
        if (addr)
        {
            FileUtil::to_hex_string((uint32_t)addr, out_msg, true);
            out_msg = "Found dialog menu strings FSB at " + out_msg;
        }
        else
        {
            out_msg = "Scanning for dialog menu strings FSB... Not found.";
        }
        
        if (!silent)
        {
            print_console(out_msg);
        }
    }
    return addr;
}


// Returns starting address of menu flags array
inline uint8_t* flags()
{
    static uint8_t* flags_start = NULL;
    if (!flags_start)
    {
        flags_start = reinterpret_cast<uint8_t*>(const_cast<SpPointer&>(menu_flags).resolve());
    }
    return flags_start;
}


// Returns reference to the menu flag at the specified index
inline uint32_t& flag(size_t index = 0)
{
    static uint32_t dummy_flag = 0xFFFFFFFF;
    dummy_flag = 0xFFFFFFFF;
    return flags() ? reinterpret_cast<uint32_t&>(flags()[index]) : dummy_flag;
}


inline bool showing()
{
    return !!flag(96);
}


inline void hide()
{
    flag(96) = 0;
}


inline void show(dlg_type dialog_type)
{
    hide();
    flag(96) = static_cast<uint32_t>(dialog_type);
}


inline void show_loader(uint32_t msg_id)
{
    Dlg::msg_id = msg_id;

    if (flags())
    {
        show(dlg_type::loading_dlg);
    }
}


inline void show_message_large_0bt(uint32_t msg_id)
{
    Dlg::msg_id = msg_id;

    if (flags())
    {
        show(dlg_type::message_large_0bt_dlg);
    }
}


inline void show_message_large_0bt(const std::wstring& msg)
{
    memset(msg_buffer, 0, sizeof(wchar_t)*DS1_OVERHAUL_CUSTOM_DIALOG_MSG_MAX_LEN_);
    wcscpy_s(msg_buffer, msg.c_str());
    // Redirect unused string ID to msg_buffer
    uint32_t offset = reinterpret_cast<uint32_t>(menu_fsb()) + 0x42C;
    *reinterpret_cast<uint32_t*>(offset) = reinterpret_cast<uint32_t>(msg_buffer) - reinterpret_cast<uint32_t>(menu_fsb());

    if (flags())
    {
        show_message_large_0bt(DS1_OVERHAUL_CUSTOM_DIALOG_MSG_DEFAULT_ID_);
    }
}


inline void show_number_picker(uint32_t msg_id, uint32_t l_bt_id = 1, uint32_t r_bt_id = 2, uint32_t default_val = 1, uint32_t min_val = 1, uint32_t max_val = 0)
{
    Dlg::msg_id = msg_id;
    bt_left_id  = l_bt_id;
    bt_right_id = r_bt_id;

    if (flags())
    {
        flag(520) = min_val;     // Number picker minimum value
        flag(516) = max_val;     // Number picker maximum value
        flag(524) = default_val; // Number picker starting value
        show(dlg_type::number_picker_dlg);
    }
}


inline void show_number_picker(const std::wstring& msg, const std::wstring& l_bt, const std::wstring& r_bt, uint32_t default_val = 1, uint32_t min_val = 1, uint32_t max_val = 0)
{
    if (menu_fsb())
    {
        memset(msg_buffer,  0, sizeof(wchar_t)*DS1_OVERHAUL_CUSTOM_DIALOG_MSG_MAX_LEN_);
        memset(bt_l_buffer, 0, sizeof(wchar_t)*DS1_OVERHAUL_CUSTOM_DIALOG_BT_MAX_LEN_ );
        memset(bt_r_buffer, 0, sizeof(wchar_t)*DS1_OVERHAUL_CUSTOM_DIALOG_BT_MAX_LEN_ );

        wcscpy_s(msg_buffer,  msg.c_str());
        wcscpy_s(bt_l_buffer, l_bt.c_str());
        wcscpy_s(bt_r_buffer, r_bt.c_str());

        // Redirect unused string ID to msg_buffer
        uint32_t offset = reinterpret_cast<uint32_t>(menu_fsb())+0x42C;
        *reinterpret_cast<uint32_t*>(offset) = reinterpret_cast<uint32_t>(msg_buffer) - reinterpret_cast<uint32_t>(menu_fsb());

        // Redirect unused string ID to bt_l_buffer
        offset = reinterpret_cast<uint32_t>(menu_fsb()) + 0x430;
        *reinterpret_cast<uint32_t*>(offset) = reinterpret_cast<uint32_t>(bt_l_buffer) - reinterpret_cast<uint32_t>(menu_fsb());

        // Redirect unused string ID to bt_r_buffer
        offset = reinterpret_cast<uint32_t>(menu_fsb()) + 0x434;
        *reinterpret_cast<uint32_t*>(offset) = reinterpret_cast<uint32_t>(bt_r_buffer) - reinterpret_cast<uint32_t>(menu_fsb());

        show_number_picker(DS1_OVERHAUL_CUSTOM_DIALOG_MSG_DEFAULT_ID_, DS1_OVERHAUL_CUSTOM_DIALOG_BT_L_DEFAULT_ID_, DS1_OVERHAUL_CUSTOM_DIALOG_BT_R_DEFAULT_ID_, default_val, min_val, max_val);
    }
    else
    {
        print_console("ERROR: Failed to create custom dialog (dialog menu strings FSB not yet loaded)");
    }
}


// Button pressed on the last shown dialog
inline int32_t button_result()
{
    return flags() ? flag(436) : 0;
}


// Value selected on the last shown number picker dialog
//inline uint32_t& number_picker_result()
//{
//    return *(uint32_t*)0x012E3400; // DARKSOULS.exe+EE3400
//}


} // namespace Dlg
} // namespace Menu


#endif // DS1_OVERHAUL_MENU_DIALOGS_MAIN_H_