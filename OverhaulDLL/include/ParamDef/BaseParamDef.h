/*
	DARK SOULS OVERHAUL

	Contributors to this file:
	Sean Pesce	-	C++


	ParamDef/BaseParamDef.h

	Generic base ParamDef class; all other ParamDef classes inherit
	basic member data and functions from this class.

*/

#pragma once

#ifndef _DS1_OVERHAUL_GENERIC_BASE_PARAM_DEF_FILE_H_
	#define _DS1_OVERHAUL_GENERIC_BASE_PARAM_DEF_FILE_H_


#include <string>



				/////////////////////////////////////////////////////
				///////////////// STRUCTS & CLASSES /////////////////
				/////////////////////////////////////////////////////

	
// Ancestor struct to all Dark Souls game parameter data structure types
struct Parameter {};


// Generic Dark Souls parameter definitions file
class BaseParamDef {
public:


	// Starting address of the parameters file
	void *base = NULL;

	// Number of parameter structures in the data array
	size_t param_count = 0;

	// Offset (relative to base address) to the start of the param structure array
	const int32_t data_start_offset = 0;
	
	// Size of a single parameter structure from the data array
	const size_t param_size = 0;
	
	// Unique array of bytes that can be used to find the param file
	const char *scan_pattern = ""; // @TODO: Find a more efficient technique for obtaining base addresses
	


	// Initializes file base pointer (start of the file, not the data array). Returns base address on success, and NULL otherwise
	void *BaseParamDef::init(bool print_result = false, bool re_init = false);
	/*{
		if (base != NULL && !re_init)
			return base;

		if (print_result)
			print_console(std::string("Searching for param defs file...").c_str());

		base = aob_scan(scan_pattern);

		if (base != NULL && print_result)
		{
			std::stringstream hex_stream;
			hex_stream << std::hex << (int)base;
			print_console(std::string("    Located param defs file (start: 0x").append(hex_stream.str()).append(")").c_str());
		}
		else if (print_result)
		{
			print_console(std::string("    Failed to locate parameter data.").c_str());
		}

		return base;
	}*/


	// Returns pointer to element at start of data array
	Parameter *BaseParamDef::data();
	/*{ 
		return (Parameter*)(base == NULL ? NULL : (((int8_t*)base) + data_start_offset));
	}*/


	// Returns param struct at the specified index in the data array
	Parameter *BaseParamDef::get(int index);
	/*{
		if (index < 0 || (size_t)index >= param_count || data() == NULL)
			return NULL;
		return (Parameter*)(((int8_t*)data()) + (param_size * index));
	}*/
};



#endif // _DS1_OVERHAUL_GENERIC_BASE_PARAM_DEF_FILE_H_