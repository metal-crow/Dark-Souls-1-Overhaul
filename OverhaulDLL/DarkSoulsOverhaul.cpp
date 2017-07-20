/*
	Authors:

	Metal-Crow	-	Reverse engineering, CE, LUA, etc
	Ashley		-	Reverse engineering, CE, LUA, etc
	Sean Pesce	-	C++ conversion
*/

#include "DarkSoulsData.h"
#include <sstream> // std::stringstream 

// Exported function
void __stdcall initialize_plugin()
{
	_GET_TEXT_FEED_->set_title("Dark Souls Overhaul Mod");
	_PRINT_OVERLAY_("-------------TEST BUILD-------------", 0, false, SP_D3D9O_TEXT_COLOR_ORANGE);


	// Obtain base address for player character data
	player_char_base = (void*)((unsigned int)ds1_base + 0xF7E204);


	// Player character status (loading, human, co-op, invader, hollow)
	player_char_status = SpPointer(player_char_base, { 0xA28 });


	// Apply remaining multi-phantom patches
	extern void apply_multiphantom_secondary_patch_dynamic();
	apply_multiphantom_secondary_patch_dynamic();


	// Set maximum number of phantoms
	extern SpPointer max_allowed_summons_ptr;
	set_mem_protection(max_allowed_summons_ptr.resolve(), 4, MEM_PROTECT_RWX);
	while (*((void**)max_allowed_summons_ptr.base) == NULL)
	{
		// Wait for max summon data to load in
		Sleep(100);
	}
	extern uint8_t max_allowed_summons8;
	uint32_t max_summons = max_allowed_summons8;
	max_allowed_summons_ptr.write(max_summons);

	// Disable "Framerate insufficient for online play" error
	disable_framerate_warning_disconnection();
}




// Changes the game version number to avoid compatibility issues with non-overhaul builds
void change_game_version_number()
{
	uint8_t patch1[5] = { 0xC6, 0x44, 0x24, 0x1C, (uint8_t)SP_DS1_VERSION_OVERHAUL }; // mov byte ptr [esp+0x1C],0x3C
	uint8_t patch2[3] = { 0x80, 0x38, (uint8_t)SP_DS1_VERSION_OVERHAUL }; // cmp byte ptr [eax],0x3C

	void *write_address = (uint8_t*)ds1_base + 0x7E73FA;
	set_mem_protection(write_address, 5, MEM_PROTECT_RWX);
	memcpy_s(write_address, 5, patch1, 5);

	write_address = (uint8_t*)ds1_base + 0x7E719D;
	set_mem_protection(write_address, 3, MEM_PROTECT_RWX);
	memcpy_s(write_address, 3, patch2, 3);

	write_address = (uint8_t*)ds1_base + 0x7E7229;
	set_mem_protection(write_address, 3, MEM_PROTECT_RWX);
	memcpy_s(write_address, 3, patch2, 3);
}


// Disables automatic game disconnection when low framerate is detected
void disable_framerate_warning_disconnection()
{
	uint8_t *FrameRateWarn = NULL;
	FrameRateWarn = (uint8_t*)aob_scan("74 17 B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B"); // Careful, this pattern returns 2 results

	if (FrameRateWarn)
	{
		// AoB Scan was successful
		set_mem_protection(FrameRateWarn, 1, MEM_PROTECT_RWX);
		FrameRateWarn -= 0x1E;
		*FrameRateWarn = 0xC3; // To disable this patch, set *FrameRateWarn = 0x51
	}
}



// Fixes input bug that causes players to be stuck at a bonfire
int fix_bonfire_input()
{
	// Get current player status
	int status = SP_DS1_PLAYER_STATUS_LOADING;
	player_char_status.read(&status);

	if (status == SP_DS1_PLAYER_STATUS_HOLLOW || status == SP_DS1_PLAYER_STATUS_HUMAN) // Check if player is hollow/human
	{
		// Write zero to bonfire animation status address
		SpPointer bonfire_anim_fix = SpPointer((void*)0x12E29E8, { 0x0, 0xFC });
		bonfire_anim_fix.write((uint32_t)0); 

		_PRINT_OVERLAY_(_SP_DS1_MOD_MSG_BONFIRE_INPUT_FIX_, 2000, true);
		SP_beep(500, 200);

		return 0;
	}
	else
	{
		// Player is not hollow/human, so can't be at a bonfire
		_PRINT_OVERLAY_(_SP_DS1_MOD_MSG_CANT_BONFIRE_INPUT_FIX_, 2000, true);
		SP_beep(300, 100);
		SP_beep(300, 100);
		return -1;
	}
}


// Prints various debugging info (developer use only)
int print_debug_info()
{
	// Check if the multiphantom patch was applied correctly
	extern uint32_t sucessful_phantomfix;
	if (sucessful_phantomfix)
	{
		_PRINT_OVERLAY_("Multiphantom patch was applied successfully", 20000, true);
		SP_beep(800, 100);
		SP_beep(800, 100);
	}
	else
	{
		_PRINT_OVERLAY_("Multiphantom patch was not applied in time", 20000, true);
		SP_beep(300, 100);
		SP_beep(300, 100);
	}

	std::string str = "BlackSosNum = aob_scan(05 00 00 00 03 00 00 00 03 00 00 00 05 00 00 00 05 00 00 00 0A 00 00 00 0A 00 00 00) = ";
	extern void *BlackSosNum;
	std::stringstream ss;
	ss << static_cast<const void*>(BlackSosNum);
	str.append(ss.str());
	_PRINT_OVERLAY_(str.c_str(), 20000, true);

	str.clear();
	ss.str(std::string());
	str = "InvadeNum = aob_scan(05 00 00 00 FA FF FF FF 08 00 00 00 FC FF FF FF 0A 00 00 00 FE FF FF FF 0C 00 00 00) = ";
	extern void *InvadeNum;
	ss << static_cast<const void*>(InvadeNum);
	str.append(ss.str());
	_PRINT_OVERLAY_(str.c_str(), 20000, true);
	str.clear();
	ss.str(std::string());

	extern SpPointer max_allowed_summons_ptr;
	str = "max_allowed_summons address: ";
	ss << static_cast<const void*>(max_allowed_summons_ptr.resolve());
	str.append(ss.str());
	_PRINT_OVERLAY_(str.c_str(), 20000, true);
	ss.str(std::string());
	str.clear();
	str = "max_allowed_summons value = ";
	int val = 0;
	max_allowed_summons_ptr.read(&val);
	str.append(std::to_string(val));
	_PRINT_OVERLAY_(str.c_str(), 20000, true);
	str.clear();

	extern uint32_t *max_allowed_summons32;
	str.append("max_allowed_summons32 = ").append(std::to_string(*max_allowed_summons32));
	_PRINT_OVERLAY_(str.c_str(), 20000, true);
	str.clear();

	return 0;
}




// Beeps at the specified frequency for a specified duration (in milliseconds),
//		if audio feedback is enabled. If audio is disabled and wait==true, the 
//		thread is put to sleep for the specified duration instead.
void SP_beep(DWORD frequency, DWORD duration, bool wait)
{
	if (*audio_feedback_enabled)
	{
		Beep(frequency, duration);
	}
	else if (wait)
	{
		Sleep(duration);
	}
}

// Beeps at the specified frequency for a specified duration (in milliseconds),
//		if audio feedback is enabled. If audio is disabled, the thread is put
//		to sleep for the specified duration instead.
// Functionality is the same as calling SP_beep(frequency, duration, true);
void SP_beep(DWORD frequency, DWORD duration)
{
	if (*audio_feedback_enabled)
	{
		Beep(frequency, duration);
	}
	else
	{
		Sleep(duration);
	}
}