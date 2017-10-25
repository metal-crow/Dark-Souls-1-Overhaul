/*
	DARK SOULS OVERHAUL
	
	Contributors to this file:
		Sean Pesce	-	C++
*/

#pragma once

#ifndef _DS1_OVERHAUL_GAME_FILES_H_
	#define _DS1_OVERHAUL_GAME_FILES_H_

#include "GameData.h"
#include "Shlobj.h"
#include "SP_IO.hpp"


/*
	Initialize constants:
*/

// Default file types for game archive files (wide char)
const wchar_t *Files::ARCHIVE_FILE_TYPE_W[2] = { L".bdt", L".bhd5" };

// Default file types for game archive files (char)
const char *Files::ARCHIVE_FILE_TYPE[2] = { ".bdt", ".bhd5" };

// Default file type for game save file
const wchar_t *Files::DEFAULT_SAVE_FILE_TYPE_W = L".sl2";




// Called when the game attempts to call CreateFileW
HANDLE WINAPI Files::intercept_create_file_w(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
					LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition,
					DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
	HANDLE return_val;
	std::wstring load_file;
	std::wstring load_file_ext;
	if (lpFileName != NULL)
	{
		load_file = lpFileName;
		if ((int)load_file.length() >= 4)
		{
			load_file_ext = load_file.substr(load_file.length() - 4);
			if ((int)Mod::custom_game_archives.length() > 0 && load_file_ext == Files::ARCHIVE_FILE_TYPE_W[0])
			{
				// Intercept archive file load (.bdt)
				load_file = Mod::custom_game_archives + load_file.substr(load_file.length() - 5);
			}
			else if ((int)Mod::custom_game_archives.length() > 0 && load_file_ext == &Files::ARCHIVE_FILE_TYPE_W[1][1])
			{
				// Intercept archive file load (.bhd5)
				load_file = Mod::custom_game_archives + load_file.substr(load_file.length() - 6);
			}
			else if ((int)Mod::custom_save_file.length() > 0 && (load_file_ext == Files::DEFAULT_SAVE_FILE_TYPE_W))
			{
				// Intercept save file load (.sl2)
				load_file = Mod::custom_save_file;
			}
		}
		// Call original function
		return_val = CreateFileW(load_file.c_str(), dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	}
	else
	{
		// Call original function with original arguments
		return_val = CreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	}

	return return_val;
}


// Called when the game attempts to call GetPrivateProfileIntW
UINT WINAPI Files::intercept_get_private_profile_int_w(LPCWSTR lpAppName, LPCWSTR lpKeyName, INT nDefault, LPCWSTR lpFileName)
{
	UINT return_val;
	
	std::wstring load_file = lpFileName;
	if ((int)Mod::custom_config_file.length() > 0)
	{
		// Intercept config file load (.ini)
		load_file = Mod::custom_config_file;
	}

	// Call original function
	return_val = GetPrivateProfileIntW(lpAppName, lpKeyName, nDefault, load_file.c_str());

	return return_val;
}


// Called when the game attempts to call WritePrivateProfileSectionW
BOOL WINAPI Files::intercept_write_private_profile_section_w(LPCWSTR lpAppName, LPCWSTR lpString, LPCWSTR lpFileName)
{
	BOOL return_val;

	std::wstring load_file = lpFileName;
	if ((int)Mod::custom_config_file.length() > 0)
	{
		// Intercept config file load (.ini)
		load_file = Mod::custom_config_file;
	}

	// Call original function
	return_val = WritePrivateProfileSectionW(lpAppName, lpString, load_file.c_str());

	return return_val;
}


// Patches game calls to various shared library functions, redirecting them to Game::intercept_create_file()
void Files::apply_function_intercepts()
{
	// CreateFileW
	static uint32_t intercept_func_w = (uint32_t)&Game::Files::intercept_create_file_w;
	static uint32_t *intercept_func_w_ptr = &intercept_func_w;
	uint8_t *intercept_func_w_b = (uint8_t *)&intercept_func_w_ptr;
	uint8_t patch_w[4] = { intercept_func_w_b[0], intercept_func_w_b[1], intercept_func_w_b[2], intercept_func_w_b[3] };
	
	void *write_address = (uint8_t*)Game::ds1_base + 0x2B7D04;	// Game archives and savegame file
	apply_byte_patch(write_address, patch_w, 4);

	write_address = (uint8_t*)Game::ds1_base + 0x2B7FEE;	// Savegame file and config directory
	apply_byte_patch(write_address, patch_w, 4);


	// GetPrivateProfileIntW
	static uint32_t intercept_gppi_func_w = (uint32_t)&Files::intercept_get_private_profile_int_w;
	static uint32_t *intercept_gppi_w_ptr = &intercept_gppi_func_w;
	uint8_t *intercept_gppi_w_b = (uint8_t *)&intercept_gppi_w_ptr;
	uint8_t patch_gppi_w[4] = { intercept_gppi_w_b[0], intercept_gppi_w_b[1], intercept_gppi_w_b[2], intercept_gppi_w_b[3] };

	write_address = (uint8_t*)Game::ds1_base + 0xB70B17;
	apply_byte_patch(write_address, patch_gppi_w, 4);

	write_address = (uint8_t*)Game::ds1_base + 0xB70B2F;
	apply_byte_patch(write_address, patch_gppi_w, 4);

	write_address = (uint8_t*)Game::ds1_base + 0xB70B79; // This seems to be the only call to GetPrivateProfileIntW that actually gets executed
	apply_byte_patch(write_address, patch_gppi_w, 4);


	// WritePrivateProfileIntW
	static uint32_t intercept_wpps_func_w = (uint32_t)&Files::intercept_write_private_profile_section_w;
	static uint32_t *intercept_wpps_w_ptr = &intercept_wpps_func_w;
	uint8_t *intercept_wpps_w_b = (uint8_t *)&intercept_wpps_w_ptr;
	uint8_t patch_wpps_w[4] = { intercept_wpps_w_b[0], intercept_wpps_w_b[1], intercept_wpps_w_b[2], intercept_wpps_w_b[3] };

	write_address = (uint8_t*)Game::ds1_base + 0xB70C99;
	apply_byte_patch(write_address, patch_wpps_w, 4);
}


// Checks if custom archive files (.bdt/.bhd5) exist. If one or more are missing, custom archives are not loaded, and default game files are loaded instead.
void Files::check_custom_archive_files()
{
	if ((int)Mod::custom_game_archives.length() == 0)
		return;

	Mod::startup_messages.push_back(std::string(Mod::output_prefix + "Checking if custom game archive files exist..."));

	// Get char* strings for printing console messages
	errno_t conversion_return;
	std::string archive_name_ch = "";
	if(conversion_return = string_wide_to_mb((wchar_t*)Mod::custom_game_archives.c_str(), archive_name_ch))
	{
		// Error converting from wide char to char
		Mod::startup_messages.push_back(std::string(Mod::output_prefix + "ERROR: Unable to parse custom archive file name (Error code ").append(std::to_string(conversion_return)).append("). Using default archive files instead."));
		Mod::custom_game_archives = L"";
		return;
	}


	// Check that custom game archive files exist
	for (int i = 0; i < ARCHIVE_FILE_PAIR_COUNT; i++)
	{
		std::ifstream check_file(std::wstring(Mod::custom_game_archives).append(std::to_wstring(i)).append(ARCHIVE_FILE_TYPE_W[0]).c_str());
		if (!check_file.good())
		{
			// Custom .bdt file doesn't exist
			Mod::startup_messages.push_back(std::string(Mod::output_prefix + "ERROR: The file \"").append(archive_name_ch).append(std::to_string(i)).append(ARCHIVE_FILE_TYPE[0]).append("\" could not be found. Using default archive files instead."));
			check_file.close();
			Mod::custom_game_archives = L"";
			return;
		}
		else
			Mod::startup_messages.push_back(std::string("    Found ").append(archive_name_ch).append(std::to_string(i)).append(ARCHIVE_FILE_TYPE[0]));
		check_file.close();
		std::ifstream check_file2(std::wstring(Mod::custom_game_archives).append(std::to_wstring(i)).append(ARCHIVE_FILE_TYPE_W[1]).c_str());
		if (!check_file2.good())
		{
			// Custom .bhd5 file doesn't exist
			Mod::startup_messages.push_back(std::string(Mod::output_prefix + "ERROR: The file \"").append(archive_name_ch).append(std::to_string(i)).append(ARCHIVE_FILE_TYPE[1]).append("\" could not be found. Using default archive files."));
			check_file2.close();
			Mod::custom_game_archives = L"";
			return;
		}
		else
			Mod::startup_messages.push_back(std::string("    Found ").append(archive_name_ch).append(std::to_string(i)).append(ARCHIVE_FILE_TYPE[1]));
		check_file2.close();
	}
	
	Mod::startup_messages.push_back(std::string(Mod::output_prefix + "SUCCESS: Custom game archive files will be loaded (\"").append(archive_name_ch).append("\")."));
}


// Checks if custom save file (.sl2) exists
void Files::check_custom_save_file()
{
	if ((int)Mod::custom_save_file.length() == 0)
		return;

	Mod::startup_messages.push_back(std::string(Mod::output_prefix + "Checking if custom save file exists..."));

	errno_t conversion_return;
	std::string filename_ch;
	if (conversion_return = string_wide_to_mb((wchar_t*)Mod::custom_save_file.c_str(), filename_ch))
	{
		// Error converting from wide char to char
		return;
	}

	std::ifstream check_file(Mod::custom_save_file.c_str());
	bool found_file = check_file.good();
	check_file.close();
	if (found_file)
	{
		Mod::startup_messages.push_back(std::string(Mod::output_prefix + "SUCCESS: Custom save file will be loaded (\"").append(filename_ch).append("\")."));
	}
	else
	{
		// Custom save file doesn't exist
		Mod::startup_messages.push_back(std::string(Mod::output_prefix + "WARNING: Custom save file was not found (\"").append(filename_ch).append("\"). No characters will be available to load."));
		return;
	}
}


// Checks if custom game config file (.ini) exists
void Files::check_custom_game_config_file()
{
	if ((int)Mod::custom_config_file.length() == 0)
		return;

	Mod::startup_messages.push_back(std::string(Mod::output_prefix + "Checking if custom game config file exists..."));
	
	// Get char* strings for printing console messages
	errno_t conversion_return;
	std::string filename_ch;
	if (conversion_return = string_wide_to_mb((wchar_t*)Mod::custom_config_file.c_str(), filename_ch))
	{
		// Error converting from wide char to char
		return;
	}
	
	std::ifstream check_file(Mod::custom_config_file.c_str());
	bool found_file = check_file.good();
	check_file.close();
	if (found_file)
	{
		Mod::startup_messages.push_back(std::string(Mod::output_prefix + "SUCCESS: Custom game config file will be loaded (\"").append(filename_ch).append("\")."));
	}
	else
	{
		// Custom config file doesn't exist
		Mod::startup_messages.push_back(std::string(Mod::output_prefix + "WARNING: Custom config file was not found (\"").append(filename_ch).append("\"). Default configuration file will be automatically generated."));
		return;
	}
}


#endif // _DS1_OVERHAUL_GAME_FILES_H_