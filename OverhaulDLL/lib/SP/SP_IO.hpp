// Author: Sean Pesce

#pragma once

#ifndef SP_IO_HPP
	#define SP_IO_HPP



#include <chrono>	// localtime_s
#include <fstream>	// ofstream
#include <iostream> // cout, endl
#include <limits>	// numeric_limits
#include <list>		// std::list
#include <sstream>  // stringstream
#include <string>	// to_string

#ifdef _WIN32
	#include <Windows.h>
#endif // _WIN32



/////////////////////// Constants & Enums ///////////////////////
#ifdef _WIN32
	#define GetAsyncKeyboardState get_async_keyboard_state
	#define _SP_KEY_DOWN_ 2147483648
	#define _SP_KEY_TOGGLED_ 1
	#define VK_TO_WMKD_SC(key) (((LONG)MapVirtualKey(key, MAPVK_VK_TO_VSC)) << 16)		// Translates a Virtual-key code into the second parameter of a WM_KEYDOWN keyboard message containing only the corresponding scan code
#endif // _WIN32

#define _SP_MAX_PP_KEY_LENGTH_ 128			// Maximum length for private profile keys
#define _SP_MAX_PP_STRING_VAL_LENGTH_ 128	// Maximum length for private profile string values


// Enumerator whose values denote different date string formats
enum SP_DATE_STRING_FORMATS {
	SP_DATE_MMDDYYYY,	// 12/31/9999
	SP_DATE_DDMMYYYY,	// 31/12/9999
	SP_DATE_MMDDYY,		// 12/31/99
	SP_DATE_DDMMYY		// 31/12/99
};


// Represents a function bound to a keypress
typedef struct SP_KEY_FUNCTION {
	unsigned int key;	// Virtual key code
	int (*function)();	// Function called when key is pressed
} SP_KEY_FUNCTION;


/////////////////////// Shell I/O ///////////////////////


/**
	sp_print_intro(const char*)
	
	Prints the program name and author information.

	@param program_name	The name of the program.
 */
void sp_print_intro(const char* program_name);


/**
	enter_to_continue(const char*)
	
	Prints a prompt message and waits for the user to press the Enter (Return) key.

	@param prompt	The prompt message to print before the user presses Enter.
 */
void enter_to_continue(const char* prompt);


/**
	enter_to_continue(const char*, const char*)
	
	Prints a prompt message, waits for the user to press Enter (Return) key, and
	prints a second message before continuing.

	@param prompt	The prompt message to print before the user presses Enter.
	@param continue_msg	The message to print after the user presses Enter.
 */
void enter_to_continue(const char* prompt, const char* continue_msg);



/////////////////////// Text File I/O ///////////////////////


/**
	file_write_text(const char*, const char*)

	Writes the specified output to a text file. If the file already exists, it
	is overwritten.
	
	@param file	Path and file name of the file to write to.
	@param msg	The string that will be written to file.
	
	@return 0 on success; return 1 if the file could not be written.
 */
int file_write_text(const char *file, const char *msg);


/**
	file_append_text(const char*, const char*)

	Appends the specified text file with the given string. If the
	specified file doesn't exist, it is created first.
	
	@param file	Path and file name of the file to append.
	@param msg	The string that will be appended to file.
	
	@return 0 on success; return 1 if the file could not be written.
 */
int file_append_text(const char *file, const char *msg);


/**
	get_private_profile_string_section(const char*, const char*, std::list<std::string>*)

	Obtains a list of all key/value pairs from the specified section of the
	specified configuration file. Each key/value pair is defined on a separate line
	and specified by the key name followed by any combination/number of '=', ' ',
	and/or '\t' characters.
	
	@param file		Path and file name of the configuration file (generally a .ini file).
	@param section	The name of the section to be parsed (formatted as "[section]" in the file).
	@param keys		The list that will be populated with parsed key/value pairs.
	
	@return	Number of key/value pairs that were successfully parsed from the specified
			file, or -1 if errors were encountered.
 */
int get_private_profile_string_section(const char *file, const char *section, std::list<std::string> *keys);



/////////////////////// General Text Data I/O ///////////////////////


/**
	get_current_timestamp_string(char*, bool)

	Constructs a timestamp string for the current 24-hour time,
	formatted as "HH:MM:SS" or "[HH:MM:SS]", and stores it in
	the specified char* buffer with a trailing null character.

	@param timestamp_string_buff	A buffer to hold the generated timestamp string.
									The buffer must be AT LEAST sizeof(char[11]) if
									surround_with_brackets is true, and must be AT
									LEAST sizeof(char[9]) if surround_with_brackets
									is false.
	@param surround_with_brackets	Specifies whether the generated timestamp should
									be contained within bracket characters ('[' and ']').

	@return 0 on success; return localtime_s error code on failure.
*/
int get_current_timestamp_string(char *timestamp_string_buff, bool surround_with_brackets);


/**
	get_current_timestamp_string(std::string*, bool)

	Constructs a timestamp string for the current 24-hour time,
	formatted as "HH:MM:SS" or "[HH:MM:SS]", and stores it in
	the specified string object. The string object is cleared
	before being filled with the generated timestamp string.

	@param timestamp_string			The string object that will be cleared and
									filled with the generated timestamp string.
	@param surround_with_brackets	Specifies whether the generated timestamp should
									be contained within bracket characters ('[' and ']').

	@return 0 on success; return localtime_s error code on failure.
*/
int get_current_timestamp_string(std::string *timestamp_string, bool surround_with_brackets);


/**
	append_current_timestamp_string(std::string*, bool)

	Constructs a timestamp string for the current 24-hour time,
	formatted as "HH:MM:SS" or "[HH:MM:SS]", and appends it to
	the specified string object.

	@param timestamp_string			The string object that will be appended with the
									generated timestamp string.
	@param surround_with_brackets	Specifies whether the generated timestamp should
									be contained within bracket characters ('[' and ']').

	@return 0 on success; return localtime_s error code on failure.
*/
int append_current_timestamp_string(std::string *timestamp_string, bool surround_with_brackets);


/**
	get_current_date_string(char*, bool, int, char)

	Constructs a formatted date string for the current day and
	stores it in the specified char* buffer with a trailing null
	character. Available date formats are as follows:
	
		31/12/9999
		12/31/9999
		31/12/99
		12/31/99

	Format variations can be specified using values from the
	SP_DATE_STRING_FORMATS enum. Date strings can also be optionally
	enclosed in brackets.

	@param date_string_buff			A buffer to hold the generated date string. Minimum
									necessary buffer size ranges from sizeof(char[9])
									to sizeof(char[13]), depending on the chosen format.
	@param surround_with_brackets	Specifies whether the generated date string should
									be contained within bracket characters ('[' and ']').
	@param format					The date format, specified by a value in the
									SP_DATE_STRING_FORMATS enum. If an invalid format
									is given, the function defaults to SP_DATE_DDMMYYYY.
	@param separator				The character separator between the day/month/year fields
									in the formatted date string (Generally '/' or '-').

	@return 0 on success; return localtime_s error code on failure.
*/
int get_current_date_string(char *date_string_buff, bool surround_with_brackets, int format, char separator);

/**
	get_current_date_string(char*, bool, int)

	Calls @get_current_date_string(char*, bool, int, char) with separator='/'.
*/
int get_current_date_string(char *date_string_buff, bool surround_with_brackets, int format);



/**
	get_current_date_string(std::string*, bool, int, char)

	Constructs a formatted date string for the current day and
	stores it in the specified std::string*. The string object
	is cleared before being filled with the generated date
	string. Available date formats are as follows:
	
		31/12/9999
		12/31/9999
		31/12/99
		12/31/99

	Format variations can be specified using values from the
	SP_DATE_STRING_FORMATS enum. Date strings can also be optionally
	enclosed in brackets.

	@param date_string				Pointer to a string object that will be cleared and
									filled with the generated date string.
	@param surround_with_brackets	Specifies whether the generated date string should
									be contained within bracket characters ('[' and ']').
	@param format					The date format, specified by a value in the
									SP_DATE_STRING_FORMATS enum. If an invalid format
									is given, the function defaults to SP_DATE_DDMMYYYY.
	@param separator				The character separator between the day/month/year fields
									in the formatted date string (Generally '/' or '-').

	@return 0 on success; return localtime_s error code on failure.
*/
int get_current_date_string(std::string *date_string, bool surround_with_brackets, int format, char separator);

/**
	get_current_date_string(std::string*, bool, int)

	Calls @get_current_date_string(std::string*, bool, int, char) with separator='/'.
*/
int get_current_date_string(std::string *date_string, bool surround_with_brackets, int format);


/**
	append_current_date_string(std::string*, bool, int, char)

	Constructs a formatted date string for the current day and
	appends it to the specified std::string. Available date
	formats are as follows:
	
		31/12/9999
		12/31/9999
		31/12/99
		12/31/99

	Format variations can be specified using values from the
	SP_DATE_STRING_FORMATS enum. Date strings can also be optionally
	enclosed in brackets.

	@param date_string				Pointer to a string object that will be appended
									with the generated date string.
	@param surround_with_brackets	Specifies whether the generated date string should
									be contained within bracket characters ('[' and ']').
	@param format					The date format, specified by a value in the
									SP_DATE_STRING_FORMATS enum. If an invalid format
									is given, the function defaults to SP_DATE_DDMMYYYY.
	@param separator				The character separator between the day/month/year fields
									in the formatted date string (Generally '/' or '-').

	@return 0 on success; return localtime_s error code on failure.
*/
int append_current_date_string(std::string *date_string, bool surround_with_brackets, int format, char separator);

/**
	append_current_date_string(std::string*, bool, int)

	Calls @append_current_date_string(std::string*, bool, int, char) with separator='/'.
*/
int append_current_date_string(std::string *date_string, bool surround_with_brackets, int format);



/////////////////////// Keyboard I/O ///////////////////////


/**
	get_vk_hotkey(const char*, const char*, const char*)
	
	Parses config file for a specified hotkey, denoted as a virtual key code.
	
	For more info on virtual key codes:
		https://msdn.microsoft.com/en-us/library/windows/desktop/dd375731%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396
	
	@param settings_file	Name of the file that will be parsed for the hotkey (generally an .ini file)
	@param section			Title of the section of the settings file where the key will be found (Format in the file should be "[section]")
	@param key_name			Name of the key where the hotkey value is specified (Format in the file should be "key_name=XX", where "XX"
							denotes a hex-formatted virtual key code)
	
	@return the virtual key code for the specified hotkey as an unsigned int (zero on failure)
 */
unsigned int get_vk_hotkey(const char *settings_file, const char *section, const char *key_name);


/**
	add_function_keybind(unsigned int, int (*)(), std::list<SP_KEY_FUNCTION>*)

	Adds a new keybound function to the given list.

	For more info on virtual key codes:
	https://msdn.microsoft.com/en-us/library/windows/desktop/dd375731%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396

	@param key		Virtual key code of the key that calls the given function when pressed
	@param function	Function called when key is pressed
	@param binds	Pointer to a list of keybound functions; the new key function is added to the end of this list

	@return 0 on success; non-zero value on failure
 */
int add_function_keybind(unsigned int key, int (*function)(), std::list<SP_KEY_FUNCTION> *binds);


#ifdef _WIN32

/**
	get_async_keyboard_state(SHORT*)
	
	Gets the async key state for all 256 virtual keys and stores them in the given buffer.
	Note: The buffer must be at least the size of an array of 256 BYTEs.
	
	@param keyboard_state_buffer Buffer where async key state data will be stored
 */
void get_async_keyboard_state(SHORT *keyboard_state_buffer);

#endif // _WIN32


#endif // SP_IO_HPP