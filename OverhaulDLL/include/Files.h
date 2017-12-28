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
    if (lpFileName != NULL)
    {
        load_file = lpFileName;
        if ((int)load_file.length() >= 4)
        {
            load_file_ext = load_file.substr(load_file.length() - 4);
            if ((int)Mod::custom_game_archive_path.length() > 0 && load_file_ext == BdtArchive::FILE_EXT_W)
            {
                // Intercept archive file load (.bdt)
                load_file = Mod::custom_game_archive_path + load_file.substr(load_file.length() - 5);
                // Store file handle for monitoring
                switch (load_file.c_str()[load_file.length() - 5]) {
                    case L'0':
                        return_val = &Files::bdt_handles[0];
                        Files::bdt_io_pos[0] = 0;
                        break;
                    case L'1':
                        return_val = &Files::bdt_handles[1];
                        Files::bdt_io_pos[1] = 0;
                        break;
                    case L'2':
                        return_val = &Files::bdt_handles[2];
                        Files::bdt_io_pos[2] = 0;
                        break;
                    case L'3':
                    default:
                        return_val = &Files::bdt_handles[3];
                        Files::bdt_io_pos[3] = 0;
                        break;
                }
            }
            else if ((int)Mod::custom_game_archive_path.length() > 0 && load_file_ext == std::wstring(&Bhd5Archive::FILE_EXT_W[1]))
            {
                // Intercept archive file load (.bhd5)
                load_file = Mod::custom_game_archive_path + load_file.substr(load_file.length() - 6);
                // Store file handle for monitoring
                switch (load_file.c_str()[load_file.length() - 6]) {
                    case L'0':
                        return_val = &Files::bhd_handles[0];
                        Files::bhd_io_pos[0] = 0;
                        break;
                    case L'1':
                        return_val = &Files::bhd_handles[1];
                        Files::bhd_io_pos[1] = 0;
                        break;
                    case L'2':
                        return_val = &Files::bhd_handles[2];
                        Files::bhd_io_pos[2] = 0;
                        break;
                    case L'3':
                    default:
                        return_val = &Files::bhd_handles[3];
                        Files::bhd_io_pos[3] = 0;
                        break;
                }
            }
            else if ((int)Mod::custom_save_file_path.length() > 0 && (load_file_ext == SaveFile::FILE_EXT_W))
            {
                // Intercept save file load (.sl2)
                load_file = Mod::custom_save_file_path;
                return_val = &Files::sl2_handle;
                Files::sl2_io_pos = 0;
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

    Files::default_filename_from_handle(*return_val, fn_buff);
    if (fn_buff.length() > 0) {
        print_console("Intercepted loading of " + fn_buff);
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
    // Call original function
    return_val = ReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
    
    if (lpOverlapped == NULL && lpNumberOfBytesRead != NULL) {
        file_pos_offset = *lpNumberOfBytesRead;
    }

    Files::default_filename_from_handle(hFile, fn_buff);
    if ((return_val || (lpNumberOfBytesRead && *lpNumberOfBytesRead)) && fn_buff.length() > 0) {
        if (lpOverlapped != NULL) {
            fn_buff += " (WARNING: Async read operation)";
        }
        uint32_t new_io_pos = Files::update_file_position(hFile, file_pos_offset);
        print_console("Reading " + std::to_string(nNumberOfBytesToRead) + " bytes from " + fn_buff + " at offset " + std::to_string(new_io_pos) + " (" + FileUtil::to_hex_string(new_io_pos, fn_buff, true) + ")");
    }
    return return_val;
}


BOOL WINAPI Files::intercept_write_file(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped)
{
    BOOL return_val;
    std::string fn_buff;
    int32_t file_pos_offset = 0;
    // Call original function
    return_val = WriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);

    if (lpOverlapped == NULL && lpNumberOfBytesWritten != NULL) {
        file_pos_offset = *lpNumberOfBytesWritten;
    }

    Files::default_filename_from_handle(hFile, fn_buff);
    if ((return_val || (lpNumberOfBytesWritten && *lpNumberOfBytesWritten)) && fn_buff.length() > 0) {
        if (lpOverlapped != NULL) {
            fn_buff += " (WARNING: Async write operation)";
        }
        uint32_t new_io_pos = Files::update_file_position(hFile, file_pos_offset);
        print_console("Writing " + std::to_string(nNumberOfBytesToWrite) + " bytes to " + fn_buff + " at offset " + std::to_string(new_io_pos) + " (" + FileUtil::to_hex_string(new_io_pos, fn_buff, true) + ")");
    }
    return return_val;
}


// Called when the game attempts to call CloseHandle
BOOL WINAPI Files::intercept_close_handle(HANDLE hObject)
{
    BOOL return_val;
    std::string fn_buff;
    // Call original function
    return_val = CloseHandle(hObject);

    Files::default_filename_from_handle(hObject, fn_buff);
    if (return_val && fn_buff.length() > 0) {
        Files::forget_file_handle(hObject);
        print_console("Closed file handle: " + fn_buff);
    }
    return return_val;
}

// Called when the game attempted to call SetFilePointer
DWORD WINAPI Files::intercept_set_file_pointer(HANDLE hFile, LONG lDistanceToMove, PLONG lpDistanceToMoveHigh, DWORD dwMoveMethod)
{
    DWORD return_val;
    std::string fn_buff;
    // Call original function
    return_val = SetFilePointer(hFile, lDistanceToMove, lpDistanceToMoveHigh, dwMoveMethod);

    Files::default_filename_from_handle(hFile, fn_buff);
    if ((return_val != INVALID_SET_FILE_POINTER) && fn_buff.length() > 0) {
        print_console("New file pointer position for " + fn_buff + ": " + std::to_string(return_val) + " (" + FileUtil::to_hex_string(return_val, fn_buff, true) + ")");
        Files::update_file_position(hFile, return_val, true);
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


// Returns the name of the game file that the specified file handle corresponds to
const char* Files::default_filename_from_handle(HANDLE handle, std::string& filename)
{
    filename.clear();
    if (handle != NULL) {
        // Unfortuntely switch statements aren't allowed for HANDLE
        if (handle == Files::sl2_handle) {
            filename = Sl2::FILE_NAME_DEFAULT + std::string(Sl2::FILE_EXT);
        } else {
            auto res = find(Files::bdt_handles.begin(), Files::bdt_handles.end(), handle);
            if (res != Files::bdt_handles.end()) {
                filename = BdtArchive::FILE_NAME_DEFAULT + std::to_string(res - Files::bdt_handles.begin()) + BdtArchive::FILE_EXT;
            } else if ((res = find(Files::bhd_handles.begin(), Files::bhd_handles.end(), handle)) != Files::bhd_handles.end()) {
                filename = Bhd5Archive::FILE_NAME_DEFAULT + std::to_string(res - Files::bhd_handles.begin()) + Bhd5Archive::FILE_EXT;
            }
        }
    }
    return filename.c_str();
}


// Removes the specified file handle from the list of monitored file handles
void Files::forget_file_handle(HANDLE handle)
{
    int i = -1;
    if (handle != NULL) {
        if (handle == Files::sl2_handle) {
            Files::sl2_handle = NULL;
            Files::sl2_io_pos = 0;
        } else {
            auto res = find(Files::bdt_handles.begin(), Files::bdt_handles.end(), handle);
            if (res != Files::bdt_handles.end()) {
                i = res - Files::bdt_handles.begin();
                Files::bdt_handles[i] = NULL;
                Files::bdt_io_pos[i] = 0;
            } else if ((res = find(Files::bhd_handles.begin(), Files::bhd_handles.end(), handle)) != Files::bhd_handles.end()) {
                i = res - Files::bhd_handles.begin();
                Files::bhd_handles[i] = NULL;
                Files::bhd_io_pos[i] = 0;
            }
        }
    }
}

// Updates the I/O offset of a monitored game file
uint32_t Files::update_file_position(HANDLE handle, int32_t offset, bool absolute_offset)
{
    int i = -1;
    if (handle != NULL) {
        if (handle == Files::sl2_handle) {
            Files::sl2_io_pos = (absolute_offset ? offset : (Files::sl2_io_pos + offset));
            return Files::sl2_io_pos;
        } else {
            auto res = find(Files::bdt_handles.begin(), Files::bdt_handles.end(), handle);
            if (res != Files::bdt_handles.end()) {
                i = res - Files::bdt_handles.begin();
                Files::bdt_io_pos[i] = (absolute_offset ? offset : (Files::bdt_io_pos[i] + offset));
                return Files::bdt_io_pos[i];
            } else if ((res = find(Files::bhd_handles.begin(), Files::bhd_handles.end(), handle)) != Files::bhd_handles.end()) {
                i = res - Files::bhd_handles.begin();
                Files::bhd_io_pos[i] = (absolute_offset ? offset : (Files::bhd_io_pos[i] + offset));
                return Files::bhd_io_pos[i];
            }
        }
    }
    return -1;
}

#endif // _DS1_OVERHAUL_GAME_FILES_H_