/*
    CONTRIBUTORS:
        Sean Pesce

*/

#pragma once

#ifndef SP_UTILITY_OPERATING_SYSTEM_MAIN_H_
#define SP_UTILITY_OPERATING_SYSTEM_MAIN_H_

#include "sp/sp.h"
#include "sp/os/win.h"
#include "sp/os/unix.h"


__SP_NAMESPACE
namespace os {


/* is_32bit()
 *
 * Determines whether the current process is running in 32-bit mode or 64-bit mode.
 *
 *  @return true if the current process is running in 32-bit mode; false otherwise.
 */
inline bool is_32bit()
{
    return (sizeof(void *) == sizeof(uint32_t));
}


/* is_64bit()
 *
 * Determines whether the current process is running in 32-bit mode or 64-bit mode.
 *
 *  @return true if the current process is running in 64-bit mode; false otherwise.
 */
inline bool is_64bit()
{
    return (sizeof(void *) == sizeof(uint64_t));
}


} // namespace os
__SP_NAMESPACE_CLOSE // namespace sp

#endif // SP_UTILITY_OPERATING_SYSTEM_MAIN_H_
