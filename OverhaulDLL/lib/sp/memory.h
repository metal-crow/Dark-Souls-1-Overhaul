/*
    CONTRIBUTORS:
        Sean Pesce

*/

#pragma once

#ifndef SP_UTILITY_MEMORY_MAIN_H_
#define SP_UTILITY_MEMORY_MAIN_H_

#include "sp/sp.h"
#include "sp/os.h"
#include <cstring>

#define zero_mem(address, size) memset(address, 0, size)


__SP_NAMESPACE
namespace mem {


/* get_process_base()
 *
 * Obtains the base address of the current process.
 */
#ifdef _WIN32
inline void* get_process_base()
{
    return GetModuleHandle(NULL);
}
#else // UNIX
void* get_process_base();
#endif


/* set_protection(void *, size_t, uint32_t, uint32_t *)
 *
 *  Cross-platform wrapper function for changing memory protection settings.
 *      On Windows, VirtualProtect is called. On Unix, mprotect is called.
 *  @param address  The location in memory where the memory protection will be changed.
 *  @param size The size of the section of memory whose protection will be changed.
 *              In Windows, the size is measured in bytes.
 *              In Unix, the size is measured in pages.
 *  @param new_protect    The new memory protection setting for the section of memory.
 *  @param old_protect    Saves the original memory protection settings on Windows.
 *                          NOTE: The old_protect parameter is not used in Unix.
 *  @return 0 if successful; non-zero value on failure.
 */
inline int set_protection(void *address, size_t size, uint32_t new_protect, uint32_t *old_protect)
{
#ifdef _WIN32
    // Windows (use VirtualProtect)
    if (old_protect == NULL)
    {
        uint32_t tmp_protect; // If the user passes NULL for old_protect, use &tmp_protect (otherwise VirtualProtect fails)
        return !VirtualProtect(address, size, (DWORD)new_protect, (DWORD*)&tmp_protect);
    } // else
    return !VirtualProtect(address, size, (DWORD)new_protect, (DWORD*)old_protect);
#else
    // Unix (use mprotect)
    if (old_protect != NULL)
    {
        *old_protect = get_protection(address); // Create backup of old memory permissions
    }
    return mprotect(get_page_base(address), size, (int)new_protect); // getMemPage is called to obtain a page-aligned address
#endif // _WIN32
}


/* set_protection(void *, size_t, uint32_t)
 *
 *    Overloaded version of set_protection(void *, size_t, uint32_t, uint32_t *). This is the same
 *        as calling the original function with a NULL old_protect argument.
 *  @params See @set_protection(void *, size_t, uint32_t, uint32_t *) documentation.
 *  @return 0 if successful; non-zero value on failure.
 */
inline int set_protection(void *address, size_t size, uint32_t new_protect)
{
#ifdef _WIN32
    uint32_t tmp_protect; // Must use &tmp_protect, otherwise VirtualProtect fails
    return !VirtualProtect(address, size, (DWORD)new_protect, (DWORD*)&tmp_protect);
#else
    // Unix (use mprotect)
    return mprotect(get_page_base(address), size, (int)new_protect); // getMemPage is called to obtain a page-aligned address
#endif // _WIN32
}


/* virtual_query(void *, MEMORY_BASIC_INFORMATION *, size_t)
 *
 * A portable function for querying properties of memory regions.
 *  On Windows, this is merely a wrapper function for VirtualQuery (no extra
 *      functionality).
 *  On Unix, this function aims to emulate VirtualQuery by parsing /proc/self/maps.
 *
 *  @param address  An address that resides within the first memory region to be
 *                  parsed.
 *  @param buff     The buffer for storing memory information for each region.
 *  @param length   The size of the buffer (in bytes). Optimally this should be
 *                  a multiple of sizeof(MEMORY_BASIC_INFORMATION).
 */
#ifdef _WIN32
inline size_t virtual_query(void *address, MEMORY_BASIC_INFORMATION *buff, size_t length)
{
    return VirtualQuery((LPCVOID)address, buff, length);
}
#else // UNIX
size_t virtual_query(void *address, MEMORY_BASIC_INFORMATION *buff, size_t length);
#endif // _WIN32


/* get_protection(void *)
 *
 * Obtains the current memory protection permissions at a given address in memory (for this process).
 *  
 *  @param address The address for which this function obtains access the current access permissions
 */
inline uint32_t get_protection(void *address)
{
    MEMORY_BASIC_INFORMATION mem_info;
    if (sp::mem::virtual_query(address, &mem_info, sizeof(mem_info)) < sizeof(mem_info))
    {
        // virtual_query failed
        return -1;
    }
    return (uint32_t)mem_info.Protect;
}


/* parse_map_region(const char *, MEMORY_BASIC_INFORMATION *)
 * Parses a line of text from /proc/self/maps which describes a region of memory
 *  and the properties that apply to it (address range, access protection, etc).
 *  NOTE: This function is for Unix systems only; the mem_info struct aims to
 *        emulate the MEMORY_BASIC_INFORMATION struct returned by VirtualQuery on
 *        Windows operating systems.
 *
 *  @param maps_entry The line of text from /proc/self/maps file.
 *  @param mem_info The memory information struct where the region's properties will
 *         be stored.
 */
 #ifndef _WIN32
void parse_map_region(const char *maps_entry, MEMORY_BASIC_INFORMATION *mem_info);
#endif // !_WIN32


/* next_region(MEMORY_BASIC_INFORMATION *, MEMORY_BASIC_INFORMATION *)
 *
 * Obtains the memory information for the first memory region following the region specified
 *  by the "current" argument. If "current" is NULL, this function obtains the first
 *  memory region for the process.
 *
 *  @param current  The current region of memory.
 *  @param next     Buffer to hold the MEMORY_BASIC_INFORMATION struct for the next region.
 *
 *  @return The base address of the next region, or NULL if the function fails.
 */
void* next_region(MEMORY_BASIC_INFORMATION *current, MEMORY_BASIC_INFORMATION *next);


/* next_region(void *)
 *
 * Obtains the base address for the first region in memory whose base address is higher than
 *  the given address.
 *
 *  @param current  An address that exists within the "current" region in memory.
 */
void* next_region(void *current);


/* get_page_size()
 *
 * Obtains the system page size.
 */
inline size_t get_page_size()
{
#ifdef _WIN32 // Windows:
    SYSTEM_INFO sysInfo;
    if (sp::os::win::is_wow64_process())
    {
        GetNativeSystemInfo(&sysInfo);
    }
    else
    {
        GetSystemInfo(&sysInfo);
    }

    return (size_t)sysInfo.dwPageSize;
#else // Unix:
    return (size_t)sysconf(_SC_PAGESIZE);
#endif // _WIN32
}


/* get_page_base(void *)
 *
 * Obtains the base address of a process page that contains the given memory address.
 *
 * @param address    The memory address that resides within the page.
 */
inline void* get_page_base(void *address)
{
    if (sp::os::is_32bit())
    { // 32-bit process
      // Calculate and return the base address of the page:
        return (void*)((uint32_t)(uint64_t)address - ((uint32_t)(uint64_t)address % sp::mem::get_page_size())); // Double cast to subdue compiler warnings
    }
    else
    { // 64-bit process
      // Calculate and return the base address of the page:
        return (void*)((uint64_t)address - ((uint64_t)address % sp::mem::get_page_size()));
    }
}


// Apply byte-array patch
inline bool patch_bytes(void* write_to, const uint8_t* patch, size_t size)
{
    if (write_to == NULL || patch == NULL)
    {
        sp::err::set(SP_ERR_INVALID_ADDRESS);
        return false;
    }
    sp::mem::set_protection(write_to, size, MEM_PROTECT_RWX);
    memcpy_s(write_to, size, patch, size);
    return true;
}


} // namespace mem
__SP_NAMESPACE_CLOSE // namespace sp

#endif // SP_UTILITY_MEMORY_MAIN_H_
