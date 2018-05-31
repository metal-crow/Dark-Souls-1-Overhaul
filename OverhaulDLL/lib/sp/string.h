/*
    CONTRIBUTORS:
        Sean Pesce

*/

#pragma once

#ifndef SP_UTILITY_STRING_MAIN_H_
#define SP_UTILITY_STRING_MAIN_H_

#include "sp/sp.h"
#include "sp/utilities.h"
#include <chrono>    // localtime_s



__SP_NAMESPACE
namespace str {


// Default locale used for string conversions
extern std::string LOCALE;


template <typename charT>
inline std::basic_string<charT> endl() {
#ifdef     _WIN32
#define SP_STRING_LF_0_ '\r'
#define SP_STRING_LF_1_ '\n'
#else  // !_WIN32
#define SP_STRING_LF_0_ '\n'
#define SP_STRING_LF_1_ '\0'
#endif //  _WIN32
    charT lf[3] = { (charT)SP_STRING_LF_0_, (charT)SP_STRING_LF_1_, (charT)'\0' };
    return lf;
#undef SP_STRING_LF_0_
#undef SP_STRING_LF_1_
}


// Trims a string by removing leading and/or trailing characters that appear in the mask string
template <typename charT>
std::basic_string<charT> trim(const std::basic_string<charT> &in, const std::basic_string<charT> &mask, bool trim_left = true, bool trim_right = true)
{
    std::basic_string<charT> s = in;
    if (mask.length() == 0)
        return s;

    // Remove leading mask characters
    if (trim_left) {
        int start = s.find_first_not_of(mask.c_str());
        s.erase(0, start);
    }

    // Remove trailing mask characters
    while (trim_right && mask.find(*(s.end() - 1)) != std::basic_string<charT>::npos) {
        s.erase(s.length() - 1, 1);
    }
    return s;
}


// Converts a character to lowercase
template <typename charT>
inline charT to_lower(charT c) {
    if (c >= (charT)'A' && c <= (charT)'Z')
        return c - ((charT)'A' - (charT)'a');
    return c;
}

// Converts a string to lowercase
template <typename charT>
std::basic_string<charT> to_lowercase(const std::basic_string<charT> &in)
{
    std::basic_string<charT> s = in;
    unsigned int i = 0;
    while (s[i] != (charT)0) {
        s[i] = to_lower(s[i]);
        i++;
    }
    return s;
}


// Converts a character to uppercase
template <typename charT>
inline charT to_upper(charT c) {
    return std::toupper(c);
}


// Convert a string to uppercase
template <typename charT>
std::basic_string<charT> to_uppercase(const std::basic_string<charT> &in)
{
    std::basic_string<charT> s = in;
    unsigned int i = 0;
    while (s[i] != (charT)0) {
        s[i] = to_upper(s[i]);
        i++;
    }
    return s;
}


// Returns whether the string contains at least one character from the given set of characters (or false if the either string is empty)
template <typename charT>
inline bool contains(const std::basic_string<charT> &in, const std::basic_string<charT> &chars)
{
    return (in.find_first_of(chars) != std::basic_string<charT>::npos);
}

template <typename charT>
inline bool contains(const std::basic_string<charT> &in, charT c)
{
    return (in.find_first_of(c) != std::basic_string<charT>::npos);
}


// Removes characters from a string if they appear in the given set of characters to remove
template <typename charT>
std::basic_string<charT> remove_chars(const std::basic_string<charT> &in, const std::basic_string<charT> &chars)
{
    std::basic_string<charT> s = in;
    int i = 0, start = -1;
    while (s[i] != (charT)0) { // While not '\0' (end of string)
        if (chars.find(s[i]) != std::basic_string<charT>::npos) {
            if (start <= -1) {
                // Save position of first char to remove
                start = i;
            }
        } else if (start > -1) {
            s.erase(start, (i - start));
            start = -1;
        }
        i++;
    }
    if (start > -1) {
        s.erase(start, s.length() - start);
    }
    return s;
}


// Removes all instances of the specified character from the given string
template <typename charT>
std::basic_string<charT> remove(const std::basic_string<charT> &in, charT c)
{
    std::basic_string<charT> s = in;
    int i = 0, start = -1;
    while (s[i] != (charT)0) { // While not '\0' (end of string)
        if (s[i] == c) {
            if (start <= -1) {
                // Save position of first instance of char to remove
                start = i;
            }
        } else if (start > -1) {
            s.erase(start, (i - start));
            start = -1;
        }
        i++;
    }
    if (start > -1) {
        s.erase(start, s.length() - start);
    }
    return s;
}


// Removes all instances of the specified substring from the given string
template <typename charT>
std::basic_string<charT> remove(const std::basic_string<charT> &in, const std::basic_string<charT> &find)
{
    std::basic_string<charT> s = in;
    size_t pos = 0;
    if (find.length() > 0) {
        while ((pos = s.find(find)) != std::basic_string<charT>::npos) {
            s.erase(pos, find.length());
        }
    }
    return s.c_str();
}


// Replaces all instances of one substring with an instance of another substring
template <typename charT>
std::basic_string<charT> replace(const std::basic_string<charT> &in, const std::basic_string<charT> &find, const std::basic_string<charT> &replace_with)
{
    std::basic_string<charT> s = in;
    size_t pos = 0;
    if (find.length() > 0 && find != replace_with) {
        while ((pos = s.find(find)) != std::basic_string<charT>::npos) {
            s.replace(pos, find.length(), replace_with);
        }
    }
    return s.c_str();
}

// Replaces all instances of one character with an instance of another character
template <typename charT>
std::basic_string<charT> replace(const std::basic_string<charT> &in, charT find, charT replace_with)
{
    std::basic_string<charT> s = in;
    for (int i = 0; i < s.length(); i++) {
        if (s[i] == find) {
            s[i] = replace_with;
        }
    }
    return s;
}

// Replaces characters if they appear in the given set of characters to replace
template <typename charT>
std::basic_string<charT> replace_chars(const std::basic_string<charT> &in, const std::basic_string<charT> &chars, charT replace_with)
{
    std::basic_string<charT> s = in;
    int i = 0;
    while (s[i] != (charT)0) { // While not '\0' (end of string)
        if (chars.find(s[i]) != std::basic_string<charT>::npos) {
            s[i] = replace_with;
        }
        i++;
    }
    return s;
}


// Converts a multibyte string (char) to a wide character string (wchar_t)
std::wstring mb_to_wide(const std::string &in, const std::string &locale = LOCALE);

// Converts a wide character string (wchar_t) to a multibyte string (char)
std::string  wide_to_mb(const std::wstring &in, const std::string &locale = LOCALE);


template <typename charT>
std::basic_string<charT> get_time_basic_string(bool _24_hour = true, bool am_pm = false)
{
    std::basic_string<charT> s;
    int hour;
    // Store current time in data structure for parsing
    time_t current_time_t = time(0);
    tm current_time;
    errno_t err = 0;
    if (err = localtime_s(&current_time, &current_time_t)) {
        sp::err::set(err);
        return s;
    }
    // Hours
    hour = current_time.tm_hour;
    if (!_24_hour) {
        if (hour == 0)
            hour = 12;
        else if (hour > 12)
            hour -= 12;
    }
    if (hour < 10) {
        s += (charT)'0';
    }
    s += std::_Integral_to_string<charT>(hour) + (charT)':';
    // Minutes
    if (current_time.tm_min < 10) {
        s += (charT)'0';
    }
    s += std::_Integral_to_string<charT>(current_time.tm_min) + (charT)':';
    // Seconds
    if (current_time.tm_sec < 10) {
        s += (charT)'0';
    }
    s += std::_Integral_to_string<charT>(current_time.tm_sec);
    if ((!_24_hour) && am_pm) {
        //s += (charT)' ';
        if (current_time.tm_hour < 12)
            s += (charT)'A';
        else
            s += (charT)'P';
        s += (charT)'M';
    }
    return s;
}


inline std::string get_time(bool _24_hour = true, bool am_pm = false)
{
    return get_time_basic_string<char>(_24_hour, am_pm);
}

inline std::wstring get_time_w(bool _24_hour = true, bool am_pm = false)
{
    return get_time_basic_string<wchar_t>(_24_hour, am_pm);
}

inline std::u16string get_time_u16(bool _24_hour = true, bool am_pm = false)
{
    return get_time_basic_string<char16_t>(_24_hour, am_pm);
}

inline std::u32string get_time_u32(bool _24_hour = true, bool am_pm = false)
{
    return get_time_basic_string<char32_t>(_24_hour, am_pm);
}


template <typename charT>
std::basic_string<charT> get_date_basic_string(util::DATE_FORMAT format = util::DATE_FORMAT::MMDDYYYY, charT separator = (charT)'-')
{
    std::basic_string<charT> s, day, month, year;
    // Get the current time and store it in a data structure for easier parsing
    time_t current_time_t = time(0);
    struct tm current_time;
    errno_t err = 0;
    if (err = localtime_s(&current_time, &current_time_t)) {
        sp::err::set(err);
        return s;
    }
    // Month
    if (current_time.tm_mon < 9) {
        month += (charT)'0';
    }
    month += std::_Integral_to_string<charT>(current_time.tm_mon + 1);
    // Day
    if (current_time.tm_mday < 10) {
        day += (charT)'0';
    }
    day += std::_Integral_to_string<charT>(current_time.tm_mday);
    // Year
    year += std::_Integral_to_string<charT>(current_time.tm_year + 1900);

    switch (format)
    {
        case util::DATE_FORMAT::MMDDYY:
            s += (month + separator + day + separator + year[2]) + year[3];
            break;
        case util::DATE_FORMAT::DDMMYY:
            s += (day + separator + month + separator + year[2]) + year[3];
            break;
        case util::DATE_FORMAT::MMDDYYYY:
            s += (month + separator + day + separator + year);
            break;
        case util::DATE_FORMAT::DDMMYYYY:
            s += (day + separator + month + separator + year);
            break;
        case util::DATE_FORMAT::YYMMDD:
            s += year[2];
            s += year[3];
            s += separator + month + separator + day;
            break;
        case util::DATE_FORMAT::YYYYMMDD:
        default:
            s += year + separator + month + separator + day;
            break;
    }
    return s;
}


inline std::string get_date(util::DATE_FORMAT format = util::DATE_FORMAT::MMDDYYYY, char separator = '-')
{
    return get_date_basic_string<char>(format, separator);
}

inline std::wstring get_date_w(util::DATE_FORMAT format = util::DATE_FORMAT::MMDDYYYY, wchar_t separator = L'-')
{
    return get_date_basic_string<wchar_t>(format, separator);
}

inline std::u16string get_date_u16(util::DATE_FORMAT format = util::DATE_FORMAT::MMDDYYYY, char16_t separator = u'-')
{
    return get_date_basic_string<char16_t>(format, separator);
}

inline std::u32string get_date_u32(util::DATE_FORMAT format = util::DATE_FORMAT::MMDDYYYY, char32_t separator = U'-')
{
    return get_date_basic_string<char32_t>(format, separator);
}


// Formats a string like sprintf
std::string format(const std::string format, ...);

// Formats a wide-character string like swprintf
std::wstring format_w(const std::wstring format, ...);

} // namespace str
__SP_NAMESPACE_CLOSE // namespace sp


#include "sp/string/string_overloads.hpp"


#endif // SP_UTILITY_STRING_MAIN_H_
