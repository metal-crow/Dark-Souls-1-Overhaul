/*
	DARK SOULS OVERHAUL
	
	Contributors to this file:
		Sean Pesce	-	C++


	This file defines the strings and helper functions used
	with the custom console command set in ConsoleCommands.cpp
*/

#pragma once

#ifndef _DS1_OVERHAUL_CONSOLE_COMMAND_STRINGS_H_
	#define _DS1_OVERHAUL_CONSOLE_COMMAND_STRINGS_H_



///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////// CONSOLE COMMAND STRINGS /////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
/*
	Console command names (used to call the commands) and help messages (displayed when using the
	built-in "help" command on another command).

	Console Help Messages are prefixed with "chm"; Console Command Names are prefixed with "ccn",
	and Console Command Aliases are prefixed with "cca".
*/

// Error message displayed when not enough arguments are passed to a command
const std::string ERROR_NOT_ENOUGH_ARGUMENTS = "ERROR: Not enough arguments";

// Error message displayed when an invalid argument is passed to a command that only accepts a boolean argument
const std::string ERROR_INVALID_BOOL_ARGUMENT = "ERROR: Assigned value must be either 1 or 0 (1 = enabled, 0 = disabled)";



#define ccn_developer_debug "dev_debug"
const char *chm_developer_debug = ccn_developer_debug" \n    For developer use.";


#define ccn_dim_lava "dim_lava"
const char *chm_dim_lava = ccn_dim_lava" [boolean]\n     Enables/disables dimmed visual effects for lava (1 = enabled, 0 = disabled).";
#define cca_lava_brightness_fix "lava_brightness_fix"


#define ccn_armor_sfx "armor_sfx"
const char *chm_armor_sfx = ccn_armor_sfx" [boolean]\n     Enables/disables armor sound effects (1 = enabled, 0 = disabled).";
#define cca_armor_sounds "armor_sounds"


#define ccn_fix_bonfire_input "fix_bonfire_input"
const char *chm_fix_bonfire_input = ccn_fix_bonfire_input"\n"
									"    Allows player to regain control of their character if they encounter the bonfire input glitch,\n"
									"    which is a bug that can cause the player to be stuck at the bonfire (usually after restoring\n"
									"    humanity with the framerate unlocked).";



#define ccn_text_feed_node_count "text_feed_node_count"
const char *chm_text_feed_node_count = ccn_text_feed_node_count" [boolean]\n"
									"    Enables/disables the multiplayer node count element of the overlay text feed info bar (1 = enabled, 0 = disabled).";
#define cca_node_count "node_count"


#define ccn_cheats "cheats"
const char *chm_cheats = ccn_cheats" [boolean]\n"
									"    Enables cheats, unlocking additional console commands (1 = enable). If cheats are enabled, saving and multiplayer"
									"    are disabled until the game is restarted.";

#define ccn_legacy_mode "legacy_mode"
const char *chm_legacy_mode = ccn_legacy_mode"\n"
                         "    Checks whether Legacy Mode is enabled (Dark Souls Overhaul Mod).\n    Note: Legacy Mode cannot be toggled. Exit the game and set the LegacyMode setting\n"
                         "    in the Overhaul config file to turn Legacy Mode on/off.";

#define ccn_credits "overhaul_credits"
const char *chm_credits = ccn_credits"\n"
                         "    Plays the credits for the Dark Souls Overhaul Mod.";

#define ccn_hud_compass_radial "hud_compass_radial"
const char *chm_hud_compass_radial = ccn_hud_compass_radial" [boolean] \n"
									"    Enables/disables radial compass HUD element from the Debug build.";

#define ccn_hud_compass_bar "hud_compass_bar"
const char *chm_hud_compass_bar = ccn_hud_compass_bar" [boolean] \n"
									"    Enables/disables bar compass HUD element from the Debug build.";

#define ccn_hud_elevation_meter "hud_elevation_meter"
const char *chm_hud_elevation_meter = ccn_hud_elevation_meter" [boolean] \n"
									"    Enables/disables elevation meter HUD element from the Debug build.";

#define ccn_multiplayer_network "multiplayer_network"
const char *chm_multiplayer_network = ccn_multiplayer_network" \n"
									"    Prints various multiplayer network information (Steam ID, ping, etc.) for each player in the current node network.";






///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////// CONSOLE HELPER FUNCTIONS ////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
/*
	Helper functions for parsing arguments, formatting output, and other useful console tasks.
*/




/*
	Creates a formatted string describing the given error code and
	stores it in the given string pointer.
*/
void error_code_to_string(DWORD last_error, std::string *message)
{
	if (message == NULL)
	{
		// Invalid argument
		SetLastError(ERROR_INVALID_PARAMETER);
		return;
	}

	message->clear();

	// Get the error message
	if (last_error == 0)
	{
		message->append("No error");
		return;
	}

	LPSTR msg_buff = NULL;
	size_t size = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, last_error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&msg_buff, 0, NULL);

	message->append(msg_buff, size);
	if (message->c_str()[message->length() - 1] == '\n')
	{
		message->erase(message->length() - 1, 1); // Remove newline from error message
	}

	// Free buffer memory
	LocalFree(msg_buff);
}


// Returns a string representing a boolean value
void bool_to_string(bool boolean, std::string *str)
{
	if (str == NULL)
		return;

	str->clear();

	if (boolean)
		str->append("true");
	else
		str->append("false");
}


/*
	Removes all leading/trailing characters from a string if they appear
	in the character mask.

	NOTE: The new_mask parameter is optional. By default, this function
	only trims whitespace characters.
*/
void trim(std::string *string, const char *new_mask = " \r\n\t")
{
	if (string == NULL || new_mask == NULL)
	{
		// Invalid arguments
		SetLastError(ERROR_INVALID_PARAMETER);
		return;
	}

	// If no mask is specified, only whitespace is trimmed
	std::string mask = new_mask;
	int start = string->find_first_not_of(mask.c_str());
	string->erase(0, start);

	while (mask.find(*(string->end() - 1)) != std::string::npos)
	{
		string->erase(string->length() - 1, 1);
	}
}


// Returns true if the given string represents a number value of zero in decimal or hexidecimal.
bool string_is_zero(const char *c_str)
{
	if (c_str == NULL)
	{
		return false;
	}

	std::string str(c_str);
	trim(&str);

	if (str.length() == 0)
	{
		return false;
	}

	for (int i = 0; i < (int)str.length(); i++)
	{
		if (i != 1 && str.c_str()[i] != '0')
		{
			return false;
		}
		else if (i == 1 && str.c_str()[i] != '0')
		{
			if (str.length() == 2 || (str.c_str()[i] != 'x' && str.c_str()[i] != 'X'))
			{
				return false;
			}
		}
	}
	return true;
}



// Parses a string argument and determines whether it equals true, false, or neither (invalid).
//	Return value: 1 = true; 0 = false; -1 = invalid argument
int parse_toggle_arg(const char *c_arg)
{
	if (c_arg == NULL)
	{
		return -1;
	}

	std::string arg(c_arg);
	trim(&arg);

	if (arg.length() < 1)
	{
		return -1;
	}
	else if (arg.length() == 1)
	{
		switch (arg.c_str()[0])
		{
			case '1':
				return 1;
				break;
			case '0':
				return 0;
				break;
			default:
				return -1;
				break;
		}
	}

	FileUtil::to_lowercase((char *)arg.c_str());

	if ((strcmp(arg.c_str(), "true") == 0) || (strcmp(arg.c_str(), "on") == 0) || (strcmp(arg.c_str(), "enable") == 0) || (strcmp(arg.c_str(), "enabled") == 0))
	{
		return 1;
	}
	else if ((strcmp(arg.c_str(), "false") == 0) || (strcmp(arg.c_str(), "off") == 0) || (strcmp(arg.c_str(), "disable") == 0) || (strcmp(arg.c_str(), "disabled") == 0) || string_is_zero(arg.c_str()))
	{
		return 0;
	}

	long parsed_val = strtol(arg.c_str(), NULL, 0);
	if (parsed_val == 1)
	{
		return true;
	}

	return -1;
}


/*
	Reconstructs a list of argument strings into a single string formatted as the arguments would be entered in the
	console input, and stores it in the given string pointer.
*/
void args_to_string(std::vector<std::string> args, std::string *str)
{
	if (str == NULL)
	{
		// Invalid argument
		SetLastError(ERROR_INVALID_PARAMETER);
		return;
	}

	for (auto macro_arg : args)
	{
		std::string arg = macro_arg;

		if (arg.length() == 0 || (arg.find_first_of(" '\n\"\t\r") != std::string::npos))
		{
			// Argument contains whitespace/quotes, so must be a string argument
			int last_match = 0;
			while ((last_match < (int)arg.length()) && (last_match = arg.find_first_of("'", last_match)) != std::string::npos)
			{
				arg.insert(last_match, "\\");
				last_match += 2; // +1 for new '\\' char, +1 to move past the '\'' char
			}

			arg.append("'");
			arg.insert(0, "'");
		}
		arg.insert(0, " ");
		str->append(arg);
	}
}




#endif // _DS1_OVERHAUL_CONSOLE_COMMAND_STRINGS_H_