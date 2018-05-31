/*
    CONTRIBUTORS:
        Sean Pesce

*/

#ifndef SP_UTILITY_STRING_MAIN_H_
#error Invalid header inclusion "util/string/string_overloads.hpp" (Use "util/string.h" instead)
#endif

#include "sp/sp.h"
#include <string>

#define SP_EMPTY_STR_ std::basic_string<charT>({(charT)'\0'})

__SP_NAMESPACE
namespace util   {
namespace string {


template <typename charT>
inline std::basic_string<charT> trim(const charT *in, const std::basic_string<charT> &mask, bool trim_left = true, bool trim_right = true)
{
    if (in == NULL) {
        sp::err::set(SP_ERR_INVALID_ADDRESS);
        return SP_EMPTY_STR_;
    }
    return trim(std::basic_string<charT>(in), mask, trim_left, trim_right);
}


template <typename charT>
inline std::basic_string<charT> trim(const std::basic_string<charT> &in, const charT *mask, bool trim_left = true, bool trim_right = true)
{
    if (mask == NULL) {
        sp::err::set(SP_ERR_INVALID_ADDRESS);
        return SP_EMPTY_STR_;
    }
    return trim(in, std::basic_string<charT>(mask), trim_left, trim_right);
}


template <typename charT>
inline std::basic_string<charT> trim(const charT *in, const charT *mask, bool trim_left = true, bool trim_right = true)
{
    if (in == NULL || mask == NULL) {
        sp::err::set(SP_ERR_INVALID_ADDRESS);
        return SP_EMPTY_STR_;
    }
    return trim(std::basic_string<charT>(in), std::basic_string<charT>(mask), trim_left, trim_right);
}


template <typename charT>
inline std::basic_string<charT> to_lowercase(const charT *in)
{
    if (in == NULL) {
        sp::err::set(SP_ERR_INVALID_ADDRESS);
        return SP_EMPTY_STR_;
    }
    return to_lowercase(std::basic_string<charT>(in));
}


template <typename charT>
inline std::basic_string<charT> to_uppercase(const charT *in)
{
    if (in == NULL) {
        sp::err::set(SP_ERR_INVALID_ADDRESS);
        return SP_EMPTY_STR_;
    }
    return to_uppercase(std::basic_string<charT>(in));
}


template <typename charT>
inline bool contains(const charT *in, const std::basic_string<charT> &chars)
{
    if (in == NULL) {
        sp::err::set(SP_ERR_INVALID_ADDRESS);
        return false;
    }
    return contains(std::basic_string<charT>(in), chars);
}


template <typename charT>
inline bool contains(const std::basic_string<charT> &in, const charT *chars)
{
    if (chars == NULL) {
        sp::err::set(SP_ERR_INVALID_ADDRESS);
        return false;
    }
    return contains(in, std::basic_string<charT>(chars));
}


template <typename charT>
inline bool contains(const charT *in, const charT *chars)
{
    if (in == NULL || chars == NULL) {
        sp::err::set(SP_ERR_INVALID_ADDRESS);
        return false;
    }
    return contains(std::basic_string<charT>(in), std::basic_string<charT>(chars));
}


template <typename charT>
inline bool contains(const charT *in, charT c)
{
    if (in == NULL) {
        sp::err::set(SP_ERR_INVALID_ADDRESS);
        return false;
    }
    return contains(std::basic_string<charT>(in), c);
}


template <typename charT>
inline std::basic_string<charT> remove_chars(const std::basic_string<charT> &in, const charT *chars)
{
    if (chars == NULL) {
        sp::err::set(SP_ERR_INVALID_ADDRESS);
        return in;
    }
    return remove_chars(in, std::basic_string<charT>(chars));
}


template <typename charT>
inline std::basic_string<charT> remove_chars(const charT *in, const std::basic_string<charT> &chars)
{
    if (in == NULL) {
        sp::err::set(SP_ERR_INVALID_ADDRESS);
        return SP_EMPTY_STR_;
    }
    return remove_chars(std::basic_string<charT>(in), chars);
}


template <typename charT>
inline std::basic_string<charT> remove_chars(const charT *in, const charT *chars)
{
    if (in == NULL) {
        sp::err::set(SP_ERR_INVALID_ADDRESS);
        return "";
    } else if (chars == NULL) {
        sp::err::set(SP_ERR_INVALID_ADDRESS);
        return in;
    }
    return remove_chars(std::basic_string<charT>(in), std::basic_string<charT>(chars));
}


template <typename charT>
inline std::basic_string<charT> remove(const charT *in, charT c)
{
    if (in == NULL) {
        sp::err::set(SP_ERR_INVALID_ADDRESS);
        return SP_EMPTY_STR_;
    }
    return remove(std::basic_string<charT>(in), c);
}


template <typename charT>
inline std::basic_string<charT> remove(const std::basic_string<charT> &in, const charT *find)
{
    if (find == NULL) {
        sp::err::set(SP_ERR_INVALID_ADDRESS);
        return in;
    }
    return remove(in, std::basic_string<charT>(find));
}


template <typename charT>
inline std::basic_string<charT> remove(const charT *in, const std::basic_string<charT> &find)
{
    if (in == NULL) {
        sp::err::set(SP_ERR_INVALID_ADDRESS);
        return SP_EMPTY_STR_;
    }
    return remove(std::basic_string<charT>(in), find);
}


template <typename charT>
inline std::basic_string<charT> remove(const charT *in, const charT *find)
{
    if (in == NULL) {
        sp::err::set(SP_ERR_INVALID_ADDRESS);
        return SP_EMPTY_STR_;
    } else if (find == NULL) {
        sp::err::set(SP_ERR_INVALID_ADDRESS);
        return in;
    }
    return remove(std::basic_string<charT>(in), std::basic_string<charT>(find));
}


template <typename charT>
inline std::basic_string<charT> replace(const charT *in, const std::basic_string<charT> &find, const std::basic_string<charT> &replace_with)
{
    if (in == NULL) {
        sp::err::set(SP_ERR_INVALID_ADDRESS);
        return SP_EMPTY_STR_;
    }
    return replace(std::basic_string<charT>(in), find, replace_with);
}


template <typename charT>
inline std::basic_string<charT> replace(const std::basic_string<charT> &in, const charT *find, const std::basic_string<charT> &replace_with)
{
    if (find == NULL) {
        sp::err::set(SP_ERR_INVALID_ADDRESS);
        return in;
    }
    return replace(in, std::basic_string<charT>(find), replace_with);
}


template <typename charT>
inline std::basic_string<charT> replace(const std::basic_string<charT> &in, const std::basic_string<charT> &find, const charT *replace_with)
{
    if (replace_with == NULL) {
        sp::err::set(SP_ERR_INVALID_ADDRESS);
        return replace(in, find, SP_EMPTY_STR_);
    }
    return replace(in, find, std::basic_string<charT>(replace_with));
}


template <typename charT>
inline std::basic_string<charT> replace(const charT *in, const charT *find, const std::basic_string<charT> &replace_with)
{
    if (in == NULL) {
        sp::err::set(SP_ERR_INVALID_ADDRESS);
        return SP_EMPTY_STR_;
    } else if (find == NULL) {
        sp::err::set(SP_ERR_INVALID_ADDRESS);
        return in;
    }
    return replace(std::basic_string<charT>(in), std::basic_string<charT>(find), replace_with);
}


template <typename charT>
inline std::basic_string<charT> replace(const charT *in, const std::basic_string<charT> &find, const charT *replace_with)
{
    if (in == NULL) {
        sp::err::set(SP_ERR_INVALID_ADDRESS);
        return SP_EMPTY_STR_;
    } else if (replace_with == NULL) {
        sp::err::set(SP_ERR_INVALID_ADDRESS);
        return replace(std::basic_string<charT>(in), find, SP_EMPTY_STR_);
    }
    return replace(std::basic_string<charT>(in), find, std::basic_string<charT>(replace_with));
}


template <typename charT>
inline std::basic_string<charT> replace(const std::basic_string<charT> &in, const charT *find, const charT *replace_with)
{
    if (find == NULL) {
        sp::err::set(SP_ERR_INVALID_ADDRESS);
        return in;
    } else if (replace_with == NULL) {
        sp::err::set(SP_ERR_INVALID_ADDRESS);
        return replace(in, std::basic_string<charT>(find), SP_EMPTY_STR_);
    }
    return replace(in, std::basic_string<charT>(find), std::basic_string<charT>(replace_with));
}


template <typename charT>
inline std::basic_string<charT> replace(const charT *in, const charT *find, const charT *replace_with)
{
    if (in == NULL) {
        sp::err::set(SP_ERR_INVALID_ADDRESS);
        return SP_EMPTY_STR_;
    } else if (find == NULL) {
        sp::err::set(SP_ERR_INVALID_ADDRESS);
        return in;
    } else if (replace_with == NULL) {
        sp::err::set(SP_ERR_INVALID_ADDRESS);
        return replace(std::basic_string<charT>(in), std::basic_string<charT>(find), SP_EMPTY_STR_);
    }
    return replace(std::basic_string<charT>(in), std::basic_string<charT>(find), std::basic_string<charT>(replace_with));
}


template <typename charT>
inline std::basic_string<charT> replace(const charT *in, charT find, charT replace_with)
{
    if (in == NULL) {
        sp::err::set(SP_ERR_INVALID_ADDRESS);
        return SP_EMPTY_STR_;
    }
    return replace(std::basic_string<charT>(in), find, replace_with);
}


} // namespace string
} // namespace util
__SP_NAMESPACE_CLOSE // namespace sp


#undef SP_EMPTY_STR_
