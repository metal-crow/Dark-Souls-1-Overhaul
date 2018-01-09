/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Sean Pesce  -  C++
*/

#pragma once

#ifndef _DS1_OVERHAUL_GAME_FILES_H_
    #define _DS1_OVERHAUL_GAME_FILES_H_

#include "DllMain.h"
#include "Shlobj.h"
#include "SP_IO.hpp"
#include "Archive/Bdt.h"
#include "Archive/Bhd5.h"
#include "Save/Sl2.h"


/*
    Initialize constants:
*/

// Default save file path used by the game
std::string Files::default_save_file_path;

// Index of the save file currently being read/written by the game
int Files::save_file_index = 0;

// Pending save file index changes
bool Files::save_file_index_pending_set_next = false;
bool Files::save_file_index_pending_set_prev = false;


// Called when the game attempts to call CreateFileW
HANDLE WINAPI Files::intercept_create_file_w(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
                                             LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition,
                                             DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
    HANDLE tmp_handle;
    HANDLE *return_val = &tmp_handle;
    std::string fn_buff;
    std::wstring load_file;
    std::wstring load_file_ext;
    bool print_action = false;
    if (lpFileName != NULL)
    {
        load_file = lpFileName;
        if ((int)load_file.length() >= 4)
        {
            load_file_ext = load_file.substr(load_file.length() - 4);
            if ((int)Mod::custom_game_archive_path.length() > 0 && load_file_ext == BdtArchive::FILE_EXT_W)
            {
                // Intercept archive file load (.bdt)
                print_action = Mod::Debug::monitor_bdt;
                load_file = Mod::custom_game_archive_path + load_file.substr(load_file.length() - 5);
                // Store file handle for monitoring
                switch (load_file.c_str()[load_file.length() - 5]) {
                    case L'0':
                        return_val = &Files::io_monitors[BDT0].handle;
                        Files::io_monitors[BDT0].io_pos = 0;
                        break;
                    case L'1':
                        return_val = &Files::io_monitors[BDT1].handle;
                        Files::io_monitors[BDT1].io_pos = 0;
                        break;
                    case L'2':
                        return_val = &Files::io_monitors[BDT2].handle;
                        Files::io_monitors[BDT2].io_pos = 0;
                        break;
                    case L'3':
                    default:
                        return_val = &Files::io_monitors[BDT3].handle;
                        Files::io_monitors[BDT3].io_pos = 0;
                        break;
                }
            }
            else if ((int)Mod::custom_game_archive_path.length() > 0 && load_file_ext == std::wstring(&Bhd5Archive::FILE_EXT_W[1]))
            {
                // Intercept archive file load (.bhd5)
                print_action = Mod::Debug::monitor_bhd;
                load_file = Mod::custom_game_archive_path + load_file.substr(load_file.length() - 6);
                // Store file handle for monitoring
                
                switch (load_file.c_str()[load_file.length() - 6]) {
                    case L'0':
                        return_val = &Files::io_monitors[BHD0].handle;
                        Files::io_monitors[BHD0].io_pos = 0;
                        break;
                    case L'1':
                        return_val = &Files::io_monitors[BHD1].handle;
                        Files::io_monitors[BHD1].io_pos = 0;
                        break;
                    case L'2':
                        return_val = &Files::io_monitors[BHD2].handle;
                        Files::io_monitors[BHD2].io_pos = 0;
                        break;
                    case L'3':
                    default:
                        return_val = &Files::io_monitors[BHD3].handle;
                        Files::io_monitors[BHD3].io_pos = 0;
                        break;
                }
            }
            else if (load_file_ext == SaveFile::FILE_EXT_W)
            {
                // Intercept save file load (.sl2)
                print_action = Mod::Debug::monitor_sl2;
                return_val = &Files::io_monitors[SL2].handle;
                Files::io_monitors[SL2].io_pos = 0;

                // On first load, store default save file path
                if (Files::default_save_file_path.length() <= 0) {
                    int conversion_ret = string_wide_to_mb((wchar_t*)lpFileName, Files::default_save_file_path);
                    if (conversion_ret != 0) {
                        // Error converting file path string
                        print_console("ERROR: Failed to obtain default save file path");
                        Files::default_save_file_path = Sl2::FILE_NAME_DEFAULT;
                    }

                    // Check save files
                    std::string save_path, current_save_file;
                    int file_index = 0;
                    if ((int)Mod::custom_save_file_path.length() > 0) {
                        conversion_ret = string_wide_to_mb((wchar_t*)Mod::custom_save_file_path.c_str(), save_path);
                        if (conversion_ret != 0) {
                            conversion_ret = 0;
                            save_path = Files::default_save_file_path;
                        }
                    } else {
                        conversion_ret = 0;
                        save_path = Files::default_save_file_path;
                    }
                    if (conversion_ret == 0) {
                        // Count save files/characters and find first free save slot
                        int save_file_count = 0, saved_char_count = 0;
                        std::tuple<int, int> first_free(-1, -1);
                        std::string free_slot_msg;
                        save_file_count = Sl2::get_save_file_count(save_path.c_str(), &saved_char_count, &first_free);
                        free_slot_msg = Mod::output_prefix + "Found " + std::to_string(saved_char_count) + " character";
                        if (saved_char_count != 1) {
                            free_slot_msg += 's';
                        }
                        free_slot_msg += " in " + std::to_string(save_file_count) + " save file";
                        if (save_file_count != 1) {
                            free_slot_msg += 's';
                        }
                        if (std::get<0>(first_free) >= 0 && std::get<1>(first_free) >= 0) {
                            free_slot_msg += " (first free slot: " + std::to_string(std::get<0>(first_free)) + "," + std::to_string(std::get<1>(first_free)) + ")";
                        }
                        print_console(free_slot_msg);
                        // If all save files are full, generate new (empty) save file
                        if (std::get<0>(first_free) < 0 && std::get<1>(first_free) < 0) {
                            if (save_file_count < 10) {
                                current_save_file = save_path + "_0" + std::to_string(save_file_count);
                            } else {
                                current_save_file = save_path + "_" + std::to_string(save_file_count);
                            }
                            print_console(Mod::output_prefix + "No free character slots; generating next empty save file:\n    " + current_save_file);
                            Sl2::generate_empty_save_file(current_save_file.c_str());
                        }
                    }
                }

                if ((int)Mod::custom_save_file_path.length() > 0) {
                    Files::get_save_file_path(load_file);
                }
            }
        }
        // Call original function
        *return_val = CreateFileW(load_file.c_str(), dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
    }
    else
    {
        // Call original function with original arguments
        *return_val = CreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
    }

    IoMonitor *io_monitor = Files::io_monitor_from_handle(*return_val);
    if (io_monitor != NULL) {
        fn_buff = io_monitor->default_filename;
    }
    if (print_action && fn_buff.length() > 0) {
        print_console("Intercepted loading of " + fn_buff + " (" + FileUtil::to_hex_string((int)io_monitor->handle, fn_buff, true) + ")");
    }
    return *return_val;
}


// Called when the game attempts to call GetPrivateProfileIntW
UINT WINAPI Files::intercept_get_private_profile_int_w(LPCWSTR lpAppName, LPCWSTR lpKeyName, INT nDefault, LPCWSTR lpFileName)
{
    std::wstring load_file = lpFileName;
    if ((int)Mod::custom_config_file_path.length() > 0)
    {
        // Intercept config file load (.ini)
        load_file = Mod::custom_config_file_path;
    }
    // Call original function
    return GetPrivateProfileIntW(lpAppName, lpKeyName, nDefault, load_file.c_str());
}


// Called when the game attempts to call WritePrivateProfileSectionW
BOOL WINAPI Files::intercept_write_private_profile_section_w(LPCWSTR lpAppName, LPCWSTR lpString, LPCWSTR lpFileName)
{
    std::wstring load_file = lpFileName;
    if ((int)Mod::custom_config_file_path.length() > 0)
    {
        // Intercept config file load (.ini)
        load_file = Mod::custom_config_file_path;
    }
    // Call original function
    return WritePrivateProfileSectionW(lpAppName, lpString, load_file.c_str());
}


// Called when the game attempts to call ReadFile
BOOL WINAPI Files::intercept_read_file(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped)
{
    BOOL return_val;
    std::string fn_buff;
    int32_t file_pos_offset = 0;
    IoMonitor *io_monitor = Files::io_monitor_from_handle(hFile);
    // Call original function
    return_val = ReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
    
    if (lpOverlapped == NULL && lpNumberOfBytesRead != NULL) {
        file_pos_offset = *lpNumberOfBytesRead;
    }

    if (io_monitor != NULL) {
        fn_buff = io_monitor->default_filename;
    }
    if ((return_val || (lpNumberOfBytesRead && *lpNumberOfBytesRead)) && fn_buff.length() > 0) {
        if (lpOverlapped != NULL) {
            fn_buff += " (WARNING: Async read operation)";
        }
        if (io_monitor->monitor) {
            print_console("Reading " + std::to_string(nNumberOfBytesToRead) + " bytes from " + fn_buff + " at offset " + std::to_string(io_monitor->io_pos) + " (" + FileUtil::to_hex_string(io_monitor->io_pos, fn_buff, true) + ") and storing in buffer at " + FileUtil::to_hex_string((uint32_t)lpBuffer, fn_buff, true));
        }
        io_monitor->io_pos += file_pos_offset;
    }
    return return_val;
}


BOOL WINAPI Files::intercept_write_file(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped)
{
    BOOL return_val;
    std::string fn_buff;
    int32_t file_pos_offset = 0;
    IoMonitor *io_monitor = Files::io_monitor_from_handle(hFile);
    // Call original function
    return_val = WriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);

    if (lpOverlapped == NULL && lpNumberOfBytesWritten != NULL) {
        file_pos_offset = *lpNumberOfBytesWritten;
    }

    if (io_monitor != NULL) {
        fn_buff = io_monitor->default_filename;
    }
    if ((return_val || (lpNumberOfBytesWritten && *lpNumberOfBytesWritten)) && fn_buff.length() > 0) {
        if (lpOverlapped != NULL) {
            fn_buff += " (WARNING: Async write operation)";
        }
        if (io_monitor->monitor) {
            print_console("Writing " + std::to_string(nNumberOfBytesToWrite) + " bytes to " + fn_buff + " at offset " + std::to_string(io_monitor->io_pos) + " (" + FileUtil::to_hex_string(io_monitor->io_pos, fn_buff, true) + ")");
        }
        io_monitor->io_pos += file_pos_offset;
    }
    return return_val;
}


// Called when the game attempts to call CloseHandle
BOOL WINAPI Files::intercept_close_handle(HANDLE hObject)
{
    BOOL return_val;
    std::string fn_buff;
    IoMonitor *io_monitor = Files::io_monitor_from_handle(hObject);
    // Call original function
    return_val = CloseHandle(hObject);

    if (io_monitor != NULL) {
        fn_buff = io_monitor->default_filename;
    }
    if (return_val && fn_buff.length() > 0) {
        io_monitor->io_pos = 0;
        io_monitor->handle = NULL;
        if (io_monitor->monitor) {
            print_console("Closed file handle: " + fn_buff);
        }
    }
    return return_val;
}

// Called when the game attempted to call SetFilePointer
DWORD WINAPI Files::intercept_set_file_pointer(HANDLE hFile, LONG lDistanceToMove, PLONG lpDistanceToMoveHigh, DWORD dwMoveMethod)
{
    DWORD return_val;
    std::string fn_buff;
    IoMonitor *io_monitor = Files::io_monitor_from_handle(hFile);
    // Call original function
    return_val = SetFilePointer(hFile, lDistanceToMove, lpDistanceToMoveHigh, dwMoveMethod);

    if (io_monitor != NULL) {
        fn_buff = io_monitor->default_filename;
    }
    if ((return_val != INVALID_SET_FILE_POINTER) && fn_buff.length() > 0) {
        if (io_monitor->monitor) {
            print_console("New file pointer position for " + fn_buff + ": " + std::to_string(return_val) + " (" + FileUtil::to_hex_string(return_val, fn_buff, true) + ")");
        }
        io_monitor->io_pos = return_val;
    }
    return return_val;
}


// Patches game calls to various shared library functions, redirecting them to Game::intercept_create_file()
void Files::apply_function_intercepts()
{
    // CreateFileW
    static uint32_t create_file_w_func = (uint32_t)&Files::intercept_create_file_w;
    static uint32_t *create_file_w_func_ptr = &create_file_w_func;
    uint8_t *create_file_w_func_b = (uint8_t *)&create_file_w_func_ptr;
    uint8_t patch_w[4] = { create_file_w_func_b[0], create_file_w_func_b[1], create_file_w_func_b[2], create_file_w_func_b[3] };
    void *write_address = (uint8_t*)Game::ds1_base + 0x2B7D04; // Game archives and savegame file
    apply_byte_patch(write_address, patch_w, 4);
    write_address = (uint8_t*)Game::ds1_base + 0x2B7FEE; // Savegame file and config directory
    apply_byte_patch(write_address, patch_w, 4);


    // GetPrivateProfileIntW
    static uint32_t intercept_gppi_w_func = (uint32_t)&Files::intercept_get_private_profile_int_w;
    static uint32_t *intercept_gppi_w_ptr = &intercept_gppi_w_func;
    uint8_t *intercept_gppi_w_b = (uint8_t *)&intercept_gppi_w_ptr;
    uint8_t patch_gppi_w[4] = { intercept_gppi_w_b[0], intercept_gppi_w_b[1], intercept_gppi_w_b[2], intercept_gppi_w_b[3] };
    write_address = (uint8_t*)Game::ds1_base + 0xB70B17;
    apply_byte_patch(write_address, patch_gppi_w, 4);
    write_address = (uint8_t*)Game::ds1_base + 0xB70B2F;
    apply_byte_patch(write_address, patch_gppi_w, 4);
    write_address = (uint8_t*)Game::ds1_base + 0xB70B79; // This seems to be the only call to GetPrivateProfileIntW that actually gets executed
    apply_byte_patch(write_address, patch_gppi_w, 4);


    // WritePrivateProfileIntW
    static uint32_t intercept_wpps_w_func = (uint32_t)&Files::intercept_write_private_profile_section_w;
    static uint32_t *intercept_wpps_w_ptr = &intercept_wpps_w_func;
    uint8_t *intercept_wpps_w_b = (uint8_t *)&intercept_wpps_w_ptr;
    uint8_t patch_wpps_w[4] = { intercept_wpps_w_b[0], intercept_wpps_w_b[1], intercept_wpps_w_b[2], intercept_wpps_w_b[3] };
    write_address = (uint8_t*)Game::ds1_base + 0xB70C99;
    apply_byte_patch(write_address, patch_wpps_w, 4);


    // ReadFile
    static uint32_t intercept_read_file_func = (uint32_t)&Files::intercept_read_file;
    static uint32_t *intercept_read_file_ptr = &intercept_read_file_func;
    uint8_t *intercept_read_file_b = (uint8_t *)&intercept_read_file_ptr;
    uint8_t patch_read_file[4] = { intercept_read_file_b[0], intercept_read_file_b[1], intercept_read_file_b[2], intercept_read_file_b[3] };
    write_address = (uint8_t*)Game::ds1_base + 0x2B7E66;
    apply_byte_patch(write_address, patch_read_file, 4);

    // WriteFile
    static uint32_t intercept_write_file_func = (uint32_t)&Files::intercept_write_file;
    static uint32_t *intercept_write_file_ptr = &intercept_write_file_func;
    uint8_t *intercept_write_file_b = (uint8_t *)&intercept_write_file_ptr;
    uint8_t patch_write_file[4] = { intercept_write_file_b[0], intercept_write_file_b[1], intercept_write_file_b[2], intercept_write_file_b[3] };
    write_address = (uint8_t*)Game::ds1_base + 0x2B7684;
    apply_byte_patch(write_address, patch_write_file, 4);

    // CloseHandle
    static uint32_t intercept_close_handle_func = (uint32_t)&Files::intercept_close_handle;
    static uint32_t *intercept_close_handle_ptr = &intercept_close_handle_func;
    uint8_t *intercept_close_handle_b = (uint8_t *)&intercept_close_handle_ptr;
    uint8_t patch_close_handle[4] = { intercept_close_handle_b[0], intercept_close_handle_b[1], intercept_close_handle_b[2], intercept_close_handle_b[3] };
    write_address = (uint8_t*)Game::ds1_base + 0x2B7535;
    apply_byte_patch(write_address, patch_close_handle, 4);

    // SetFilePointer
    static uint32_t intercept_set_file_ptr_func = (uint32_t)&Files::intercept_set_file_pointer;
    static uint32_t *intercept_set_file_ptr_ptr = &intercept_set_file_ptr_func;
    uint8_t *intercept_set_file_ptr_b = (uint8_t *)&intercept_set_file_ptr_ptr;
    uint8_t patch_set_file_ptr[4] = { intercept_set_file_ptr_b[0], intercept_set_file_ptr_b[1], intercept_set_file_ptr_b[2], intercept_set_file_ptr_b[3] };
    write_address = (uint8_t*)Game::ds1_base + 0x2B69EE;
    apply_byte_patch(write_address, patch_set_file_ptr, 4);
}

// Initializes game file I/O monitor structs
void Files::init_io_monitors()
{
    io_monitors[SL2].default_filename   = std::string(Sl2::FILE_NAME_DEFAULT)    + Sl2::FILE_EXT;
    io_monitors[SL2].default_filename_w = std::wstring(Sl2::FILE_NAME_DEFAULT_W) + Sl2::FILE_EXT_W;

    io_monitors[BDT0].default_filename   = std::string(BdtArchive::FILE_NAME_DEFAULT)    +  "0" + BdtArchive::FILE_EXT;
    io_monitors[BDT0].default_filename_w = std::wstring(BdtArchive::FILE_NAME_DEFAULT_W) + L"0" + BdtArchive::FILE_EXT_W;
    io_monitors[BDT1].default_filename   = std::string(BdtArchive::FILE_NAME_DEFAULT)    +  "1" + BdtArchive::FILE_EXT;
    io_monitors[BDT1].default_filename_w = std::wstring(BdtArchive::FILE_NAME_DEFAULT_W) + L"1" + BdtArchive::FILE_EXT_W;
    io_monitors[BDT2].default_filename   = std::string(BdtArchive::FILE_NAME_DEFAULT)    +  "2" + BdtArchive::FILE_EXT;
    io_monitors[BDT2].default_filename_w = std::wstring(BdtArchive::FILE_NAME_DEFAULT_W) + L"2" + BdtArchive::FILE_EXT_W;
    io_monitors[BDT3].default_filename   = std::string(BdtArchive::FILE_NAME_DEFAULT)    +  "3" + BdtArchive::FILE_EXT;
    io_monitors[BDT3].default_filename_w = std::wstring(BdtArchive::FILE_NAME_DEFAULT_W) + L"3" + BdtArchive::FILE_EXT_W;

    io_monitors[BHD0].default_filename   = std::string(Bhd5Archive::FILE_NAME_DEFAULT)    +  "0" + Bhd5Archive::FILE_EXT;
    io_monitors[BHD0].default_filename_w = std::wstring(Bhd5Archive::FILE_NAME_DEFAULT_W) + L"0" + Bhd5Archive::FILE_EXT_W;
    io_monitors[BHD1].default_filename   = std::string(Bhd5Archive::FILE_NAME_DEFAULT)    +  "1" + Bhd5Archive::FILE_EXT;
    io_monitors[BHD1].default_filename_w = std::wstring(Bhd5Archive::FILE_NAME_DEFAULT_W) + L"1" + Bhd5Archive::FILE_EXT_W;
    io_monitors[BHD2].default_filename   = std::string(Bhd5Archive::FILE_NAME_DEFAULT)    +  "2" + Bhd5Archive::FILE_EXT;
    io_monitors[BHD2].default_filename_w = std::wstring(Bhd5Archive::FILE_NAME_DEFAULT_W) + L"2" + Bhd5Archive::FILE_EXT_W;
    io_monitors[BHD3].default_filename   = std::string(Bhd5Archive::FILE_NAME_DEFAULT)    +  "3" + Bhd5Archive::FILE_EXT;
    io_monitors[BHD3].default_filename_w = std::wstring(Bhd5Archive::FILE_NAME_DEFAULT_W) + L"3" + Bhd5Archive::FILE_EXT_W;
}


// Checks if custom archive files (.bdt/.bhd5) exist. If one or more are missing, custom archives are not loaded, and default game files are loaded instead.
void Files::check_custom_archive_file_path()
{
    if ((int)Mod::custom_game_archive_path.length() == 0)
        return;
    Mod::startup_messages.push_back(std::string(Mod::output_prefix + "Checking if custom game archive files exist..."));

    // Get char* strings for printing console messages
    errno_t conversion_return;
    std::string archive_name_ch = "";
    if (conversion_return = string_wide_to_mb((wchar_t*)Mod::custom_game_archive_path.c_str(), archive_name_ch))
    {
        // Error converting from wide char to char
        Mod::startup_messages.push_back(std::string(Mod::output_prefix + "ERROR: Unable to parse custom archive file name (Error code ").append(std::to_string(conversion_return)).append("). Using default archive files instead."));
        Mod::custom_game_archive_path = L"";
        return;
    }

    // Check that custom game archive files exist
    for (int i = 0; i < BdtArchive::FILE_COUNT_DEFAULT; i++)
    {
        if (!FileUtil::file_exists(std::wstring(Mod::custom_game_archive_path).append(std::to_wstring(i)).append(BdtArchive::FILE_EXT_W).c_str()))
        {
            // Custom .bdt file doesn't exist
            Mod::startup_messages.push_back(std::string(Mod::output_prefix + "ERROR: The file \"").append(archive_name_ch).append(std::to_string(i)).append(BdtArchive::FILE_EXT).append("\" could not be found. Using default archive files instead."));
            Mod::custom_game_archive_path = L"";
            return;
        }
        else
            Mod::startup_messages.push_back(std::string("    Found ").append(archive_name_ch).append(std::to_string(i)).append(BdtArchive::FILE_EXT));

        if (!FileUtil::file_exists(std::wstring(Mod::custom_game_archive_path).append(std::to_wstring(i)).append(Bhd5Archive::FILE_EXT_W).c_str()))
        {
            // Custom .bhd5 file doesn't exist
            Mod::startup_messages.push_back(std::string(Mod::output_prefix + "ERROR: The file \"").append(archive_name_ch).append(std::to_string(i)).append(Bhd5Archive::FILE_EXT).append("\" could not be found. Using default archive files."));
            Mod::custom_game_archive_path = L"";
            return;
        }
        else
            Mod::startup_messages.push_back(std::string("    Found ").append(archive_name_ch).append(std::to_string(i)).append(Bhd5Archive::FILE_EXT));
    }
    Mod::startup_messages.push_back(std::string(Mod::output_prefix + "SUCCESS: Custom game archive files will be loaded (\"").append(archive_name_ch).append("\")."));
}


// Checks if custom save file (.sl2) exists
void Files::check_custom_save_file_path()
{
    if ((int)Mod::custom_save_file_path.length() == 0)
        return;
    Mod::startup_messages.push_back(std::string(Mod::output_prefix + "Checking if custom save file exists..."));

    errno_t conversion_return;
    std::string filename_ch;
    if (conversion_return = string_wide_to_mb((wchar_t*)Mod::custom_save_file_path.c_str(), filename_ch))
    {
        // Error converting from wide char to char
        return;
    }

    if (FileUtil::file_exists(filename_ch.c_str()))
    {
        Mod::startup_messages.push_back(std::string(Mod::output_prefix + "SUCCESS: Custom save file will be loaded (\"").append(filename_ch).append("\")."));
    }
    else
    {
        // Custom save file doesn't exist
        Mod::startup_messages.push_back(std::string(Mod::output_prefix + "WARNING: Custom save file was not found (\"").append(filename_ch).append("\"). No characters will be available to load."));
    }
}


// Checks if custom game config file (.ini) exists
void Files::check_custom_game_config_file_path()
{
    if ((int)Mod::custom_config_file_path.length() == 0)
        return;

    Mod::startup_messages.push_back(std::string(Mod::output_prefix + "Checking if custom game config file exists..."));

    // Get char* strings for printing console messages
    errno_t conversion_return;
    std::string filename_ch;
    if (conversion_return = string_wide_to_mb((wchar_t*)Mod::custom_config_file_path.c_str(), filename_ch))
    {
        // Error converting from wide char to char
        return;
    }

    if (FileUtil::file_exists(Mod::custom_config_file_path.c_str()))
    {
        Mod::startup_messages.push_back(std::string(Mod::output_prefix + "SUCCESS: Custom game config file will be loaded (\"").append(filename_ch).append("\")."));
    }
    else
    {
        // Custom config file doesn't exist
        Mod::startup_messages.push_back(std::string(Mod::output_prefix + "WARNING: Custom config file was not found (\"").append(filename_ch).append("\"). Default configuration file will be automatically generated."));
    }
}

// Returns the full file path of the current save file (with index)
const wchar_t *Files::get_save_file_path(std::wstring &buffer)
{
    buffer.clear();
    // Get base file name
    if (Mod::custom_save_file_path.length() <= 0) {
        if (Files::default_save_file_path.length() <= 0 || (string_mb_to_wide((char*)Files::default_save_file_path.c_str(), buffer) != 0)) {
            buffer = Sl2::FILE_NAME_DEFAULT_W;
        }
    } else {
        buffer = Mod::custom_save_file_path;
    }
    // Append index
    if (Files::save_file_index > 0) {
        if (Files::save_file_index < 10) {
            buffer += L"_0" + std::to_wstring(Files::save_file_index);
        } else {
            buffer += L"_" + std::to_wstring(Files::save_file_index);
        }
    }
    return buffer.c_str();
}


// Changes the current save file index (and writes all corresponding data)
void Files::set_save_file_index(int unsigned index, bool print_output)
{
    std::string save_path;
    int save_file_count = 0, player_char_status = DS1_PLAYER_STATUS_LOADING;
    uint8_t saved_chars_menu_flag = 0;
    SpPointer char_preview_data = SpPointer((void*)((uint32_t)Game::ds1_base + 0xF78700), { 0x30, 0x10 });
    //SpPointer menu_logo = SpPointer((void*)((uint32_t)Game::ds1_base + 0xF786D0), { 0x6C });
    SpPointer saved_chars_menu_flag_ptr = SpPointer((void*)((uint32_t)Game::ds1_base + 0xF786D0), { 0x90 });
    bool return_to_saves_screen = false;
    if ((int)index > 99) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return;
    }
    if (Game::ds1_base == NULL || char_preview_data.resolve() == NULL) {
        SetLastError(ERROR_BAD_ENVIRONMENT);
        return;
    }
    // Get base save file name
    if (Mod::custom_save_file_path.length() <= 0) {
        if (Files::default_save_file_path.length() <= 0) {
            save_path = Sl2::FILE_NAME_DEFAULT;
        } else {
            save_path = Files::default_save_file_path;
        }
    } else {
        if (string_wide_to_mb((wchar_t*)Mod::custom_save_file_path.c_str(), save_path) != 0) {
            SetLastError(ERROR_CLUSTER_INVALID_STRING_FORMAT);
            if (print_output)
                print_console("ERROR: Failed to set save file index (Conversion error)");
            return;
        }
    }
    // Check if index is out of range
    save_file_count = Sl2::get_save_file_count(save_path.c_str());
    if ((int)index >= save_file_count) {
        SetLastError(ERROR_RANGE_NOT_FOUND);
        if (print_output)
            print_console("ERROR: Failed to set save file index (out of range)");
        return;
    }
    // Check if character is loaded
    if (Game::player_char_status.read(&player_char_status) != ERROR_SUCCESS || player_char_status != DS1_PLAYER_STATUS_LOADING) {
        SetLastError(ERROR_BAD_ENVIRONMENT);
        if (print_output)
            print_console("ERROR: Failed to set save file index (Character is loaded)");
        return;
    }
    // Check if viewing saves menu
    if (saved_chars_menu_flag_ptr.read(&saved_chars_menu_flag) != ERROR_SUCCESS || (saved_chars_menu_flag != 4 && saved_chars_menu_flag != 3)) {
        SetLastError(ERROR_BAD_ENVIRONMENT);
        if (print_output)
            print_console("ERROR: Failed to set save file index (Must be viewing saved characters)");
        return;
    }
    saved_chars_menu_flag_ptr.write((uint8_t)0);
    Files::save_file_index = index;
    // Get full filename (with index)
    if (Files::save_file_index > 0) {
        if (Files::save_file_index < 10) {
            save_path += "_0" + std::to_string(Files::save_file_index);
        } else {
            save_path += "_" + std::to_string(Files::save_file_index);
        }
    }
    // Overwrite character preview data
    if (Sl2::read_character_preview_data_from_file(save_path.c_str(), char_preview_data.resolve()) != ERROR_SUCCESS) {
        if (print_output)
            print_console("ERROR: Failed to set save file index (I/O error)");
        saved_chars_menu_flag_ptr.write((uint8_t)3);
        return;
    }
    Sleep(500);
    // Re-load saved characters menu
    saved_chars_menu_flag_ptr.write((uint8_t)3);
    if (print_output)
        print_console("Save file index changed to " + std::to_string(index));
    SetLastError(ERROR_SUCCESS);
}

// Changes the to the next save file (if current save file is the last one, new file is first save file)
void Files::set_save_file_next(bool print_output)
{
    int count;
    std::string file;
    if (Mod::custom_save_file_path.length() > 0) {
        if (string_wide_to_mb((wchar_t*)Mod::custom_save_file_path.c_str(), file)) {
            SetLastError(ERROR_CLUSTER_INVALID_STRING_FORMAT);
            return;
        }
    } else if (Files::default_save_file_path.length() > 0) {
        file = Files::default_save_file_path;
    } else {
        SetLastError(ERROR_FILE_NOT_FOUND);
        return;
    }
    count = Sl2::get_save_file_count(file.c_str());
    if (Files::save_file_index == (count - 1)) {
        Files::set_save_file_index(0, print_output);
    } else {
        Files::set_save_file_index(Files::save_file_index + 1, print_output);
    }
}

// Changes the to the previous save file (if current save file is the first one, new file is last save file)
void Files::set_save_file_prev(bool print_output)
{
    int count;
    std::string file;
    if (Mod::custom_save_file_path.length() > 0) {
        if (string_wide_to_mb((wchar_t*)Mod::custom_save_file_path.c_str(), file)) {
            SetLastError(ERROR_CLUSTER_INVALID_STRING_FORMAT);
            return;
        }
    } else if (Files::default_save_file_path.length() > 0) {
        file = Files::default_save_file_path;
    } else {
        SetLastError(ERROR_FILE_NOT_FOUND);
        return;
    }
    count = Sl2::get_save_file_count(file.c_str());
    if (Files::save_file_index == 0) {
        Files::set_save_file_index(count - 1, print_output);
    } else {
        Files::set_save_file_index(Files::save_file_index - 1, print_output);
    }
}


// Checks if the saved characters menu is currently open
bool Files::saves_menu_is_open()
{
    SpPointer saved_chars_menu_flag = SpPointer((void*)((uint32_t)Game::ds1_base + 0xF786D0), { 0x90 });
    uint8_t flag_value = 0;
    int status = 0;
    saved_chars_menu_flag.read(&flag_value);
    // Check if viewing saves menu
    if (saved_chars_menu_flag.resolve() == NULL || (flag_value != 4 && flag_value != 3)) {
        return false;
    }
    // Check if character is loaded
    return !(Game::player_char_status.read(&status) != ERROR_SUCCESS || status != DS1_PLAYER_STATUS_LOADING);
}


// Returns the address of the file I/O monitoring struct corresponding to the specified file handle
Files::IoMonitor *Files::io_monitor_from_handle(HANDLE handle) {
    if (handle != NULL) {
        // Unfortuntely switch statements aren't allowed for HANDLE
        if (handle == io_monitors[SL2].handle) {
            return &(io_monitors[SL2]);
        }  else if (handle == io_monitors[BDT0].handle) {
            return &(io_monitors[BDT0]);
        } else if (handle == io_monitors[BDT1].handle) {
            return &(io_monitors[BDT1]);
        } else if (handle == io_monitors[BDT2].handle) {
            return &(io_monitors[BDT2]);
        } else if (handle == io_monitors[BDT3].handle) {
            return &(io_monitors[BDT3]);
        } else if (handle == io_monitors[BHD0].handle) {
            return &(io_monitors[BHD0]);
        } else if (handle == io_monitors[BHD1].handle) {
            return &(io_monitors[BHD1]);
        } else if (handle == io_monitors[BHD2].handle) {
            return &(io_monitors[BHD2]);
        } else if (handle == io_monitors[BHD3].handle) {
            return &(io_monitors[BHD3]);
        }
    }
    return NULL;
}

#endif // _DS1_OVERHAUL_GAME_FILES_H_