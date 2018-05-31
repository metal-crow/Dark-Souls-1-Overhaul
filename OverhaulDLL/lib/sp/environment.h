/*
    CONTRIBUTORS:
        Sean Pesce

*/

#pragma once

#ifndef SP_UTILITY_ENVIRONMENT_MAIN_H_
#define SP_UTILITY_ENVIRONMENT_MAIN_H_

#include "sp/sp.h"
#include "sp/string.h"
#include "sp/memory.h"


__SP_NAMESPACE
namespace env  {


inline void* exe_base()
{
    return sp::mem::get_process_base();
}


inline void* lib_base()
{
    // Source: https://stackoverflow.com/a/2396380/7891239
    MEMORY_BASIC_INFORMATION mem_info;
    size_t length = VirtualQueryEx(GetCurrentProcess(), (void*)lib_base, &mem_info, sizeof(mem_info));
    void* _lib_base = length ? (HMODULE)mem_info.AllocationBase : NULL;
    if (_lib_base == NULL)
    {
        sp::err::set(SP_ERR_INVALID_ADDRESS);
    }
    return _lib_base;
}


inline std::string hostname()
{
    char  buff[MAX_PATH + 1];
    DWORD buff_size = MAX_PATH + 1;
    std::string _hostname;
    // Get hostname and local username
    if (!GetComputerName(buff, &buff_size))
    {
        // Handle error
        _hostname = "";
        sp::err::set(SP_ERR_INVALID_NAME);
    }
    else
    {
        _hostname = buff;
    }
    return _hostname;
}


inline std::string username()
{
    char  buff[MAX_PATH + 1];
    DWORD buff_size = MAX_PATH + 1;
    std::string _username;
    if (!GetUserName(buff, &buff_size))
    {
        // Handle error
        _username = "";
        sp::err::set(SP_ERR_USER_PROFILE);
    }
    else
    {
        _username = buff;
    }
    return _username;
}


inline std::string lib_path()
{
    char  buff[MAX_PATH + 1];
    DWORD buff_size = MAX_PATH + 1;
    // Get path to folder containing DLL
    if (!GetModuleFileName((HMODULE)lib_base(), buff, buff_size))
    {
        // Handle error
        sp::err::set(SP_ERR_INVALID_DIR);
        return "";
    }
    return buff;
}


inline std::string exe_path()
{
    char  buff[MAX_PATH + 1];
    DWORD buff_size = MAX_PATH + 1;
    // Get path to folder containing executable
    if (!GetModuleFileName(NULL, buff, buff_size))
    {
        // Handle error
        sp::err::set(SP_ERR_INVALID_DIR);
        return "";
    }
    return buff;
}


inline std::string lib_name()
{
    std::string _lib_name = lib_path();
    if (_lib_name.length() == 0)
    {
        return _lib_name;
    }
    size_t file_name_start = _lib_name.find_last_of('\\');
    if (file_name_start == std::string::npos)
    {
        return "";
    }
    return _lib_name.substr(++file_name_start);
}


inline std::string exe_name()
{
    std::string _exe_name = exe_path();
    if (_exe_name.length() == 0)
    {
        return _exe_name;
    }
    size_t file_name_start = _exe_name.find_last_of('\\');
    if (file_name_start == std::string::npos)
    {
        return "";
    }
    return _exe_name.substr(++file_name_start);
}


inline std::string exe_dir()
{
    std::string _exe_dir = exe_path();
    if (_exe_dir.length() == 0)
    {
        return _exe_dir;
    }
    size_t file_name_start = _exe_dir.find_last_of('\\');
    if (file_name_start == std::string::npos)
    {
        return "";
    }
    return _exe_dir.substr(0, file_name_start);
}


inline std::string lib_dir()
{
    std::string _lib_dir = lib_path();
    if (_lib_dir.length() == 0)
    {
        return _lib_dir;
    }
    size_t file_name_start = _lib_dir.find_last_of('\\');
    if (file_name_start == std::string::npos)
    {
        return "";
    }
    return _lib_dir.substr(0, file_name_start);
}


inline std::string system_dir()
{
    char  buff[MAX_PATH + 1];
    DWORD buff_size = MAX_PATH + 1;
    std::string _system_dir;
    if (!GetSystemDirectory(buff, buff_size))
    {
        // Handle error
        _system_dir = "";
        sp::err::set(SP_ERR_INVALID_DIR);
    }
    else
    {
        _system_dir = buff;
    }
    return _system_dir;
}


inline uint32_t process_id()
{
    return GetCurrentProcessId();
}


inline std::string to_string(const std::string &prefix = "")
{
    std::string s = "";
    s += prefix + "EXE: " + exe_name() + "\n";
    s += prefix + "  Path: " + exe_dir() + "\n";
    s += prefix + "  Base: " + sp::str::format("0x%p", exe_base()) + " (" + std::to_string((unsigned long long)exe_base()) + ")\n";
    s += prefix + "Library: " + lib_name() + "\n";
    s += prefix + "  Path: " + lib_dir() + "\n";
    s += prefix + "  Base: " + sp::str::format("0x%p", lib_base()) + " (" + std::to_string((unsigned long long)lib_base()) + ")\n";
    s += prefix + "PID: " + std::to_string(process_id()) + "\n";
    s += prefix + "System path: " + system_dir() + "\n";
    s += prefix + "Host: " + hostname() + "\n";
    s += prefix + "User: " + username() + "\n";
    return s;
}



} // namespace env
__SP_NAMESPACE_CLOSE // namespace sp

#endif // SP_UTILITY_ENVIRONMENT_MAIN_H_
