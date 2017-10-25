/*
	DARK SOULS OVERHAUL

	Contributors to this file:
		Sean Pesce	-	C++


	ParamDef/GameParamDef.h

	Generic base ParamDef class; all other ParamDef classes inherit
	basic member data and functions from this class.

*/

#pragma once

#ifndef _DS1_OVERHAUL_GENERIC_BASE_PARAM_DEF_FILE_H_
	#define _DS1_OVERHAUL_GENERIC_BASE_PARAM_DEF_FILE_H_


#include "PluginImports.h"
#include "SP_AoB_Scan.hpp"

// Macro for testing paramdef objects
#define _test_param_def_(pd,p,attribute) {print_console(#p);int i = 0; p *param; for(i = 0; i < 5; i++){param = pd.get(i);print_console(std::to_string(i) + ":     "#attribute"=" + std::to_string(param->attribute));}i = pd.param_count-1;param=pd.get(i);print_console(std::to_string(i) + ":     "#attribute"=" + std::to_string(param->attribute) + "\n");}

	
// Ancestor struct to all Dark Souls game parameter data structure types
typedef struct Parameter {} Param;


// Generic Dark Souls parameter definitions file class
class GameParamDef {
public:

	// Starting address of the parameters file
	void *base;

	// Number of parameter structures in the data array
	size_t param_count;

	// Offset (relative to base address) to the start of the param structure array
	const int32_t data_start_offset;
	
	// Size of a single parameter structure from the data array
	const size_t param_size;
	
	// Unique array of bytes that can be used to find the param file
	std::string scan_pattern; // @TODO: Find a more efficient technique for obtaining base addresses

	// Title of the param def file (used when printing feedback to overlay)
	std::string title;

	// File name of the param def file (without the .paramdef file extension)
	std::string file;
	
	// File extension for parameter definition files
	static const char *paramdef_extension;

	// File extension for parameter files
	static const char *param_extension;

protected:
	GameParamDef(void *base_init = NULL, int32_t data_start_offset_init = 0, size_t param_count_init = 0, size_t param_size_init = sizeof(Param), const char *scan_pattern_init = "", const char *file_init = "", const char *title_init = "")
		: base(base_init), data_start_offset(data_start_offset_init), param_count(param_count_init), param_size(param_size_init), scan_pattern(scan_pattern_init), file(file_init), title(title_init)
	{
	}

	// Initializes file base pointer (start of the file, not the data array). Returns base address on success, and NULL otherwise
	void *init(void **start, const char *aob, bool print_result = false, bool re_init = false)
	{
		if (start == NULL || aob == NULL)
		{
			SetLastError(ERROR_INVALID_PARAMETER);
			return NULL;
		}

		if ((*start) != NULL && !re_init)
			return (*start);

		(*start) = aob_scan(aob);

		if ((*start) != NULL && print_result)
		{
			std::stringstream hex_stream;
			hex_stream << std::hex << (int)(*start);
			print_console(std::string("    Found param file: " + this->file + this->param_extension + " (Location: 0x").append(hex_stream.str()).append(")"));
		}
		else if (print_result)
		{
			print_console(std::string("    ERROR: Failed to locate parameter data file (").append(this->file + this->param_extension) + ")");
		}

		return (*start);
	}

	static Param *data(void *start, uint32_t offset)
	{
		return (Param*)(start == NULL ? NULL : (((int8_t*)start) + offset));
	}

	static Param *get(int index, size_t element_size, void *data_start)
	{
		return (Param*)(((int8_t*)data_start) + (element_size * index));
	}

	// Returns pointer to element at start of data array
	Param *data()
	{
		return GameParamDef::data(base, data_start_offset);
	}


	// Returns param struct at the specified index in the data array
	Param *get(int index)
	{
		return GameParamDef::get(index, param_size, GameParamDef::data(base, data_start_offset));
	}

public:
	// Initializes file base pointer (start of the file, not the data array). Returns base address on success, and NULL otherwise
	void *init(bool print_result = false, bool re_init = false)
	{
		return GameParamDef::init(&base, scan_pattern.c_str(), print_result, re_init);
	}
};


#endif // _DS1_OVERHAUL_GENERIC_BASE_PARAM_DEF_FILE_H_