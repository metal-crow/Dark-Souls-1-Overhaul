/*
	DARK SOULS OVERHAUL
	
	Contributors to this file:
		Sean Pesce	-	C++

	DllMain.cpp: Defines the entry point for the DLL application.
*/

#include "DllMain.h"


DWORD plugin_thread_id;
HANDLE plugin_thread_handle;

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			// Obtain base address of Dark Souls game process
			GameData::ds1_base = GetModuleHandle(NULL);

			// Create a new thread to run code without halting the library-loading thread:
			plugin_thread_handle = CreateThread(
				NULL,				// Default security attributes
				0,					// Use default stack size
				on_process_attach_async, // Thread function name
				NULL,				// Argument to thread function
				0,					// Use default creation flags
				&plugin_thread_id);	// Returns the thread identifier

			// Run important startup code that must be executed before the game loads:
			on_process_attach();
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			on_process_detach();
			break;
	}
	return TRUE;
}
