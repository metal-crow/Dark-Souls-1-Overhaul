/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Sean Pesce        -  Reverse engineering, C++
*/


#pragma once

#ifndef DS1_OVERHAUL_MENU_SAVED_CHARACTERS_H_
    #define DS1_OVERHAUL_MENU_SAVED_CHARACTERS_H_

#include <cstdint>
#include <string>

#define DS1_MENU_SAVED_CHARS_DEFAULT_CUSTOM_LOAD_HEADER_MSG      std::wstring(L"Select data to load")
#define DS1_MENU_SAVED_CHARS_DEFAULT_CUSTOM_LOAD_BUTTONS_MSG     std::wstring(L"<?selectUD?>:Select <?conclusion?>:Enter <?cancel?>:Back <?viewChange?>:Toggle Display <?commando?>:Delete")
#define DS1_MENU_SAVED_CHARS_DEFAULT_CUSTOM_LOAD_BUTTONS_ALT_MSG std::wstring(L"<?conclusion?>:Enter <?cancel?>:Back <?viewChange?>:Toggle Display <?commando?>:Delete  <?categoryChangeL?>/<?categoryChangeR?>:Change save file <?slectMenuSwitch?>:Choose save file")
#define DS1_MENU_SAVED_CHARS_DEFAULT_CUSTOM_DELETE_HEADER_MSG    std::wstring(L"Select data to delete")
#define DS1_MENU_SAVED_CHARS_DEFAULT_CUSTOM_DELETE_BUTTONS_MSG   std::wstring(L"<?selectUD?>:Select <?conclusion?>:Enter <?cancel?>:Back <?viewChange?>:Toggle Display <?commando?>:Delete")

namespace Menu {
namespace Saves {

// Opens a number picker dialog that allows the user to choose a save file by index
void open_dialog();


void init_custom_strings(std::wstring &load_header_msg      = DS1_MENU_SAVED_CHARS_DEFAULT_CUSTOM_LOAD_HEADER_MSG,
                         std::wstring &load_buttons_msg     = DS1_MENU_SAVED_CHARS_DEFAULT_CUSTOM_LOAD_BUTTONS_MSG,
                         std::wstring &load_buttons_alt_msg = DS1_MENU_SAVED_CHARS_DEFAULT_CUSTOM_LOAD_BUTTONS_ALT_MSG,
                         std::wstring &delete_header_msg    = DS1_MENU_SAVED_CHARS_DEFAULT_CUSTOM_DELETE_HEADER_MSG,
                         std::wstring &delete_buttons_msg   = DS1_MENU_SAVED_CHARS_DEFAULT_CUSTOM_DELETE_BUTTONS_MSG);
void set_custom_header_load_msg(std::wstring &msg);
void set_custom_buttons_load_msg(std::wstring &msg);
void set_custom_buttons_load_alt_msg(std::wstring &msg);
void set_custom_header_delete_msg(std::wstring &msg);
void set_custom_buttons_delete_msg(std::wstring &msg);
void set_custom_header_msgs(std::wstring &load_message, std::wstring &delete_message);
void set_custom_buttons_msgs(std::wstring &load_message, std::wstring &load_message_alt, std::wstring &delete_message);
void set_custom_msgs(std::wstring &load_header_msg,
                     std::wstring &load_buttons_msg,
                     std::wstring &load_buttons_alt_msg,
                     std::wstring &delete_header_msg,
                     std::wstring &delete_buttons_msg);
void remove_custom_strings_patch();

// Injected assembly function that enables custom messages in place of "Select data to load" and "Select data to delete"
void __stdcall asm_custom_strings();


} // namespace Saves
} // namespace Menu

#undef DS1_MENU_SAVED_CHARS_DEFAULT_CUSTOM_LOAD_HEADER_MSG
#undef DS1_MENU_SAVED_CHARS_DEFAULT_CUSTOM_LOAD_BUTTONS_MSG
#undef DS1_MENU_SAVED_CHARS_DEFAULT_CUSTOM_LOAD_BUTTONS_ALT_MSG
#undef DS1_MENU_SAVED_CHARS_DEFAULT_CUSTOM_DELETE_HEADER_MSG
#undef DS1_MENU_SAVED_CHARS_DEFAULT_CUSTOM_DELETE_BUTTONS_MSG

#endif // DS1_OVERHAUL_MENU_SAVED_CHARACTERS_H_