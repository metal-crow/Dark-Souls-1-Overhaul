/*
    DARK SOULS GAME FILE C++ LIBRARY

    Contributors to this file:
        Sean Pesce    -  C++
        Burton Radons -  Reverse-engineering Dark Souls game files. GitHub: https://github.com/Burton-Radons


    Archive/Bdt.h

    Contains data about BDT (.bdt) archive files, as well as functions and
    structures that facilitate working with this filetype.


    References:
        https://github.com/Burton-Radons/Alexandria


*/

#pragma once

#ifndef _DS1_FILE_LIB_BDT_ARCHIVE_FILE_H_
    #define _DS1_FILE_LIB_BDT_ARCHIVE_FILE_H_



#include "Archive/Header.h"


/*
    ///////////////////////////////////////////////////////////////////////////////
    ////////////////////////////// BDT FILE OVERVIEW //////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////

    BDT (.bdt) files serve as an archive for storing game files. For each BDT file,
    there is a BHD5 (.bhd5) file with the same filename (other than the file
    extension). The corresponding BHD5 file acts as an index for the BDT file; that
    is, the BHD5 file contains data that can be used to locate individual files that
    are packed into the BDT file.

      - Header:       The header data resides in the first 16 (0x10) bytes of the BDT
                      file, and contains constant data that is equivalent across all
                      BDT files.

      - Packed files: A contiguous array of the file data for each file that is
                      packed into the BDT archive. This file data varies in size
                      and occupies the entirety of the BDT file after the Header.
                      To access individual packed files, the corresponding BHD5
                      file must be parsed for data regarding the location of each
                      packed file.
*/



/*
    Header structure for a BDT archive file (16 bytes, or 0x10)

    The header data doesn't seem to have any use other than to identify the file
    as a BDT file. All bytes in the header are constant; that is, they are the
    same for all BDT files. The following is a breakdown of the header data:

        - [0x00-0x0F] Constant 16-byte string ("BDF307D7R6\0\0\0\0\0\0")
*/
typedef struct BdtArchiveHeader : ArchiveFileHeader {
    /*  Magic identifier bytes (defined in ArchiveHeader ancestor struct):
        ARCHIVE_MAGIC_0 magic32_0; // 0x33464442     = "BDF3"
        ARCHIVE_MAGIC_1 magic32_1; // 0x37443730     = "07D7"
        ARCHIVE_MAGIC_2 magic32_2; // 0x00003652     = "R6\0\0"
    */  const uint32_t  magic32_3 = 0x00000000; // = "\0\0\0\0"
} BdtHeader;


/*
    Class containing data and functions for reading and/or manipulating BDT archive files (.bdt)
*/
typedef class BdtArchiveFile {
private:

public:

    // BDT file extension
    static const char    *FILE_EXT;   //  ".bdt" (char)
    static const wchar_t *FILE_EXT_W; // L".bdt" (wide char)

    // Default filename for Dark Souls BDT files
    static const char *FILE_NAME_DEFAULT;  // "dvdbnd"

    // Default number of Dark Souls BDT files
    static const unsigned int FILE_COUNT_DEFAULT = 4;
} BdtArchive;







#endif // _DS1_FILE_LIB_BDT_ARCHIVE_FILE_H_
