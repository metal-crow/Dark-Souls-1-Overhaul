/*
    CONTRIBUTORS:
        Sean Pesce

*/

#pragma once

#ifndef SP_UTILITY_OS_WINDOWS_MAIN_H_
#define SP_UTILITY_OS_WINDOWS_MAIN_H_

#include "sp/sp.h"
#include "Winnt.h"

#ifdef _WIN32

// Windows memory protection settings
//   NOTE: Not all Windows protection settings are included. See Microsoft documentation for more info:
//   https://msdn.microsoft.com/en-us/library/windows/desktop/aa366786(v=vs.85).aspx
#define MEM_PROTECT_NONE PAGE_NOACCESS
#define MEM_PROTECT_R PAGE_READONLY
#define MEM_PROTECT_W PAGE_WRITECOPY
#define MEM_PROTECT_X PAGE_EXECUTE
#define MEM_PROTECT_RW PAGE_READWRITE
#define MEM_PROTECT_RX PAGE_EXECUTE_READ
#define MEM_PROTECT_WX PAGE_EXECUTE_WRITECOPY
#define MEM_PROTECT_RWX PAGE_EXECUTE_READWRITE


__SP_NAMESPACE
namespace os  {
namespace win {


typedef BOOL(WINAPI *is_wow64_proc_t) (HANDLE, PBOOL);  // For compatibility with non-supporting Windows systems when calling IsWow64Process()


///////////// Function prototypes /////////////


/* is_wow64_process()
 *
 * A wrapper function for the Windows API function IsWow64Process(). This wrapper function
 *    calls IsWow64Process() using GetProcAddress() to provide compatibility on a wider range
 *    of systems. This method is described in the Windows API documentation for IsWow64Process():
 *
 *        "IsWow64Process is not available on all supported versions of Windows.
 *         Use GetModuleHandle to get a handle to the DLL that contains the function
 *         and GetProcAddress to get a pointer to the function if available."
 *
 *  NOTE: This function is Windows-only, due to the way 32-bit processes are handled on 64-bit
 *        versions of Windows.
 *
 *    If GetProcAddress returns NULL, we know that the current system does not support IsWow64Process().
 */
BOOL is_wow64_process();




} // namespace win
} // namespace os
__SP_NAMESPACE_CLOSE // namespace sp


#endif // _WIN32

#endif // SP_UTILITY_OS_WINDOWS_MAIN_H_

