/*
    DARK SOULS GAME FILE C++ LIBRARY

    Contributors to this file:
        Sean Pesce    -  C++
        Burton Radons -  Reverse-engineering Dark Souls game files. GitHub: https://github.com/Burton-Radons


    Archive/Bhd5.h

    Contains data about BHD5 (.bhd5) files, as well as functions and structures
    that facilitate working with this filetype.


    References:
        https://github.com/Burton-Radons/Alexandria


*/

#pragma once

#ifndef _DS1_FILE_LIB_BHD5_ARCHIVE_FILE_H_
    #define _DS1_FILE_LIB_BHD5_ARCHIVE_FILE_H_



#include "Archive/Header.h"


/*
    ///////////////////////////////////////////////////////////////////////////////
    ////////////////////////////// BHD5 FILE OVERVIEW /////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////

    BHD5 (.bhd5) files serve as an index for BDT (.bdt) files. For each BHD5
    file, there is a BDT (.bdt) file with the same filename (other than the file
    extension). Each BHD5 file contains information about the location of files
    stored inside the corresponding BDT archive file. BHD5 files contain the
    following data sections:

      - Header:  The header data resides in the first 24 (0x18) bytes of the BHD5
                 file, and contains information about the file.

      - Bins:    A contiguous array of Bin structures, each of which holds data
                 about a set of Record structures. The Bin array starts at the
                 same offset (0x18) in every BHD5 file. Each Bin structure is
                 8 bytes.

      - Records: A contiguous array of Record structures, each of which holds
                 data about a file stored inside the BDT archive that
                 corresponds to the BHD5 file containing the Record. Each
                 Record structure is 16 (0x10) bytes. The Record array starts
                 after the end of the Bin array, so the starting offset of the
                 Record array varies in every BHD5 file, because the number
                 of Bins in each file is different. For this reason, Records
                 are referenced through data in the corresponding Bin
                 structure, rather than by index in the array.
*/



/*
    Header structure of a BHD5 index file (24 bytes, or 0x18)

    The header contains information about the BHD5 file itself, as well as
    data about the Bin structures it contains. The following is a breakdown
    of the header data:

        - [0x00-0x03] Constant 4-byte string ("BHD5"; Note: This string is not null-terminated)
        - [0x04-0x0B] Two constant 32-bit integers (0x000000FF and 0x00000001; Little Endian)
        - [0x0C-0x0F] Size (in bytes) of the BHD5 file (32-bit Little Endian integer)
        - [0x10-0x13] Total number of Bin structures in the Bin array (32-bit Little Endian integer)
        - [0x14-0x17] Offset (in bytes) of the start of the Bin structure array, relative to the beginning of the file (32-bit Little Endian integer). Note: This offset seems to be constant

*/
typedef struct Bhd5ArchiveHeader : ArchiveFileHeader {
/*  Magic identifier bytes (defined in ArchiveHeader ancestor struct):
    ARCHIVE_MAGIC_0 magic32_0;  // "BHD5"
    ARCHIVE_MAGIC_1 magic32_1;  // 0x000000FF
    ARCHIVE_MAGIC_2 magic32_2;  // 0x00000001
*/  uint32_t file_size = 0; // Total size of the BHD5 file (in bytes)
    uint32_t bin_count = 0;
    const uint32_t bin_offset = 0x00000018; // Constant offset of the first bin struct (relative to beginning of file)
} Bhd5Header;



/*
    Bin data structure from a .bhd5 archive header file (8 bytes)

    Each Bin structure contain data about the location (offset in file) of an
    array of Record structures in the BHD5 file (see below).

    @member   record_count  Number of Records in this Bin's Record array
    @member   record_offset Starting offset of the Bin's Record array (relative
              to the beginning of the file)
*/
typedef struct Bhd5ArchiveBin {
    uint32_t  record_count = 0;
    uint32_t record_offset = 0; // Offset of bin's record array (relative to beginning of file)
} Bhd5Bin;


/*
    Record data structure from a BHD5 archive header file (16 bytes)

    Each Record structure contains information about a file that has been packed into the
    BDT archive corresponding to the BHD5 file containing the Record.

    @member   hash    32-bit hash of the original filename of the file this Record contains data for.
                      See description of Bhd5ArchiveFile::filename_to_hash(const char*) for full
                      explanation of hashing algorithm.
    @member   size    Size (in bytes) of the packed file data inside in the .bdt archive.
    @member   offset  Location of the packed file data, as an offset relative to the beginning of the
                      BDT archive.
*/
typedef struct Bhd5ArchiveRecord {
    uint32_t   hash = 0; // Hash code produced from the name of the file that corresponds to this record
    uint32_t   size = 0; // Size (in bytes) of record data
    uint32_t offset = 0; // Offset to the record data in the .bdt content file
    const uint32_t padding = 0x00000000;
} Bhd5Record;


// Index of a Bhd5 record
typedef struct Bhd5ArchiveRecordIndex {
    uint32_t bin_index;
    uint32_t record_index;
} Bhd5RecordIndex;



/*
    Class containing data and functions for reading and/or manipulating BHD5 index files (.bhd5)
*/
typedef class Bhd5ArchiveFile {
private:
    bool  _manual_alloc; // Signifies whether the memory for this object was dynamically allocated
    Bhd5Header *_header; // File header
    Bhd5Bin    *_bins;   // Array of bin structs
    Bhd5Record *_records_base; // Base address of the contiguous array of record structs (WARNING: Records should not be referenced this way)

public:

    // BHD5 file extension
    static const char    *FILE_EXT;   //  ".bhd5" (char)
    static const wchar_t *FILE_EXT_W; // L".bhd5" (wide char)

    // Default filename for Dark Souls BHD5 files
    static const char *FILE_NAME_DEFAULT;  // "dvdbnd"

    // Default number of Dark Souls BHD5 files
    static const unsigned int FILE_COUNT_DEFAULT = 4;

    // Constructor
    Bhd5ArchiveFile(void *new_header_start = NULL, void *new_body_start = NULL, bool is_dynamically_allocated = false)
        : _header((Bhd5Header*)new_header_start), _manual_alloc(is_dynamically_allocated)
    {
        if (new_header_start == NULL)
        {
            _header = NULL;
            _bins = NULL;
            _records_base = NULL;
            SetLastError(ERROR_CANNOT_MAKE);
        }
        else
        {
            // Get starting address of bin array
            if (new_body_start == NULL)
                _bins = (Bhd5Bin*)(((uint8_t*)_header) + _header->bin_offset);
            else
                _bins = (Bhd5Bin*)new_body_start;

            // Get starting address of record array
            _records_base = (Bhd5Record*)(((uint8_t*)_bins) + (_header->bin_count * sizeof(Bhd5Bin)));
        }
    }

    // Destructor
    ~Bhd5ArchiveFile()
    {
        if (_manual_alloc && _header != NULL)
        {
            CoTaskMemFree((void*)_header);
            _header = NULL;
        }
    }


    // Returns true if file data was manually allocated (false otherwise)
    bool file_data_is_manual_alloc() { if (_header == NULL) { SetLastError(ERROR_CANNOT_MAKE); } return _manual_alloc; };

    // Returns pointer to the header structure of the file (address is equivalent to the start of the file)
    Bhd5Header *header() { if (_header == NULL) { SetLastError(ERROR_CANNOT_MAKE); } return _header; }

    // Returns pointer to the first byte of the file (address is equivalent to the address of the first byte of the header structure)
    void *file_base() { if (_header == NULL) { SetLastError(ERROR_CANNOT_MAKE); } return (void*)_header; }

    // Returns total size of the BHD5 file (in bytes)
    uint32_t file_size() { if (_header == NULL) { SetLastError(ERROR_CANNOT_MAKE); return 0; } return _header->file_size; }

    // Returns number of bin structures in the file
    uint32_t bin_count() { if (_header == NULL) { SetLastError(ERROR_CANNOT_MAKE); return 0; } return _header->bin_count; }

    // Returns the start of the array of bin structures
    Bhd5Bin *bins() { if (_header == NULL) { SetLastError(ERROR_CANNOT_MAKE); return NULL; } return _bins; }

    // Returns the start of the array of record structures (WARNING: Records are not meant to be referenced through this array)
    Bhd5Record *records() { if (_header == NULL) { SetLastError(ERROR_CANNOT_MAKE); return NULL; } return _records_base; }

    // Returns number of record structures across all bin structs in the file
    uint32_t record_count()
    {
        if (_header == NULL) { SetLastError(ERROR_CANNOT_MAKE); return 0; }

        uint32_t count = 0;

        for (int i = 0; i < (int)_header->bin_count; i++)
            count += get_bin_by_index(i)->record_count;

        return count;
    }


    // Returns pointer to the bin struct at the given index in the bins array
    Bhd5Bin *get_bin_by_index(uint32_t index)
    {
        if (_header == NULL) { SetLastError(ERROR_CANNOT_MAKE); return NULL; }

        if (index >= _header->bin_count)
        {
            SetLastError(ERROR_RANGE_NOT_FOUND);
            return NULL;
        }

        return (Bhd5Bin*)(&_bins[index]);
    }


    // Returns pointer to the bin struct containing the record struct that contains the given file name hash
    Bhd5Bin *get_bin_by_hash(uint32_t hash)
    {
        if (_header == NULL) { SetLastError(ERROR_CANNOT_MAKE); return NULL; }

        for (int i = 0; i < (int)_header->bin_count; i++)
            for (int j = 0; j < (int)get_bin_by_index(i)->record_count; j++)
                if (hash == get_record_by_index(i, j)->hash)
                    return get_bin_by_index(i);
        // Failed to find a record with the given hash
        SetLastError(ERROR_SECTOR_NOT_FOUND);
        return NULL;
    }


    // Returns pointer to the bin struct containing the record struct that contains the hash for the specified file name
    Bhd5Bin *get_bin_by_filename(const char *filename)
    {
        if (_header == NULL) { SetLastError(ERROR_CANNOT_MAKE); return NULL; }

        if (filename == NULL)
        {
            SetLastError(ERROR_INVALID_ADDRESS);
            return NULL;
        }

        uint32_t hash = Bhd5ArchiveFile::filename_to_hash(filename);

        return get_bin_by_hash(hash);
    }


    // Returns index of bin struct containing the record struct that contains the given file name hash, or -1 if bin was not found
    int32_t get_index_of_bin_by_hash(uint32_t hash)
    {
        if (_header == NULL) { SetLastError(ERROR_CANNOT_MAKE); return NULL; }
        SetLastError(ERROR_SUCCESS);

        for (int i = 0; i < (int)_header->bin_count; i++)
            for (int j = 0; j < (int)get_bin_by_index(i)->record_count; j++)
                if (hash == get_record_by_index(i, j)->hash)
                    return i;
        // Failed to find a record with the given hash
        SetLastError(ERROR_SECTOR_NOT_FOUND);
        return -1;
    }


    // Returns index of bin struct containing the record struct that contains the hash of the given file name, or -1 if bin was not found
    int32_t get_index_of_bin_by_filename(const char *filename)
    {
        if (_header == NULL) { SetLastError(ERROR_CANNOT_MAKE); return NULL; }
        SetLastError(ERROR_SUCCESS);

        if (filename == NULL)
        {
            SetLastError(ERROR_INVALID_ADDRESS);
            return NULL;
        }

        uint32_t hash = Bhd5ArchiveFile::filename_to_hash(filename);

        return get_index_of_bin_by_hash(hash);
    }


    // Returns pointer to the record struct at the given index in the records array of the bin struct at the given index of the bins array
    Bhd5Record *get_record_by_index(uint32_t bin_index, unsigned int record_index)
    {
        if (_header == NULL) { SetLastError(ERROR_CANNOT_MAKE); return NULL; }

        if (bin_index >= _header->bin_count
            || record_index >= get_bin_by_index(bin_index)->record_count)
        {
            SetLastError(ERROR_RANGE_NOT_FOUND);
            return NULL;
        }

        return &(((Bhd5Record*)(((uint8_t*)_bins) + (get_bin_by_index(bin_index)->record_offset - sizeof(Bhd5Header))))[record_index]);
    }


    // Returns pointer to the record struct that contains the given file name hash
    Bhd5Record *get_record_by_hash(uint32_t hash)
    {
        if (_header == NULL) { SetLastError(ERROR_CANNOT_MAKE); return NULL; }

        for (int b = 0; b < (int)_header->bin_count; b++)
        {
            for (int r = 0; r < (int)get_bin_by_index(b)->record_count; r++)
            {
                if (hash == get_record_by_index(b, r)->hash)
                    return get_record_by_index(b, r);
            }
        }
        // Failed to find a record with the given hash
        SetLastError(ERROR_SECTOR_NOT_FOUND);
        return NULL;
    }


    // Returns pointer to the record struct that contains the hash of the specified file name
    Bhd5Record *get_record_by_filename(const char *filename)
    {
        if (_header == NULL) { SetLastError(ERROR_CANNOT_MAKE); return NULL; }

        if (filename == NULL)
        {
            SetLastError(ERROR_INVALID_ADDRESS);
            return NULL;
        }
        uint32_t hash = Bhd5ArchiveFile::filename_to_hash(filename);
        return get_record_by_hash(hash);
    }


    // Returns index (in a bin's record array) of the record struct that contains the given file name hash; otherwise returns -1 on error.
    // If bin_index is not NULL, the bin index is stored at the address of bin_index on success; otherwise 0 is stored on error.
    int get_index_of_record_by_hash(uint32_t hash, uint32_t *bin_index = NULL)
    {
        if (_header == NULL) { SetLastError(ERROR_CANNOT_MAKE); if (bin_index != NULL) { (*bin_index) = 0; } return -1; }
        SetLastError(ERROR_SUCCESS);

        for (int b = 0; b < (int)_header->bin_count; b++)
        {
            for (int r = 0; r < (int)get_bin_by_index(b)->record_count; r++)
            {
                if (hash == get_record_by_index(b, r)->hash)
                {
                    if (bin_index != NULL)
                        (*bin_index) = b;
                    return r;
                }
            }
        }
        // Failed to find a record with the given hash
        SetLastError(ERROR_SECTOR_NOT_FOUND);
        if (bin_index != NULL)
            (*bin_index) = 0;
        return -1;
    }


    // Returns index (in a bin's record array) of the record struct that contains the hash of the given file name; otherwise returns -1 on error.
    // If bin_index is not NULL, the bin index is stored at the address of bin_index on success; otherwise 0 is stored on error.
    int get_index_of_record_by_filename(const char *filename, uint32_t *bin_index = NULL)
    {
        if (_header == NULL) { SetLastError(ERROR_CANNOT_MAKE); if (bin_index != NULL) { (*bin_index) = 0; } return -1; }
        SetLastError(ERROR_SUCCESS);

        if (filename == NULL)
        {
            SetLastError(ERROR_INVALID_ADDRESS);
            if (bin_index != NULL)
                (*bin_index) = 0;
            return -1;
        }
        uint32_t hash = Bhd5ArchiveFile::filename_to_hash(filename);
        return get_index_of_record_by_hash(hash, bin_index);
    }


    // Returns offset of the file data (in the .bdt file) corresponding to the record struct at the given
    //   index of the record array corresponding to the bin struct at the given index of the bin array
    inline uint32_t get_offset_in_bdt_by_index(uint32_t bin_index, unsigned int record_index)
    {
        if (_header == NULL) { SetLastError(ERROR_CANNOT_MAKE); return NULL; }
        return get_record_by_index(bin_index, record_index)->offset;
    }


    // Returns offset of the file data (in the .bdt file) corresponding to the record struct that contains the given file name hash
    inline uint32_t get_offset_in_bdt_by_hash(uint32_t hash)
    {
        if (_header == NULL) { SetLastError(ERROR_CANNOT_MAKE); return NULL; }
        return get_record_by_hash(hash)->offset;
    }


    // Returns offset of the file data (in the .bdt file) corresponding to the record struct that contains the hash of the specified file name
    inline uint32_t get_offset_in_bdt_by_filename(const char *filename)
    {
        return get_record_by_filename(filename)->offset;
    }


    /*
        Writes the BHD5 file data from memory to a new file on disk.

        @return ERROR_SUCCESS if successful, otherwise returns relevant error code
    */
    int to_file(const char *file, bool overwrite = true)
    {
        if (_header == NULL) { SetLastError(ERROR_CANNOT_MAKE); return ERROR_CANNOT_MAKE; }

        return FileUtil::dump_file(_header, _header->file_size, file, overwrite);
    }


    /*
        "Extract Packed File from BDT File into Memory Using BHD5 Object as Index"

        Extracts a packed file from a BDT (.bdt) archive and stores it in memory.

        If extract_buffer is not NULL, it is assumed that extract_buffer is large
        enough to hold the full file. If extract_buffer is NULL, memory is
        dynamically allocated for the data, and must be freed manually using
        CoTaskMemFree.

        @return Pointer to the start of the extracted file in memory if successful,
                or NULL if an error occurred.
    */
    void *extract_from_bdt_by_index(const char *bdt_file, void *extract_buffer, uint32_t bin_index, unsigned int record_index)
    {
        // Get pointer to record
        Bhd5Record *record;
        if ((record = get_record_by_index(bin_index, record_index)) == NULL)
            return NULL; // Error occurred
        // Load packed file into memory
        return FileUtil::read_from_offset(bdt_file, record->offset, record->size, extract_buffer, true);
    }

    // Overload of "Extract Packed File from BDT File into Memory Using BHD5 Object as Index"
    void *extract_from_bdt_by_hash(const char *bdt_file, void *extract_buffer, uint32_t hash)
    {
        // Get pointer to record
        Bhd5Record *record;
        if ((record = get_record_by_hash(hash)) == NULL)
            return NULL; // Error occurred
        // Load packed file into memory
        return FileUtil::read_from_offset(bdt_file, record->offset, record->size, extract_buffer, true);
    }


    // Overload of "Extract Packed File from BDT File into Memory Using BHD5 Object as Index"
    void *extract_from_bdt_by_filename(const char *bdt_file, void *extract_buffer, const char *filename)
    {
        // Get pointer to record
        Bhd5Record *record;
        if ((record = get_record_by_filename(filename)) == NULL)
            return NULL; // Error occurred
        // Load packed file into memory
        return FileUtil::read_from_offset(bdt_file, record->offset, record->size, extract_buffer, true);
    }


    /*
        "Extract Packed File from BDT File to Disk Using BHD5 Object as Index"

        Extracts a packed file from a BDT (.bdt) archive and stores it on disk.

        To obtain the offset of the packed file in the BDT archive, the specified
        BHD5 file is first parsed for indexing data.

        @return ERROR_SUCCESS on success; otherwise returns a relevant error code.
    */
    int extract_from_bdt_by_index(const char *bdt_file, uint32_t bin_index, unsigned int record_index, const char *write_file, bool overwrite = true)
    {
        // Get the record structure
        Bhd5Record *record;
        if ((record = get_record_by_index(bin_index, record_index)) == NULL)
            return GetLastError();
        // Load packed file into memory
        void *extracted_file_in_mem;
        if ((extracted_file_in_mem = FileUtil::read_from_offset(bdt_file, record->offset, record->size, NULL, true)) == NULL)
            return GetLastError();

        int return_val = FileUtil::dump_file(extracted_file_in_mem, record->size, write_file, overwrite);

        // Free the memory that was allocated for the file
        CoTaskMemFree(extracted_file_in_mem);

        return return_val;
    }

    // Overload of "Extract Packed File from BDT File to Disk Using BHD5 Object as Index"
    int extract_from_bdt_by_hash(const char *bdt_file, uint32_t hash, const char *write_file, bool overwrite = true)
    {
        // Get the record structure
        Bhd5Record *record;
        if ((record = get_record_by_hash(hash)) == NULL)
            return GetLastError();
        // Load packed file into memory
        void *extracted_file_in_mem;
        if ((extracted_file_in_mem = FileUtil::read_from_offset(bdt_file, record->offset, record->size, NULL, true)) == NULL)
            return GetLastError();

        int return_val = FileUtil::dump_file(extracted_file_in_mem, record->size, write_file, overwrite);

        // Free the memory that was allocated for the file
        CoTaskMemFree(extracted_file_in_mem);

        return return_val;
    }


    // Overload of "Extract Packed File from BDT File to Disk Using BHD5 Object as Index"
    int extract_from_bdt_by_filename(const char *bdt_file, const char *packed_filename, const char *write_file = NULL, bool overwrite = true)
    {
        if (packed_filename == NULL)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return NULL;
        }
        uint32_t hash = filename_to_hash(packed_filename);
        // If output file name is NULL, use the original name of the packed file
        if (write_file == NULL)
            write_file = packed_filename;

        return extract_from_bdt_by_hash(bdt_file, hash, write_file, overwrite);
    }


    /*
        Calculates the 32-bit BHD5 Record hash value for the given filename.

        Algorithm source:
            https://sites.google.com/site/fileformats/engines/dark-souls/archive-files

        Hashing algorithm reversed by Burton Radons (GitHub: https://github.com/Burton-Radons )



        ////////////////////////////////// Algorithm Explained //////////////////////////////////////

        - Start with hash=0 and some UTF-8 (?) filename. Let's say the original
          filename is:

                "N:\FRPG\data\INTERROOT_win32\map\m10_00_00_00\m2080B0A10.flver.dcx"

        - Filename should include parent folders, but (for default game files) only
          the folders below "N:\FRPG\data\INTERROOT_win32". So now the file name is:

                "\map\m10_00_00_00\m2080B0A10.flver.dcx"

        - Convert all alphabetical characters to lowercase, and all backslashes
          to forward slashes. Now we have the filename that will be used in the
          hash algorithm. Original vs final filename:

                Original: "N:\FRPG\data\INTERROOT_win32\map\m10_00_00_00\m2080B0A10.flver.dcx"
                Final:    "/map/m10_00_00_00/m2080B0a10.flver.dcx"

        - For every character in the file name, add the number value of that character
          to the current hash. Example hash after parsing the first 3 characters:

                Start values:
                Hash = 0                            (NOTE: 32-bit hex value of hash = 0x00000000)
                "/map/m10_00_00_00/m2080B0a10.flver.dcx"

                First character:                           (NOTE: '/' = 47    and    0 + 47 = 47)
                Hash = 47                           (NOTE: 32-bit hex value of hash = 0x0000002F)
                "map/m10_00_00_00/m2080B0a10.flver.dcx"

                Second character:                       (NOTE: 'm' = 109   and    47 + 109 = 156)
                Hash = 156                          (NOTE: 32-bit hex value of hash = 0x0000009C)
                "ap/m10_00_00_00/m2080B0a10.flver.dcx"

                Third character:                         (NOTE: 'a' = 97   and    156 + 97 = 253)
                Hash = 253                          (NOTE: 32-bit hex value of hash = 0x000000FD)
                "p/m10_00_00_00/m2080B0a10.flver.dcx"

        ///////////////////////////// End of algorithm explanation /////////////////////////////////



        @return 32-bit hash of the given filename, or zero (with error set) if an
                error occurs. NOTE: Hash can be zero without encountering an error,
                in which case GetLastError will return ERROR_SUCCESS. Error code
                only needs to be checked if the function returns zero.
    */
    static uint32_t filename_to_hash(const char *filename)
    {
        if (filename == NULL)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return 0;
        }

        uint32_t hash = 0;

        // Convert filename to lowercase
        std::string filename_lower = filename;
        FileUtil::to_lowercase((char*)filename_lower.c_str());

        // Get first char
        char *c = (char*)filename_lower.c_str();

        // Apply hashing algorithm
        while ((*c) != '\0')
        {
            hash *= 37;
            hash += (uint8_t)(*c);
            c++;
        }

        SetLastError(ERROR_SUCCESS);
        return hash;
    }


    /*
        "Load BHD5 File into Memory"

        Copies all data from the specified BHD5 file into the specified
        buffer in memory. If file_buffer is not NULL, it is assumed that
        file_buffer is large enough to hold all data from the file.

        If  file_buffer is NULL, memory is dynamically allocated for the
        file data, and must be freed manually using CoTaskMemFree (unless
        this function was called by file_to_obj).

        @return Starting address of file_buffer on success, or NULL if
                an error occurred.
    */
    static void *file_to_mem(const char *bhd5_file, void *file_buffer)
    {
        if (bhd5_file == NULL)
        {
            SetLastError(ERROR_INVALID_ADDRESS);
            return NULL;
        }
        bool manual_allocation = false;


        // Check if file exists
        if (!FileUtil::file_exists(bhd5_file))
        {
            SetLastError(ERROR_FILE_NOT_FOUND);
            return NULL;
        }

        Bhd5Header header;
        if (!Bhd5ArchiveFile::header_from_file(bhd5_file, &header))
        {
            SetLastError(ERROR_READ_FAULT);
            return NULL;
        }

        if (file_buffer == NULL)
        {
            manual_allocation = true;
            file_buffer = CoTaskMemAlloc(header.file_size);
        }

        if (file_buffer == NULL)
        {
            // Failed to allocate memory for file data
            SetLastError(ERROR_OUTOFMEMORY);
            return NULL;
        }

        // Read the file contents into the buffer
        std::ifstream in_stream(bhd5_file, std::ios::in | std::ios::binary);
        in_stream.read((char*)file_buffer, header.file_size);
        in_stream.close();

        return file_buffer;
    }


    /*
        "Create Bhd5Archive object from BHD5 File"

        Reads all data from the specified BHD5 file and stores the data
        in memory at the specified buffer, and constructs a new Bhd5Archive
        object that utilizes the data. The object is dynamically allocated,
        so it must be manually freed with "delete".

        If file_buffer is not NULL, it is assumed that file_buffer is large
        enough to hold all data from the file. If file_buffer is NULL,
        memory is dynamically allocated for the file data, and will be freed
        by the object destructor. If the memory is manually freed before the
        object is destroyed, an access violation exception is likely.

        @return Pointer to the new BHD5 file object in memory if successful;
                otherwise returns NULL.
    */
    static Bhd5ArchiveFile *from_file(const char *bhd5_file, void *file_buffer)
    {
        if (bhd5_file == NULL)
        {
            SetLastError(ERROR_INVALID_ADDRESS);
            return NULL;
        }
        bool manual_allocation = false;

        if (file_buffer == NULL)
            manual_allocation = true;

        if (!(file_buffer = file_to_mem(bhd5_file, file_buffer))) // Failed to read file
            return NULL; // Note: Error code should have been set by file_to_mem()

        return new Bhd5ArchiveFile(file_buffer, NULL, manual_allocation);
    }


    /*
        "Load Header from BHD5 File into Memory"

        Reads header data from the specified BHD5 file and stores the data
        in memory at the specified buffer.

        If header_buffer is not NULL, it is assumed that header_buffer is
        large enough to hold all file header data. If header_buffer is NULL,
        memory is dynamically allocated for the header data, and must be
        freed manually using CoTaskMemFree.

        @return Pointer to the Bhd5Header structure if successful, or NULL
                if an error occurred.
    */
    static Bhd5Header *header_from_file(const char *bhd5_file, Bhd5Header *header_buffer)
    {
        if (bhd5_file == NULL)
        {
            SetLastError(ERROR_INVALID_ADDRESS);
            return NULL;
        }
        bool manual_allocation = false;

        // Manually allocate memory for the header data, if necessary
        if (header_buffer == NULL)
        {
            manual_allocation = true;
            if ((header_buffer = (Bhd5Header*)CoTaskMemAlloc(sizeof(Bhd5Header))) == NULL)
            {
                // Unable to allocate memory
                SetLastError(ERROR_OUTOFMEMORY);
                return NULL;
            }
        }

        // Open file
        std::ifstream in_stream(bhd5_file, std::ios::in | std::ios::binary);
        if (!in_stream.good())
        {
            // File doesn't exist
            in_stream.close();
            if (manual_allocation)
                CoTaskMemFree(header_buffer);
            SetLastError(ERROR_FILE_NOT_FOUND);
            return NULL;
        }
        // Read file contents into buffer
        in_stream.read((char*)header_buffer, sizeof(Bhd5Header));
        in_stream.close();

        return header_buffer;
    }


    /*
        "Load Bin from BHD5 File into Memory"

        Reads a bin entry from the specified BHD5 file and stores the data
        in memory at the specified buffer.

        If bin_buffer is not NULL, it is assumed that bin_buffer is large
        enough to hold the full bin structure. If bin_buffer is NULL,
        memory is dynamically allocated for the data, and must be freed
        manually using CoTaskMemFree.

        If check_bin_index is true, the file header is first checked to confirm
        that the specified bin index is within the bounds of the bin array (Set
        check_bin_index to false for faster reads).

        @return Pointer to the Bhd5Bin structure if successful, or NULL if
                an error occurred.
    */
    static Bhd5Bin *bin_from_file_by_index(const char *bhd5_file, Bhd5Bin *bin_buffer, uint32_t bin_index, bool check_bin_index = true)
    {
        // If check_bin_index is true, confirm that the specified bin index is within the bounds of the bin array
        if (check_bin_index)
        {
            Bhd5Header header;
            if (!header_from_file(bhd5_file, &header))
                return NULL; // Error reading file

            if (bin_index >= header.bin_count)
            {
                SetLastError(ERROR_INVALID_PARAMETER); // Bin doesn't exist in this file
                return NULL;
            }
        }

        return (Bhd5Bin*)FileUtil::read_from_offset(bhd5_file, sizeof(Bhd5Header) + (bin_index * sizeof(Bhd5Bin)), sizeof(Bhd5Bin), bin_buffer);
    }


    // Essentially an overload of "Load Bin from BHD5 File into Memory"
    static Bhd5Bin *bin_from_file_by_hash(const char *bhd5_file, Bhd5Bin *bin_buffer, uint32_t hash)
    {
        Bhd5Header header;
        if (!header_from_file(bhd5_file, &header))
            return NULL; // Error reading file

        // Get the Bin structure that contains the Record with the given hash
        bool found = false; // True if record with hash was found
        Bhd5Bin bin;
        Bhd5Record record;
        int i = 0, j = 0;
        for (i = 0; i < (int)header.bin_count; i++)
        {
            if (!bin_from_file_by_index(bhd5_file, &bin, i, false))
                return NULL; // Failed to get Bin from BHD5 file
            for (j = 0; j < (int)bin.record_count; j++)
            {
                if (!record_from_file_by_index(bhd5_file, &record, i, j, false))
                    return NULL; // Failed to get Record from BHD5 file
                if (i < 3)
                    if (hash == record.hash)
                    {
                        found = true;
                        break;
                    }
            }
            if (found)
                break;
        }
        if (found)
            return (Bhd5Bin*)FileUtil::read_from_offset(bhd5_file, sizeof(Bhd5Header) + (i * sizeof(Bhd5Bin)), sizeof(Bhd5Bin), bin_buffer);
        // Failed to find a record with the given hash
        SetLastError(ERROR_SECTOR_NOT_FOUND);
        return NULL;
    }


    // Overload of "Load Bin from BHD5 File into Memory"
    static Bhd5Bin *bin_from_file_by_filename(const char *bhd5_file, Bhd5Bin *bin_buffer, const char* packed_filename)
    {
        if (packed_filename == NULL)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return NULL;
        }
        uint32_t hash = filename_to_hash(packed_filename);
        return bin_from_file_by_hash(bhd5_file, bin_buffer, hash);
    }


    /*
        "Load Record from BHD5 File into Memory"

        Reads a record structure from the specified BHD5 file and stores the data
        in memory at the specified buffer.

        If record_buffer is not NULL, it is assumed that record_buffer is large
        enough to hold the full record structure. If record_buffer is NULL,
        memory is dynamically allocated for the data, and must be freed
        manually using CoTaskMemFree.

        If check_bin_index is true, the file header is first checked to confirm
        that the specified bin index is within the bounds of the bin array (Set
        check_bin_index to false for faster reads).

        @return Pointer to the Bhd5Record structure if successful, or NULL if
                an error occurred.
    */
    static Bhd5Record *record_from_file_by_index(const char *bhd5_file, Bhd5Record *record_buffer, uint32_t bin_index, unsigned int record_index, bool check_bin_index = true)
    {
        // If check_bin_index is true, confirm that the specified bin index is within the bounds of the bin array
        if (check_bin_index)
        {
            Bhd5Header header;
            if (!header_from_file(bhd5_file, &header))
                return NULL; // Error reading file

            if (bin_index >= header.bin_count)
            {
                SetLastError(ERROR_INVALID_PARAMETER); // Bin doesn't exist in this file
                return NULL;
            }
        }

        Bhd5Bin bin;
        // Get bin struct (which holds data needed to obtain record struct)
        if (!(Bhd5Bin*)FileUtil::read_from_offset(bhd5_file, sizeof(Bhd5Header) + (bin_index * sizeof(Bhd5Bin)), sizeof(Bhd5Bin), &bin))
            return NULL; // Error reading file

        if (record_index >= bin.record_count)
        {
            SetLastError(ERROR_INVALID_PARAMETER); // Record doesn't exist in this bin
            return NULL;
        }

        return (Bhd5Record*)FileUtil::read_from_offset(bhd5_file, bin.record_offset + (record_index * sizeof(Bhd5Record)), sizeof(Bhd5Record), record_buffer);
    }


    // Overload of "Load Record from BHD5 File into Memory"
    static Bhd5Record *record_from_file_by_hash(const char *bhd5_file, Bhd5Record *record_buffer, uint32_t hash)
    {
        Bhd5Header header;
        if (!header_from_file(bhd5_file, &header))
            return NULL; // Error reading file

        // Get the Bin structure that contains the Record with the given hash
        bool found = false; // True if record with hash was found
        Bhd5Bin bin;
        Bhd5Record record;
        int i = 0, j = 0;
        for (i = 0; i < (int)header.bin_count; i++)
        {
            if (!bin_from_file_by_index(bhd5_file, &bin, i, false))
                return NULL; // Failed to get Bin from BHD5 file
            for (j = 0; j < (int)bin.record_count; j++)
            {
                if (!record_from_file_by_index(bhd5_file, &record, i, j, false))
                    return NULL; // Failed to get Record from BHD5 file
                if (hash == record.hash)
                {
                    found = true;
                    break;
                }
            }
            if (found)
                break;
        }
        if (found)
            return (Bhd5Record*)FileUtil::read_from_offset(bhd5_file, bin.record_offset + (j * sizeof(Bhd5Record)), sizeof(Bhd5Record), record_buffer);
        // Failed to find a record with the given hash
        SetLastError(ERROR_SECTOR_NOT_FOUND);
        return NULL;
    }


    // Overload of "Load Record from BHD5 File into Memory"
    static Bhd5Record *record_from_file_by_filename(const char *bhd5_file, Bhd5Record *record_buffer, const char* packed_filename)
    {
        if (packed_filename == NULL)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return NULL;
        }
        uint32_t hash = filename_to_hash(packed_filename);
        return record_from_file_by_hash(bhd5_file, record_buffer, hash);
    }


    /*
        "Load Record Array from BHD5 File into Memory"

        Reads the full array of record structures corresponding to a bin structure
        in the specified BHD5 file and stores the data in memory at the specified
        buffer.

        If record_buffer is not NULL, it is assumed that record_buffer is large
        enough to hold the full record array. If record_buffer is NULL,
        memory is dynamically allocated for the data, and must be freed
        manually using CoTaskMemFree.

        If check_bin_index is true, the file header is first checked to confirm
        that the specified bin index is within the bounds of the bin array (Set
        check_bin_index to false for faster reads).

        @return Pointer to the Bhd5Record structure if successful, or NULL if
                an error occurred.
    */
    static Bhd5Record *bin_records_from_file_by_index(const char *bhd5_file, Bhd5Record *record_buffer, uint32_t bin_index, bool check_bin_index = true)
    {
        // If check_bin_index is true, confirm that the specified bin index is within the bounds of the bin array
        if (check_bin_index)
        {
            Bhd5Header header;
            if (!header_from_file(bhd5_file, &header))
                return NULL; // Error reading file

            if (bin_index >= header.bin_count)
            {
                SetLastError(ERROR_INVALID_PARAMETER); // Bin doesn't exist in this file
                return NULL;
            }
        }

        Bhd5Bin bin;
        // Get bin struct (which holds data needed to obtain record struct)
        if (!(Bhd5Bin*)FileUtil::read_from_offset(bhd5_file, sizeof(Bhd5Header) + (bin_index * sizeof(Bhd5Bin)), sizeof(Bhd5Bin), &bin))
            return NULL; // Error reading file

        return (Bhd5Record*)FileUtil::read_from_offset(bhd5_file, bin.record_offset, (sizeof(Bhd5Record) * bin.record_count), record_buffer);
    }


    // Essentially an overload of "Load Record Array from BHD5 File into Memory"
    static Bhd5Record *bin_records_from_file_by_hash(const char *bhd5_file, Bhd5Record *record_buffer, uint32_t hash)
    {
        Bhd5Header header;
        if (!header_from_file(bhd5_file, &header))
            return NULL; // Error reading file

        // Get the Bin structure that contains the Record with the given hash
        bool found = false; // True if record with hash was found
        Bhd5Bin bin;
        Bhd5Record record;
        int i = 0, j = 0;
        for (i = 0; i < (int)header.bin_count; i++)
        {
            if (bin_from_file_by_index(bhd5_file, &bin, i, false) == NULL)
                return NULL; // Failed to get Bin from BHD5 file
            for (j = 0; j < (int)bin.record_count; j++)
            {
                if (record_from_file_by_index(bhd5_file, &record, i, j, false) == NULL)
                    return NULL; // Failed to get Record from BHD5 file
                if (hash == record.hash)
                {
                    found = true;
                    break;
                }
            }
            if (found)
                break;
        }
        if (found)
            return (Bhd5Record*)FileUtil::read_from_offset(bhd5_file, bin.record_offset, (sizeof(Bhd5Record) * bin.record_count), record_buffer);
        // Failed to find a record with the given hash
        SetLastError(ERROR_SECTOR_NOT_FOUND);
        return NULL;
    }


    // Overload of "Load Record Array from BHD5 File into Memory"
    static Bhd5Record *bin_records_from_file_by_filename(const char *bhd5_file, Bhd5Record *record_buffer, const char* packed_filename)
    {
        if (packed_filename == NULL)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return NULL;
        }
        uint32_t hash = filename_to_hash(packed_filename);
        return bin_records_from_file_by_hash(bhd5_file, record_buffer, hash);
    }


    /*
        "Extract Packed File from BDT File into Memory Using BHD5 File as Index"

        Extracts a packed file from a BDT (.bdt) archive and stores it in memory.

        To obtain the offset of the packed file in the BDT archive, the specified
        BHD5 file is first parsed for indexing data.

        If extract_buffer is not NULL, it is assumed that extract_buffer is large
        enough to hold the full file. If extract_buffer is NULL, memory is
        dynamically allocated for the data, and must be freed manually using
        CoTaskMemFree.

        If check_bin_index is true, the file header is first checked to confirm
        that the specified bin index is within the bounds of the bin array (Set
        check_bin_index to false for faster reads).

        @return Pointer to the start of the extracted file in memory if successful,
                or NULL if an error occurred.
    */
    static void *extract_from_bdt_by_index(const char *bdt_file, const char *bhd5_file, void *extract_buffer, uint32_t bin_index, unsigned int record_index, bool check_bin_index = true)
    {
        // Get the record structure
        Bhd5Record record;
        if (record_from_file_by_index(bhd5_file, &record, bin_index, record_index, check_bin_index) == NULL)
            return NULL; // Error occurred
        // Load packed file into memory
        return FileUtil::read_from_offset(bdt_file, record.offset, record.size, extract_buffer, true);
    }


    // Overload of "Extract Packed File from BDT File into Memory Using BHD5 File as Index"
    static void *extract_from_bdt_by_hash(const char *bdt_file, const char *bhd5_file, void *extract_buffer, uint32_t hash)
    {
        // Get the record structure
        Bhd5Record record;
        if (record_from_file_by_hash(bhd5_file, &record, hash) == NULL)
            return NULL; // Error occurred
        // Load packed file into memory
        return FileUtil::read_from_offset(bdt_file, record.offset, record.size, extract_buffer, true);
    }


    // Overload of "Extract Packed File from BDT File into Memory Using BHD5 File as Index"
    static void *extract_from_bdt_by_filename(const char *bdt_file, const char *bhd5_file, void *extract_buffer, const char* packed_filename)
    {
        // Get the record structure
        Bhd5Record record;
        if (record_from_file_by_filename(bhd5_file, &record, packed_filename) == NULL)
            return NULL; // Error occurred
        // Load packed file into memory
        return FileUtil::read_from_offset(bdt_file, record.offset, record.size, extract_buffer, true);
    }


    /*
        "Extract Packed File from BDT File to Disk Using BHD5 File as Index"

        Extracts a packed file from a BDT (.bdt) archive and stores it on disk.

        To obtain the offset of the packed file in the BDT archive, the specified
        BHD5 file is first parsed for indexing data.

        If check_bin_index is true, the file header is first checked to confirm
        that the specified bin index is within the bounds of the bin array (Set
        check_bin_index to false for faster reads).

        @return ERROR_SUCCESS on success; otherwise returns a relevant error code.
    */
    static int extract_from_bdt_by_index(const char *bdt_file, const char *bhd5_file, uint32_t bin_index, unsigned int record_index, const char *write_file, bool overwrite = true, bool check_bin_index = true)
    {
        // Get the record structure
        Bhd5Record record;
        if (record_from_file_by_index(bhd5_file, &record, bin_index, record_index, check_bin_index) == NULL)
            return GetLastError();
        // Load packed file into memory
        void *extracted_file_in_mem;
        if ((extracted_file_in_mem = FileUtil::read_from_offset(bdt_file, record.offset, record.size, NULL, true)) == NULL)
            return GetLastError();

        int return_val = FileUtil::dump_file(extracted_file_in_mem, record.size, write_file, overwrite);

        // Free the memory that was allocated for the file
        CoTaskMemFree(extracted_file_in_mem);

        return return_val;
    }

    // Overload of "Extract Packed File from BDT File to Disk Using BHD5 File as Index"
    static int extract_from_bdt_by_hash(const char *bdt_file, const char *bhd5_file, uint32_t hash, const char *write_file, bool overwrite = true)
    {
        // Get the record structure
        Bhd5Record record;
        if (record_from_file_by_hash(bhd5_file, &record, hash) == NULL)
            return GetLastError();
        // Load packed file into memory
        void *extracted_file_in_mem;
        if ((extracted_file_in_mem = FileUtil::read_from_offset(bdt_file, record.offset, record.size, NULL, true)) == NULL)
            return GetLastError();

        int return_val = FileUtil::dump_file(extracted_file_in_mem, record.size, write_file, overwrite);

        // Free the memory that was allocated for the file
        CoTaskMemFree(extracted_file_in_mem);

        return return_val;
    }

    // Overload of "Extract Packed File from BDT File to Disk Using BHD5 File as Index"
    static int extract_from_bdt_by_filename(const char *bdt_file, const char *bhd5_file, const char* packed_filename, const char *write_file = NULL, bool overwrite = true)
    {
        if (packed_filename == NULL)
        {
            return ERROR_INVALID_PARAMETER;
        }
        uint32_t hash = filename_to_hash(packed_filename);

        // If output file name is NULL, use the original name of the packed file
        if (write_file == NULL)
            write_file = packed_filename;

        return extract_from_bdt_by_hash(bdt_file, bhd5_file, hash, write_file, overwrite);
    }
} Bhd5Archive;







#endif // _DS1_FILE_LIB_BHD5_ARCHIVE_FILE_H_
