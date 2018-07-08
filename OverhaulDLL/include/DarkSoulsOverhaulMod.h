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

#define WIN32_LEAN_AND_MEAN   // Exclude rarely-used stuff from Windows headers

// Windows Header Files:
#include <Windows.h>

// Game data/Overhaul mod data headers:
#include "ModData.h"
#include "GameData.h"
#include "Plugin/KeybindFunctions.h"

// Preprocessor definition utilized by the Dark Souls Game Files C++ library (DsGameFiles.lib)
#define __DS1_OVERHAUL_MOD

#define DS1_OVERHAUL_TXT_INTRO "\n+-------------------------------+\n|      Dark Souls Overhaul      |\n+-------------------------------+"


DWORD WINAPI on_process_attach_async(LPVOID lpParam);

void inline FATALERROR(const char* error_str) {
    MessageBox(NULL, error_str, NULL, MB_OK);
    exit(1);
}

#endif
