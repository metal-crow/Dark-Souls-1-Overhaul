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

// SP library headers:
#include "SP_AoB_Scan.hpp"
#include "SP_AsmInject_x86.hpp"
#include "SP_IO.hpp"
#include "SP_SysUtils.hpp"

// Data imported from DirectX9 overlay:
#include "PluginImports.h"

// Game data/Overhaul mod data headers:
#include "GameData.h"
#include "ModData.h"



// Macro for applying byte-array patches
#define apply_byte_patch(dest_addr, patch, len) { set_mem_protection(dest_addr,len,MEM_PROTECT_RWX); memcpy_s(dest_addr,len,patch,len); }



// Functions called from DllMain()
DWORD WINAPI on_process_attach(LPVOID lpParam);
void on_process_detach();


#endif // _DS1_OVERHAUL_DLL_MAIN_H_