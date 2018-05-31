/*
    CONTRIBUTORS:
        Sean Pesce

*/

#pragma once

#ifndef SP_UTILS_DYNAMIC_LINK_LIBRARY_H_
#define SP_UTILS_DYNAMIC_LINK_LIBRARY_H_

#include "sp/sp.h"
#include "sp/environment.h"

#include <string>

#ifndef SP_MAX_LOAD_LIBS_
#define SP_MAX_LOAD_LIBS_ 100
#endif // SP_MAX_LOAD_LIBS_


__SP_NAMESPACE
namespace lib {


// If given a relative file path, loads a DLL from the default system directory
inline void* load_shared(const std::string& filename)
{
    return LoadLibrary((sp::env::system_dir() + "\\" + filename).c_str());
}


size_t load_user_specified();


} // namespace lib
__SP_NAMESPACE_CLOSE // namespace sp

#endif // SP_UTILS_DYNAMIC_LINK_LIBRARY_H_
