// Author: Sean Pesce

#pragma once

#ifndef _SP_D3D9_OVERLAY_H_
	#define _SP_D3D9_OVERLAY_H_

//#include "stdafx.h"
#include "SpD3D9.h"

// Default overlay properties
#define _SP_D3D9O_DEFAULT_MODULES_ENABLED_ (SP_D3D9O_TEXT_FEED_ENABLED)

#ifdef _SP_D3D9O_TF_USE_ID3DX_FONT_
	// Total number of supported text colors
	#define _SP_D3D9O_TEXT_COLOR_COUNT_ 11
	// Denotes supported text colors
	enum SP_D3D9O_TEXT_COLOR_ENUM {
		SP_D3D9O_TEXT_COLOR_WHITE,
		SP_D3D9O_TEXT_COLOR_BLACK,
		SP_D3D9O_TEXT_COLOR_RED,
		SP_D3D9O_TEXT_COLOR_ORANGE,
		SP_D3D9O_TEXT_COLOR_YELLOW,
		SP_D3D9O_TEXT_COLOR_GREEN,
		SP_D3D9O_TEXT_COLOR_CYAN,
		SP_D3D9O_TEXT_COLOR_BLUE,
		SP_D3D9O_TEXT_COLOR_PURPLE,
		SP_D3D9O_TEXT_COLOR_PINK,
		SP_D3D9O_TEXT_COLOR_CYCLE_ALL
	};
#else
	// Total number of supported text colors
	#define _SP_D3D9O_TEXT_COLOR_COUNT_ 10
	// Denotes supported text colors
	enum SP_D3D9O_TEXT_COLOR_ENUM {
		SP_D3D9O_TEXT_COLOR_WHITE,
		SP_D3D9O_TEXT_COLOR_BLACK,
		SP_D3D9O_TEXT_COLOR_RED,
		SP_D3D9O_TEXT_COLOR_ORANGE,
		SP_D3D9O_TEXT_COLOR_YELLOW,
		SP_D3D9O_TEXT_COLOR_GREEN,
		SP_D3D9O_TEXT_COLOR_CYAN,
		SP_D3D9O_TEXT_COLOR_BLUE,
		SP_D3D9O_TEXT_COLOR_PURPLE,
		SP_D3D9O_TEXT_COLOR_PINK
	};
#endif // _SP_D3D9O_TF_USE_ID3DX_FONT_

class SpD3D9Interface;
class SpD3D9Device;
class SpD3D9OTextFeed;

// Denotes which modules of the overlay are currently active (text feed, etc)
enum SP_D3D9O_ENABLE_MODULES_ENUM {
	SP_D3D9O_TEXT_FEED_ENABLED = 0x00000001,
	SP_D3D9O_ALL_MODULES_ENABLED = 0xFFFFFFFF // Denotes all modules enabled, but also forces enum to use 32-bit ints
};
// Used to disable overlay modules using the bitwise AND operator
enum SP_D3D9O_DISABLE_MODULES_ENUM {
	SP_D3D9O_TEXT_FEED_DISABLED = 0xFFFFFFFE,
	SP_D3D9O_ALL_MODULES_DISABLED = 0x00000000
};

// Supported text colors
#ifdef _SP_D3D9O_TF_USE_ID3DX_FONT_
const D3DXCOLOR SP_D3D9O_TEXT_COLORS[_SP_D3D9O_TEXT_COLOR_COUNT_] =
{
	D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),	// White
	D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f),	// Black
	D3DXCOLOR(0xFFF44E42),				// Red
	D3DXCOLOR(0xFFF4BC42),				// Orange
	D3DXCOLOR(0xFFEEF442),				// Yellow
	D3DXCOLOR(0xFF42F450),				// Green
	D3DXCOLOR(0xFF42D4F4),				// Cyan
	D3DXCOLOR(0xFF4286F4),				// Blue
	D3DXCOLOR(0xFFBC42F4),				// Purple
	D3DXCOLOR(0xFFF442EB),				// Pink
	D3DXCOLOR(0xFFFF0000)				// Starting color when cycling all colors
};
#else
const D3DXCOLOR SP_D3D9O_TEXT_COLORS[_SP_D3D9O_TEXT_COLOR_COUNT_] =
{
	D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),	// White
	D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f),	// Black
	D3DXCOLOR(0xFFF44E42),				// Red
	D3DXCOLOR(0xFFF4BC42),				// Orange
	D3DXCOLOR(0xFFEEF442),				// Yellow
	D3DXCOLOR(0xFF42F450),				// Green
	D3DXCOLOR(0xFF42D4F4),				// Cyan
	D3DXCOLOR(0xFF4286F4),				// Blue
	D3DXCOLOR(0xFFBC42F4),				// Purple
	D3DXCOLOR(0xFFF442EB),				// Pink
};

// Escape characters for when D3DFONT_COLORTABLE flag is used
const char SP_D3D9O_TEXT_COLOR_TABLE_CHARS[_SP_D3D9O_TEXT_COLOR_COUNT_] =
{
	'0', // White
	'9', // Black
	'1', // Red
	'7', // Orange
	'4', // Yellow
	'2', // Green
	'8', // Light blue
	'3', // Blue
	'5', // Purple
	'6', // Pink
};
#endif // _SP_D3D9O_TF_USE_ID3DX_FONT_

class SpD3D9Overlay
{
public:
	// Direct3D 9 member data
	SpD3D9Interface *d3d_interface = NULL;
	SpD3D9Device *device = NULL;

	// Overlay member data
	SpD3D9OTextFeed *text_feed = NULL;
	uint32_t enabled_modules = _SP_D3D9O_DEFAULT_MODULES_ENABLED_; // Denotes which modules of the overlay are currently active (text feed, etc)
	bool is_windowed; // Specifies whether the program is running in windowed or exclusive full-screen mode
	bool needs_update = true; 
	HWND *game_window = NULL; // Main game window (should be pointer to focus_window or device_window)
	HWND focus_window = NULL;
	HWND device_window = NULL;
	RECT back_buffer;
	IDirect3DStateBlock9 *overlay_state_block = NULL; // State block applied before drawing overlay
	unsigned int fps_count = 0;
	unsigned int fps_timer_id = 0; // ID of timer used to update FPS count once per second

	// Constructor/destructor
	SpD3D9Overlay(SpD3D9Interface *new_interface, SpD3D9Device *new_device, HWND new_focus_window, D3DPRESENT_PARAMETERS *present_params);
	~SpD3D9Overlay();

	// Member functions
	void SpD3D9Overlay::draw(IDirect3DSwapChain9 *swap_chain);
	void SpD3D9Overlay::update_back_buffer_parameters();
	void SpD3D9Overlay::create_state_block(); // Creates a suitable state block for drawing the overlay
	void SpD3D9Overlay::release_tasks(); // Called in device wrapper's Release() function
	void SpD3D9Overlay::force_release_tasks(); // Called in device wrapper's ForceRelease() function
	void SpD3D9Overlay::reset_tasks(); // Called in device wrapper's Reset() function (before original device Reset() call)
	void SpD3D9Overlay::post_reset_tasks(D3DPRESENT_PARAMETERS *present_params); // Called in device wrapper's Reset() function (after original device Reset() call)
	void SpD3D9Overlay::end_scene_tasks(); // Called in device wrapper's EndScene() function


private:
	
};


void CALLBACK update_fps_count(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime); // (Called once per second) Records the number of frames that were rendered in the last second


#endif // _SP_D3D9_OVERLAY_H_