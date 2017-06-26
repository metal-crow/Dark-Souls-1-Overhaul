// Author: Sean Pesce

#include "DarkSoulsData.h"

// Exported function
void __stdcall initialize_plugin()
{
	player_char_base = (void*)((unsigned int)ds1_base + 0xF7E204); // Obtain base address for player character data

	player_char_status = SpPointer(player_char_base, { 0xA28 }); // Player character status (loading, human, co-op, invader, hollow)

	extern void apply_multiphantom_patch();
	apply_multiphantom_patch();
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



// Fixes input bug that causes players to be stuck at a bonfire
int fix_bonfire_input()
{
	// Get current player status
	int status = SP_DS1_PLAYER_STATUS_LOADING;
	player_char_status.read(&status);

	if (status == SP_DS1_PLAYER_STATUS_HOLLOW || status == SP_DS1_PLAYER_STATUS_HUMAN) // Check if player is hollow/human
	{
		SpPointer bonfire_anim_fix = SpPointer((void*)0x12E29E8, { 0x0, 0xFC });

		bonfire_anim_fix.write((uint32_t)0); // Write zero to bonfire animation status address

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