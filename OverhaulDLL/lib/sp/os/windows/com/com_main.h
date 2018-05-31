/*
    CONTRIBUTORS:
        Sean Pesce

*/
#ifdef _WIN32
#pragma once

#ifndef SP_WINDOWS_COM_MAIN_H_
#define SP_WINDOWS_COM_MAIN_H_

#include "sp/sp.h"
#include <string>


__SP_NAMESPACE
namespace os  {
namespace win {
namespace com {


namespace guid {
std::string to_string(const GUID &guid);
} // namespace guid


} // namespace com
} // namespace win
} // namespace os
__SP_NAMESPACE_CLOSE // namespace sp


#endif // SP_WINDOWS_COM_MAIN_H_


#endif // _WIN32
