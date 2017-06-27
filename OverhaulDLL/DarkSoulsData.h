// Author: Sean Pesce

#pragma once


// Windows Header Files
#include <Windows.h>


// Other Header Files:
#include "SP_SysUtils.hpp"

class SpD3D9Device;
class SpD3D9Overlay;

#include "SpD3D9Interface.h"
#include "SpD3D9Device.h"
#include "SpD3D9.h"

//////////////////////// CONSTANTS & ENUMS ////////////////////////
#ifndef _DEVICE_READY_
	#define _DEVICE_READY_ (device != NULL && (*device) != NULL && (*device)->overlay != NULL && (*device)->overlay->text_feed != NULL) // Checks that the D3D9 device, overlay, and text feed are initialized
#endif
#ifndef _GET_TEXT_FEED_
	#define _GET_TEXT_FEED_ (*device)->overlay->text_feed	// Text feed object (prints messages)
#endif
#ifndef _PRINT_OVERLAY_VERBOSE_
	#define _PRINT_OVERLAY_VERBOSE_(...) {if(_DEVICE_READY_ && (*verbose_output_enabled)){_GET_TEXT_FEED_->print(##__VA_ARGS__);}} // Performs necessary checks and prints message to overlay text feed, but only if verbose output is enabled
#endif
#ifndef _PRINT_OVERLAY_
	#define _PRINT_OVERLAY_(...) {if(_DEVICE_READY_){_GET_TEXT_FEED_->print(##__VA_ARGS__);}} // Performs necessary checks and prints message to overlay text feed
#endif

extern SpD3D9Device **device; // ID3D9Device wrapper object with built-in overlay
extern bool *verbose_output_enabled;
extern bool *audio_feedback_enabled;

enum SP_DS1_GAME_VERSION_ENUM {
	SP_DS1_VERSION_RELEASE = 0x2E,
	SP_DS1_VERSION_DEBUG = 0x2C,
	SP_DS1_VERSION_OVERHAUL = 0x3C
};

enum SP_DS1_PLAYER_STATUS_ENUM {
	SP_DS1_PLAYER_STATUS_LOADING = -1,
	SP_DS1_PLAYER_STATUS_HUMAN = 0,
	SP_DS1_PLAYER_STATUS_COOP = 1,
	SP_DS1_PLAYER_STATUS_INVADER = 2,
	SP_DS1_PLAYER_STATUS_HOLLOW = 8
};

// Overlay messages
#define _SP_DS1_MOD_MSG_BONFIRE_INPUT_FIX_ "Bonfire input fix applied"
#define _SP_DS1_MOD_MSG_CANT_BONFIRE_INPUT_FIX_ "Bonfire input fix was not applied; no character loaded"


//////////////////////// GAME VARIABLES, POINTERS, & OTHER DATA ////////////////////////

#define _MAX_SUMMONS_ 9

void *ds1_base; // Base address of Dark Souls game process

void *player_char_base; // Base address for player character data
SpPointer player_char_status; // Player character status (loading, human, co-op, invader, hollow)


//////////////////////// FUNCTION PROTOTYPES ////////////////////////
void __stdcall initialize_plugin(); // Exported function
void change_game_version_number(); // Changes the game version number to avoid compatibility issues with non-overhaul builds
int fix_bonfire_input(); // Fixes input bug that causes players to be stuck at a bonfire

int print_debug_info();

void SP_beep(DWORD frequency, DWORD duration, bool wait);
void SP_beep(DWORD frequency, DWORD duration);