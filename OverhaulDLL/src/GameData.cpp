/*
	DARK SOULS OVERHAUL
	
	Contributors to this file:
		Ashley						-	Reverse engineering, Low FPS Disconnect patch technique
		jellybaby34					-	Game version number patch technique
		Metal-Crow					-	Reverse engineering, Phantom Limit patch, C++
		Sean Pesce					-	C++, automataed Bonfire Input Fix (FPSFix+)
		Youri "NullBy7e" de Mooij	-	Original Bonfire Input Fix technique (FPSFix)
		Zullie The Witch			-	Toggle armor sounds, Dim Lava Effects (C++ conversions by Sean)
	
*/

#include "DllMain.h"
#include "Shlobj.h"
#include "SP_IO.hpp"


/*
	Initialize constants:
*/

// Default file types for game archive files (wide char)
const wchar_t *ARCHIVE_FILE_TYPE_W[2] = { L".bdt", L".bhd5" };

// Default file types for game archive files (char)
const char *ARCHIVE_FILE_TYPE[2] = { ".bdt", ".bhd5" };

// Default filename prefix and suffix for game save file
const wchar_t *DEFAULT_SAVE_FILE_SUFFIX = L"0005.sl2";

// Default filename for game config file
const wchar_t *DEFAULT_GAME_CONFIG_FILE = L"DarkSouls.ini";


/*
	Initialize static variables:
*/

// Base address of Dark Souls game process
void *Game::ds1_base = NULL;

// Base address for player character data
void *Game::player_char_base = NULL;

// Player character status (loading, human, co-op, invader, hollow)
SpPointer Game::player_char_status;

// Flag to determine if any characters have been loaded since the game was launched (useful if player had a character loaded but returned to main menu)
bool Game::characters_loaded = false;

// File extension for param def files
const char *BaseParamDef::extension = ".paramdef";

// Address of lava brightness effect (used for dimming lava)
uint8_t *Game::lava_luminosity = NULL;

// Game saving on/off
SpPointer Game::saves_enabled;

// Multiplayer node count
int Game::node_count = -1;





// Initializes pointers that depend on the game's base address
void Game::init_pointers()
{
	print_console(std::string(Mod::output_prefix + "Initializing pointers...").c_str());

	// Obtain base address for player character data
	Game::player_char_base = (void*)((unsigned int)Game::ds1_base + 0xF7E204);

	// Player character status (loading, human, co-op, invader, hollow)
	Game::player_char_status = SpPointer(Game::player_char_base, { 0xA28 });

	// Game saving on/off
	Game::saves_enabled = SpPointer((void*)0x13784A0, { 0xB40 });
	
}


// Performs tasks that were deferred until a character was loaded
void Game::on_first_character_loaded()
{
	Game::characters_loaded = true;

	// Obtain lava effect address
	if (Game::lava_luminosity == NULL)
		Game::lava_luminosity = (uint8_t*)aob_scan("66 66 26 40 00 00 80 3F 00 00 80 3F 00 00 80 3F 00 00 80 3F 00 00 80 40");

	if (Mod::dim_lava_pref)
		Game::enable_dim_lava(true);

	// Get params def files
	ParamDef::Armor().init(true);
	ParamDef::CamLock().init(true);

	// Disable armor sounds if it was specified in the config file
	if (Mod::disable_armor_sfx_pref)
		Game::enable_armor_sfx(false);
}


// Obtains the current game version number
uint8_t Game::get_game_version()
{
	return *((uint8_t*)Game::ds1_base + 0x7E719F);
}


// Changes the game version number to avoid compatibility issues with different game builds
void Game::set_game_version(uint8_t version_number)
{
	std::stringstream hex_stream;
	hex_stream << std::hex << (int)version_number;
	std::string new_version_str = hex_stream.str();
	hex_stream.str(""); // Clear string stream
	hex_stream << std::hex << (int)Game::get_game_version();

	Mod::startup_messages.push_back(std::string(Mod::output_prefix + "Changing game version number from 0x").append(hex_stream.str()).append(" to 0x").append(new_version_str).append("..."));
	uint8_t patch1[5] = { 0xC6, 0x44, 0x24, 0x1C, version_number }; // mov byte ptr [esp+0x1C],0x3C
	uint8_t patch2[3] = { 0x80, 0x38, version_number }; // cmp byte ptr [eax],0x3C

	void *write_address = (uint8_t*)Game::ds1_base + 0x7E73FA;
	apply_byte_patch(write_address, patch1, 5);

	write_address = (uint8_t*)Game::ds1_base + 0x7E719D;
	apply_byte_patch(write_address, patch2, 3);

	write_address = (uint8_t*)Game::ds1_base + 0x7E7229;
	apply_byte_patch(write_address, patch2, 3);
}


// Checks if the player is stuck at the bonfire, and if so, automatically applies the bonfire input fix
void Game::check_bonfire_input_bug()
{
	// Check that the mod is fully loaded
	if (!Mod::initialized)
		return;

	/*
		If glitch conditions do not exist, this variable is set to 0.
		When bonfire input glitch is detected and first_detected is 0, the time is logged in first_detected.

		If the glitch is detected, and the time in first_detected is more than 1 second ago, the bonfire input fix is applied.
	*/
	static DWORD first_detected; // (Static variable persists between function calls)

	// Get current player status
	int status = DS1_PLAYER_STATUS_LOADING;
	if (player_char_status.read(&status) != ERROR_SUCCESS)
	{
		// Error reading memory location
		first_detected = 0;
		return;
	}


	// Check for bonfire input glitch conditions
	if (status == DS1_PLAYER_STATUS_HOLLOW || status == DS1_PLAYER_STATUS_HUMAN) // Check if player is hollow/human
	{
		// Nested if-statements are necessary to avoid crashes related to resolving multi-level pointers

		// Get pointer to current character animation
		SpPointer bonfire_anim_fix = SpPointer((void*)0x12E29E8, { 0x0, 0xFC });

		// Read current character animation
		uint32_t current_anim;
		if (bonfire_anim_fix.read(&current_anim) != ERROR_SUCCESS)
		{
			// Error reading memory location
			first_detected = 0;
			return;
		}


		// Read bonfire menu flag
		SpPointer bonfire_menu_flag = SpPointer((void*)((uint32_t)GetModuleHandle(NULL) + 0xF786D0), { 0x40 });
		uint8_t bonfire_menu_is_open;
		if (bonfire_menu_flag.read(&bonfire_menu_is_open) != ERROR_SUCCESS)
		{
			first_detected = 0;
			return;
		}

		// Read repair menu flag
		SpPointer repair_menu_flag = SpPointer((void*)((uint32_t)GetModuleHandle(NULL) + 0xF786D0), { 0x4C });
		uint8_t repair_menu_is_open;
		if (repair_menu_flag.read(&repair_menu_is_open) != ERROR_SUCCESS)
		{
			first_detected = 0;
			return;
		}

		// Read level up menu flag
		SpPointer level_menu_flag = SpPointer((void*)((uint32_t)GetModuleHandle(NULL) + 0xF786D0), { 0x78 });
		uint8_t level_menu_is_open;
		if (level_menu_flag.read(&level_menu_is_open) != ERROR_SUCCESS)
		{
			first_detected = 0;
			return;
		}

		// Read bottomless box menu flag
		SpPointer bottomless_menu_flag = SpPointer((void*)((uint32_t)GetModuleHandle(NULL) + 0xF786D0), { 0x84 });
		uint8_t bottomless_menu_is_open;
		if (bottomless_menu_flag.read(&bottomless_menu_is_open) != ERROR_SUCCESS)
		{
			first_detected = 0;
			return;
		}

		// Read attune magic menu flag
		SpPointer attune_menu_flag = SpPointer((void*)((uint32_t)GetModuleHandle(NULL) + 0xF786D0), { 0x80 });
		uint8_t attune_menu_is_open;
		if (attune_menu_flag.read(&attune_menu_is_open) != ERROR_SUCCESS)
		{
			first_detected = 0;
			return;
		}

		// Read reinforce weapon/armor menu flag
		SpPointer reinforce_menu_flag = SpPointer((void*)((uint32_t)GetModuleHandle(NULL) + 0xF786D0), { 0x50 });
		uint8_t reinforce_menu_is_open;
		if (reinforce_menu_flag.read(&reinforce_menu_is_open) != ERROR_SUCCESS)
		{
			first_detected = 0;
			return;
		}

		// Read warp menu flag
		SpPointer warp_menu_flag = SpPointer((void*)((uint32_t)GetModuleHandle(NULL) + 0xF786D0), { 0xAC });
		uint8_t warp_menu_is_open;
		if (warp_menu_flag.read(&warp_menu_is_open) != ERROR_SUCCESS)
		{
			first_detected = 0;
			return;
		}

		// Read "reverse hollowing?"/"can't kindle"/"can't reverse hollowing"/"insufficient attunement slots" dialog flag
		SpPointer dialog_menu_flag = SpPointer((void*)((uint32_t)GetModuleHandle(NULL) + 0xF786D0), { 0x60 });
		uint8_t dialog_menu_is_open;
		if (dialog_menu_flag.read(&dialog_menu_is_open) != ERROR_SUCCESS)
		{
			first_detected = 0;
			return;
		}


		// Check bonfire input fix conditions
		if ( (current_anim == 7701 || current_anim == 7711 || current_anim == 7721) // 3 different bonfire resting animation IDs
			&& !bonfire_menu_is_open && !repair_menu_is_open && !bottomless_menu_is_open && !reinforce_menu_is_open
			&& !level_menu_is_open && !attune_menu_is_open && !dialog_menu_is_open && !warp_menu_is_open) // Make sure no bonfire menus are open
		{
			// Bonfire input glitch conditions currently exist

			if (first_detected == 0)
			{
				// First detection; log time
				first_detected = GetTickCount();
				return;
			}
			else if ((GetTickCount() - first_detected) >= 1000)
			{
				// Too much time has elapsed; bonfire input glitch is definitely present
				fix_bonfire_input();
				print_console(_DS1_MOD_MSG_BONFIRE_INPUT_FIX_AUTOMATIC_);
			}
			else
			{
				// Not enough time has elapsed; return and check again later
				return;
			}
		}
	}

	first_detected = 0;
}


// Fixes input bug that causes players to be stuck at a bonfire (usually after turning human with framerate unlocked)
int Game::fix_bonfire_input(bool print_to_text_feed, bool print_to_console)
{
	// Get current player status
	int status = DS1_PLAYER_STATUS_LOADING;
	if (player_char_status.read(&status) != ERROR_SUCCESS)
	{
		// Error reading memory location
		if (print_to_text_feed)
			print(_DS1_MOD_MSG_BONFIRE_INPUT_FIX_NO_INIT_FAIL_);

		if (print_to_console)
			print_console(_DS1_MOD_MSG_BONFIRE_INPUT_FIX_NO_INIT_FAIL_);
		return ERROR_INVALID_ADDRESS;
	}


	if (status == DS1_PLAYER_STATUS_HOLLOW || status == DS1_PLAYER_STATUS_HUMAN) // Check if player is hollow/human
	{
		// Get pointer to current character animation
		SpPointer bonfire_anim_fix = SpPointer((void*)0x12E29E8, { 0x0, 0xFC });

		// Read current character animation
		uint32_t current_anim;
		bonfire_anim_fix.read(&current_anim);

		// Check that player is currently sitting at a bonfire
		if (current_anim == 7701 || current_anim == 7711 || current_anim == 7721) // 3 different bonfire resting animation IDs
		{
			// Write zero to bonfire animation status address
			bonfire_anim_fix.write((uint32_t)0);

			if (print_to_text_feed)
				print(_DS1_MOD_MSG_BONFIRE_INPUT_FIX_SUCCESS_);

			if (print_to_console)
				print_console(_DS1_MOD_MSG_BONFIRE_INPUT_FIX_SUCCESS_);

			return ERROR_SUCCESS;
		}
		else
		{
			// Player is not at a bonfire
			if (print_to_text_feed)
				print(_DS1_MOD_MSG_BONFIRE_INPUT_FIX_NOT_AT_BONFIRE_FAIL_);

			if (print_to_console)
				print_console(_DS1_MOD_MSG_BONFIRE_INPUT_FIX_NOT_AT_BONFIRE_FAIL_);
		}
	}
	else
	{
		// Player is not hollow/human, so can't be at a bonfire
		if (status == DS1_PLAYER_STATUS_LOADING)
		{
			if (print_to_text_feed)
				print(_DS1_MOD_MSG_BONFIRE_INPUT_FIX_NO_CHAR_FAIL_);

			if (print_to_console)
				print_console(_DS1_MOD_MSG_BONFIRE_INPUT_FIX_NO_CHAR_FAIL_);
		}
		else
		{
			if (print_to_text_feed)
				print(_DS1_MOD_MSG_BONFIRE_INPUT_FIX_NOT_AT_BONFIRE_FAIL_);

			if (print_to_console)
				print_console(_DS1_MOD_MSG_BONFIRE_INPUT_FIX_NOT_AT_BONFIRE_FAIL_);
		}
	}

	return ERROR_BAD_ENVIRONMENT;
}


// Check if dim lava mod is currently active
bool Game::dim_lava_enabled()
{
	if (Game::lava_luminosity == NULL)
		Game::lava_luminosity = (uint8_t*)aob_scan("66 66 26 40 00 00 80 3F 00 00 80 3F 00 00 80 3F 00 00 80 3F 00 00 80 40");

	if (Game::lava_luminosity == NULL)
	{
		// Unable to find lava brightness effects in memory
		set_error(ERROR_FILE_NOT_FOUND);
		return false;
	}

	uint8_t check_lava[4] = { 0x66, 0x66, 0x26, 0x40 };
	for (int i = 0; i < 4; i++)
		if (check_lava[i] != Game::lava_luminosity[i])
			return true;

	return false;
}

// Dim lava effects or restore default lava visuals
void Game::enable_dim_lava(bool dim)
{
	if (dim != Game::dim_lava_enabled() && Game::lava_luminosity != NULL)
	{
		uint8_t patch_bytes[4];

		if (dim)
		{
			// Dim lava effects
			patch_bytes[0] = 0xD6, patch_bytes[1] = 0xCC, patch_bytes[2] = 0x4C, patch_bytes[3] = 0x3E;
			apply_byte_patch(Game::lava_luminosity, patch_bytes, 4);

			patch_bytes[0] = 0x00, patch_bytes[0] = 0x00, patch_bytes[0] = 0x80, patch_bytes[0] = 0x3F;
			apply_byte_patch(Game::lava_luminosity + 0xAE0, patch_bytes, 4);
			apply_byte_patch(Game::lava_luminosity + 0x1050, patch_bytes, 4);
		}
		else
		{
			// Restore default lava effects
			patch_bytes[0] = 0x66, patch_bytes[1] = 0x66, patch_bytes[2] = 0x26, patch_bytes[3] = 0x40;
			apply_byte_patch(Game::lava_luminosity, patch_bytes, 4);

			patch_bytes[0] = 0x00, patch_bytes[0] = 0x00, patch_bytes[0] = 0x30, patch_bytes[0] = 0x40;
			apply_byte_patch(Game::lava_luminosity + 0xAE0, patch_bytes, 4);

			patch_bytes[0] = 0x00, patch_bytes[0] = 0x00, patch_bytes[0] = 0x80, patch_bytes[0] = 0x40;
			apply_byte_patch(Game::lava_luminosity + 0x1050, patch_bytes, 4);
		}
	}
	else if(Game::lava_luminosity == NULL)
		set_error(ERROR_FILE_NOT_FOUND);
}


// Checks if armor sound effects are enabled
bool Game::armor_sfx_enabled()
{
	ParamDef::Armor().init(true);

	ArmorParam *first_param = ParamDef::Armor().data();

	return (first_param->defenseMaterial == 59 && first_param->defenseMaterial_Weak == 29);
}


// Toggles armor sound effecs
void Game::enable_armor_sfx(bool enable)
{
	ParamDef::Armor().init(true);


	// Static variable persists between function calls
	static std::vector<std::vector<uint8_t>> default_armor_sfx_values;

	// Check if default armor sound effects have been stored yet
	bool backup_defaults = default_armor_sfx_values.empty();


	for (int i = 0; i < (int)ParamDef::Armor().param_count; i++)
	{
		// First time, store default armor sound effects
		if (backup_defaults)
			default_armor_sfx_values.push_back({ 
						ParamDef::Armor().get(i)->defenseMaterial,
						ParamDef::Armor().get(i)->defenseMaterial_Weak });
			
		if (enable)
		{
			ParamDef::Armor().get(i)->defenseMaterial = default_armor_sfx_values.at(i).at(0);
			ParamDef::Armor().get(i)->defenseMaterial_Weak = default_armor_sfx_values.at(i).at(1);
		}
		else
		{
			ParamDef::Armor().get(i)->defenseMaterial = 0;
			ParamDef::Armor().get(i)->defenseMaterial_Weak = 0;
		}
	}
}


// Returns multiplayer node count as an int (or -1 if player is not online)
int Game::get_node_count()
{
	SpPointer node_count_ptr = SpPointer((uint8_t*)ds1_base + 0xF7F77C, { 0x2C, 0x778, 0x80 });

	// Get node count
	int nodes = -1;
	node_count_ptr.read(&nodes);
	return nodes;
}




/////////////////////////////////////////
//////////////// PATCHES ////////////////
/////////////////////////////////////////


// Disables automatic game disconnection when low framerate is detected
void Game::enable_low_fps_disconnect(bool enable)
{
	uint8_t *fps_warn = NULL;
	fps_warn = (uint8_t*)aob_scan("74 17 B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B"); // WARNING: this pattern returns 2 results

	if (fps_warn)
	{
		// AoB Scan was successful
		set_mem_protection(fps_warn, 1, MEM_PROTECT_RWX);
		fps_warn -= 0x1E;
		
		if (enable)
		{
			print_console(Mod::output_prefix + "Enabling low FPS disconnect...");
			*fps_warn = 0x51; // Enable low FPS disconnect
		}
		else
		{
			print_console(Mod::output_prefix + "Disabling low FPS disconnect...");
			*fps_warn = 0xC3; // Disable low FPS disconnect
		}
			
	}
}

// Increase available pool of memory Dark Souls allocates itself
void Game::increase_memory_limit()
{
	Mod::startup_messages.push_back(Mod::output_prefix + "Increasing available memory...");

	uint8_t patch[5] = { 0x68, 0x00, 0x00, 0x00, 0x06 }; // push 0x06000000. The constant can be increased as desired, and represents dark souls total memory pool

	void *write_address = (uint8_t*)Game::ds1_base + 0xB8B7E5;
	apply_byte_patch(write_address, patch, 5);

	write_address = (uint8_t*)Game::ds1_base + 0x9E20;
	apply_byte_patch(write_address, patch, 5);

	write_address = (uint8_t*)Game::ds1_base + 0x9E41;
	apply_byte_patch(write_address, patch, 5);
}


// Set the .bdt files to be loaded by the game (WARNING: archive_name parameter must be exactly 6 characters)
void Game::load_custom_bdt_files(wchar_t *archive_name)
{
	if (archive_name == NULL || (int)std::wstring(archive_name).length() == 0)
	{
		return;
	}

	Mod::startup_messages.push_back(std::string(Mod::output_prefix + "Checking if custom game archive files exist..."));

	// Check that the custom archive name prefix is the correct length
	size_t custom_name_len = 0;
	if ((custom_name_len = (int)std::wstring(archive_name).length()) != ARCHIVE_FILE_PREFIX_LENGTH)
	{
		Mod::startup_messages.push_back(std::string(Mod::output_prefix + "ERROR: Custom archive name prefix was invalid length (").append(std::to_string(custom_name_len)).append("). Using default archive files instead."));
		return;
	}

	// Get char* strings for printing console messages
	errno_t conversion_return;
	std::string archive_name_ch = "";
	if(conversion_return = string_wide_to_mb(archive_name, archive_name_ch))
	{
		// Error converting from wide char to char
		Mod::startup_messages.push_back(std::string(Mod::output_prefix + "ERROR: Unable to parse custom archive file name (Error code ").append(std::to_string(conversion_return)).append("). Using default archive files instead."));
		return;
	}


	// Check that custom game archive files exist
	for (int i = 0; i < ARCHIVE_FILE_PAIR_COUNT; i++)
	{
		std::ifstream check_file(std::wstring(archive_name).append(std::to_wstring(i)).append(ARCHIVE_FILE_TYPE_W[0]).c_str());
		if (!check_file.good())
		{
			// Custom .bdt file doesn't exist
			Mod::startup_messages.push_back(std::string(Mod::output_prefix + "ERROR: The file \"").append(archive_name_ch).append(std::to_string(i)).append(ARCHIVE_FILE_TYPE[0]).append("\" could not be found. Using default archive files instead."));
			check_file.close();
			return;
		}
		else
			Mod::startup_messages.push_back(std::string("    Found ").append(archive_name_ch).append(std::to_string(i)).append(ARCHIVE_FILE_TYPE[0]));
		check_file.close();
		std::ifstream check_file2(std::wstring(archive_name).append(std::to_wstring(i)).append(ARCHIVE_FILE_TYPE_W[1]).c_str());
		if (!check_file2.good())
		{
			// Custom .bhd5 file doesn't exist
			Mod::startup_messages.push_back(std::string(Mod::output_prefix + "ERROR: The file \"").append(archive_name_ch).append(std::to_string(i)).append(ARCHIVE_FILE_TYPE[1]).append("\" could not be found. Using default archive files."));
			check_file2.close();
			return;
		}
		else
			Mod::startup_messages.push_back(std::string("    Found ").append(archive_name_ch).append(std::to_string(i)).append(ARCHIVE_FILE_TYPE[1]));
		check_file2.close();
	}
	
	
	Mod::startup_messages.push_back(std::string(Mod::output_prefix + "SUCCESS: Custom game archive files will be loaded (\"").append(archive_name_ch).append("\")."));
	void *dvd0_bdt = (uint8_t*)Game::ds1_base + 0xD63AF6;
	apply_byte_patch(dvd0_bdt, archive_name, 12);
	void *dvd0_bhd = (uint8_t*)Game::ds1_base + 0xD63B22;
	apply_byte_patch(dvd0_bhd, archive_name, 12);
	void *dvd1_bdt = (uint8_t*)Game::ds1_base + 0xD63B5E;
	apply_byte_patch(dvd1_bdt, archive_name, 12);
	void *dvd1_bhd = (uint8_t*)Game::ds1_base + 0xD63B8A;
	apply_byte_patch(dvd1_bhd, archive_name, 12);
	void *dvd2_bdt = (uint8_t*)Game::ds1_base + 0xD63BC6;
	apply_byte_patch(dvd2_bdt, archive_name, 12);
	void *dvd2_bhd = (uint8_t*)Game::ds1_base + 0xD63BF2;
	apply_byte_patch(dvd2_bhd, archive_name, 12);
	void *dvd3_bdt = (uint8_t*)Game::ds1_base + 0xD63C2E;
	apply_byte_patch(dvd3_bdt, archive_name, 12);
	void *dvd3_bhd = (uint8_t*)Game::ds1_base + 0xD63C5A;
	apply_byte_patch(dvd3_bhd, archive_name, 12);
}


// Set the *.0005.sl2 file that will be loaded by the game (WARNING: filename_prefix parameter must be exactly 5 characters)
void Game::load_custom_save_file(wchar_t *filename_prefix)
{
	if (filename_prefix == NULL || (int)std::wstring(filename_prefix).length() == 0)
	{
		return;
	}

	Mod::startup_messages.push_back(std::string(Mod::output_prefix + "Checking if custom save file exists..."));

	// Check that the custom filename prefix is the correct length
	size_t custom_name_len = 0;
	if ((custom_name_len = (int)std::wstring(filename_prefix).length()) != SAVE_FILE_PREFIX_LENGTH)
	{
		Mod::startup_messages.push_back(std::string(Mod::output_prefix + "ERROR: Custom game save file name prefix was invalid length (").append(std::to_string(custom_name_len)).append("). Using default save file instead."));
		return;
	}

	// Get char* strings for printing console messages
	errno_t conversion_return;
	std::string file_prefix_ch = "";
	std::string file_suffix_ch = "";
	if ((conversion_return = string_wide_to_mb(filename_prefix, file_prefix_ch)) || (conversion_return = string_wide_to_mb((wchar_t*)DEFAULT_SAVE_FILE_SUFFIX, file_suffix_ch)))
	{
		// Error converting from wide char to char
		Mod::startup_messages.push_back(std::string(Mod::output_prefix + "ERROR: Unable to parse custom save file name (Error code ").append(std::to_string(conversion_return)).append("). Using default save file instead."));
		return;
	}

	// Check that custom save file exists
	bool found_file = false; // If true, the Documents folder couldn't be found
	char docs_path[MAX_PATH];
	docs_path[MAX_PATH - 1] = '\0';
	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, docs_path)))
	{
		std::ifstream check_file(((std::string(docs_path) + "\\NBGI\\DarkSouls\\" + file_prefix_ch + file_suffix_ch).c_str()));
		found_file = check_file.good();
		check_file.close();
	}
	
	// If file wasn't in user Documents, try the shared Documents folder
	//if (!found_file && SUCCEEDED(SHGetFolderPath(NULL, CSIDL_COMMON_DOCUMENTS, NULL, SHGFP_TYPE_CURRENT, docs_path)))
	//{
	//	std::ifstream check_file(((std::string(docs_path) + "\\NBGI\\DarkSouls\\" + file_prefix_ch + file_suffix_ch).c_str()));
	//	found_file = check_file.good();
	//	check_file.close();
	//}


	if (!found_file)
	{
		// Custom save file doesn't exist
		Mod::startup_messages.push_back(std::string(Mod::output_prefix + "ERROR: The file \"").append(file_prefix_ch).append(file_suffix_ch).append("\" could not be found. Using default save file instead."));
		return;
	}
	else
		Mod::startup_messages.push_back(std::string("    Found ").append(file_prefix_ch).append(file_suffix_ch));


	Mod::startup_messages.push_back(std::string(Mod::output_prefix + "SUCCESS: Custom save file will be loaded (\"").append(file_prefix_ch).append(file_suffix_ch).append("\")."));
	void *save_file = (uint8_t*)Game::ds1_base + 0xD6A460;
	apply_byte_patch(save_file, filename_prefix, 10);
}


// Set the config file that will be loaded by the game (WARNING: filename parameter must be exactly 13 characters)
void Game::load_custom_game_config_file(wchar_t *filename)
{
	if (filename == NULL || (int)std::wstring(filename).length() == 0)
	{
		return;
	}

	Mod::startup_messages.push_back(std::string(Mod::output_prefix + "Checking if custom game config file exists..."));

	// Check that the custom filename is the correct length
	size_t custom_name_len = 0;
	if ((custom_name_len = (int)std::wstring(filename).length()) != GAME_CONFIG_FILE_NAME_LENGTH)
	{
		Mod::startup_messages.push_back(std::string(Mod::output_prefix + "ERROR: Custom game config file name was invalid length (").append(std::to_string(custom_name_len)).append("). Using default config file instead."));
		return;
	}

	// Get char* strings for printing console messages
	errno_t conversion_return;
	std::string filename_ch = "";
	if (conversion_return = string_wide_to_mb(filename, filename_ch))
	{
		// Error converting from wide char to char
		Mod::startup_messages.push_back(std::string(Mod::output_prefix + "ERROR: Unable to parse custom config file name (Error code ").append(std::to_string(conversion_return)).append("). Using default config file instead."));
		return;
	}

	// Check that custom config file exists
	bool found_file = false; // If true, the AppData folder couldn't be found
	char appdata_path[MAX_PATH];
	appdata_path[MAX_PATH - 1] = '\0';
	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, appdata_path)))
	{
		appdata_path[std::string(appdata_path).find_last_of('\\')] = '\0';
		std::ifstream check_file(((std::string(appdata_path) + "\\Local\\NBGI\\DarkSouls\\" + filename_ch).c_str()));
		found_file = check_file.good();
		check_file.close();
	}
	
	// If file wasn't in user AppData, try the shared AppData folder
	//if (!found_file && SUCCEEDED(SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, SHGFP_TYPE_CURRENT, docs_path)))
	//{
	//	appdata_path[std::string(appdata_path).find_last_of('\\')] = '\0';
	//	std::ifstream check_file(((std::string(appdata_path) + "\\Local\\NBGI\\DarkSouls\\" + filename_ch).c_str()));
	//	found_file = check_file.good();
	//	check_file.close();
	//}


	if (!found_file)
	{
		// Custom config file doesn't exist
		Mod::startup_messages.push_back(std::string(Mod::output_prefix + "ERROR: The file \"").append(filename_ch).append("\" could not be found. Using default config file instead."));
		return;
	}
	else
		Mod::startup_messages.push_back(std::string("    Found ").append(filename_ch));


	Mod::startup_messages.push_back(std::string(Mod::output_prefix + "SUCCESS: Custom game config file will be loaded (\"").append(filename_ch).append("\")."));
	void *cfg_file = (uint8_t*)Game::ds1_base + 0xD6C7AC;
	apply_byte_patch(cfg_file, filename, 26);
}
