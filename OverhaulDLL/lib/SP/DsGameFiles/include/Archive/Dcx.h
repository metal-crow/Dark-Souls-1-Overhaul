/*
    DARK SOULS GAME FILE C++ LIBRARY

    Contributors to this file:
        Sean Pesce    -  C++
        Burton Radons -  Reverse-engineering Dark Souls game files. GitHub: https://github.com/Burton-Radons
        zlib          -  Data compression/decompression library; used by the code in this file to extract data from .dcx archives. Homepage: https://www.zlib.net


    Archive/Dcx.h

    Contains data about DCX (.dcx) archive files, as well as functions and
    structures that facilitate working with this filetype.

    References:
        https://github.com/Burton-Radons/Alexandria
        https://www.zlib.net/zlib_how.html
        https://gist.github.com/arq5x/5315739

*/

#pragma once

#ifndef _DS1_FILE_LIB_DCX_ARCHIVE_FILE_H_
    #define _DS1_FILE_LIB_DCX_ARCHIVE_FILE_H_


#include "Archive/Header.h"
#include "zlib.h"

/*
    ///////////////////////////////////////////////////////////////////////////////
    ////////////////////////////// DCX FILE OVERVIEW //////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////

    Each DCX (.dcx) archive file contains another file as compressed data. The data may
    be compressed using either of the following algorithms:

      - DEFLATE (More info here: https://en.wikipedia.org/wiki/DEFLATE )
      - EDGE    (?)

    DCX files consist of the following data sections:

      - Header:          The header data resides in the first 76 (0x4C) bytes of the
                         DCX file, and contains information about both the DCX file
                         itself and the compressed file it contains.

      - Compressed Data: Data of the stored file, compressed with either the DEFLATE
                         algorithm or the EDGE algorithm. The compressed data varies
                         in size and occupies the entirety of the DCX file after the
                         Header.

*/


// Compression methods used in .dcx archive files
enum DCX_COMPRESSION_ALGORITHM : uint32_t {
    ERROR_NO_DCX_COMPRESSION_DATA = 0x00000000,   // Only used in scenarios where an error occurs
    DEFLATE = 0x544C4644, // "DFLT" (ASCII, no null-terminator)
    DFLT = DEFLATE,
    EDGE = 0x45474445     // "EDGE" (ASCII, no null-terminator)
};


/*
    Header structure of a DCX archive file (76 bytes, or 0x4C)

    The header contains information about both the DCX file itself and the compressed file it
    contains, as well as some constant values. The following is a breakdown of the header data:

        - [0x00-0x03] Constant 4-byte string ("DCX\0")
        - [0x04-0x13] Four constant 32-bit integers (0x00000100, 0x18000000, 0x24000000, and 0x24000000; Little Endian)
        - [0x14-0x17] Size (in bytes) of the DCX file subheader (32-bit Big Endian integer)
        - [0x18-0x1B] Constant 4-byte string ("DCS\0")
        - [0x1C-0x1F] Original (decompressed) size (in bytes) of the contained file (32-bit Big Endian integer)
        - [0x20-0x23] Size (in bytes) of the compressed file data (32-bit Big Endian integer)
        - [0x24-0x27] Constant 4-byte string ("DCP\0")
        - [0x28-0x2B] Compression algorithm as a 4-byte string ("DFLT" for DEFLATE algorithm, or "EDGE"; Note: These strings are not null-terminated)
        - [0x2C-0x33] Two constant 32-bit integers (0x20000000 and 0x00000009; Little Endian)
        - [0x34-0x37] 32-bit integer that differs depending on the version (0x00000000 for Dark Souls 1, and 0x00000100 for Dark Souls 2; Little Endian)
        - [0x38-0x3F] 8 bytes of 0x00
        - [0x40-0x43] 32-bit integer that differs depending on the version (0x00010100 for Dark Souls 1, and 0x00000100 for Dark Souls 2; Little Endian)
        - [0x44-0x47] Constant 4-byte string ("DCA\0")
        - [0x48-0x4B] Size of the compression header (32-bit Big Endian integer)


*/
typedef struct DcxArchiveFileHeader : ArchiveFileHeader {
/*  Magic identifier bytes (defined in ArchiveHeader ancestor struct):
    ARCHIVE_MAGIC_0 magic32_0;  // "DCX\0"
    ARCHIVE_MAGIC_1 magic32_1;  // 0x00000100
    ARCHIVE_MAGIC_2 magic32_2;  // 0x18000000
*/  const uint32_t magic32_3 = 0x24000000;
    const uint32_t magic32_4 = 0x24000000;
    uint32_t subheader_size_big_endian = 0; // Size (in bytes) of the DCX file subheader (WARNING: Big Endian)
    const uint32_t DCS = 0x00534344; // "DCS\0"
    uint32_t size_decompressed_big_endian = 0; // WARNING: Big Endian
    uint32_t size_compressed_big_endian = 0;   // WARNING: Big Endian
    const uint32_t DCP = 0x00504344; // "DCP\0"
    DCX_COMPRESSION_ALGORITHM compression = DEFLATE; // Compression method (WARNING: Not null-terminated); Options are "DFLT" (DEFLATE algorithm) or "EDGE"
    const uint32_t magic32_5 = 0x20000000;
    const uint32_t magic32_6 = 0x00000009;
    const uint32_t is_dark_souls_2_low = 0x0; // In Dark Souls 2 .dcx files, this would be 0x100
    const uint32_t padding[2] = { 0x0, 0x0 };
    const uint32_t is_dark_souls_2_high = 0x10100; // In Dark Souls 2 .dcx files, this would be 0x11000
    const uint32_t DCA = 0x00414344; // "DCA\0"
    uint32_t compression_header_size_big_endian = 0; // WARNING: Big Endian
} DcxHeader;




/*
    Class containing data and functions for reading and/or manipulating DCX archive files (.dcx)
*/
typedef class DcxArchiveFile {
private:
    // File header
    DcxHeader *_header;
    const bool _copy;
public:
    // DCX file extension
    static const char    *FILE_EXT;   //  ".dcx" (char)
    static const wchar_t *FILE_EXT_W; // L".dcx" (wide char)

    // Pointer to allocated memory where decompressed data is stored
    void *saved_extract_data = NULL;

    // Get-methods that automatically convert Big Endian values to Little Endian:

    void *file_start() { if (_header == NULL) { SetLastError(ERROR_CANNOT_MAKE); return NULL; } return (void*)_header; }
    void *data_start() { if (_header == NULL) { SetLastError(ERROR_CANNOT_MAKE); return NULL; } return (void*)(((uint8_t*)_header) + sizeof(DcxHeader)); }
    uint32_t subheader_size() { if (_header == NULL) { SetLastError(ERROR_CANNOT_MAKE); return 0; } return FileUtil::reverse_endian(_header->subheader_size_big_endian); }
    uint32_t size_decompressed() { if (_header == NULL) { SetLastError(ERROR_CANNOT_MAKE); return 0; } return FileUtil::reverse_endian(_header->size_decompressed_big_endian); }
    uint32_t size_compressed() { if (_header == NULL) { SetLastError(ERROR_CANNOT_MAKE); return 0; } return FileUtil::reverse_endian(_header->size_compressed_big_endian); } // Size of the compressed data ONLY
    uint32_t file_size() { if (_header == NULL) { SetLastError(ERROR_CANNOT_MAKE); return 0; } return size_compressed() + sizeof(DcxHeader); } // Size of the full .dcx archive file (compressed data + DCX header)
    uint32_t compression_header_size() { if (_header == NULL) { SetLastError(ERROR_CANNOT_MAKE); return 0; } return FileUtil::reverse_endian(_header->compression_header_size_big_endian); }
    DCX_COMPRESSION_ALGORITHM compression_algorithm() { if (_header == NULL) { SetLastError(ERROR_CANNOT_MAKE); return ERROR_NO_DCX_COMPRESSION_DATA; } return _header->compression; }
    int compression_algorithm_string(char *buffer) { if (buffer == NULL) { return ERROR_INVALID_PARAMETER; } if (_header == NULL) { return ERROR_CANNOT_MAKE; } char *alg_str = (char*)&_header->compression; for (int i = 0; i < 4; i++) { buffer[i] = alg_str[i]; } buffer[4] = '\0'; return ERROR_SUCCESS; }

    // If data was extracted, free the memory holding the data
    void free_extract_data()
    {
        if (saved_extract_data != NULL)
        {
            CoTaskMemFree(saved_extract_data);
            saved_extract_data = NULL;
        }
    }


    /*
        Dump the .dcx archive from memory to a file.

        @return 0 on success, otherwise returns relevant error code
    */
    int to_file(const char *new_file, bool overwrite = true)
    {
        if (_header == NULL)
            return ERROR_CANNOT_MAKE;

        if (new_file == NULL)
            return ERROR_INVALID_ADDRESS;

        // Write file
        return FileUtil::dump_file(_header, file_size(), new_file, overwrite);
    }


    /*
        Dump the decompressed archive data to a file.

        @return 0 on success, otherwise returns relevant error code
    */
    int extract_and_dump(const char *new_file, bool free_memory = true, bool overwrite = true)
    {
        if (_header == NULL)
            return ERROR_CANNOT_MAKE;

        if (new_file == NULL)
            return ERROR_INVALID_ADDRESS;

        // Check if data has been extracted yet
        if (saved_extract_data == NULL)
        {
            // Attempt to extract the data
            if (extract() == NULL)
                return GetLastError(); // Extraction failed
        }

        // Write file
        return FileUtil::dump_file(saved_extract_data, size_decompressed(), new_file, overwrite);
    }


    // Decompress .dcx archive data and store in memory
    void *extract()
    {
        if (_header == NULL)
        {
            SetLastError(ERROR_CANNOT_MAKE);
            return NULL;
        }

        // Free previously-extracted data
        free_extract_data();

        // Allocate memory
        saved_extract_data = CoTaskMemAlloc(size_decompressed());
        if (saved_extract_data == NULL)
        {
            // Unable to allocate memory
            SetLastError(ERROR_OUTOFMEMORY);
            return NULL;
        }

        int err;
        int end_err;

        switch (_header->compression)
        {
            case DEFLATE: // DFLT

                // Initialize data stream
                z_stream inflate_stream;
                inflate_stream.zalloc = Z_NULL;
                inflate_stream.zfree = Z_NULL;
                inflate_stream.opaque = Z_NULL;

                inflate_stream.avail_in = size_compressed();      // Input size (compressed)
                inflate_stream.next_in = (uint8_t*)data_start();  // Starting address of compressed data
                inflate_stream.avail_out = size_decompressed();   // Output size (decompressed)
                inflate_stream.next_out = (uint8_t*)saved_extract_data; // Starting address of buffer for decompressed data

                // Extract the data
                inflateInit(&inflate_stream);
                err = inflate(&inflate_stream, Z_NO_FLUSH);
                end_err = inflateEnd(&inflate_stream);

                // Check for errors
                if (err == Z_STREAM_ERROR)
                {
                    free_extract_data();
                    SetLastError(ERROR_DATA_CHECKSUM_ERROR);
                    return NULL;
                }
                else if (err == Z_MEM_ERROR)
                {
                    free_extract_data();
                    SetLastError(ERROR_INVALID_BLOCK);
                    return NULL;
                }
                else if (err == Z_DATA_ERROR)
                {
                    free_extract_data();
                    SetLastError(ERROR_INVALID_DATA);
                    return NULL;
                }
                else
                {
                    switch (err)
                    {
                        case Z_NEED_DICT:
                            SetLastError(ERROR_OUT_OF_STRUCTURES);
                            break;
                        case Z_BUF_ERROR:
                            SetLastError(ERROR_INSUFFICIENT_BUFFER);
                            break;
                        default:
                            if (end_err == Z_STREAM_ERROR)
                                SetLastError(ERROR_DATA_CHECKSUM_ERROR);
                            else
                                SetLastError(ERROR_SUCCESS);
                            break;
                    }

                    return saved_extract_data;
                }
                break; // case DEFLATE

            default:
            case EDGE:
                // @TODO
                return NULL;
                break;
        }
    }

    // Constructors
    DcxArchiveFile(void *new_file_start, bool create_as_copy = false)
        : _header((DcxHeader*)new_file_start), _copy(create_as_copy)
    {
        uint32_t size = FileUtil::reverse_endian(((DcxHeader*)new_file_start)->size_compressed_big_endian) + sizeof(DcxHeader);
        _header = (DcxHeader*)CoTaskMemAlloc(size);

        if (_header != NULL)
        {
            memcpy_s(_header, size, new_file_start, size);
        }

        if (new_file_start == NULL || _header == NULL)
            SetLastError(ERROR_CANNOT_MAKE);
    }



    // Destructor
    ~DcxArchiveFile()
    {
        // Free allocated memory blocks
        free_extract_data();

        if (_copy && _header != NULL)
        {
            CoTaskMemFree(saved_extract_data);
            _header = NULL;
        }
    }
} DcxArchive;


#endif // _DS1_FILE_LIB_DCX_ARCHIVE_FILE_H_
