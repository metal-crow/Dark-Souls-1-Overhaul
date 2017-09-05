/*
	DARK SOULS OVERHAUL
	
	Contributors to this file:
		Sean Pesce	-	C++, DirectX9 overlay


	This file defines custom console commands that will
	be registered with the in-game console at runtime.
*/

#include "DllMain.h"
#include "ConsoleCommands.h"


///////////////////////////////////////////////////////////
//////////////// Console command functions ////////////////
///////////////////////////////////////////////////////////



// Enables cheats until game is restarted
int cc_cheats(std::vector<std::string> args, std::string *output)
{
	int return_val = CONSOLE_COMMAND_SUCCESS;

	if (args.size() > 0)
	{
		switch (parse_toggle_arg(args.at(0).c_str()))
		{
			case 0:
				if (ModData::cheats)
					output->append("Restart the game to disable cheats and re-enable saving and multiplayer.");
				else
					output->append("Cheats = disabled");
				break;
			case 1:
				if (!ModData::cheats)
				{
					GameData::saves_enabled.write(false);
					// @TODO: Disable multiplayer
					print("WARNING: Cheats enabled. Saving and multiplayer functions disabled. Restart game to disable cheats", 0, false, SP_D3D9O_TEXT_COLOR_RED);
				}
				else
					output->append("Cheats = enabled. Saving and multiplayer functions have been disabled. Restart the game to disable cheats.");
				ModData::cheats = true;
				break;
			default:
				output->append("ERROR: Assigned value must be either 1 or 0 (1 = enabled, 0 = disabled)\n");
				if (ModData::cheats)
					output->append("Cheats = enabled. Saving and multiplayer functions have been disabled. Restart the game to disable cheats.");
				else
					output->append("Cheats = disabled");
				return_val = ERROR_INVALID_PARAMETER;
				break;
		}
	}
	else
	{
		if (ModData::cheats)
			output->append("Cheats = enabled. Saving and multiplayer functions have been disabled. Restart the game to disable cheats.");
		else
			output->append("Cheats = disabled");
	}

	return return_val;
}



// Applies the Bonfire input fix
int cc_fix_bonfire_input(std::vector<std::string> args, std::string *output)
{
	// Apply fix and print message to console (and not to text feed)
	return GameData::fix_bonfire_input(false, true);
}



// Enables/disables multiplayer node count element of overlay text feed
int cc_text_feed_node_count(std::vector<std::string> args, std::string *output)
{
	int return_val = CONSOLE_COMMAND_SUCCESS;

	if (args.size() > 0)
	{
		switch (parse_toggle_arg(args.at(0).c_str()))
		{
			case 0:
				ModData::show_node_count = false;
				break;
			case 1:
				ModData::show_node_count = true;
				break;
			default:
				output->append("ERROR: Assigned value must be either 1 or 0 (1 = enabled, 0 = disabled)\n");
				return_val = ERROR_INVALID_PARAMETER;
				break;
		}
	}

	// If no argument is specified, simply print the status
	if (ModData::show_node_count)
	{
		output->append("Display multiplayer node count = enabled");
	}
	else
	{
		output->append("Display multiplayer node count = disabled");
	}

	return return_val;
}




/*
	Called once at startup; registers all the above commands
	for use with the in-game console.
*/
void ModData::register_console_commands()
{
	register_console_command(ccn_fix_bonfire_input, cc_fix_bonfire_input, chm_fix_bonfire_input);
	register_console_command(ccn_text_feed_node_count, cc_text_feed_node_count, chm_text_feed_node_count);
	register_console_alias(cca_node_count, ccn_text_feed_node_count);
	register_console_command(ccn_cheats, cc_cheats, chm_cheats);
}