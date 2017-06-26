// Author: Sean Pesce

#pragma once


#define WIN32_LEAN_AND_MEAN	// Exclude rarely-used stuff from Windows headers


// Windows Header Files
#include <Windows.h>

// Other Header Files:
#include "SP_IO.hpp"

class SpD3D9Device;
class SpD3D9Overlay;

#include "SpD3D9Interface.h"
#include "SpD3D9Device.h"
#include "SpD3D9.h"


//////////////////////// CONSTANTS & ENUMS ////////////////////////
#ifdef _DEVICE_READY_
	#undef _DEVICE_READY_
#endif
#ifdef _GET_TEXT_FEED_
	#undef GET_TEXT_FEED_
#endif
#ifdef _SETTINGS_FILE_
	#undef _SETTINGS_FILE_
#endif
#ifdef _KEYBINDS_SECTION_
	#undef _KEYBINDS_SECTION_
#endif
#ifdef _PRINT_OVERLAY_VERBOSE_
	#undef _PRINT_OVERLAY_VERBOSE_
#endif
#ifdef _PRINT_OVERLAY_
	#undef _PRINT_OVERLAY_
#endif

#define _DEVICE_READY_ (device != NULL && (*device) != NULL && (*device)->overlay != NULL && (*device)->overlay->text_feed != NULL) // Checks that the D3D9 device, overlay, and text feed are initialized
#define _GET_TEXT_FEED_ (*device)->overlay->text_feed	// Text feed object (prints messages)
#define _SETTINGS_FILE_ ".//d3d9_Mod.ini"				// File to load settings from
#define _KEYBINDS_SECTION_ "Keybinds"					// Section of settings file that holds keybind assignments
#define _PRINT_OVERLAY_VERBOSE_(...) {if(_DEVICE_READY_ && (*verbose_output_enabled)){_GET_TEXT_FEED_->print(##__VA_ARGS__);}} // Performs necessary checks and prints message to overlay text feed, but only if verbose output is enabled
#define _PRINT_OVERLAY_(...) {if(_DEVICE_READY_){_GET_TEXT_FEED_->print(##__VA_ARGS__);}} // Performs necessary checks and prints message to overlay text feed

// Configuration setting key names
#define _SP_DS1_MOD_HOTKEY_BONFIRE_INPUT_FIX_ "BonfireInputFix"
#define _SP_DS1_MOD_HOTKEY_CHECK_MULTIPHANTOM_PATCH_ "CheckMultiphantomPatch"

// Global variables & data:
SpD3D9Device **device;
bool *verbose_output_enabled;
std::list<SP_KEY_FUNCTION> *keybinds;
bool *audio_feedback_enabled;
std::string settings_file;
std::string keybinds_section;

// Exported functions:
extern void __stdcall initialize_plugin();
void __stdcall load_keybinds(std::list<SP_KEY_FUNCTION> *new_keybinds, bool *audio_feedback);
void __stdcall set_device_wrapper(SpD3D9Device **new_device, bool *verbose_output);
