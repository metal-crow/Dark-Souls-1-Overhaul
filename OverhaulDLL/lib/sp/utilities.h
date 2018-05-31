/*
    CONTRIBUTORS:
        Sean Pesce

*/

#pragma once

#ifndef SP_UTILITY_UTIL_MAIN_H_
#define SP_UTILITY_UTIL_MAIN_H_

#include "sp/sp.h"
#include <sstream>

__SP_NAMESPACE
namespace util {


enum DATE_FORMAT {
    MMDDYY,
    DDMMYY,
    MMDDYYYY,
    DDMMYYYY,
    YYMMDD,
    YYYYMMDD
};


} // namespace util
__SP_NAMESPACE_CLOSE // namespace sp

#endif // SP_UTILITY_UTIL_MAIN_H_
