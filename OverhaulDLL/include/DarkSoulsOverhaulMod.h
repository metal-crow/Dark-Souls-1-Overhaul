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

static const uint64_t VERSION_RAW = 2023020200;
#define VERSION_STRING  "Beta16"
#define DS1_OVERHAUL_TXT_INTRO "\n+------------------------------------------------------+\n|      Dark Souls Overhaul   ["VERSION_STRING"]                 |\n+------------------------------------------------------+"

extern HMODULE d3d11_module;

DWORD WINAPI on_process_attach_async(LPVOID lpParam);

[[noreturn]] void inline FATALERROR(const char* error_str, ...) {
    char dest[1024];
    va_list argptr;
    va_start(argptr, error_str);
    vsnprintf(dest, 1024, error_str, argptr);
    va_end(argptr);

    global::cmd_out << dest << "\n";

    crash_handler(NULL, dest);
}

static const char* logfilename = "dsoverhaul_logging.txt";
extern FILE* logfile;

void inline ConsoleWrite(const char* str, ...)
{
    if (logfile == NULL)
    {
        fopen_s(&logfile, logfilename, "w");
    }

    char dest[1024];
    va_list argptr;
    va_start(argptr, str);
    vsnprintf(dest, 1024, str, argptr);
    va_end(argptr);

    uint64_t time;
    QueryUnbiasedInterruptTime(&time); //returns time in units of 100 nanoseconds since system started
    time = time / 10000; //to ms

    global::cmd_out << "[" << std::to_string(time) << "] " << dest << "\n";

    if (logfile != NULL)
    {
        fprintf(logfile, "[%lld] %s\n", time, dest);
    }
}

#endif
