/*
    DARK SOULS GAME FILE C++ LIBRARY

    Contributors to this file:
        Sean Pesce     -  C++
        Burton Radons  -  Reverse-engineering Dark Souls game files (Github: https://github.com/Burton-Radons )


    Archive/Bnd4.h

    References:
        https://github.com/Burton-Radons/Alexandria


*/

#pragma once

#ifndef DS1_FILE_LIB_BND4_ARCHIVE_FILE_H_
    #define DS1_FILE_LIB_BND4_ARCHIVE_FILE_H_


#include "Windows.h"
#include "Archive/Header.h"



// For reading and manipulating BND4 archive files
typedef class Bnd4ArchiveFile {
public:

    typedef struct ArchiveHeader {
        // Magic identifier bytes
        uint32_t magic32_0 = 0x34444E42; // "BND4"
    } Header;

    // Extended by Sl2

} Bnd4Archive;
typedef Bnd4Archive Bnd4;



#endif // DS1_FILE_LIB_BND4_ARCHIVE_FILE_H_
