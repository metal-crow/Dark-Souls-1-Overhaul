// Author: Sean Pesce

#pragma once

#ifndef SP_SYSUTILS_HPP
	#define SP_SYSUTILS_HPP


#include <cstdint> // uint32_t
#include <cstring> // size_t, strcmp(), memcpy()
#include <vector> // std::vector

#ifdef _WIN32
	#include <Windows.h> // GetSystemInfo(), GetNativeSystemInfo(), GetProcAddress(), GetCurrentProcess(), IsWow64Process(), VirtualProtect(), SetLastError()
#else
	#include <stdio.h>      // sprintf(), sscanf()
	#include <unistd.h>     // sysconf(_SC_PAGESIZE), getpid()
	#include <sys/mman.h>   // mprotect()
    #include <fstream>      // ifstream
    #include <errno.h>      // errno, error constants
#endif // _WIN32


// Portable memory protection/info setting constants:
#ifdef _WIN32
    // Windows memory protection settings:
	// NOTE: Not all Windows protection settings are included. See Microsoft documentation for more info:
    // https://msdn.microsoft.com/en-us/library/windows/desktop/aa366786(v=vs.85).aspx
    #define MEM_PROTECT_NONE PAGE_NOACCESS
    #define MEM_PROTECT_R PAGE_READONLY
    #define MEM_PROTECT_W PAGE_WRITECOPY
    #define MEM_PROTECT_X PAGE_EXECUTE
    #define MEM_PROTECT_RW PAGE_READWRITE
    #define MEM_PROTECT_RX PAGE_EXECUTE_READ
    #define MEM_PROTECT_WX PAGE_EXECUTE_WRITECOPY
    #define MEM_PROTECT_RWX PAGE_EXECUTE_READWRITE
#else
    /* Linux memory protection settings:
     *
     * Extra info:
     *  "-The Execute access right always implies the Read access right.
     *   -The Write access right always implies the Read access right."
     *      Source: Understanding the Linux Kernel (3rd Edition) by Daniel Bovet; Section 9.3.2 - Memory Region Access Rights
     *
     */
	#define MEM_PROTECT_NONE PROT_NONE
    #define MEM_PROTECT_R PROT_READ		// PROT_READ == VM_READ
    #define MEM_PROTECT_W PROT_WRITE	// PROT_WRITE == VM_WRITE
    #define MEM_PROTECT_X PROT_EXEC		// PROT_EXEC == VM_EXEC
    #define MEM_PROTECT_RW (PROT_READ|PROT_WRITE)
    #define MEM_PROTECT_RX (PROT_READ|PROT_EXEC)
    #define MEM_PROTECT_WX (PROT_WRITE|PROT_EXEC)
    #define MEM_PROTECT_RWX (PROT_READ|PROT_WRITE|PROT_EXEC)
    /*MEM_IMAGE
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
#endif // _WIN32


// Portable error constants:
// @TODO: Populate this list using these resources:
//          Linux:      http://www.virtsync.com/c-error-codes-include-errno
//          Windows:    https://msdn.microsoft.com/en-us/library/windows/desktop/ms681381(v=vs.85).aspx
#ifdef _WIN32
    #define SP_NO_ERROR ERROR_SUCCESS
    #define SP_ERROR_INVALID_PARAMETER ERROR_INVALID_PARAMETER
    #define SP_ERROR_FILE_NOT_FOUND ERROR_FILE_NOT_FOUND
    #define SP_ERROR_PATH_NOT_FOUND ERROR_PATH_NOT_FOUND
    #define SP_ERROR_ACCESS_DENIED ERROR_ACCESS_DENIED
    #define SP_ERROR_INVALID_ADDRESS ERROR_INVALID_ADDRESS
    #define SP_ERROR_INSUFFICIENT_BUFFER ERROR_INSUFFICIENT_BUFFER
#else
    #define SP_NO_ERROR 0
    #define SP_ERROR_INVALID_PARAMETER EINVAL
    #define SP_ERROR_FILE_NOT_FOUND ENOENT
    #define SP_ERROR_PATH_NOT_FOUND ENOENT
    #define SP_ERROR_ACCESS_DENIED EACCES
    #define SP_ERROR_INVALID_ADDRESS EFAULT
    #define SP_ERROR_INSUFFICIENT_BUFFER ENOBUFS
#endif // _WIN32



//Other constants & typedefs:
#ifdef _WIN32
    typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);  // For compatibility with non-supporting Windows systems when calling IsWow64Process()
#else
    // Formatter string for resolving the memory map file of a process:
    #define MEM_MAP_FILE_DIR_FORMAT "/proc/%d/maps"	// The %d will be filled in with the process ID
    // Formatter string for resolving executable file simlink of a process:
    #define PROC_EXE_FILE_LINK_FORMAT "/proc/%d/exe" // The %d will be filled in with the process ID
    // Memory info struct to emulate the kind used in the Windows API, seen here:
    //  https://msdn.microsoft.com/en-us/library/windows/desktop/aa366775(v=vs.85).aspx
    typedef struct SP_MEMORY_BASIC_INFORMATION {
        void*       BaseAddress;
        void*       AllocationBase;
        void*       AllocationProtect; // Unused for now
        size_t      RegionSize;
        uint32_t    State; // Unused for now
        uint32_t    Protect;
        uint32_t    Type;
    } MEMORY_BASIC_INFORMATION;
#endif // _WIN32




///////////// Classes /////////////


/*
	Multi-level pointer class
*/
class SpPointer {

public:
	void *base = NULL; // Starting address of multi-level pointer
	std::vector<long> offsets; // Chain of offsets used to resolve the final address


	// Constructors/destructor
	SpPointer()
	{
		base = NULL;
	}
	SpPointer(void *starting_address, std::vector<long> new_offsets)
	{
		base = starting_address;
		for (auto offset : new_offsets)
		{
			offsets.push_back(offset);
		}
	}
	SpPointer(void *starting_address)
	{
		base = starting_address;
	}

	~SpPointer(){};


	// Returns final resolved address
	void *SpPointer::resolve()
	{
		void *address = base;
		for (auto offset : offsets)
		{
			address = (void*)((*(uint64_t*)address) + offset);
		}
		return address;
	}

	// Writes a value to the resolved address
	template <typename T>
	void SpPointer::write(T value)
	{
		*(T*)resolve() = value;
	}
	
	// Copies memory from given source address to the resolved address
	template <typename T>
	void SpPointer::write_copy(T *source)
	{
		memcpy(resolve(), source, sizeof(T));
	}

	// Reads a value from the resolved address to the given buffer
	template <typename T>
	void SpPointer::read(T *buffer)
	{
		*buffer = *(T*)resolve();
	}
};




///////////// Function prototypes /////////////


/*is_wow64_process()
 * A wrapper function for the Windows API function IsWow64Process(). This wrapper function
 *	calls IsWow64Process() using GetProcAddress() to provide compatibility on a wider range
 *	of systems. This method is described in the Windows API documentation for IsWow64Process():
 *
 *		"IsWow64Process is not available on all supported versions of Windows.
 *		 Use GetModuleHandle to get a handle to the DLL that contains the function
 *		 and GetProcAddress to get a pointer to the function if available."
 *
 *  NOTE: This function is Windows-only, due to the way 32-bit processes are handled on 64-bit
 *        versions of Windows.
 *
 *	If GetProcAddress returns NULL, we know that the current system does not support IsWow64Process().
 */
#ifdef _WIN32
BOOL is_wow64_process();
#endif // _WIN32


/*get_process_base
 * Obtains the base address of the current process.
 */
void *get_process_base();


/*is_32bit()
 * Determines whether the current process is running in 32-bit mode or 64-bit mode.
 *
 *  @return true if the current process is running in 32-bit mode; false otherwise.
 */
bool is_32bit();


/*is_64bit()
 * Determines whether the current process is running in 32-bit mode or 64-bit mode.
 *
 *  @return true if the current process is running in 64-bit mode; false otherwise.
 */
bool is_64bit();


/*set_error(uint32_t)
 * Sets the system error number.
 */
void set_error(uint32_t newError);


/*get_error()
 * Gets the current system error number.
 *
 *  @return The current system error number.
 */
uint32_t get_error();



/* set_mem_protection(void *, size_t, uint32_t, uint32_t *)
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
int set_mem_protection(void *address, size_t size, uint32_t new_protect, uint32_t *old_protect);


/* set_mem_protection(void *, size_t, uint32_t)
 *	Overloaded version of set_mem_protection(void *, size_t, uint32_t, uint32_t *). This is the same
 *		as calling the original function with a NULL old_protect argument.
 *  @params See @set_mem_protection(void *, size_t, uint32_t, uint32_t *) documentation.
 *  @return 0 if successful; non-zero value on failure.
 */
int set_mem_protection(void *address, size_t size, uint32_t new_protect);


/*virtual_query(void *, MEMORY_BASIC_INFORMATION *, size_t)
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
size_t virtual_query(void *address, MEMORY_BASIC_INFORMATION *buff, size_t length);

/*get_mem_protection(void *)
 * Obtains the current memory protection permissions at a given address in memory (for this process).
 *  
 *  @param address The address for which this function obtains access the current access permissions
 */
uint32_t get_mem_protection(void *address);


/*parse_mem_map_region(const char *, MEMORY_BASIC_INFORMATION *)
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
void parse_mem_map_region(const char *maps_entry, MEMORY_BASIC_INFORMATION *mem_info);
#endif // !_WIN32


/*next_mem_region(MEMORY_BASIC_INFORMATION *, MEMORY_BASIC_INFORMATION *)
 * Obtains the memory information for the first memory region following the region specified
 *  by the "current" argument. If "current" is NULL, this function obtains the first
 *  memory region for the process.
 *
 *  @param current  The current region of memory.
 *  @param next     Buffer to hold the MEMORY_BASIC_INFORMATION struct for the next region.
 *
 *  @return The base address of the next region, or NULL if the function fails.
 */
void *next_mem_region(MEMORY_BASIC_INFORMATION *current, MEMORY_BASIC_INFORMATION *next);


/*next_mem_region(void *)
 * Obtains the base address for the first region in memory whose base address is higher than
 *  the given address.
 *
 *  @param current  An address that exists within the "current" region in memory.
 */
void *next_mem_region(void *current);


/*get_page_base(void *)
 * Obtains the base address of a process page that contains the given memory adress.
 *
 * @param address	The memory address that resides within the page.
 */
void *get_page_base(void *address);


/*get_page_size()
 * Obtains the system page size.
 */
size_t get_page_size();


#endif // SP_SYSUTILS_HPP
