/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Sean Pesce    -  C++
        Metal-Crow    -  C++
        RavagerChris  -  Reverse engineering of game files, gesture cancelling technique
*/

#pragma once

#ifndef _DS1_OVERHAUL_GAME_DATA_H_
    #define _DS1_OVERHAUL_GAME_DATA_H_



#include "SpPointer.h"
#include "Asset/Animation/Tae.h"



enum DS1_GAME_VERSION_ENUM {
    DS1_VERSION_DEBUG = 0x2C,           // 44
    DS1_VERSION_STEAM_BETA = 0x2D,      // 45
    DS1_VERSION_RELEASE = 0x2E,         // 46
    DS1_VERSION_OVERHAUL = 0x3C,        // 60
    DS1_VERSION_OVERHAUL_CHEATS = 0x3D, // 61     // Multiplayer network for Overhaul players that have cheats enabled
    DS1_VERSION_REKINDLED = 0x55,       // 85
    DS1_VERSION_REKINDLED_CHEATS = 0x56 // 86     // Multiplayer network for Rekindled players that have cheats enabled
};

enum DS1_PLAYER_STATUS_ENUM {
    DS1_PLAYER_STATUS_LOADING = -1,
    DS1_PLAYER_STATUS_HUMAN = 0,
    DS1_PLAYER_STATUS_COOP = 1,
    DS1_PLAYER_STATUS_INVADER = 2,
    DS1_PLAYER_STATUS_HOLLOW = 8
};

enum TAE_type0_param_values {
    cancel_by_atk = 4,
    lock_rotation = 7,
    activate_iframes = 8,
    cancel_by_moving = 11,
    cancel_by_left_atk = 16,
    cancel_by_left_block = 22,
    cancel_by_rolling_or_backstepping = 26,
    cancel_by_using_consumable = 31,
    cancel_by_two_handing = 32,
    allow_animation_cancel_events = 87,
    cap_movement_to_walk_speed = 90,
};


class Game
{
public:

    ////////////////////////////////////////
    ////////////// GAME DATA ///////////////
    ////////////////////////////////////////

    // Base address of Dark Souls game process
    static void *ds1_base;

    // Base address of fmodex dll Dark Souls loads
    static void *fmodex_base;

    // Base address for player character data
    static void *player_char_base;

    // Base address for world character data
    static void *world_char_base;

    // Player character status (loading, human, co-op, invader, hollow)
    static SpPointer player_char_status;

    // Time Action Events for the player character's animations
    static Tae player_tae;

    // Flag to determine if any characters have been loaded since the game was launched (useful if player had a character loaded but returned to main menu)
    static bool characters_loaded;

    // Address of lava brightness effect (used for dimming lava)
    static uint8_t *lava_luminosity;

    // Game saving on/off
    static SpPointer saves_enabled;

    // Multiplayer node count
    static int node_count;

    // Size of the pool of memory the game allocates for itself
    static uint32_t memory_limit;



    /////////////////////////////////////////
    /////////////// FUNCTIONS ///////////////
    /////////////////////////////////////////

    // Initializes pointers that depend on the game's base address
    static void init_pointers();

    // Performs tasks that were deferred until all game parameter files were loaded
    static void on_all_params_loaded();

    // Runs tasks that were deferred until a character was loaded
    static void on_first_character_loaded();

    // Performs tasks that must be rerun after any loading screen
    static void on_reloaded();

    // Obtains the current game version number
    static uint8_t get_game_version();

    // Runs code specific to the Debug build of Dark Souls
    static void run_debug_tasks();

    // Changes the game version number to avoid compatibility issues with different game builds
    static void set_game_version(uint8_t version_number);

    // Checks if the player is stuck at the bonfire, and if so, automatically applies the bonfire input fix
    static void check_bonfire_input_bug();

    // Fixes input bug that causes players to be stuck at a bonfire (usually after turning human with framerate unlocked)
    static int fix_bonfire_input(bool print_to_text_feed = false, bool print_to_console = false);

    // Check if dim lava mod is currently active
    static bool dim_lava_enabled();

    // Dim lava effects or restore default lava visuals
    static void enable_dim_lava(bool dim);

    // Returns multiplayer node count as an int (or -1 if player is not online)
    static int get_node_count();

    // Checks if armor sound effects are enabled
    static bool armor_sfx_enabled();

    // Toggles armor sound effecs
    static void enable_armor_sfx(bool enable);

    // Enables/Disables automatic game disconnection when low framerate is detected
    static void enable_low_fps_disconnect(bool enable);

    // Checks if player is currently locked onto an enemy
    static bool player_is_locked_on();

    // Set the current animation speed for the player character
    static void set_current_player_animation_speed(float speed);

    // Returns true if item auto-equip is currently enabled/unmodified
    static bool item_auto_equip_enabled();

    // Enable/disable item auto-equip
    static void set_item_auto_equip(bool enabled, bool silent = true);

    // Returns current player character body animation ID (attacking, rolling, gestures, etc)
    static int32_t get_player_body_anim_id();

    static int32_t get_player_upper_body_anim_id();

    static int32_t get_player_lower_body_anim_id();

    // Return current game time in milliseconds since the game has started
    static uint32_t Game::get_game_time_ms();

    // Sets whether player character will automatically turn toward enemies when locked on
    static bool allow_rotation_when_locked_on(bool allow);


                    /////////////////////////////////////////
                    //////////////// PATCHES ////////////////
                    /////////////////////////////////////////

    // Set available pool of memory that Dark Souls allocates for itself
    static void set_memory_limit(uint32_t mem_limit);

    // Allow effect IDs to be transferred between clients without bounds restrictions
    static void unrestrict_network_synced_effectids();

    // Fix the bug where the player HP could be greater than the displayed GUI bar
	static void increase_gui_hpbar_max();

    // Two-part patch to increase the multiplayer phantom limit:
    static void increase_phantom_limit1(); // Called from on_process_attach()
    static void increase_phantom_limit2(); // Called from initialize_plugin()




                    /////////////////////////////////////////
                    ////////////// HUD-RELATED //////////////
                    /////////////////////////////////////////

    class Hud {
    public:

        // Additional HUD elements
        static bool get_show_compass_radial();
        static void set_show_compass_radial(bool enable);
        static bool get_show_compass_bar();
        static void set_show_compass_bar(bool enable);
        static bool get_show_elevation_meter();
        static void set_show_elevation_meter(bool enable);
        static bool get_show_node_graph();
        static void set_show_node_graph(bool enable, bool game_flag_only = true);
    };



                    /////////////////////////////////////////
                    ////////////// FILE-RELATED /////////////
                    /////////////////////////////////////////
    class Files {
    public:

        enum IoMonitorIndex {
            BHD0 = 0,
            BHD1 = 1,
            BHD2 = 2,
            BHD3 = 3,
            BDT0 = 4,
            BDT1 = 5,
            BDT2 = 6,
            BDT3 = 7,
            SL2 = 8
        };

        // Structure for managing game file I/O
        struct IoMonitor {
            HANDLE handle = NULL;
            uint32_t io_pos = 0;
            bool monitor = false;
            std::string  default_filename;
            std::wstring default_filename_w;
            // @TODO: Fill in custom filename (if a custom file path was specified)
            std::string  custom_filename;
            std::wstring custom_filename_w;
        };

        // Strutures for tracking file I/O data for the game's BDT, BHD5, and SL2 files
        static IoMonitor io_monitors[9];

        // Filter for I/O monitoring output; only strings containing this string will be printed
        static std::string io_output_filter;

        // Default save file path used by the game
        static std::string default_save_file_path;

        // Index of the save file currently being read/written by the game
        static int save_file_index;

        // Pending save file index changes
        static bool save_file_index_pending_set_next;
        static bool save_file_index_pending_set_prev;


        // Returns the address of the file I/O monitoring struct corresponding to the specified file handle
        static IoMonitor *io_monitor_from_handle(HANDLE handle);

        // Initializes game file I/O monitor structs
        static void init_io_monitors();

        // Patches game calls to Win32 API file I/O funcs, redirecting them to interceptor functions
        static void apply_function_intercepts();

        // Checks if custom archive files exist (.bdt/.bhd5)
        static void check_custom_archive_file_path();

        // Checks if custom save file exists (.sl2)
        static void check_custom_save_file_path();

        // Checks if custom game config file exists (.ini)
        static void check_custom_game_config_file_path();

        // Returns the full file path of the current save file (with index)
        static const wchar_t *get_save_file_path(std::wstring &buffer);

        // Changes the current save file index (and writes all corresponding data)
        static void set_save_file_index(int unsigned index, bool print_output = true);

        // Changes the to the next save file (if current save file is the last one, new file is first save file)
        static void set_save_file_next(bool print_output = true);

        // Changes the to the previous save file (if current save file is the first one, new file is last save file)
        static void set_save_file_prev(bool print_output = true);

        // Checks if the saved characters menu is currently open
        static bool saves_menu_is_open();

        

                                            
                                            //////////////////////////////////////////////////////////
                                            ////////////// Win32 API File I/O Intercepts /////////////
                                            //////////////////////////////////////////////////////////

        // Called when the game attempts to call CreateFileW
        static HANDLE WINAPI intercept_create_file_w(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
                                                     LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition,
                                                     DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);

        // Called when the game attempts to call GetPrivateProfileIntW
        static UINT WINAPI intercept_get_private_profile_int_w(LPCWSTR lpAppName, LPCWSTR lpKeyName, INT nDefault, LPCWSTR lpFileName);

        // Called when the game attempts to call WritePrivatePrivateProfileW
        static BOOL WINAPI intercept_write_private_profile_section_w(LPCWSTR lpAppName, LPCWSTR lpString, LPCWSTR lpFileName);

        // Called when the game attempts to call ReadFile
        static BOOL WINAPI intercept_read_file(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped);

        // Called when the game attempts to call WriteFile
        static BOOL WINAPI intercept_write_file(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped);

        // Called when the game attempts to call CloseHandle
        static BOOL WINAPI intercept_close_handle(HANDLE hObject);

        // Called when the game attempted to call SetFilePointer
        static DWORD WINAPI intercept_set_file_pointer(HANDLE hFile, LONG lDistanceToMove, PLONG lpDistanceToMoveHigh, DWORD dwMoveMethod);
    };
};
typedef Game::Files Files;
typedef Game::Hud Hud;



#endif // _DS1_OVERHAUL_GAME_DATA_H_
