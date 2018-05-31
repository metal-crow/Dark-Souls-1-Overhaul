/*
    CONTRIBUTORS:
        Sean Pesce

*/

#pragma once

#ifndef SP_UTILITY_INI_CONFIG_H_
#define SP_UTILITY_INI_CONFIG_H_

#include "sp/sp.h"
#include "sp/file.h"
#include "sp/string.h"

#include <string>
#include <sstream>

#define SP_INI_CFG_FILE_NAME_DEFAULT_ "SP_config.ini"


__SP_NAMESPACE
namespace cfg {


//template <typename T>
//typedef struct user_preference {
//    std::string section;
//    std::string name;
//    T value;
//};


// Path to config file
extern std::string file;


// Creates a new config file if the file is missing
inline bool initialize(const std::string &settings_file = file)
{
    if (!sp::file::exists(settings_file)) {
        if (sp::file::write_text(settings_file, std::string(""), false)) {
            // Error occurred
            return false;
        }
    }
    return true;
}


// Reads a boolean-valued setting (1 or 0) from config file (Returns true for any non-zero value)
inline bool read_bool(const std::string &section, const std::string &pref_name, bool default_value = false, const std::string &settings_file = file)
{
#ifdef _WIN32
    return ((unsigned short)GetPrivateProfileInt(section.c_str(), pref_name.c_str(), default_value, sp::file::path_abs(settings_file).c_str()) != 0);
#else // !_WIN32

    // @TODO: Unix implementation: https://github.com/ndevilla/iniparser

#endif // _WIN32
}


inline bool write_bool(const std::string &section, const std::string &pref_name, bool value, const std::string &settings_file = file)
{
#ifdef _WIN32
    char c[2] = { '\0', '\0' };
    c[0] = value ? '1' : '0';
    return !!WritePrivateProfileString(section.c_str(), pref_name.c_str(), c, sp::file::path_abs(settings_file).c_str());
#else // !_WIN32

    // @TODO: Unix implementation: https://github.com/ndevilla/iniparser

#endif // _WIN32
}


// Reads an int setting from config file
inline int read_int(const std::string &section, const std::string &pref_name, int default_value = 0, const std::string &settings_file = file)
{
#ifdef _WIN32
    return GetPrivateProfileInt(section.c_str(), pref_name.c_str(), default_value, sp::file::path_abs(settings_file).c_str());
#else // !_WIN32

    // @TODO: Unix implementation: https://github.com/ndevilla/iniparser

#endif // _WIN32
}


inline bool write_int(const std::string &section, const std::string &pref_name, int value, const std::string &settings_file = file)
{
#ifdef _WIN32
    return !!WritePrivateProfileString(section.c_str(), pref_name.c_str(), std::to_string(value).c_str(), sp::file::path_abs(settings_file).c_str());
#else // !_WIN32

    // @TODO: Unix implementation: https://github.com/ndevilla/iniparser

#endif // _WIN32
}


// Reads a virtual-key code from config file
inline uint8_t read_vk_code(const std::string &section, const std::string &hotkey_name, const std::string &settings_file = file)
{
    unsigned short vk_code = 0;
#ifdef _WIN32
    char hk[3];
    GetPrivateProfileString(section.c_str(), hotkey_name.c_str(), NULL, hk, 3, sp::file::path_abs(settings_file).c_str());
    std::stringstream in;
    in << std::hex << hk;
    in >> vk_code;
#else // !_WIN32

    // @TODO: Unix implementation: https://github.com/ndevilla/iniparser

#endif // _WIN32
    return (uint8_t)vk_code; // Return the hotkey as a virtual key code
}


inline bool write_vk_code(const std::string &section, const std::string &hotkey_name, uint8_t value, const std::string &settings_file = file)
{
#ifdef _WIN32
    return !!WritePrivateProfileString(section.c_str(), hotkey_name.c_str(), sp::str::format("%2x", (unsigned short)value).c_str(), sp::file::path_abs(settings_file).c_str());
#else // !_WIN32

    // @TODO: Unix implementation: https://github.com/ndevilla/iniparser

#endif // _WIN32
}



// Reads a string-valued setting from config file
inline std::string read_string(const std::string &section, const std::string &pref_name, const std::string &default_value = "", const std::string &settings_file = file)
{
#ifdef _WIN32
    char buff[MAX_PATH+1];
    buff[MAX_PATH] = '\0';
    GetPrivateProfileString(section.c_str(), pref_name.c_str(), default_value.c_str(), buff, MAX_PATH+1, sp::file::path_abs(settings_file).c_str());
    return std::string(buff);
#else // !_WIN32

    // @TODO: Unix implementation: https://github.com/ndevilla/iniparser

#endif // _WIN32
}


inline bool write_string(const std::string &section, const std::string &pref_name, const std::string &value, const std::string &settings_file = file)
{
#ifdef _WIN32
    return !!WritePrivateProfileString(section.c_str(), pref_name.c_str(), value.c_str(), sp::file::path_abs(settings_file).c_str());
#else // !_WIN32

    // @TODO: Unix implementation: https://github.com/ndevilla/iniparser

#endif // _WIN32
}


} // namespace cfg
__SP_NAMESPACE_CLOSE // namespace sp

#endif // SP_UTILITY_INI_CONFIG_H_
