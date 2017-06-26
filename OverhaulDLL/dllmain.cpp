// Author: Sean Pesce

#include "SpD3D9Plugin.h"


BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			extern void* ds1_base;
			ds1_base = GetModuleHandle(NULL); // Obtain base address of Dark Souls game process

			extern void change_game_version_number();
			change_game_version_number();

			//extern void apply_multiphantom_patch();
			//apply_multiphantom_patch();
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
		default:
			break;
	}
	return TRUE;
}


// Exported functions:

extern void __stdcall initialize_plugin();

void __stdcall load_keybinds(std::list<SP_KEY_FUNCTION> *new_keybinds, bool *audio_feedback)
{
	audio_feedback_enabled = audio_feedback;
	keybinds = new_keybinds;

	if (keybinds != NULL)
	{
		settings_file = _SETTINGS_FILE_;
		keybinds_section = _KEYBINDS_SECTION_;

		unsigned int key = 0;

		if (key = get_vk_hotkey(settings_file.c_str(), keybinds_section.c_str(), _SP_DS1_MOD_HOTKEY_BONFIRE_INPUT_FIX_))
		{
			extern int fix_bonfire_input();
			add_function_keybind(key, fix_bonfire_input, keybinds);
		}
		key = 0;
	}
}

void __stdcall set_device_wrapper(SpD3D9Device **new_device, bool *verbose_output)
{
	verbose_output_enabled = verbose_output;
	device = new_device;
}