/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Metal-Crow  -  C++
        Sean Pesce  -  C++

*/
#pragma once

#ifndef DS1_OVERHAUL_PROJECT_MAIN_H_
#define DS1_OVERHAUL_PROJECT_MAIN_H_


#include "sp/sp.h"
#include "dllmain.h"
#include "CrashHandler.h"

#define WIN32_LEAN_AND_MEAN   // Exclude rarely-used stuff from Windows headers

// Windows Header Files:
#include <Windows.h>

// Game data/Overhaul mod data headers:
#include "ModData.h"
#include "GameData.h"

// Preprocessor definition utilized by the Dark Souls Game Files C++ library (DsGameFiles.lib)
#define __DS1_OVERHAUL_MOD

#define DS1_OVERHAUL_TXT_INTRO "\n+-------------------------------+\n|      Dark Souls Overhaul      |\n+-------------------------------+"

extern HMODULE d3d11_module;

DWORD WINAPI on_process_attach_async(LPVOID lpParam);
DWORD WINAPI wait_for_first_char_load(LPVOID lpParam);

void inline FATALERROR(const char* error_str, ...) {
    char dest[1024];
    va_list argptr;
    va_start(argptr, error_str);
    vsnprintf(dest, 1024, error_str, argptr);
    va_end(argptr);

    crash_handler(dest);
}

void inline ConsoleWrite(const char* str, ...)
{
    char dest[1024];
    va_list argptr;
    va_start(argptr, str);
    vsnprintf(dest, 1024, str, argptr);
    va_end(argptr);

    global::cmd_out << dest << "\n";
}

#endif
