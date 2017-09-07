/*
	DARK SOULS OVERHAUL
	
	Contributors to this file:
		Sean Pesce	-	C++
	
*/

#pragma once

#ifndef _DS1_OVERHAUL_GAME_DATA_H_
	#define _DS1_OVERHAUL_GAME_DATA_H_


#include "SpPointer.h"



enum DS1_GAME_VERSION_ENUM {
	DS1_VERSION_RELEASE = 0x2E,
	DS1_VERSION_DEBUG = 0x2C,
	DS1_VERSION_OVERHAUL = 0x3C
};

enum DS1_PLAYER_STATUS_ENUM {
	DS1_PLAYER_STATUS_LOADING = -1,
	DS1_PLAYER_STATUS_HUMAN = 0,
	DS1_PLAYER_STATUS_COOP = 1,
	DS1_PLAYER_STATUS_INVADER = 2,
	DS1_PLAYER_STATUS_HOLLOW = 8
};




class GameData
{
public:

	// Base address of Dark Souls game process
	static void *ds1_base;

	// Base address for player character data
	static void *player_char_base;

	// Player character status (loading, human, co-op, invader, hollow)
	static SpPointer player_char_status;

	// Game saving on/off
	static SpPointer saves_enabled;

	// Multiplayer node count
	static int node_count;
	



	// Initializes pointers that depend on the game's base address
	static void init_pointers();

	// Changes the game version number to avoid compatibility issues with different game builds
	static void set_game_version(uint8_t version_number);

	// Checks if the player is stuck at the bonfire, and if so, automatically applies the bonfire input fix
	static void check_bonfire_input_bug();

	// Fixes input bug that causes players to be stuck at a bonfire (usually after turning human with framerate unlocked)
	static int fix_bonfire_input(bool print_to_text_feed = false, bool print_to_console = false);

	// Returns multiplayer node count as an int (or -1 if player is not online)
	static int get_node_count();



	/////////////////////////////////////////
	//////////////// PATCHES ////////////////
	/////////////////////////////////////////

	// Disables automatic game disconnection when low framerate is detected
	static void low_fps_disconnect_enabled(bool enable);

	// Two-part patch to increase the multiplayer phantom limit:
	static void increase_phantom_limit1(); // Called from on_process_attach()
	static void increase_phantom_limit2(); // Called from initialize_plugin()
};




#endif // _DS1_OVERHAUL_GAME_DATA_H_