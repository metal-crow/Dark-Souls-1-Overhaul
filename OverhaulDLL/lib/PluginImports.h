/*
    Author: Sean Pesce

    The data in this header is exported from the DirectX9 overlay
    and imported by this DLL when loaded as a plugin for the overlay.
*/

#pragma once

#ifndef _SP_D3D9O_PLUGIN_IMPORTS_H_
    #define _SP_D3D9O_PLUGIN_IMPORTS_H_

#include "d3d9.h" // NOTE: This header requires installation of Microsoft's DirectX SDK (June 2010): https://www.microsoft.com/en-us/download/details.aspx?id=6812
#include <string>
#include <vector>



////////////////////////////////////////////////////
///////////////////// CONSTANTS ////////////////////
////////////////////////////////////////////////////

// Value that a console command should return if no errors occurred
#define CONSOLE_COMMAND_SUCCESS ERROR_SUCCESS

// Value returned by the console itself if an unknown command is executed
#define CONSOLE_COMMAND_NOT_FOUND_ERROR (-1)

// Obtain the IDirect3DDevice9 for rendering new elements
#define _d3d9_dev get_d3d9_device()

// Obtain the main game window handle
#define _game_window get_game_window()


// Enumerated message color values for use in the optional text_color parameter when calling print()
enum SP_D3D9O_TEXT_COLOR_ENUM {
    SP_D3D9O_TEXT_COLOR_WHITE,
    SP_D3D9O_TEXT_COLOR_BLACK,
    SP_D3D9O_TEXT_COLOR_RED,
    SP_D3D9O_TEXT_COLOR_ORANGE,
    SP_D3D9O_TEXT_COLOR_YELLOW,
    SP_D3D9O_TEXT_COLOR_GREEN,
    SP_D3D9O_TEXT_COLOR_CYAN,
    SP_D3D9O_TEXT_COLOR_BLUE,
    SP_D3D9O_TEXT_COLOR_PURPLE,
    SP_D3D9O_TEXT_COLOR_PINK
};




////////////////////////////////////////////////////
//////////////// IMPORTED VARIABLES ////////////////
////////////////////////////////////////////////////


// Filename of the DirectX9 wrapper overlay DLL (Note: It may not be "d3d9.dll")
__declspec(dllimport) std::string d3d9o_dll_filename;





////////////////////////////////////////////////////
//////////////// IMPORTED FUNCTIONS ////////////////
////////////////////////////////////////////////////


/*
    Obtains a pointer to the ID3D9Device.

    Note: Recommended to use the _d3d9_dev macro instead of directly calling this function.

    @return    Pointer to the game's IDirect3DDevice9, or NULL if no device has been initialized
*/
__declspec(dllimport) IDirect3DDevice9 *get_d3d9_device();




/*
    Prints message to overlay text feed and console.

    @param message              Message to be printed
    @param duration             [Optional parameter] Time (in milliseconds) that the message will stay on-screen
    @param include_timestamp    [Optional parameter] Specifies whether to prepend a 24-hour timestamp to the printed message
    @param text_color           [Optional parameter] Color of the printed text

    @return						True if successful; false otherwise (generally fails if overlay hasn't been initialized yet)
*/
__declspec(dllimport) bool print(const char *message, unsigned long long duration = 2000, bool include_timestamp = true, SP_D3D9O_TEXT_COLOR_ENUM text_color = SP_D3D9O_TEXT_COLOR_WHITE);
__declspec(dllimport) bool print(std::string &message, unsigned long long duration = 2000, bool include_timestamp = true, SP_D3D9O_TEXT_COLOR_ENUM text_color = SP_D3D9O_TEXT_COLOR_WHITE);



/*
    Prints message to console output only.

    @param message    Message to be printed

    @return           True if successful; false otherwise (generally fails if overlay hasn't been initialized yet)
*/
__declspec(dllimport) bool print_console(const char *message);
__declspec(dllimport) bool print_console(std::string &message);




/*
    Sets the overlay text feed title.

    @param title    New title for the overlay text feed

    @return         True if successful; false otherwise (generally fails if overlay hasn't been initialized yet)
*/
__declspec(dllimport) bool set_text_feed_title(const char *new_title);




/*
    Binds a function to a keybind.

    For more info on virtual-key codes, see the Microsoft documentation:
    https://msdn.microsoft.com/en-us/library/windows/desktop/dd375731(v=vs.85).aspx

    @param vk_hotkey    Virtual-key code for the key that triggers the specified function
    @param function     Function that is called when the hotkey is pressed

    @return             Value of the vk_hotkey parameter if success; otherwise returns 0 (if vk_hotkey was 0 or function parameter was NULL)
*/
__declspec(dllimport) unsigned int register_hotkey_function(unsigned int vk_hotkey, int(*function)());




/*
    Registers a command that can be executed via the console.

    @param command         String that must be entered into the console window's input to execute the command
    @param function        Function that is called when the command is entered
    @param help_message    Message that will be printed if user calls the "help" command with this command as an argument

    @return                0 if successful, otherwise returns one of the following error codes:
                           ERROR_INVALID_ADDRESS            If one or more arguments are NULL
                           ERROR_INVALID_PARAMETER          If the command parameter is an empty string (or only whitespace characters)
                           ERROR_SXS_XML_E_BADCHARINSTRING  If the command parameter contains invalid characters (such as whitespace characters)
                           ERROR_DUP_NAME                   If a command already exists with the same name as the command parameter
*/
__declspec(dllimport) int register_console_command(const char *command, int(*function)(std::vector<std::string>, std::string *), const char *help_message);




/*
    Creates an alias for an existing console command or existing alias, allowing the existing command/alias to be called from the new alias.

    @param new_alias           Alias for an existing console command/alias. Alias cannot be identical to an existing command/alias.
    @param existing_command    Name of the existing console command/alias that will be called if new_alias is entered into the console
    @param args                [Optional parameter] If non-empty, this will be the set of arguments that the existing command/alias will be called with when new_alias is called

    @return                    0 if successful, otherwise returns one of the following error codes:
                               ERROR_INVALID_ADDRESS            If one or more arguments are NULL
                               ERROR_INVALID_PARAMETER          If the new_alias parameter is an empty string (or only whitespace characters)
                               ERROR_SXS_XML_E_BADCHARINSTRING  If the new_alias parameter contains invalid characters (such as whitespace characters)
                               ERROR_DUP_NAME                   If a command already exists with the same name as the new_alias parameter
                               ERROR_PROC_NOT_FOUND             If existing_command does not match any existing command or alias name
*/
__declspec(dllimport) int register_console_alias(const char *new_alias, const char *existing_command, std::vector<std::string> args = {});




/*
    Executes a command string as if it were entered from the console input.

    @param command        The command to be parsed and executed by the console.
    @param return_code    [Optional parameter] Buffer to store the return code of the executed command. If NULL, the return code is discarded. If the specified command
                          doesn't exist, return_code is set to CONSOLE_COMMAND_NOT_FOUND_ERROR.
    @param output         [Optional parameter] The output stream where console command output will be printed. If NULL, output is sent to the console's default output stream.

    @return               CONSOLE_COMMAND_SUCCESS          If successful, otherwise returns one of the following error codes:
                          ERROR_INVALID_ADDRESS            If the command argument is NULL
                          PEERDIST_ERROR_NOT_INITIALIZED   If the IDirect3D9Device, overlay, and/or console aren't initialized yet
                          CONSOLE_COMMAND_NOT_FOUND_ERROR  If the specified command does not match an existing console command
*/
__declspec(dllimport) int execute_console_command(const char *command, int *return_code = NULL, std::string *output = NULL);




/*
    Checks if the in-game console is open.

    @return    True if the in-game console is currently open; otherwise returns false.

    @error     If the DirectX9 device or overlay is not yet initialized, last error code is set to ERROR_INVALID_ADDRESS
*/
__declspec(dllimport) bool console_is_open();




/*
    Obtains a handle to the main game window.

    NOTE: Recommended to use the _game_window preprocessor macro every time the game window handle needs to be obtained.

    @return    Game's primary window handle if successful; otherwise returns NULL.

    @error     If the DirectX9 device or overlay is not yet initialized, last error code is set to ERROR_INVALID_ADDRESS
*/
__declspec(dllimport) HWND get_game_window();



/*
    Gets the current global frame count (total number of frames rendered since the game was launched).

    @return    Current global frame count if successful; otherwise returns 0.

    @error     If the DirectX9 device or overlay is not yet initialized, last error code is set to ERROR_INVALID_ADDRESS
*/
__declspec(dllimport) unsigned long long get_global_frame_count();


#endif // _SP_D3D9O_PLUGIN_IMPORTS_H_