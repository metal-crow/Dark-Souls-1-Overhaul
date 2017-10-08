/*
	DARK SOULS OVERHAUL
	
	Contributors to this file:
		Sean Pesce	-	C++, DirectX9 overlay
		Wulf2k		-	Multiplayer network data addresses


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


// Prints information on every player in the user's multiplayer node network
int cc_multiplayer_network(std::vector<std::string> args, std::string *output)
{
	if (GameData::node_count < 0)
	{
		output->append("Multiplayer network is unavailable.");
	}
	else if (GameData::node_count == 0)
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
		SpPointer connection_list = SpPointer((uint8_t*)GameData::ds1_base + 0xF62D24, { 0x54 } );
		SpPointer entry = SpPointer(NULL);
		for (int i = 0; i < GameData::node_count && connection_list.resolve() != entry.resolve(); i++)
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
	else
	{
		if (GameData::node_count < 0)
		{
			output->append("Multiplayer network is unavailable.\n");
		}
		else
		{
			output->append("Current node count: ").append(std::to_string(GameData::node_count)).append("\n");
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
	register_console_command(ccn_multiplayer_network, cc_multiplayer_network, chm_multiplayer_network);
}