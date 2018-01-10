/*
    DARK SOULS GAME FILE C++ LIBRARY

    Contributors to this file:
        Sean Pesce	-	C++


    Param/Param.h

    Generic base ParamDef class; all other ParamDef classes inherit
    basic member data and functions from this class.

    NOTE: Not to be confused with Param/Params.h, which defines the
    namespace for referencing derived Param classes in a nicely
    formatted fashion.

*/

#pragma once

#ifndef _DS1_FILE_LIB_GENERIC_BASE_PARAM_DEF_FILE_H_
    #define _DS1_FILE_LIB_GENERIC_BASE_PARAM_DEF_FILE_H_


#include "PluginImports.h"
#include "SP_AoB_Scan.hpp"
#include <map>
#include "Archive/Bhd5.h"

// Macro for testing paramdef objects
#define _test_param_def_(pd,p,attribute) {print_console(#p);int i = 0; p *param; for(i = 0; i < 5; i++){param = pd.get(i);print_console(std::to_string(i) + ":     "#attribute"=" + std::to_string(param->attribute));}i = pd.param_count-1;param=pd.get(i);print_console(std::to_string(i) + ":     "#attribute"=" + std::to_string(param->attribute) + "\n");}


// Struct that maps parameter structs to their IDs (array of ParamBasicInfo structs found at 0x30 in .param files)
typedef struct ParameterBasicInfo {
    uint32_t id;
    uint32_t param_offset;
    uint32_t jp_name_offset; // Offset (from start of .param file) of the parameter name (in Japanese, Shift-JIS encoding)
} ParamBasicInfo;


// Ancestor struct to all Dark Souls game parameter data structure types
typedef struct Parameter {
    // File extension for parameter data files
    static const char    *FILE_EXT;   //  ".param" (wide char)
    static const wchar_t *FILE_EXT_W; // L".param" (char)
} Param;


// Generic Dark Souls parameter definitions file class
class ParamDef {
public:

    // Starting address of the parameters file
    void *base;

    // Number of parameter structures in the data array
    size_t param_count;

    // Offset (relative to base address) to the start of the param structure array
    int32_t data_start_offset;

    // Size of a single parameter structure from the data array
    const size_t param_size;

    // Maps param struct indices to their unique IDs
    std::map<uint32_t, int> id_to_index;

    // Unique array of bytes that can be used to find the param file
    std::string scan_pattern; // @TODO: Find a more efficient technique for obtaining base addresses

    // (Unofficial) title of the param def file (used when printing feedback to overlay)
    std::string title;

    // File name of the param def file (without the .paramdef file extension)
    std::string file;

    // Title string from the .param file header
    std::string header_title;

    // Name string for GameParam.parambnd
    static const char *game_param_bnd;

    // Name string for GameParam.parambnd.dcx (used for comparing hashes)
    static const char *game_param_dcx;

    // File extension for parameter definition files
    static const char    *FILE_EXT;   //  ".paramdef" (wide char)
    static const wchar_t *FILE_EXT_W; // L".paramdef" (char)

    // Record struct for GameParam.param.dcx in dvdbnd1.bhd5
    static Bhd5Record game_param_dcx_record;

protected:
    ParamDef(void *base_init = NULL,
             /*int32_t data_start_offset_init = 0, size_t param_count_init = 0,*/
             size_t param_size_init = sizeof(Param),
             const char *scan_pattern_init = "",
             const char *file_init = "",
             const char *title_init = "")
        : base(base_init),
        /*data_start_offset(data_start_offset_init), param_count(param_count_init),*/
        param_size(param_size_init),
        scan_pattern(scan_pattern_init),
        file(file_init),
        title(title_init)
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
            // Get number of parameters and file header title:
            param_count = *(uint16_t*)(((uint8_t*)(*start)) + 0xA);
            header_title = (char*)(((uint8_t*)(*start)) + 0xC);

            // Save start of param structure array
            ParamBasicInfo *info = (ParamBasicInfo*)(((uint8_t*)(*start)) + 0x30);
            data_start_offset = info[0].param_offset;

            // Map indices to unique IDs
            for (int i = 0; i < (int)param_count; i++)
                id_to_index.insert(std::make_pair(info[i].id, i));

            std::stringstream hex_stream;
            hex_stream << std::hex << (int)(*start);
            print_console(std::string("    Found param file: " + this->file + Param::FILE_EXT + " (Location: 0x").append(hex_stream.str()).append(") containing " + std::to_string(param_count) + " parameters"));
        }
        else if (print_result)
        {
            print_console(std::string("    ERROR: Failed to locate parameter data file (").append(this->file + Param::FILE_EXT) + ")");
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
        return ParamDef::data(base, data_start_offset);
    }


    // Returns param struct at the specified index in the data array (or NULL if the param file hasn't loaded yet)
    Param *get(int index)
    {
        if (base == NULL)
            return NULL;

        return ParamDef::get(index, param_size, ParamDef::data(base, data_start_offset));
    }

public:
    // Initializes file base pointer (start of the file, not the data array). Returns base address on success, and NULL otherwise
    void *init(bool print_result = false, bool re_init = false)
    {
        return ParamDef::init(&base, scan_pattern.c_str(), print_result, re_init);
    }

    // Returns param struct with the specified ID in the data array
    Param *get_by_id(uint32_t id)
    {
        if (base == NULL)
            return NULL;


        try {
            return get(id_to_index.at(id));
        }
        catch (const std::out_of_range&) {
            return NULL;
        }
    }

    // Returns the unique ID of the parameter at the given index in the data array (or -1 if the param file hasn't loaded yet)
    uint32_t get_id(int index)
    {
        if (base == NULL)
            return -1;

        return ((ParamBasicInfo*)(((uint8_t*)base) + 0x30))[index].id;
    }
};


#endif // _DS1_FILE_LIB_GENERIC_BASE_PARAM_DEF_FILE_H_
