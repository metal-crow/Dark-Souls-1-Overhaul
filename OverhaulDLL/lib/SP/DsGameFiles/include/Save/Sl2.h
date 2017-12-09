/*
    DARK SOULS GAME FILE C++ LIBRARY

    Contributors to this file:
        Sean Pesce    -  C++


    Save/Sl2.h

    Contains data about SL2 (.sl2) savegame files, as well as functions and
    structures that facilitate working with this filetype.


    References:
        https://github.com/tarvitz/dsfp
        https://github.com/tarvitz/dsfp/blob/master/docs/datasheet.rst


*/

#pragma once

#ifndef _DS1_FILE_LIB_SL2_SAVE_FILE_H_
#define _DS1_FILE_LIB_SL2_SAVE_FILE_H_

#include "PluginImports.h"

/*
    ///////////////////////////////////////////////////////////////////////////////
    ////////////////////////////// SL2 FILE OVERVIEW //////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////

        @TODO
    
*/



/*
    Header structure for a SL2 gamesave file (?? bytes, or 0x??)

    @TODO
*/
typedef struct Sl2Header {
    // @TODO
} Sl2Header;


/*
    Class containing data and functions for reading and/or manipulating SL2 archive files (.sl2)
*/
typedef class Sl2SaveFile {
private:

public:

    // SL2 file extension
    static const char    *FILE_EXT;   //  ".sl2" (char)
    static const wchar_t *FILE_EXT_W; // L".sl2" (wide char)

    // Default filename for Dark Souls SL2 files
    static const char    *FILE_NAME_DEFAULT;   //  "DRAKS0005" (char)
    static const wchar_t *FILE_NAME_DEFAULT_W; // L"DRAKS0005" (wide char)

} SaveFile;
typedef Sl2SaveFile Sl2;







#endif // _DS1_FILE_LIB_SL2_SAVE_FILE_H_
