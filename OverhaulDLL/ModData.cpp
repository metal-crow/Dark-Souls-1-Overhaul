/*
	DARK SOULS OVERHAUL
	
	Contributors to this file:
		Sean Pesce	-	C++

*/

#include "DllMain.h"

#define _SP_DEFINE_VK_NAME_STRINGS_		// Must be defined to use Virtual-key code name strings from SP_IO_Strings.hpp (opt-in by default because it increases filesize by a few KB)

#include "SP_IO_Strings.hpp"
#include <clocale>



/*
	Initialize static variables:
*/


// Signifies whether the mod has finished executing the inititalize_plugin() function
bool ModData::initialized = false;

// Console messages from events that took place before the in-game console was loaded
std::vector<std::string> ModData::startup_messages;

// Cheats on/off. If cheats are enabled, saving and multiplayer are disabled until the game is restarted
bool ModData::cheats = false;

// Determines whether node count is displayed on the overlay text feed info header
bool ModData::show_node_count = true;




// Get user-defined settings preferences from the settings file
void ModData::get_user_preferences()
{
	// Buffer for string-related tasks
	char buffer[_DS1_OVERHAUL_SETTINGS_STRING_BUFF_LEN_];
	buffer[0] = '\0';


	// Begin loading setting preferences
	print_console("[Overhaul Mod] Loading user preferences...");

	// Display multiplayer node count in text feed info header
	ModData::show_node_count = ((int)GetPrivateProfileInt(_DS1_OVERHAUL_PREFS_SECTION_, _DS1_OVERHAUL_PREF_SHOW_NODE_COUNT_, 1, _DS1_OVERHAUL_SETTINGS_FILE_) != 0);
	strcpy_s(buffer, _DS1_OVERHAUL_SETTINGS_STRING_BUFF_LEN_, "    Display multiplayer node count = ");
	if (ModData::show_node_count)
		print_console(std::string(buffer).append("enabled").c_str());
	else
		print_console(std::string(buffer).append("disabled").c_str());



	// @TODO Load additional user preferences here



}



// Get user-defined keybinds from the settings file
void ModData::get_user_keybinds()
{
	// Begin loading keybinds
	print_console("[Overhaul Mod] Loading keybinds...");

	
	// Bonfire input fix keybind
	get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_BONFIRE_INPUT_FIX_, kf_fix_bonfire_input);

	// Toggle multiplayer node count in text feed info header
	get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_TOGGLE_NODE_COUNT_, kf_toggle_node_count);



	// @TODO Load additional keybinds here



}



// Helper function for get_user_keybinds() that loads the specified keybind from the config file and binds it to the specified function
void ModData::get_single_user_keybind(const char *keybind_name, int(*function)())
{
	// Variable that holds the Virtual-key code of the keybind when read from settings file
	uint8_t key;

	// Bonfire input fix keybind
	if ( (key = (uint8_t)get_vk_hotkey(_DS1_OVERHAUL_SETTINGS_FILE_, _DS1_OVERHAUL_KEYBINDS_SECTION_, keybind_name)) // Obtain user's preferred key
		&& register_hotkey_function(key, function) )	// Register the keybind
	{
		// Successfully loaded and registered keybind; now print feedback to console
		std::string output = std::string("    Registered ").append(keybind_name).append(" keybind: ");
		output.append(VK_NAME[key]).append(" (0x");   // Get the key name as a string
		if (key < 0x10)
			output += '0';
		std::stringstream hex_stream;
		hex_stream << std::hex << (int)key; // Convert Virtual-key code to hex string
		output.append(hex_stream.str());
		output += ')';
		print_console(output.c_str());
	}
}



// Get custom game archive file name prefix from the settings file
void ModData::get_custom_archive_file(std::wstring *custom_archive_file_prefix)
{
	char custom_archive_prefix_buff[ARCHIVE_FILE_PREFIX_LENGTH + 1];
	custom_archive_prefix_buff[ARCHIVE_FILE_PREFIX_LENGTH] = '\0';
	GetPrivateProfileString(_DS1_OVERHAUL_PREFS_SECTION_,
							_DS1_OVERHAUL_PREF_CUSTOM_GAME_ARCHIVE_,
							NULL,
							custom_archive_prefix_buff,
							ARCHIVE_FILE_PREFIX_LENGTH + 1,
							_DS1_OVERHAUL_SETTINGS_FILE_);

	// Convert string to wide chars
	wchar_t custom_archive_prefix_buff_w[ARCHIVE_FILE_PREFIX_LENGTH + 1];
	custom_archive_prefix_buff_w[ARCHIVE_FILE_PREFIX_LENGTH] = L'\0';
	std::setlocale(LC_ALL, "en_US.utf8");
	size_t chars_converted;
	errno_t return_error = 0;
	if (return_error = mbstowcs_s(&chars_converted, custom_archive_prefix_buff_w, ARCHIVE_FILE_PREFIX_LENGTH + 1, custom_archive_prefix_buff, _TRUNCATE)) {
		// Conversion error
		*custom_archive_file_prefix = L"";
		return;
	}
	
	*custom_archive_file_prefix = std::wstring(custom_archive_prefix_buff_w);
}

