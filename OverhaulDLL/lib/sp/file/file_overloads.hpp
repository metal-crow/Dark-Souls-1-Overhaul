/*
    CONTRIBUTORS:
        Sean Pesce

*/

#ifndef SP_UTILITY_FILE_H_
#error Invalid header inclusion "util/file/file_overloads.hpp" (Use "util/file.h" instead)
#endif

#include "sp/sp.h"
#include <string>

__SP_NAMESPACE
namespace file {


template <typename charT>
inline size_t size(const charT *file)
{
    if (file == NULL) {
        sp::err::set(SP_ERR_INVALID_ADDRESS);
        return 0;
    }
    return size(std::basic_string<charT>(file));
}


template <typename charT>
inline std::basic_string<charT> path_abs(const charT *file)
{
    if (file == NULL) {
        sp::err::set(SP_ERR_INVALID_ADDRESS);
        return "";
    }
    return path_abs(std::basic_string<charT>(file));
}


template <typename charT>
inline std::basic_string<charT> name(const charT *file)
{
    if (file == NULL) {
        sp::err::set(SP_ERR_INVALID_ADDRESS);
        return "";
    }
    return name(std::basic_string<charT>(file));
}


template <typename charT>
inline std::basic_string<charT> name_no_ext(const charT *file)
{
    if (file == NULL) {
        sp::err::set(SP_ERR_INVALID_ADDRESS);
        return "";
    }
    return name_no_ext(std::basic_string<charT>(file));
}


template <typename charT>
inline std::basic_string<charT> dir(const charT *file)
{
    if (file == NULL) {
        sp::err::set(SP_ERR_INVALID_ADDRESS);
        return "";
    }
    return dir(std::basic_string<charT>(file));
}


template <typename charT>
inline std::basic_string<charT> ext(const charT *file)
{
    if (file == NULL) {
        sp::err::set(SP_ERR_INVALID_ADDRESS);
        return "";
    }
    return ext(std::basic_string<charT>(file));
}


template <typename charT>
inline bool exists(const charT *file)
{
    if (file == NULL) {
        sp::err::set(SP_ERR_INVALID_ADDRESS);
        return false;
    }
    return exists(std::basic_string<charT>(file));
}


template <typename charT>
inline long write_text(const charT *file, const std::basic_string<charT> &data, bool overwrite = true)
{
    if (file == NULL) {
        return SP_ERR_INVALID_ADDRESS;
    }
    return write_text(std::basic_string<charT>(file), data, overwrite);
}


template <typename charT>
inline long write_text(const std::basic_string<charT> &file, const charT *data, bool overwrite = true)
{
    if (data == NULL) {
        return SP_ERR_INVALID_ADDRESS;
    }
    return write_text(file, std::basic_string<charT>(data), overwrite);
}


template <typename charT>
inline long write_text(const charT *file, const charT *data, bool overwrite = true)
{
    if (file == NULL || data == NULL) {
        return SP_ERR_INVALID_ADDRESS;
    }
    return write_text(std::basic_string<charT>(file), std::basic_string<charT>(data), overwrite);
}


template <typename charT>
inline long append_text(const charT *file, const std::basic_string<charT> &data)
{
    if (file == NULL) {
        return SP_ERR_INVALID_ADDRESS;
    }
    return append_text(std::basic_string<charT>(file), data);
}


template <typename charT>
inline long append_text(const std::basic_string<charT> &file, const charT *data)
{
    if (data == NULL) {
        return SP_ERR_INVALID_ADDRESS;
    }
    return append_text(file, std::basic_string<charT>(data));
}


template <typename charT>
inline long append_text(const charT *file, const charT *data)
{
    if (file == NULL || data == NULL) {
        return SP_ERR_INVALID_ADDRESS;
    }
    return append_text(std::basic_string<charT>(file), std::basic_string<charT>(data));
}


} // namespace file
__SP_NAMESPACE_CLOSE // namespace sp

