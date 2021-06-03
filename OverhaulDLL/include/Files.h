/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Sean Pesce  -  C++
*/

#pragma once

#ifndef _DS1_OVERHAUL_GAME_FILES_H_
    #define _DS1_OVERHAUL_GAME_FILES_H_

#include "DarkSoulsOverhaulMod.h"
#include <set>

class Files {
public:
    static int string_wide_to_mb(wchar_t *in_string, std::string &out_string);
    static int string_mb_to_wide(char *in_string, std::wstring &out_string);

    static void apply_function_intercepts();
    static void check_custom_archive_file_path();
    static void check_custom_save_file_path();
    static void check_custom_game_config_file_path();
    static std::wstring get_save_file_path();
    static bool saves_menu_is_open();
    static void set_save_file_next();
    static void set_save_file_prev();
    static void create_new_save_file();

    // Save file path and filename used
    static std::string save_file;

    // Index of the save file currently being read/written by the game
    static int save_file_index;

    // Pending save file index changes
    static bool save_file_index_pending_set_next;
    static bool save_file_index_pending_set_prev;
    static bool save_file_index_make_new;

    static std::set<std::wstring> files_to_intercept_loading;

    static bool UseCustomFiles;

private:
    static void set_save_file_index(int unsigned index);
};



#endif // _DS1_OVERHAUL_GAME_FILES_H_
