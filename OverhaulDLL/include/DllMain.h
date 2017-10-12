/*
	DARK SOULS OVERHAUL
	
	Contributors to this file:
		Sean Pesce	-	C++


	DllMain.h: include file for standard system include files,
	or project specific include files that are used frequently, but
	are changed infrequently
*/


#pragma once

#ifndef _DS1_OVERHAUL_DLL_MAIN_H_
	#define _DS1_OVERHAUL_DLL_MAIN_H_


#define WIN32_LEAN_AND_MEAN   // Exclude rarely-used stuff from Windows headers

// Windows Header Files:
#include <Windows.h>

// Sean's SP library headers:
#include "SP_AoB_Scan.hpp"
#include "SP_AsmInject_x86.hpp"
#include "SP_IO.hpp"
#include "SP_SysUtils.hpp"
#include "SpDetours.h"

// Data imported from DirectX9 overlay:
#include "PluginImports.h"

// Game data/Overhaul mod data headers:
#include "ModData.h"
#include "GameData.h"
#include "ParamDef.h"
#include "Plugin/KeybindFunctions.h"



//////////////////////////// MACROS ////////////////////////////

// Apply byte-array patch
#define apply_byte_patch(dest_addr, patch, len) { set_mem_protection(dest_addr,len,MEM_PROTECT_RWX); memcpy_s(dest_addr,len,patch,len); }

// Check if current game version number exists in the list of supported versions
#define game_version_is_supported (std::find(Mod::supported_game_versions.begin(), Mod::supported_game_versions.end(), Game::get_game_version()) != Mod::supported_game_versions.end())



// Functions called from DllMain()
void on_process_attach();
DWORD WINAPI on_process_attach_async(LPVOID lpParam);
DWORD WINAPI deferred_tasks(LPVOID lpParam);
void on_process_detach();


#endif // _DS1_OVERHAUL_DLL_MAIN_H_