/*
    DARK SOULS GAME FILE C++ LIBRARY

    Contributors to this file:
        Sean Pesce  -  C++


*/

#pragma once

#ifndef _DS1_FILE_LIB_MAIN_DEFS_H_
    #define _DS1_FILE_LIB_MAIN_DEFS_H_




#ifdef __DS1_OVERHAUL_MOD
    #define __output_prefix Mod::output_prefix
#else
    #define __output_prefix std::string("")
#endif


#include <stdint.h>


#endif // _DS1_FILE_LIB_MAIN_DEFS_H_
