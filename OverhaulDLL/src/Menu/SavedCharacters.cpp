/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Sean Pesce        -  Reverse engineering, C++
*/

#include "DllMain.h"
#include "Menu/SavedCharacters.h"
#include "Menu/Dialog.h"

#include "FileUtil.h"
#include "Save/Sl2.h"


#define DS1_MENU_SAVED_CHARS_CUSTOM_MSG_BUFF_LEN 256
#define DS1_MENU_SAVED_CHARS_CUST_STR_INJECTION_OFFSET  0x911C26
#define DS1_MENU_SAVED_CHARS_CUST_STR_INJECTION_ADDRESS ((void*)((uint32_t)Game::ds1_base + DS1_MENU_SAVED_CHARS_CUST_STR_INJECTION_OFFSET))


namespace Menu  {
namespace Saves {

bool custom_strings_initialized = false;
// For injecting custom strings in place of "Select data to load" and "Select data to delete"
wchar_t custom_header_load_msg_buff[DS1_MENU_SAVED_CHARS_CUSTOM_MSG_BUFF_LEN];
wchar_t custom_buttons_load_msg_buff[DS1_MENU_SAVED_CHARS_CUSTOM_MSG_BUFF_LEN];
wchar_t custom_buttons_load_alt_msg_buff[DS1_MENU_SAVED_CHARS_CUSTOM_MSG_BUFF_LEN];
wchar_t custom_header_delete_msg_buff[DS1_MENU_SAVED_CHARS_CUSTOM_MSG_BUFF_LEN];
wchar_t custom_buttons_delete_msg_buff[DS1_MENU_SAVED_CHARS_CUSTOM_MSG_BUFF_LEN];
uint32_t custom_header_load_msg_address      = (uint32_t)&custom_header_load_msg_buff[0];
uint32_t custom_buttons_load_msg_address     = (uint32_t)&custom_buttons_load_msg_buff[0];
uint32_t custom_buttons_load_alt_msg_address = (uint32_t)&custom_buttons_load_alt_msg_buff[0];
uint32_t custom_header_delete_msg_address    = (uint32_t)&custom_header_delete_msg_buff[0];
uint32_t custom_buttons_delete_msg_address   = (uint32_t)&custom_buttons_delete_msg_buff[0];
uint32_t custom_strings_return = 0;



DWORD WINAPI _open_dialog_internal(LPVOID lpParam)
{
    // Determine save file path
    std::string file;
    if (Mod::custom_save_file_path.length() <= 0)
    {
        if (Files::default_save_file_path.length() <= 0)
        {
            file = Sl2::FILE_NAME_DEFAULT;
        } 
        else 
        {
            file = Files::default_save_file_path;
        }
    }
    else
    {
        if (string_wide_to_mb(const_cast<wchar_t*>(Mod::custom_save_file_path.c_str()), file))
        {
            // Conversion error
            print_console("ERROR: Unable to open save file dialog");
            return 0;
        }
    }

    int count = Sl2::get_save_file_count(file.c_str());

    if (count > 1)
    {
        Menu::Dlg::show_number_picker(L"Select a save file to load", L"OK", L"CANCEL", Files::save_file_index + 1, 1, count);
        Sleep(100);
        Menu::Dlg::flag(0x2CC) = 1; // Disable menu input
        
        while (Menu::Dlg::showing())
        {
            Sleep(200);
        }
        
        Menu::Dlg::flag(0x2CC) = 0; // Re-enable menu input
        if (Menu::Dlg::button_result() == static_cast<int32_t>(Menu::Dlg::dlg_button_result::left_button)
            && (Menu::Dlg::number_picker_result-1) != Files::save_file_index)
        {
            Files::set_save_file_index(Menu::Dlg::number_picker_result - 1);
        }
    }
    return 0;
}


// Opens a number picker dialog that allows the user to choose a save file by index
void open_dialog()
{
    if (!CreateThread(NULL, 0, _open_dialog_internal, NULL, 0, NULL))
    {
        print_console("ERROR: Failed to create thread");
    }
}


void init_custom_strings(std::wstring &load_header_msg,
                         std::wstring &load_buttons_msg,
                         std::wstring &load_buttons_alt_msg,
                         std::wstring &delete_header_msg,
                         std::wstring &delete_buttons_msg)
{
    custom_header_load_msg_buff[0]      = 0;
    custom_buttons_load_msg_buff[0]     = 0;
    custom_buttons_load_alt_msg_buff[0] = 0;
    custom_header_delete_msg_buff[0]    = 0;
    custom_buttons_delete_msg_buff[0]   = 0;
    set_mem_protection(DS1_MENU_SAVED_CHARS_CUST_STR_INJECTION_ADDRESS, 8, MEM_PROTECT_RWX);
    inject_jmp_5b((uint8_t*)DS1_MENU_SAVED_CHARS_CUST_STR_INJECTION_ADDRESS, &custom_strings_return, 3, asm_custom_strings);
    custom_strings_initialized = true;
    set_custom_msgs(load_header_msg,
                    load_buttons_msg,
                    load_buttons_alt_msg,
                    delete_header_msg,
                    delete_buttons_msg);
}


void set_custom_header_load_msg(std::wstring &msg)
{
    int load_msg_length = msg.length();
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
    int load_msg_length = msg.length();
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
    int load_msg_length = msg.length();
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
    int delete_msg_length = msg.length();
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
    int delete_msg_length = msg.length();
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


void remove_custom_strings_patch()
{
    uint8_t original_bytes[8] = { 0x66, 0x83, 0x38, 0x00, 0x8B, 0x5C, 0x24, 0x7C };
    apply_byte_patch(DS1_MENU_SAVED_CHARS_CUST_STR_INJECTION_ADDRESS, original_bytes, 8);
    custom_strings_initialized = false;
}


// Injected assembly function that enables custom messages in place of "Select data to load" and "Select data to delete"
void __declspec(naked) __stdcall asm_custom_strings()
{
    __asm
    {
        cmp edi, 0xA028  // 41000 = ID of "Select data to load"
        je string_id_load
        cmp edi, 0xA02A  // 41002 = ID of "Select data to load" (alt)
        je string_id_load_alt
        cmp edi, 0xA029  // 41001 = ID of "Select data to delete"
        je string_id_delete
        jmp original_code

        string_id_load:
        cmp byte ptr[eax], 0x3C // 0x3C == '<'; First character of buttons/dialog strings but not header string (language-independent implementation)
        jne display_custom_load_string
        cmp byte ptr[eax + 0x6], 0x65 // 0x65 == 'e'; Fourth character of buttons string (and not dialog string)
        je display_custom_load_buttons_string
        jmp original_code

        string_id_load_alt:
        cmp byte ptr[eax], 0x3C
        jne display_custom_load_string
        cmp byte ptr[eax + 0x6], 0x65
        je display_custom_load_buttons_string_alt
        jmp original_code

        string_id_delete:
        cmp byte ptr[eax], 0x3C
        jne display_custom_delete_string
        cmp byte ptr[eax + 0x6], 0x65
        je display_custom_delete_buttons_string
        jmp original_code

        // Display custom message in place of "Select data to load"
        display_custom_load_string:
        mov eax, custom_header_load_msg_address
        jmp original_code

        // Display custom message in place of buttons string
        display_custom_load_buttons_string:
        mov eax, custom_buttons_load_msg_address
        jmp original_code

        // Display custom message in place of buttons string
        display_custom_load_buttons_string_alt:
        mov eax, custom_buttons_load_alt_msg_address
        jmp original_code

        // Display custom message in place of "Select data to delete"
        display_custom_delete_string:
        mov eax, custom_header_delete_msg_address
        jmp original_code

        // Display custom message in place of buttons string
        display_custom_delete_buttons_string:
        mov eax, custom_buttons_delete_msg_address

        original_code:
        cmp word ptr [eax], 0x00
        mov ebx, [esp+0x7C]

        jmp custom_strings_return
    }
}

} // namespace Saves
} // namespace Menu

#undef DS1_MENU_SAVED_CHARS_CUSTOM_MSG_BUFF_LEN
#undef DS1_MENU_SAVED_CHARS_CUST_STR_INJECTION_ADDRESS
#undef DS1_MENU_SAVED_CHARS_CUST_STR_INJECTION_OFFSET