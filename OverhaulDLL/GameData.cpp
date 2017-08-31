/*
	DARK SOULS OVERHAUL
	
	Contributors to this file:
		Ashley						-	Reverse engineering, Low FPS Disconnect patch
		jellybaby34					-	Game version number patch
		Metal-Crow					-	Reverse engineering, Phantom Limit patch
		Youri "NullBy7e" de Mooij	-	Bonfire Input Fix
		Sean Pesce					-	C++
	
*/

#include "DllMain.h"



/*
	Initialize static variables:
*/

// Base address of Dark Souls game process
void *GameData::ds1_base = NULL;

// Base address for player character data
void *GameData::player_char_base = NULL;

// Player character status (loading, human, co-op, invader, hollow)
SpPointer GameData::player_char_status;





// Initializes pointers that depend on the game's base address
void GameData::init_pointers()
{
	print_console(std::string("[Overhaul Mod] Initializing pointers...").c_str());

	// Obtain base address for player character data
	GameData::player_char_base = (void*)((unsigned int)GameData::ds1_base + 0xF7E204);

	// Player character status (loading, human, co-op, invader, hollow)
	GameData::player_char_status = SpPointer(GameData::player_char_base, { 0xA28 });


}


// Changes the game version number to avoid compatibility issues with different game builds
void GameData::set_game_version(uint8_t version_number)
{
	std::stringstream hex_stream;
	hex_stream << std::hex << version_number;
	print_console(std::string("[Overhaul Mod] Setting game version number = 0x").append(hex_stream.str()).append("...").c_str());

	uint8_t patch1[5] = { 0xC6, 0x44, 0x24, 0x1C, version_number }; // mov byte ptr [esp+0x1C],0x3C
	uint8_t patch2[3] = { 0x80, 0x38, version_number }; // cmp byte ptr [eax],0x3C

	void *write_address = (uint8_t*)GameData::ds1_base + 0x7E73FA;
	apply_byte_patch(write_address, patch1, 5);

	write_address = (uint8_t*)GameData::ds1_base + 0x7E719D;
	apply_byte_patch(write_address, patch2, 3);

	write_address = (uint8_t*)GameData::ds1_base + 0x7E7229;
	apply_byte_patch(write_address, patch2, 3);
}


// Disables automatic game disconnection when low framerate is detected
void GameData::disable_low_fps_disconnect()
{
	print_console("[Overhaul Mod] Disabling low FPS disconnect...");

	uint8_t *fps_warn = NULL;
	fps_warn = (uint8_t*)aob_scan("74 17 B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B"); // Careful, this pattern returns 2 results

	if (fps_warn)
	{
		// AoB Scan was successful
		set_mem_protection(fps_warn, 1, MEM_PROTECT_RWX);
		fps_warn -= 0x1E;
		*fps_warn = 0xC3; // To disable this patch, set *fps_warn = 0x51
	}
}


// Fixes input bug that causes players to be stuck at a bonfire (usually after turning human)
int GameData::fix_bonfire_input()
{
	// Get current player status
	int status = DS1_PLAYER_STATUS_LOADING;
	player_char_status.read(&status);

	if (status == DS1_PLAYER_STATUS_HOLLOW || status == DS1_PLAYER_STATUS_HUMAN) // Check if player is hollow/human
	{
		// Write zero to bonfire animation status address
		SpPointer bonfire_anim_fix = SpPointer((void*)0x12E29E8, { 0x0, 0xFC });
		bonfire_anim_fix.write((uint32_t)0);

		print(_DS1_MOD_MSG_BONFIRE_INPUT_FIX_);

		Sleep(200); // Sleep to avoid keypress being detected multiple times
		return 0;
	}
	else
	{
		// Player is not hollow/human, so can't be at a bonfire
		print(_DS1_MOD_MSG_CANT_BONFIRE_INPUT_FIX_);
		
		Sleep(200); // Sleep to avoid keypress being detected multiple times
		return -1;
	}
}


// Adds current multiplayer node count to the text feed info string and returns node count as an int.
//		If argument is NULL, simply returns node count (or -1 if player is not online)
int GameData::get_node_count(std::string *text_feed_info_header)
{
	// First check that the node count pointer can be resolved
	SpPointer node_count_ptr_check = SpPointer((uint8_t*)GameData::ds1_base + 0xF7F77C, { 0x2C, 0x778 });

	if (ModData::initialized && *(void**)(node_count_ptr_check.resolve()))
	{
		// Get node count
		SpPointer node_count_ptr = SpPointer((uint8_t*)ds1_base + 0xF7F77C, { 0x2C, 0x778, 0x80 });
		int node_count;
		node_count_ptr.read(&node_count);

		if (text_feed_info_header != NULL && ModData::show_node_count)
		{
			text_feed_info_header->append("[Nodes: ");
			text_feed_info_header->append(std::to_string(node_count));
			text_feed_info_header->append("]  ");
		}
		return node_count;
	}
	else
	{
		// Node count pointer could not be resolved; game is still loading or player is offline
		if (text_feed_info_header != NULL && ModData::show_node_count)
		{
			text_feed_info_header->append("[Nodes: --]  ");
		}
		return -1;
	}
}
