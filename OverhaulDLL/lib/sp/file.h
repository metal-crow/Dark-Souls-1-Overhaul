/*
    CONTRIBUTORS:
        Sean Pesce

*/

#pragma once

#ifndef SP_UTILITY_FILE_H_
#define SP_UTILITY_FILE_H_

#include "sp/sp.h"
#include <fstream>


__SP_NAMESPACE
namespace file {


// Returns the size (in bytes) of the specified file
size_t size(const std::wstring &file);
// Returns the size (in bytes) of the specified file
size_t size(const std::string &file);


// Returns absolute file path
template <typename charT>
std::basic_string<charT> path_abs(const std::basic_string<charT> &file)
{
    charT buff[MAX_PATH + 1];
    buff[MAX_PATH] = (charT)'\0';
    if (sizeof(charT) == 1) {
        if (GetFullPathNameA((char*)file.c_str(), MAX_PATH+1, (char*)buff, NULL)) {
            return buff;
        }
    } else {
        if (GetFullPathNameW((wchar_t*)file.c_str(), MAX_PATH+1, (wchar_t*)buff, NULL)) {
            return buff;
        }
    }
    buff[0] = (charT)'\0';
    return buff;
}


// Returns file name ONLY
template <typename charT>
std::basic_string<charT> name(const std::basic_string<charT> &file)
{
    char    *name   = NULL;
    wchar_t *name_w = NULL;
    charT buff[MAX_PATH + 1];
    buff[MAX_PATH] = (charT)'\0';
    if (sizeof(charT) == 1) {
        if (GetFullPathNameA((char*)file.c_str(), MAX_PATH + 1, (char*)buff, &name) && name != NULL) {
            return (charT*)name;
        }
    } else {
        if (GetFullPathNameW((wchar_t*)file.c_str(), MAX_PATH + 1, (wchar_t*)buff, &name_w) && name_w != NULL) {
            return (charT*)name_w;
        }
    }
    buff[0] = (charT)'\0';
    return buff;
}


// Returns file name ONLY (without extension)
template <typename charT>
std::basic_string<charT> name_no_ext(const std::basic_string<charT> &file)
{
    std::basic_string<charT> f = name(file);
    size_t pos = f.find_last_of((charT)'.');
    if (pos != std::basic_string<charT>::npos) {
        return file.substr(0, pos);
    }
    return f;
}


// Returns the folder containing the given file
template <typename charT>
std::basic_string<charT> dir(const std::basic_string<charT> &file)
{
    char    *name   = NULL;
    wchar_t *name_w = NULL;
    charT buff[MAX_PATH + 1];
    buff[MAX_PATH] = (charT)'\0';
    if (sizeof(charT) == 1) {
        if (GetFullPathNameA((char*)file.c_str(), MAX_PATH + 1, (char*)buff, &name) && name != NULL) {
            return std::string(buff).substr(0, ((unsigned long long)name - (unsigned long long)buff) / sizeof(charT));
        }
    } else {
        if (GetFullPathNameW((wchar_t*)file.c_str(), MAX_PATH + 1, (wchar_t*)buff, &name_w) && name_w != NULL) {
            return std::string(buff).substr(0, ((unsigned long long)name_w - (unsigned long long)buff) / sizeof(charT));
        }
    }
    buff[0] = (charT)'\0';
    return buff;
}


// Returns file extension ONLY
template <typename charT>
std::basic_string<charT> ext(const std::basic_string<charT> &file)
{
    size_t pos = name(file).find_last_of((charT)'.');
    if (pos != std::basic_string<charT>::npos) {
        return file.substr(pos);
    }
    return std::basic_string<charT>();
}


/*
    Reads the given number of bytes from the specified offset of a file and stores the data in the supplied buffer
    If buffer is not NULL, it is assumed that the buffer size is at least num_bytes. If buffer is NULL, and allow_alloc
    is true, memory is dynamically allocated for the data, and must be manually freed with CoTaskMemFree.
*/
template <typename charT>
void* read_from_offset(const std::basic_string<charT> &file, unsigned long offset, size_t num_bytes, void *buffer, bool allow_alloc = true)
{
    bool manual_allocation = false;
    if ((!allow_alloc) && buffer == NULL) {
        sp::err::set(SP_ERR_INVALID_ADDRESS);
        return NULL;
    }

    // Manually allocate memory for the data, if necessary
    if (buffer == NULL) {
        manual_allocation = true;
        if ((buffer = CoTaskMemAlloc(num_bytes)) == NULL) {
            // Unable to allocate memory
            sp::err::set(SP_ERR_OUT_OF_MEMORY);
            return NULL;
        }
    }

    // Open file
    std::ifstream in_stream(file.c_str(), std::ios::in | std::ios::binary);
    if (!in_stream.good()) {
        // File doesn't exist
        in_stream.close();
        if (manual_allocation)
            CoTaskMemFree(buffer);
        sp::err::set(SP_ERR_FILE_NOT_FOUND);
        return NULL;
    }

    // Move file pointer to offset
    in_stream.seekg(offset);
    // Read file contents into buffer
    in_stream.read((char*)buffer, num_bytes);

    // Check for read error
    if (in_stream.fail()) {
        // Error occured when reading file
        in_stream.close();
        if (manual_allocation)
            CoTaskMemFree(buffer);
        sp::err::set(SP_ERR_INVALID_DATA);
        return NULL;
    }
    in_stream.close();
    return buffer;
}


// Writes a block of memory to a file
template <typename charT>
long dump_memory(void *source, size_t size, const std::basic_string<charT> &new_file, bool overwrite = true)
{
    if (source == NULL)
        return SP_ERR_INVALID_ADDRESS;

    // Replace slashes with backslashes
    for (int i = 0; i < (int)new_file.length(); i++)
        if (new_file[i] == (charT)'/')
            new_file[i] = (charT)'\\';

    // Trim whitespace
    charT trim_mask[5] = { (charT)' ', (charT)'\r', (charT)'\n', (charT)'\t', (charT)0 };
    string::trim(new_file, std::basic_string<charT>(trim_mask));

    // Remove leading slashes
    while (new_file[0] == (charT)'\\')
        new_file = new_file.substr(1);

    if ((!overwrite) && exists(new_file)) {
        return SP_ERR_FILE_EXISTS;
    }

    // Create directory, if it doesn't exist
    int i = 0;
    std::basic_string<charT> tmp;
    while (new_file.find((charT)'\\', i) != std::basic_string<charT>::npos) {
        tmp = new_file.substr(0, new_file.find((charT)'\\', i));
        int ret = CreateDirectory(tmp.c_str(), NULL);
        if (!ret) {
            ret = sp::err::get();
            if (ret != ERROR_ALREADY_EXISTS)
                return ret;
        }
        i++;
    }

    std::ofstream out_stream(new_file.c_str(), std::ios::out | std::ios::binary);
    out_stream.write((const char*)source, size);
    out_stream.close();
    return SP_ERR_SUCCESS;
}


// Returns whether the specified file exists
template <typename charT>
inline bool exists(const std::basic_string<charT> &file)
{
    std::ifstream check_file(file.c_str());
    bool exists = check_file.good();
    check_file.close();
    return exists;
}



template <typename charT>
long write_text(const std::basic_string<charT> &file, const std::basic_string<charT> &data, bool overwrite = true)
{
    if ((!overwrite) && exists(file)) {
        return SP_ERR_FILE_EXISTS;
    }
    std::ofstream out;
    out.open(file);
    if (out.fail()) {
        // Failed to open output file
        return SP_ERR_WRITE;
    }
    out << data;
    out.close();
    return SP_ERR_SUCCESS;
}


template <typename charT>
long append_text(const std::basic_string<charT> &file, const std::basic_string<charT> &data)
{
    std::ofstream out;
    out.open(file, std::ofstream::out | std::ofstream::app);
    if (out.fail()) {
        // Failed to open output file
        return SP_ERR_WRITE;
    }
    out << data;
    out.close();
    return SP_ERR_SUCCESS;
}


} // namespace file
__SP_NAMESPACE_CLOSE // namespace sp


#include "sp/file/file_overloads.hpp"


#endif // SP_UTILITY_FILE_H_
