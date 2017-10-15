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


// Used in console messages to inform users that a message is being printed by the Overhaul mod
const std::string Mod::output_prefix = "[Overhaul Mod] ";

// Set to true after inititalize_plugin() has executed
bool Mod::initialized = false;

// Set to true after deferred_tasks() has executed (or can be set to true to cancel unfinished deferred tasks)
bool Mod::deferred_tasks_complete = false;

// Console messages from events that took place before the in-game console was loaded
std::vector<std::string> Mod::startup_messages;

// List of supported game versions
std::vector<uint8_t> Mod::supported_game_versions = { DS1_VERSION_RELEASE };

// Determines whether to start in legacy mode (only applies fixes, no gameplay changes)
bool Mod::legacy_mode = false;

// Cheats on/off. If cheats are enabled, saving and multiplayer are disabled until the game is restarted
bool Mod::cheats = false;

// Determines whether node count is displayed on the overlay text feed info header
bool Mod::show_node_count = true;

// User preference setting; determines whether the brightness of lava visual effects should be lowered
bool Mod::dim_lava_pref = false;

// User preference setting; determines whether armor sound effects will be disabled
bool Mod::disable_armor_sfx_pref = false;

// Custom game archive files to load instead of the vanilla game files
std::wstring Mod::custom_game_archives;



// Get user-defined startup preferences from the settings file
void Mod::get_startup_preferences()
{

	// Begin loading startup preferences
	Mod::startup_messages.push_back(Mod::output_prefix + "Loading startup preferences...");

	// Check if legacy mode is enabled
	Mod::legacy_mode = ((int)GetPrivateProfileInt(_DS1_OVERHAUL_PREFS_SECTION_, _DS1_OVERHAUL_PREF_LEGACY_MODE_, (int)Mod::legacy_mode, _DS1_OVERHAUL_SETTINGS_FILE_) != 0);
	if (Mod::legacy_mode)
		Mod::startup_messages.push_back("    Legacy mode enabled. Gameplay changes will not be applied.");

	// Check for custom game archive files
	Mod::get_custom_archive_files();

	// @TODO Load additional startup preferences here


}



// Get user-defined settings preferences from the settings file
void Mod::get_user_preferences()
{
	// Buffer for reading string preferences
	char buffer[_DS1_OVERHAUL_SETTINGS_STRING_BUFF_LEN_];
	buffer[0] = '\0';

	// Temporary string for printing console messages
	std::string msg;


	// Begin loading setting preferences
	print_console(Mod::output_prefix + "Loading user preferences...");

	// Display multiplayer node count in text feed info header
	Mod::show_node_count = ((int)GetPrivateProfileInt(_DS1_OVERHAUL_PREFS_SECTION_, _DS1_OVERHAUL_PREF_SHOW_NODE_COUNT_, (int)Mod::show_node_count, _DS1_OVERHAUL_SETTINGS_FILE_) != 0);
	msg = "    Display multiplayer node count = ";
	if (Mod::show_node_count)
		print_console(msg.append("enabled").c_str());
	else
		print_console(msg.append("disabled").c_str());


	// Check whether to lower the brightness of lava effects
	Mod::dim_lava_pref = ((int)GetPrivateProfileInt(_DS1_OVERHAUL_PREFS_SECTION_, _DS1_OVERHAUL_PREF_DIM_LAVA_, 0, _DS1_OVERHAUL_SETTINGS_FILE_) != 0);
	if (Mod::dim_lava_pref)
		print_console("    Lava visual effects will be dimmed when a character is loaded");


	// Check whether to disable armor sound effects
	Mod::disable_armor_sfx_pref = ((int)GetPrivateProfileInt(_DS1_OVERHAUL_PREFS_SECTION_, _DS1_OVERHAUL_PREF_DISABLE_ARMOR_SFX_, 0, _DS1_OVERHAUL_SETTINGS_FILE_) != 0);
	if (Mod::disable_armor_sfx_pref)
		print_console("    Armor sound effects will be disabled when a character is loaded");



	// @TODO Load additional user preferences here



}



// Get user-defined keybinds from the settings file
void Mod::get_user_keybinds()
{
	// Begin loading keybinds
	print_console(Mod::output_prefix + "Loading keybinds...");

	
	// Bonfire input fix keybind
	get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_BONFIRE_INPUT_FIX_, kf_fix_bonfire_input);

	// Toggle multiplayer node count in text feed info header
	get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_TOGGLE_NODE_COUNT_, kf_toggle_node_count);


	// Toggle dimmed lava visual effects
	get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_TOGGLE_DIM_LAVA_, kf_toggle_dim_lava);

	// Toggle armor sound effects
	get_single_user_keybind(_DS1_OVERHAUL_HOTKEY_TOGGLE_ARMOR_SFX_, kf_toggle_armor_sfx);


	// @TODO Load additional keybinds here



}



// Helper function for get_user_keybinds() that loads the specified keybind from the config file and binds it to the specified function
void Mod::get_single_user_keybind(const char *keybind_name, int(*function)())
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
void Mod::get_custom_archive_files()
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
		Mod::custom_game_archives = L"";
		return;
	}
	
	Mod::startup_messages.push_back(std::string("    Found custom game file definition: \"").append(custom_archive_prefix_buff).append("\""));

	Mod::custom_game_archives = std::wstring(custom_archive_prefix_buff_w);
}

