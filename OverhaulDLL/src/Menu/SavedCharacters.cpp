/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Sean Pesce        -  Reverse engineering, C++
*/

#include "DarkSoulsOverhaulMod.h"
#include "SP/memory/injection/asm/x64.h"
#include "Menu/SavedCharacters.h"
#include "Files.h"
#include "MainLoop.h"

#define DS1_MENU_SAVED_CHARS_CUSTOM_MSG_BUFF_LEN 256
const uint64_t DS1_MENU_SAVED_CHARS_CUST_STR_INJECTION_OFFSET = 0x54a99d;

namespace Menu {
namespace Saves {

bool custom_strings_initialized = false;
// For injecting custom strings in place of "Select data to load" and "Select data to delete"
wchar_t custom_header_load_msg_buff[DS1_MENU_SAVED_CHARS_CUSTOM_MSG_BUFF_LEN];
wchar_t custom_buttons_load_msg_buff[DS1_MENU_SAVED_CHARS_CUSTOM_MSG_BUFF_LEN];
wchar_t custom_buttons_load_alt_msg_buff[DS1_MENU_SAVED_CHARS_CUSTOM_MSG_BUFF_LEN];
wchar_t custom_header_delete_msg_buff[DS1_MENU_SAVED_CHARS_CUSTOM_MSG_BUFF_LEN];
wchar_t custom_buttons_delete_msg_buff[DS1_MENU_SAVED_CHARS_CUSTOM_MSG_BUFF_LEN];

extern "C" {
    uint64_t custom_strings_return;
    void custom_strings_inject();

    uint64_t custom_header_load_msg_address = (uint64_t)&custom_header_load_msg_buff[0];
    uint64_t custom_buttons_load_msg_address = (uint64_t)&custom_buttons_load_msg_buff[0];
    uint64_t custom_buttons_load_alt_msg_address = (uint64_t)&custom_buttons_load_alt_msg_buff[0];
    uint64_t custom_header_delete_msg_address = (uint64_t)&custom_header_delete_msg_buff[0];
    uint64_t custom_buttons_delete_msg_address = (uint64_t)&custom_buttons_delete_msg_buff[0];
}

bool ui_save_file_index_changes(void* unused);

void init_custom_saves(std::wstring &load_header_msg,
                         std::wstring &load_buttons_msg,
                         std::wstring &load_buttons_alt_msg,
                         std::wstring &delete_header_msg,
                         std::wstring &delete_buttons_msg)
{
    // Inject custom strings
    custom_header_load_msg_buff[0]      = 0;
    custom_buttons_load_msg_buff[0]     = 0;
    custom_buttons_load_alt_msg_buff[0] = 0;
    custom_header_delete_msg_buff[0]    = 0;
    custom_buttons_delete_msg_buff[0]   = 0;
    sp::mem::code::x64::inject_jmp_14b((uint8_t*)Game::ds1_base + DS1_MENU_SAVED_CHARS_CUST_STR_INJECTION_OFFSET, &custom_strings_return, 2, &custom_strings_inject);
    custom_strings_initialized = true;
    set_custom_msgs(load_header_msg,
                    load_buttons_msg,
                    load_buttons_alt_msg,
                    delete_header_msg,
                    delete_buttons_msg);

    //Prevents the "Load Game" menu button from disappearring
    uint8_t nop_patch[] = { 0x90, 0x90 };
    uint8_t *write_address = (uint8_t*)(disable_load_save_button_check + Game::ds1_base);
    sp::mem::patch_bytes(write_address, nop_patch, sizeof(nop_patch));

    //Remove the check during "Load Game" button press which throws an error if the save file has 0 entries
    //Replaces the conditional with const jump
    uint8_t jmp_patch[] = { 0xEB, 0x3F };
    write_address = (uint8_t*)(disable_load_save_entry_count_check + Game::ds1_base);
    sp::mem::patch_bytes(write_address, jmp_patch, sizeof(jmp_patch));

    //start the callback for listening for UI changes/button presses to change the save file
    MainLoop::setup_mainloop_callback(ui_save_file_index_changes, NULL, "ui_save_file_index_changes");
}

bool ui_save_file_index_changes(void* unused)
{
    // Check for pending save file index changes
    if (Files::save_file_index_pending_set_next)
    {
        if (Files::saves_menu_is_open())
        {
            Files::set_save_file_next();
        }
        Files::save_file_index_pending_set_next = false;
    }
    if (Files::save_file_index_pending_set_prev)
    {
        if (Files::saves_menu_is_open())
        {
            Files::set_save_file_prev();
        }
        Files::save_file_index_pending_set_prev = false;
    }
    if (Files::save_file_index_make_new)
    {
        if (Files::saves_menu_is_open())
        {
            Files::create_new_save_file();
        }
        Files::save_file_index_make_new = false;
    }

    return true;
}

void set_custom_header_load_msg(std::wstring &msg)
{
    size_t load_msg_length = msg.length();
    SetLastError(ERROR_SUCCESS);
    memset(custom_header_load_msg_buff, 0, DS1_MENU_SAVED_CHARS_CUSTOM_MSG_BUFF_LEN);
    if (custom_strings_initialized) {
        // Shorten string (if larger than buffer)
        if (load_msg_length >= DS1_MENU_SAVED_CHARS_CUSTOM_MSG_BUFF_LEN) {
            load_msg_length = DS1_MENU_SAVED_CHARS_CUSTOM_MSG_BUFF_LEN - 1;
            SetLastError(ERROR_BUFFER_OVERFLOW);
        }
        // Copy string
        memcpy_s(custom_header_load_msg_buff, sizeof(wchar_t) * DS1_MENU_SAVED_CHARS_CUSTOM_MSG_BUFF_LEN, msg.c_str(), sizeof(wchar_t) * load_msg_length);
        // Manually null-terminate string to be safe
        custom_header_load_msg_buff[DS1_MENU_SAVED_CHARS_CUSTOM_MSG_BUFF_LEN - 1] = 0;
    } else {
        SetLastError(ERROR_BAD_ENVIRONMENT);
    }
}


void set_custom_buttons_load_msg(std::wstring &msg)
{
    size_t load_msg_length = msg.length();
    SetLastError(ERROR_SUCCESS);
    memset(custom_buttons_load_msg_buff, 0, DS1_MENU_SAVED_CHARS_CUSTOM_MSG_BUFF_LEN);
    if (custom_strings_initialized) {
        // Shorten string (if larger than buffer)
        if (load_msg_length >= DS1_MENU_SAVED_CHARS_CUSTOM_MSG_BUFF_LEN) {
            load_msg_length = DS1_MENU_SAVED_CHARS_CUSTOM_MSG_BUFF_LEN - 1;
            SetLastError(ERROR_BUFFER_OVERFLOW);
        }
        // Copy string
        memcpy_s(custom_buttons_load_msg_buff, sizeof(wchar_t) * DS1_MENU_SAVED_CHARS_CUSTOM_MSG_BUFF_LEN, msg.c_str(), sizeof(wchar_t) * load_msg_length);
        // Manually null-terminate string to be safe
        custom_buttons_load_msg_buff[DS1_MENU_SAVED_CHARS_CUSTOM_MSG_BUFF_LEN - 1] = 0;
    } else {
        SetLastError(ERROR_BAD_ENVIRONMENT);
    }
}


void set_custom_buttons_load_alt_msg(std::wstring &msg)
{
    size_t load_msg_length = msg.length();
    SetLastError(ERROR_SUCCESS);
    memset(custom_buttons_load_alt_msg_buff, 0, DS1_MENU_SAVED_CHARS_CUSTOM_MSG_BUFF_LEN);
    if (custom_strings_initialized) {
        // Shorten string (if larger than buffer)
        if (load_msg_length >= DS1_MENU_SAVED_CHARS_CUSTOM_MSG_BUFF_LEN) {
            load_msg_length = DS1_MENU_SAVED_CHARS_CUSTOM_MSG_BUFF_LEN - 1;
            SetLastError(ERROR_BUFFER_OVERFLOW);
        }
        // Copy string
        memcpy_s(custom_buttons_load_alt_msg_buff, sizeof(wchar_t) * DS1_MENU_SAVED_CHARS_CUSTOM_MSG_BUFF_LEN, msg.c_str(), sizeof(wchar_t) * load_msg_length);
        // Manually null-terminate string to be safe
        custom_buttons_load_alt_msg_buff[DS1_MENU_SAVED_CHARS_CUSTOM_MSG_BUFF_LEN - 1] = 0;
    } else {
        SetLastError(ERROR_BAD_ENVIRONMENT);
    }
}


void set_custom_header_delete_msg(std::wstring &msg)
{
    size_t delete_msg_length = msg.length();
    SetLastError(ERROR_SUCCESS);
    memset(custom_header_delete_msg_buff, 0, DS1_MENU_SAVED_CHARS_CUSTOM_MSG_BUFF_LEN);
    if (custom_strings_initialized) {
        // Shorten string (if larger than buffer)
        if (delete_msg_length >= DS1_MENU_SAVED_CHARS_CUSTOM_MSG_BUFF_LEN) {
            delete_msg_length = DS1_MENU_SAVED_CHARS_CUSTOM_MSG_BUFF_LEN - 1;
            SetLastError(ERROR_BUFFER_OVERFLOW);
        }
        // Copy string
        memcpy_s(custom_header_delete_msg_buff, sizeof(wchar_t) * DS1_MENU_SAVED_CHARS_CUSTOM_MSG_BUFF_LEN, msg.c_str(), sizeof(wchar_t) * delete_msg_length);
        // Manually null-terminate string to be safe
        custom_header_delete_msg_buff[DS1_MENU_SAVED_CHARS_CUSTOM_MSG_BUFF_LEN - 1] = 0;
    } else {
        SetLastError(ERROR_BAD_ENVIRONMENT);
    }
}


void set_custom_buttons_delete_msg(std::wstring &msg)
{
    size_t delete_msg_length = msg.length();
    SetLastError(ERROR_SUCCESS);
    memset(custom_buttons_delete_msg_buff, 0, DS1_MENU_SAVED_CHARS_CUSTOM_MSG_BUFF_LEN);
    if (custom_strings_initialized) {
        // Shorten string (if larger than buffer)
        if (delete_msg_length >= DS1_MENU_SAVED_CHARS_CUSTOM_MSG_BUFF_LEN) {
            delete_msg_length = DS1_MENU_SAVED_CHARS_CUSTOM_MSG_BUFF_LEN - 1;
            SetLastError(ERROR_BUFFER_OVERFLOW);
        }
        // Copy string
        memcpy_s(custom_buttons_delete_msg_buff, sizeof(wchar_t) * DS1_MENU_SAVED_CHARS_CUSTOM_MSG_BUFF_LEN, msg.c_str(), sizeof(wchar_t) * delete_msg_length);
        // Manually null-terminate string to be safe
        custom_buttons_delete_msg_buff[DS1_MENU_SAVED_CHARS_CUSTOM_MSG_BUFF_LEN - 1] = 0;
    } else {
        SetLastError(ERROR_BAD_ENVIRONMENT);
    }
}


void set_custom_header_msgs(std::wstring &load_message, std::wstring &delete_message)
{
    set_custom_header_load_msg(load_message);
    set_custom_header_delete_msg(delete_message);
}

void set_custom_buttons_msgs(std::wstring &load_message, std::wstring &load_message_alt, std::wstring &delete_message)
{
    set_custom_buttons_load_msg(load_message);
    set_custom_buttons_load_alt_msg(load_message_alt);
    set_custom_buttons_delete_msg(delete_message);
}


void set_custom_msgs(std::wstring &load_header_msg,
                     std::wstring &load_buttons_msg,
                     std::wstring &load_buttons_alt_msg,
                     std::wstring &delete_header_msg,
                     std::wstring &delete_buttons_msg)
{
    set_custom_header_load_msg(load_header_msg);
    set_custom_buttons_load_msg(load_buttons_msg);
    set_custom_buttons_load_alt_msg(load_buttons_alt_msg);
    set_custom_header_delete_msg(delete_header_msg);
    set_custom_buttons_delete_msg(delete_buttons_msg);
}

} // namespace Saves
} // namespace Menu

#undef DS1_MENU_SAVED_CHARS_CUSTOM_MSG_BUFF_LEN
#undef DS1_MENU_SAVED_CHARS_CUST_STR_INJECTION_ADDRESS
#undef DS1_MENU_SAVED_CHARS_CUST_STR_INJECTION_OFFSET
