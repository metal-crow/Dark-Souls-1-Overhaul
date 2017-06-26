// Author: Sean Pesce

#pragma once

#ifndef _SP_D3D9O_TEXT_FEED_H_
	#define _SP_D3D9O_TEXT_FEED_H_

//#include "stdafx.h"
#include "SpD3D9.h"

#include "D3DFont.h"

// Default text feed attributes
#ifdef _SP_D3D9O_TF_USE_ID3DX_FONT_
	#define _SP_D3D9O_TF_DEFAULT_FONT_HEIGHT_ 20
#else
	#define _SP_D3D9O_TF_DEFAULT_FONT_HEIGHT_ 12
#endif
#define _SP_D3D9O_TF_DEFAULT_SHADOW_X_OFFSET_ 2
#define _SP_D3D9O_TF_DEFAULT_SHADOW_Y_OFFSET_ 2
#define _SP_D3D9O_TF_DEFAULT_OUTLINE_THICKNESS_ 2
#define _SP_D3D9O_TF_DEFAULT_COLOR_ SP_D3D9O_TEXT_COLOR_WHITE
#define _SP_D3D9O_TF_DEFAULT_OUTLINE_COLOR_ colors[SP_D3D9O_TEXT_COLOR_BLACK]
#define _SP_D3D9O_TF_DEFAULT_SHADOW_COLOR_ colors[SP_D3D9O_TEXT_COLOR_BLACK]
#define _SP_D3D9O_TF_DEFAULT_POSITION_ (DT_NOCLIP | DT_TOP | DT_CENTER)
#define _SP_D3D9O_TF_DEFAULT_STYLE_ SP_D3D9O_OUTLINED_TEXT
#define _SP_D3D9O_TF_DEFAULT_FONT_FAMILY_ "Arial"
#define _SP_D3D9O_TF_DEFAULT_INFO_BAR_ (SP_D3D9O_INFO_BAR_TITLE | SP_D3D9O_INFO_BAR_FPS)
#define _SP_D3D9O_TF_DEFAULT_TITLE_ "Direct3D Overlay by Sean Pesce"

// Overlay class data
extern const D3DXCOLOR SP_D3D9O_TEXT_COLORS[_SP_D3D9O_TEXT_COLOR_COUNT_];


// Denotes supported text styles (outlined, shadowed, or plain)
enum SP_D3D9O_TEXT_FEED_STYLE_ENUM {
	SP_D3D9O_OUTLINED_TEXT,
	SP_D3D9O_SHADOWED_TEXT,
	SP_D3D9O_PLAIN_TEXT
};

// Denotes whether to display each info header attribute
enum SP_D3D9O_TEXT_FEED_INFO_BAR_ENUM {
	SP_D3D9O_INFO_BAR_DISABLED = 0,
	SP_D3D9O_INFO_BAR_TITLE = 1,
	SP_D3D9O_INFO_BAR_DATE = 2,
	SP_D3D9O_INFO_BAR_TIME = 4,
	SP_D3D9O_INFO_BAR_FPS = 8
};


typedef struct SP_D3D9O_TEXT_FEED_ENTRY {
	// Data for a single text feed message entry
	std::string message = std::string("");	// Message
	unsigned long long expire_time;			// Time at which the message expires
	SP_D3D9O_TEXT_COLOR_ENUM color;			// Text color
} SP_D3D9O_TEXT_FEED_ENTRY;


typedef struct SP_D3D9O_TEXT_FEED_BOUNDS {
	// Screenspace boundaries for positioning each text style
	RECT plain;
	RECT shadowed[2];
	RECT outlined[9];
} SP_D3D9O_TEXT_FEED_BOUNDS;

#ifndef _SP_D3D9O_TF_USE_ID3DX_FONT_
typedef struct SP_D3D9O_TEXT_FEED_COORDINATES {
	float x = 0.0f;
	float y = 0.0f;
} SP_D3D9O_TEXT_FEED_COORDINATES;

typedef struct SP_D3D9O_TEXT_FEED_LOCATION {
	// Screenspace coordinates for positioning each text style
	DWORD flags = 0;
	SP_D3D9O_TEXT_FEED_COORDINATES plain;
	SP_D3D9O_TEXT_FEED_COORDINATES shadowed[2];
	SP_D3D9O_TEXT_FEED_COORDINATES outlined[9];
} SP_D3D9O_TEXT_FEED_LOCATION;
#endif


class SpD3D9OTextFeed
{
public:
	SpD3D9Overlay *overlay = NULL; // D3D9 overlay that this text feed belongs to
	std::list<SP_D3D9O_TEXT_FEED_ENTRY> messages; // List of messages in the overlay text feed
	SP_D3D9O_TEXT_FEED_BOUNDS bounds; // Screenspace boundaries for positioning each text style
	DWORD position = _SP_D3D9O_TF_DEFAULT_POSITION_;
	SP_D3D9O_TEXT_FEED_STYLE_ENUM style = _SP_D3D9O_TF_DEFAULT_STYLE_;
	int shadow_x_offset = _SP_D3D9O_TF_DEFAULT_SHADOW_X_OFFSET_;	// Text shadow horizontal offset
	int shadow_y_offset = _SP_D3D9O_TF_DEFAULT_SHADOW_Y_OFFSET_;	// Text shadow vertical offset
	unsigned int outline_thickness = _SP_D3D9O_TF_DEFAULT_OUTLINE_THICKNESS_; // Text outline thickness
	D3DXCOLOR colors[_SP_D3D9O_TEXT_COLOR_COUNT_]; // ARGB values for supported text colors
	#ifdef _SP_D3D9O_TF_USE_ID3DX_FONT_
		ID3DXFont *font = NULL;
		D3DXCOLOR color = SP_D3D9O_TEXT_COLORS[_SP_D3D9O_TF_DEFAULT_COLOR_]; // Default text color
	#else
		CD3DFont *font = NULL;
		int font_height = _SP_D3D9O_TF_DEFAULT_FONT_HEIGHT_;
		SP_D3D9O_TEXT_FEED_LOCATION coordinates; // X and Y positioning for each text style
	#endif // _SP_D3D9O_TF_USE_ID3DX_FONT_
	D3DXCOLOR shadow_color;
	D3DXCOLOR outline_color;
	int show_info_bar = _SP_D3D9O_TF_DEFAULT_INFO_BAR_; // Denotes whether to display each info bar attribute

	// Constructor/destructor
	SpD3D9OTextFeed(SpD3D9Overlay *new_overlay);
	~SpD3D9OTextFeed();

	bool SpD3D9OTextFeed::is_enabled();
	void SpD3D9OTextFeed::set_enabled(bool enabled);
	void SpD3D9OTextFeed::enable();
	void SpD3D9OTextFeed::disable();
	void SpD3D9OTextFeed::draw();
	void SpD3D9OTextFeed::print(const char *message, unsigned long long duration, bool include_timestamp); // Prints message with default-colored text
	void SpD3D9OTextFeed::print(const char *message, unsigned long long duration, bool include_timestamp, SP_D3D9O_TEXT_COLOR_ENUM text_color); // Prints message with specified text color
	void SpD3D9OTextFeed::set_bounds(RECT *window_boundaries); // Initializes screenspace boundaries for each text style
	void SpD3D9OTextFeed::set_font_height(unsigned int new_text_height); // Set font height
	void SpD3D9OTextFeed::clean_feed(); // Removes expired messages


private:
	unsigned int new_font_size = 0; // If non-zero, acts as a flag for update_text_height() to be called
	DWORD current_rgb_cycle_vals[3] = { 0x00FF0000, 0x00000000, 0x00000000 }; // Current RGB values for color-cycling text
	std::string info_string = ""; // String for info header line
	std::string feed_string[_SP_D3D9O_TEXT_COLOR_COUNT_]; // Array of strings to hold text (of each supported color) for text feed
	std::string feed_string_full = "";	// String to hold the feed text as a single string (used for shadow/outline)
	void SpD3D9OTextFeed::build_feed();
	void SpD3D9OTextFeed::update_font_height(); // Update text to new font height
	void SpD3D9OTextFeed::update_info_header(); // Updates info string attributes (date, time, FPS, etc)
	#ifdef _SP_D3D9O_TF_USE_ID3DX_FONT_
		void SpD3D9OTextFeed::cycle_color(); // Cycles text to next color in RGB spectrum
	#else
		void SpD3D9OTextFeed::update_position(); // Updates text feed position based on boundaries and number of messages
	#endif
};




#endif // _SP_D3D9O_TEXT_FEED_H_