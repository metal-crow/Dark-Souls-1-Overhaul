/*
	Author: Sean Pesce

	Multi-level pointer class
*/

#pragma once

#ifndef _SP_POINTER_CLASS_H_
	#define _SP_POINTER_CLASS_H_

#include <vector>


class SpPointer
{
public:
	void *base = NULL; // Starting address of multi-level pointer
	std::vector<long> offsets; // Chain of offsets used to resolve the final address


	// Constructors/destructors
	SpPointer();
	SpPointer(void *starting_address, std::vector<long> new_offsets);
	SpPointer(void *starting_address);
	~SpPointer();
	

	// Returns final resolved address
	void *resolve();

	// Writes a value to the resolved address
	template <typename T>
	void write(T value);

	// Copies memory from given source address to the resolved address
	template <typename T>
	void write_copy(T *source);

	// Reads a value from the resolved address to the given buffer
	template <typename T>
	void read(T *buffer);
};




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

#endif _SP_POINTER_CLASS_H_