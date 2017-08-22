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



// Applies the Bonfire input fix
void cc_bonfire_input_fix(std::vector<std::string> args, std::string *output)
{
	GameData::fix_bonfire_input();
}



// Enables/disables multiplayer node count element of overlay text feed
void cc_text_feed_node_count(std::vector<std::string> args, std::string *output)
{
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
				break;
		}
	}

	// If no argument is specified, simply print the status
	if (ModData::show_node_count)
	{
		output->append("Text feed info bar multiplayer node count = enabled");
	}
	else
	{
		output->append("Text feed info bar multiplayer node count = disabled");
	}
}




/*
	Called once at startup; registers all the above commands
	for use with the in-game console.
*/
void ModData::register_console_commands()
{
	register_console_command(ccn_bonfire_input_fix, cc_bonfire_input_fix, chm_bonfire_input_fix);
	register_console_command(ccn_text_feed_node_count, cc_text_feed_node_count, chm_text_feed_node_count);
}