/*
    DARK SOULS GAME FILE C++ LIBRARY

    Contributors to this file:
    Sean Pesce    -  C++

    Archive/Header.h
*/

#pragma once

#ifndef _DS1_FILE_LIB_ARCHIVE_FILE_HEADER_H_
    #define _DS1_FILE_LIB_ARCHIVE_FILE_HEADER_H_

#include "Windows.h"
#include <stdint.h>
#include <fstream>
#include "FileUtil.h"

// Each type of archive file has at least 12 magic identifier bytes in the header

// Magic bytes from 0x0 to 0x3
typedef enum class ARCHIVE_FILE_HEADER_MAGIC_0 : uint32_t {
    BDT = 0x33464442,
    BHD5 = 0x35444842,
    BND3 = 0x33444E42,
    DCX = 0x00584344
} ARCHIVE_FILE_HEADER_MAGIC;
typedef ARCHIVE_FILE_HEADER_MAGIC_0 ARCHIVE_MAGIC_0;
typedef ARCHIVE_FILE_HEADER_MAGIC_0 ARCHIVE_MAGIC;

// Magic bytes from 0x4 to 0x7
typedef enum class ARCHIVE_FILE_HEADER_MAGIC_1 : uint32_t {
    BDT = 0x37443730,
    BHD5 = 0x000000FF,
    BND3 = BDT,
    DCX = 0x00000100
} ARCHIVE_MAGIC_1;

// Magic bytes from 0x8 to 0xB
typedef enum class ARCHIVE_FILE_HEADER_MAGIC_2 : uint32_t {
    BDT = 0x00003652,
    BHD5 = 0x00000001,
    BND3 = BDT,
    DCX = 0x18000000
} ARCHIVE_MAGIC_2;



// Ancestor structure type for all Dark Souls archive files
typedef struct ArchiveFileHeader {
    ARCHIVE_MAGIC_0 magic32_0; // First 4 identifier bytes (0x0-0x3)
    ARCHIVE_MAGIC_1 magic32_1; // Identifier bytes 0x4-0x7
    ARCHIVE_MAGIC_2 magic32_2; // Identifier bytes 0x8-0xB
} ArchiveHeader;

#endif // _DS1_FILE_LIB_ARCHIVE_FILE_HEADER_H_