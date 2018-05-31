/*
    CONTRIBUTORS:
        Sean Pesce

*/

#pragma once

#ifndef SP_UTILITY_OS_UNIX_MAIN_H_
#define SP_UTILITY_OS_UNIX_MAIN_H_

#include "sp/sp.h"

#ifndef _WIN32


/* Linux memory protection settings:
 *
 * Extra info:
 *  "-The Execute access right always implies the Read access right.
 *   -The Write access right always implies the Read access right."
 *      Source: Understanding the Linux Kernel (3rd Edition) by Daniel Bovet; Section 9.3.2 - Memory Region Access Rights
 *
 */
#define MEM_PROTECT_NONE PROT_NONE
#define MEM_PROTECT_R PROT_READ        // PROT_READ == VM_READ
#define MEM_PROTECT_W PROT_WRITE    // PROT_WRITE == VM_WRITE
#define MEM_PROTECT_X PROT_EXEC        // PROT_EXEC == VM_EXEC
#define MEM_PROTECT_RW (PROT_READ|PROT_WRITE)
#define MEM_PROTECT_RX (PROT_READ|PROT_EXEC)
#define MEM_PROTECT_WX (PROT_WRITE|PROT_EXEC)
#define MEM_PROTECT_RWX (PROT_READ|PROT_WRITE|PROT_EXEC)
/* MEM_IMAGE
 * MS documentation:
 *  "Indicates that the memory pages within the region are mapped into the view of an image section."
 * Non-MS documentation:
 *  "The page is used to store the module(EXE/DLL). It may share the physical storage with other
 *   process because shared DLL is loaded to memory only once and shared by multiple processes."
 *   Source: https://codesequoia.wordpress.com/2008/11/28/understand-process-address-space-usage/
 */
#define MEM_IMAGE 0x1000000
#define MEM_MAPPED 0x40000      // MS documentation: "Indicates that the memory pages within the region are mapped into the view of a section."
#define MEM_PRIVATE 0x20000     // MS documentation: "Indicates that the memory pages within the region are private (that is, not shared by other processes)."


 // Formatter string for resolving the memory map file of a process:
#define MEM_MAP_FILE_DIR_FORMAT "/proc/%d/maps"    // The %d will be filled in with the process ID
 // Formatter string for resolving executable file simlink of a process:
#define PROC_EXE_FILE_LINK_FORMAT "/proc/%d/exe" // The %d will be filled in with the process ID
 // Memory info struct to emulate the kind used in the Windows API, seen here:
 //  https://msdn.microsoft.com/en-us/library/windows/desktop/aa366775(v=vs.85).aspx
typedef struct SP_MEMORY_BASIC_INFORMATION {
    void*       BaseAddress;
    void*       AllocationBase;
    void*       AllocationProtect; // Unused (for now)
    size_t      RegionSize;
    uint32_t    State; // Unused (for now)
    uint32_t    Protect;
    uint32_t    Type;
} MEMORY_BASIC_INFORMATION;



__SP_NAMESPACE
namespace os   {
namespace unix {





} // namespace unix
} // namespace os
__SP_NAMESPACE_CLOSE // namespace sp


#endif // !_WIN32

#endif // SP_UTILITY_OS_UNIX_MAIN_H_

