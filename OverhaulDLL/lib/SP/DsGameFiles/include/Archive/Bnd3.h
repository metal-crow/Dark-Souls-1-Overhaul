/*
    DARK SOULS GAME FILE C++ LIBRARY

    Contributors to this file:
        Sean Pesce       -     C++
        Burton Radons    -     Reverse-engineering Dark Souls game files (Github: https://github.com/Burton-Radons )


    Archive/Bnd3.h

    References:
        https://github.com/Burton-Radons/Alexandria


*/

#pragma once

#ifndef _DS1_FILE_LIB_BND3_ARCHIVE_FILE_H_
    #define _DS1_FILE_LIB_BND3_ARCHIVE_FILE_H_


#include "Windows.h"
#include "FileUtil.h"
#include <map>
#include <fstream>
#include "Archive/Header.h"




// Header structure of BND3 archive file (.*bnd)
typedef struct Bnd3ArchiveHeader : ArchiveFileHeader {
    uint32_t version = 0x00000074; // BND file version
    uint32_t record_count = 0;
    uint32_t header_size = 0; // Size of header section + record structs section + file name section
    const uint32_t const32b_0 = 0x00000000;
    const uint32_t const32b_1 = 0x00000000;
} Bnd3Header;


// BND3 record data structure
typedef struct Bnd3ArchiveRecord {
    const uint32_t const32b_0 = 0x00000040;
    uint32_t size = 0; // Size of packed file
    uint32_t offset = 0; // Offset of packed file data (relative to start of BND3 file)
    uint32_t id = 0;
    uint32_t name_offset = 0; // Offset of ASCII file name string (relative to start of BND3 file)
    uint32_t size2 = size;    // Always the same as size
} Bnd3Record;



// For reading and manipulating BND3 archive files (*.parambnd, etc)
typedef class Bnd3ArchiveFile {
public:

    // Maps filename to record index
    std::map<std::string, uint32_t> filename_to_index;

    // Maps unique ID to record index
    std::map<uint32_t, uint32_t> id_to_index;

    // BND3 file header
    Bnd3Header *header;

    // Record structs array start
    Bnd3Record *records;

    // Start of the string array that holds filenames of packed files
    char *filenames;

    // Constructor
    Bnd3ArchiveFile(void *new_header_start, void *new_records_start = NULL) : header((Bnd3Header*)new_header_start)
    {
        if (new_header_start != NULL && header->record_count > 0)
        {
            if (new_records_start == NULL)
                records = ((Bnd3Record*)(((uint8_t*)new_header_start) + sizeof(Bnd3Header))); // Assume file is contiguous in memory
            else
                records = (Bnd3Record*)new_records_start;

            // Obtain starting address of filename string array
            filenames = (char*)((uint8_t*)header) + records[0].name_offset;

            // Map file names to indices
            char *current_filename = (char*)filenames;
            for (int i = 0; i < (int)header->record_count; i++)
            {
                std::string cfn = current_filename;

                // Remove directory prefix
                if (cfn.find(Bnd3ArchiveFile::packed_file_dir) == 0)
                    cfn = cfn.substr(Bnd3ArchiveFile::packed_file_dir.length());

                // Replace backslashes
                for (int c = 0; c < (int)cfn.length(); c++)
                    if (cfn[c] == '\\')
                        cfn[c] = '/';

                filename_to_index.insert(std::make_pair(cfn, i));
                current_filename += (std::string(current_filename).length() + 1);

                id_to_index.insert(std::make_pair(records[i].id, i));
            }
        }
        else if (new_header_start != NULL)
        {
            // No records
            records = NULL;
        }
        else
        {
            header = NULL;
            records = NULL;
            SetLastError(ERROR_CANNOT_MAKE);
        }
    }


    // Returns pointer to the record at the given index
    Bnd3Record *get_record(uint32_t index)
    {
        if (header == NULL || records == NULL || index >= header->record_count)
            return NULL;
        return &(records[index]);
    }

    // Returns pointer to the record with the given filename
    Bnd3Record *get_record(const char *filename)
    {
        if (header == NULL || records == NULL)
            return NULL;

        try {
            return get_record(filename_to_index.at(filename));
        }
        catch (const std::out_of_range&) {
            return NULL;
        }
        return NULL;
    }

    // Returns pointer to the record with the given ID
    Bnd3Record *get_record_by_id(uint32_t id)
    {
        if (header == NULL || records == NULL)
            return NULL;

        try {
            return get_record(id_to_index.at(id));
        }
        catch (const std::out_of_range&) {
            return NULL;
        }
        return NULL;
    }

    // Returns pointer to compressed file at the given index
    void *get_packed_file(uint32_t index)
    {
        if (header == NULL || records == NULL || index >= header->record_count)
            return NULL;
        return (((uint8_t*)header) + records[index].offset);
    }

    // Returns pointer to compressed file with the given filename
    void *get_packed_file(const char *filename)
    {
        if (header == NULL || records == NULL)
            return NULL;

        try {
            return get_packed_file(filename_to_index.at(filename));
        }
        catch (const std::out_of_range&) {
            return NULL;
        }
        return NULL;
    }

    // Returns file name of the packed file at the given index
    int get_packed_filename(uint32_t index, std::string& out)
    {
        if (header == NULL || records == NULL)
            return ERROR_INVALID_ADDRESS;
        if (index >= header->record_count)
            return ERROR_BUFFER_OVERFLOW;

        out.clear();

        for (auto pair : filename_to_index)
        {
            if (pair.second == index)
            {
                out.append(pair.first);
                return ERROR_SUCCESS;
            }
        }
        return ERROR_SUCCESS;
    }

    // Returns file name of the packed file, found using the given ID
    int get_packed_filename_by_id(uint32_t id, std::string& out)
    {
        if (header == NULL || records == NULL)
            return ERROR_INVALID_ADDRESS;

        try {
            return get_packed_filename(id_to_index.at(id), out);
        }
        catch (const std::out_of_range&) {
            return ERROR_FILE_NOT_FOUND;
        }
        return ERROR_SUCCESS;
    }

    // Extracts packed file data to an external file
    int dump_packed_file(uint32_t index, const char *new_directory, bool overwrite = true)
    {
        if (header == NULL || records == NULL)
            return ERROR_INVALID_DATA;

        if (new_directory == NULL)
            return ERROR_INVALID_ADDRESS;


        // Build output path string
        std::string file = new_directory;
        if (file[file.length() - 1] == '/' || file[file.length() - 1] == '\\')
            file = file.substr(0, file.length() - 1);

        std::string tmp;
        if (get_packed_filename(index, tmp)) // Get file name
            return ERROR_INVALID_PARAMETER;
        file += tmp;

        // Write file
        return FileUtil::dump_file(((uint8_t*)header + get_record(index)->offset), get_record(index)->size, file.c_str(), overwrite);
    }


    // Extracts all packed file data to a external files
    int dump_all_packed_files(const char *new_directory, bool overwrite = true)
    {
        if (header == NULL || records == NULL)
            return ERROR_INVALID_DATA;

        if (new_directory == NULL)
            return ERROR_INVALID_ADDRESS;


        // Build output path string
        std::string file = new_directory;
        if (file[file.length() - 1] == '/' || file[file.length() - 1] == '\\')
            file = file.substr(0, file.length() - 1);

        // Replace slashes with backslashes
        for (int c = 0; c < (int)file.length(); c++)
            if (file[c] == '/')
                file[c] = '\\';

        // Dump every packed file in the archive
        for (int i = 0; i < (int)record_count(); i++)
        {
            std::string tmp_file = file,
                tmp_fn;
            if (get_packed_filename(i, tmp_fn)) // Get file name
                return ERROR_INVALID_PARAMETER;
            tmp_file += tmp_fn;

            // Write file
            int ret;
            if (ret = FileUtil::dump_file(((uint8_t*)header + get_record(i)->offset), get_record(i)->size, tmp_file.c_str(), overwrite))
                return ret;
        }

        return ERROR_SUCCESS;
    }


    // Get-info methods:
    uint32_t record_count() { if (header == NULL) { return 0; } return header->record_count; } // Returns number of BND3 record structs in the file
    uint32_t  header_size() { if (header == NULL) { return 0; } return header->header_size; }


    // Packed filename prefix
    const std::string packed_file_dir = "N:\\FRPG\\data\\INTERROOT_win32";
} Bnd3Archive;




#endif // _DS1_FILE_LIB_BND3_ARCHIVE_FILE_H_
