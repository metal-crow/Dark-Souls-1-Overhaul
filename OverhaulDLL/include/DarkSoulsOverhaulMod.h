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

// Preprocessor definition utilized by the Dark Souls Game Files C++ library (DsGameFiles.lib)
#define __DS1_OVERHAUL_MOD

#define DS1_OVERHAUL_TXT_INTRO "\n+-------------------------------+\n|      Dark Souls Overhaul      |\n+-------------------------------+"


DWORD WINAPI on_process_attach_async(LPVOID lpParam);


#endif
