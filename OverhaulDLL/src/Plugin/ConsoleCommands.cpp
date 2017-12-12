/*
	DARK SOULS OVERHAUL
	
	Contributors to this file:
		Sean Pesce	-	C++, DirectX9 overlay
		Wulf2k		-	Reverse engineering (multiplayer network data)


	This file defines custom console commands that will
	be registered with the in-game console at runtime.
*/

#include "DllMain.h"
#include "Plugin/ConsoleCommands.h"


///////////////////////////////////////////////////////////
//////////////// Console command functions ////////////////
///////////////////////////////////////////////////////////



// Plays the mod credits (lists the people who helped create the mod)
int cc_credits(std::vector<std::string> args, std::string *output)
{
    // @TODO
    output->append("\n\n");
    output->append("    ///////////////////////////////////////////////////////////\n");
    output->append("    ///////////////// Dark Souls Overhaul Mod /////////////////\n");
    output->append("    ///////////////////////////////////////////////////////////\n");
    output->append("\n                ...credits coming eventually...\n\n");

    return CONSOLE_COMMAND_SUCCESS;
}



// Checks if Legacy Mode is enabled
int cc_legacy_mode(std::vector<std::string> args, std::string *output)
{
    if (Mod::legacy_mode)
        output->append("Legacy Mode is enabled.");
    else
        output->append("Legacy Mode is disabled.");

    return CONSOLE_COMMAND_SUCCESS;
}


// Enables cheats until game is restarted (also disables saving and @TODO multiplayer)
int cc_cheats(std::vector<std::string> args, std::string *output)
{
	int return_val = CONSOLE_COMMAND_SUCCESS;

	if (args.size() > 0)
	{
		switch (parse_toggle_arg(args.at(0).c_str()))
		{
			case 0:
				if (Mod::cheats)
					output->append("Restart the game to disable cheats and re-enable saving and multiplayer.");
				else
					output->append("Cheats = disabled");
				break;
			case 1:
				if (!Mod::cheats)
				{
					Game::saves_enabled.write(false);
					// @TODO: Disable multiplayer
					print("WARNING: Cheats enabled. Saving and multiplayer functions disabled. Restart game to disable cheats", 0, false, SP_D3D9O_TEXT_COLOR_RED);
				}
				else
					output->append("Cheats = enabled. Saving and multiplayer functions have been disabled. Restart the game to disable cheats.");
				Mod::cheats = true;
				break;
			default:
				output->append(ERROR_INVALID_BOOL_ARGUMENT + "\n");
				if (Mod::cheats)
					output->append("Cheats = enabled. Saving and multiplayer functions have been disabled. Restart the game to disable cheats.");
				else
					output->append("Cheats = disabled");
				return_val = ERROR_INVALID_PARAMETER;
				break;
		}
	}
	else
	{
		if (Mod::cheats)
			output->append("Cheats = enabled. Saving and multiplayer functions have been disabled. Restart the game to disable cheats.");
		else
			output->append("Cheats = disabled");
	}

	return return_val;
}



// Enables/disables dim lava effects
int cc_dim_lava(std::vector<std::string> args, std::string *output)
{
	int ret_val = CONSOLE_COMMAND_SUCCESS;
	if (args.size() > 0)
	{
		switch (parse_toggle_arg(args.at(0).c_str()))
		{
			case 0:
				Game::enable_dim_lava(false);
				break;
			case 1:
				Game::enable_dim_lava(true);
				break;
			default:
				output->append(ERROR_INVALID_BOOL_ARGUMENT + "\n");
				ret_val = ERROR_INVALID_PARAMETER;
				break;
		}
	}

	if (Game::dim_lava_enabled())
	{
		output->append("Dim lava = enabled");
	}
	else
	{
		output->append("Dim lava = disabled");
	}
	return ret_val;
}



// Enables/disables armor sound effects
int cc_armor_sfx(std::vector<std::string> args, std::string *output)
{
	int ret_val = CONSOLE_COMMAND_SUCCESS;
	if (args.size() > 0)
	{
		switch (parse_toggle_arg(args.at(0).c_str()))
		{
			case 0:
				Game::enable_armor_sfx(false);
				break;
			case 1:
				Game::enable_armor_sfx(true);
				break;
			default:
				output->append(ERROR_INVALID_BOOL_ARGUMENT + "\n");
				ret_val = ERROR_INVALID_PARAMETER;
				break;
		}
	}

	if (Game::armor_sfx_enabled())
	{
		output->append("Armor sfx = enabled");
	}
	else
	{
		output->append("Armor sfx = disabled");
	}
	return ret_val;
}



// Applies the Bonfire input fix (Probably unnecessary now that the fix is automatic)
int cc_fix_bonfire_input(std::vector<std::string> args, std::string *output)
{
	// Apply fix and print message to console (and not to text feed)
	return Game::fix_bonfire_input(false, true);
}


// Prints information on every player in the user's multiplayer node network
int cc_multiplayer_network(std::vector<std::string> args, std::string *output)
{
	if (Game::node_count < 0)
	{
		output->append("Multiplayer network is unavailable.");
	}
	else if (Game::node_count == 0)
	{
		output->append("No players in multiplayer network.");
	}
	else
	{
		output->append("\n");
		std::string header = "     Steam64 ID         |    Steam64 ID (Hex)   |   Ping    ";
		output->append(header);
		output->append("\n");
		for (int i = 0; i < (int)header.length(); i++)
		{
			output->append("-");
		}
		output->append("\n");
		SpPointer connection_list = SpPointer((uint8_t*)Game::ds1_base + 0xF62D24, { 0x54 } );
		SpPointer entry = SpPointer(NULL);
		for (int i = 0; i < Game::node_count && connection_list.resolve() != entry.resolve(); i++)
		{
			if (i == 0)
				entry = SpPointer(connection_list.resolve(), { 0x0 });

			SpPointer status_ptr = SpPointer(entry.resolve(), { 0x8, 0x8 });
			SpPointer steam_id_ptr = SpPointer(entry.resolve(), { 0x8, 0x170 });
			SpPointer ping_ptr = SpPointer(entry.resolve(), { 0x8, 0x178 });

			int status;
			status_ptr.read(&status);
			if (status > 2)
			{
				output->append("  ");

				// Get Steam64 ID as decimal string
				unsigned long long steam64_id_val;
				steam_id_ptr.read(&steam64_id_val);
				std::string steam64_id = std::to_string(steam64_id_val);
				for (int j = 0, len = steam64_id.length(); j < (22 - len); j++) // 20 = largest possible length of a Steam64 ID string
					steam64_id += ' ';
				output->append(steam64_id);
				output->append("|    ");


				// Get Steam64 ID as hex string
				uint8_t steam_id_bytes[8];
				memcpy_s(steam_id_bytes, 8, steam_id_ptr.resolve(), 8);
				for (int j = 0; j < 8; j++)
				{
					uint8_t b = steam_id_bytes[7 - j];
					std::stringstream hex_stream;
					hex_stream << std::hex << (int)b; // Convert Virtual-key code to hex string
					if (b < 16)
						output->append("0");
					output->append(hex_stream.str());
				}
				output->append("   |   ");


				// Get ping
				int ping;
				ping_ptr.read(&ping);
				if (ping > 999)
					ping = 999;
				output->append(std::to_string(ping));
				if (ping < 10)
					output->append("  ");
				else if (ping < 100)
					output->append(" ");
				output->append("\n");
			}

			// Get next connection
			entry = SpPointer(entry.resolve(), { 0x0 });
		}
	}

	return CONSOLE_COMMAND_SUCCESS;
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
				Mod::show_node_count = false;
				break;
			case 1:
				Mod::show_node_count = true;
				break;
			default:
				output->append("ERROR: Assigned value must be either 1 or 0 (1 = enabled, 0 = disabled)\n");
				return_val = ERROR_INVALID_PARAMETER;
				break;
		}
	}
	else
	{
		if (Game::node_count < 0)
		{
			output->append("Multiplayer network is unavailable.\n");
		}
		else
		{
			output->append("Current node count: ").append(std::to_string(Game::node_count)).append("\n");
		}
	}

	// If no argument is specified, simply print the status
	if (Mod::show_node_count)
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
						/////////////////////////////
						///// FOR DEVELOPER USE /////
						/////////////////////////////

	Placeholder console command for devs to test/debug various data at runtime.

	NOTE:
	 Code implemented here is for developer use only; changes to this function
	 should NOT be pushed to the repository.

*/
int cc_developer_debug(std::vector<std::string> args, std::string *output)
{

	return CONSOLE_COMMAND_SUCCESS;
}




/*
	Called once at startup; registers all the above commands
	for use with the in-game console.
*/
void Mod::register_console_commands()
{
	register_console_command(ccn_developer_debug, cc_developer_debug, chm_developer_debug);
    register_console_command(ccn_credits, cc_credits, chm_credits);
    register_console_command(ccn_legacy_mode, cc_legacy_mode, chm_legacy_mode);
	register_console_command(ccn_armor_sfx, cc_armor_sfx, chm_armor_sfx);
	register_console_alias(cca_armor_sounds, ccn_armor_sfx);
	register_console_command(ccn_dim_lava, cc_dim_lava, chm_dim_lava);
	register_console_alias(cca_lava_brightness_fix, ccn_dim_lava);
	register_console_command(ccn_fix_bonfire_input, cc_fix_bonfire_input, chm_fix_bonfire_input);
	register_console_command(ccn_text_feed_node_count, cc_text_feed_node_count, chm_text_feed_node_count);
	register_console_alias(cca_node_count, ccn_text_feed_node_count);
	register_console_command(ccn_cheats, cc_cheats, chm_cheats);
	register_console_command(ccn_multiplayer_network, cc_multiplayer_network, chm_multiplayer_network);



	#ifdef _DS1_OVERHAUL_MOD_DBG_

	Mod::register_console_commands_debug();

	#endif // _DS1_OVERHAUL_MOD_DBG_
}