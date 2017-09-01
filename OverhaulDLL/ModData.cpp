/*
	DARK SOULS OVERHAUL
	
	Contributors to this file:
		Sean Pesce	-	C++

*/

#include "DllMain.h"

#define _SP_DEFINE_VK_NAME_STRINGS_		// Must be defined to use Virtual-key code name strings from SP_IO_Strings.hpp (opt-in by default because it increases filesize by a few KB)

#include "SP_IO_Strings.hpp"



/*
	Initialize static variables:
*/


// Signifies whether the mod has finished executing the inititalize_plugin() function
bool ModData::initialized = false;

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



	print_console("");
}



// Get user-defined keybinds from the settings file
void ModData::get_user_keybinds()
{
	// Begin loading keybinds
	print_console("[Overhaul Mod] Loading keybinds...");


	// Variable that holds each keybind when read from settings file
	uint8_t key = 0;

	
	// Bonfire input fix keybind
	if ( (key = (uint8_t)get_vk_hotkey(_DS1_OVERHAUL_SETTINGS_FILE_, _DS1_OVERHAUL_KEYBINDS_SECTION_, _DS1_OVERHAUL_HOTKEY_BONFIRE_INPUT_FIX_)) // Obtain user's preferred key
		&& register_hotkey_function(key, GameData::fix_bonfire_input) )	// Register the keybind
	{
		// Successfully loaded and registered keybind; now print feedback to console
		std::string output = "    Registered " _DS1_OVERHAUL_HOTKEY_BONFIRE_INPUT_FIX_ " keybind: ";
		output.append(VK_NAME[key]).append(" (0x");   // Get the key name as a string
		if (key < 0x10)
			output += '0';
		std::stringstream hex_stream;
		hex_stream << std::hex << (int)key; // Convert Virtual-key code to hex string
		output.append(hex_stream.str());
		output += ')';
		print_console(output.c_str());
	}
	key = 0;



	// @TODO Load additional keybinds here



	print_console("");
}


