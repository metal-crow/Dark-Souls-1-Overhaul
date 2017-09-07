/*
	Author: Sean Pesce

	Multi-level pointer class
*/

#pragma once

#ifndef _SP_POINTER_CLASS_H_
	#define _SP_POINTER_CLASS_H_

#ifdef _WIN32
	#include <winerror.h>
#else
	#include <errno.h>
#endif // _WIN32

#include <vector>


class SpPointer
{
public:
	void *base = NULL; // Starting address of multi-level pointer
	std::vector<long> offsets; // Chain of offsets used to resolve the final address


	// Constructors/destructors
	SpPointer(void *starting_address = NULL, std::vector<long> new_offsets = {});
	~SpPointer();
	

	// Returns final resolved address (or NULL if address is inaccessible)
	void *resolve();

	// Writes a value to the resolved address
	template <typename T>
	uint32_t write(T value);

	// Copies memory from given source address to the resolved address
	template <typename T>
	uint32_t write_copy(T *source);

	// Reads a value from the resolved address to the given buffer
	template <typename T>
	uint32_t read(T *buffer);
};




/*
	Writes a value to the resolved address.

	@param value	Value to be written to the resolved address

	@return			SP_NO_ERROR (0) on success; if pointer resolution fails, SP_ERROR_INVALID_ADDRESS is returned instead.
*/
template <typename T>
uint32_t SpPointer::write(T value)
{
	void *resolved_address = resolve();
	if (resolved_address == NULL)
	#ifdef _WIN32
		return ERROR_INVALID_ADDRESS;
	#else
		return EFAULT;
	#endif // _WIN32

	*(T*)resolved_address = value;
	return 0;
}


/*
	Copies memory from given source address to the resolved address.

	@param source	Starting address of the source data

	@return			SP_NO_ERROR (0) on success; if pointer resolution fails, SP_ERROR_INVALID_ADDRESS is returned instead.
*/
template <typename T>
uint32_t SpPointer::write_copy(T *source)
{
	void *resolved_address = resolve();
	if (resolved_address == NULL)
		#ifdef _WIN32
			return ERROR_INVALID_ADDRESS;
		#else
			return EFAULT;
		#endif // _WIN32

	memcpy(resolved_address, source, sizeof(T));
	return 0;
}


/*
	Reads a value from the resolved address to the given buffer.

	@param buffer	Buffer to store the value that was read from the resolved address

	@return			SP_NO_ERROR (0) on success; if pointer resolution fails, SP_ERROR_INVALID_ADDRESS is returned instead.
*/
template <typename T>
uint32_t SpPointer::read(T *buffer)
{
	void *resolved_address = resolve();
	if (resolved_address == NULL)
		#ifdef _WIN32
			return ERROR_INVALID_ADDRESS;
		#else
			return EFAULT;
		#endif // _WIN32

	*buffer = *(T*)resolved_address;
	return 0;
}

#endif _SP_POINTER_CLASS_H_