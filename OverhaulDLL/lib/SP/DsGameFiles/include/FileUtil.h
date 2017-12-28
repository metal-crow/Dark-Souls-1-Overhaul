/*
    DARK SOULS GAME FILE C++ LIBRARY

    Contributors to this file:
        Sean Pesce  -  C++

    FileUtil.h

    Defines functions and variables that are used for file-related
    tasks in general, rather than specific file types.
*/

#pragma once

#ifndef _DS1_FILE_LIB_FILE_UTIL_H_
    #define _DS1_FILE_LIB_FILE_UTIL_H_

#include <string>
#include <vector>
#include "Ds1GameFileLib.h"


class Ds1FileUtil
{
public:


#ifdef _DS1_FILE_LIB_INCLUDE_BDT_FILE_LIST_
    /*
        Use of the packed file list header must be explicitly defined because
        its inclusion increases compiled file size by ~200KB
    */

    // Number of filename strings in the filename list used by hashing functions
#define FILENAME_LIST_SIZE 6128

// List of game files that are packed into the Dark Souls game archives
    static const char* const FILENAMES[FILENAME_LIST_SIZE];

    // Stores hashed filenames
    static uint32_t FILENAME_HASHES[FILENAME_LIST_SIZE];

    // Calculates and stores hashes for all default game filenames
    static int hash_default_filenames();
#endif // _DS1_FILE_LIB_INCLUDE_BDT_FILE_LIST_


    // Checks if the specified file exists
    static bool file_exists(const char* file);    // char
    static bool file_exists(const wchar_t* file); // wchar_t

    // Obtains the size of the specified file (in bytes), or 0 (with error code set) if error occurred
    static size_t file_size(const wchar_t* file);
    static size_t file_size(const char* file);

    // Reads the given number of bytes from the specified offset of a file and stores the data in the supplied buffer
    static void *read_from_offset(const char* file, unsigned long offset, size_t num_bytes, void *buffer, bool allow_alloc = true);

    // Writes a block of memory to a file
    static int dump_file(void *source, size_t size, const char *new_file, bool overwrite = true);

    // Calculates the .bhd5 hash value for a filename (credit to Burton Radons for reversing this)
    static uint32_t get_filename_hash(const char *filename);

    // Converts a char array to lowercase
    static int to_lowercase(char *string);

    /*
        Reverses the byte order of the given unsigned 32-bit integer.

        If from_endian_int is an unsigned 32-bit Big Endian integer, return value is an unsigned 32-bit Little Endian integer.
        If from_endian_int is an unsigned 32-bit Litte Endian integer, return value is an unsigned 32-bit Big Endian integer.
    */
    static uint32_t reverse_endian(uint32_t from_endian_int);
    
    /*
        Converts the given integer value to hexadecimal string representation
        and stores it in the supplied output string.
    */
    static const char *to_hex_string(int number, std::string &out_string, bool hex_prefix = false, bool full_bytes = true);


    /*
        Calculates the MD5 hash for the data in a given block of memory
    */
    static uint32_t calculate_md5_hash(uint8_t *data, uint32_t size, uint32_t hash_length, std::vector<uint8_t> &output);
};
typedef Ds1FileUtil FileUtil;


#endif // _DS1_FILE_LIB_FILE_UTIL_H_
