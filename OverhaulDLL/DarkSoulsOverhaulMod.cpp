/*
	DARK SOULS OVERHAUL
	
	Contributors to this file:
		Sean Pesce	-	C++
	
*/

#include "DllMain.h"



/*
	Called from DllMain when the plugin DLL is first loaded into memory (PROCESS_ATTACH case).
	This function runs in a separate thread from DllMain.
*/
DWORD WINAPI on_process_attach(LPVOID lpParam)
{
	// Load user preferences from settings file
	ModData::get_user_prefs();

	// Obtain base address of Dark Souls game process
	GameData::ds1_base = GetModuleHandle(NULL);

	// Change game version number
	GameData::set_game_version(DS1_VERSION_OVERHAUL);

	// Apply first part of phantom limit patch
	GameData::increase_phantom_limit1();

	return 0;
}



// Called from DllMain when the plugin DLL is unloaded (PROCESS_DETACH case)
void on_process_detach()
{
	// Exit tasks should be performed here

}





//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// Exported functions ///////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
/*
	The functions below are exported from this DLL and imported
	by the DirectX9 overlay when loading this DLL as a plugin.

	These functions are called during important events throughout
	the execution of the program, allowing the developer(s) to run
	code that takes advantage of these processes (such as rendering
	DirectX elements and reading player input).
*/



/*
	Called exactly once after DirectX9 overlay is initialized.
*/
__declspec(dllexport) void __stdcall initialize_plugin()
{
	// Set overlay info strings
	set_text_feed_title("Dark Souls Overhaul Mod");
	print("-------------TEST BUILD-------------", 0, false, SP_D3D9O_TEXT_COLOR_ORANGE);


	// Register console commands
	ModData::register_console_commands();

	// Initialize pointers
	GameData::init_pointers();

	// Apply secondary phantom limit patch
	GameData::increase_phantom_limit2();

	// Disable "Framerate insufficient for online play" error
	GameData::disable_low_fps_disconnect();


	
	
	ModData::initialized = true; // Should be the last statement of this function
}



/*
	Continuously called from the main thread loop in the overlay DLL.
*/
__declspec(dllexport) void __stdcall main_loop()
{
	// Use this function for code that must be called rerpeatedly, such as checking flags or waiting for values to change


}



/*
	Called every frame inside of a BeginScene()/EndScene() pair.

		Microsoft documentation for IDirect3DDevice9 Interface:
		https://msdn.microsoft.com/en-us/library/windows/desktop/bb174336(v=vs.85).aspx
		
*/
__declspec(dllexport) void __stdcall draw_overlay(std::string *text_feed_info_header)
{
	/*
		Allows drawing of additional overlay elements and live text feed info header elements.
		An appropriate stateblock for overlay drawing is applied before this function is called.
		
		NOTE: Use the _d3d9_dev macro to obtain the game's IDirect3DDevice9 for drawing new elements
	*/
	

	// Show node count in text feed info header
	GameData::get_node_count(text_feed_info_header);

}



/*
	Called every frame from the Present() member function of either the D3D9 device or SwapChain (depends on the game's rendering method; Dark Souls uses Device).
	If called from IDirect3DDevice9::Present (rather than IDirect3DSwapChain9::Present), dwFlags is always zero.
		
		Microsoft documentation for IDirect3DDevice9::Present():
		https://msdn.microsoft.com/en-us/library/windows/desktop/bb174423(v=vs.85).aspx

*/
__declspec(dllexport) void __stdcall present(const RECT *pSourceRect, const RECT *pDestRect, HWND hDestWindowOverride, const RGNDATA *pDirtyRegion, DWORD dwFlags)
{
	// NOTE: Use the _d3d9_dev macro to obtain the game's IDirect3DDevice9 for drawing new elements


}




/*
	Called from the EndScene() member function of the D3D9 device.
	Note: This function isn't necessarily called exactly once per frame (could be more, less, or not at all).
		
		Microsoft documentation for IDirect3DDevice9::EndScene():
		https://msdn.microsoft.com/en-us/library/windows/desktop/bb174375(v=vs.85).aspx

*/
__declspec(dllexport) void __stdcall end_scene()
{
	// NOTE: Use the _d3d9_dev macro to obtain the game's IDirect3DDevice9 for drawing new elements


}




/*
	Called every time GetRawInputData() is called with a non-null pData argument.
		
		Microsoft documentation for GetRawInputData:
		https://msdn.microsoft.com/en-us/library/windows/desktop/ms645596(v=vs.85).aspx

*/
__declspec(dllexport) void __stdcall get_raw_input_data(RAWINPUT *pData, PUINT pcbSize)
{
	// Use this function to read player input, if needed


}




/*
	Called every time GetRawInputData() is called with a non-null pData argument.
	If return value is true, in-game keyboard input is disabled.
*/
__declspec(dllexport) bool __stdcall disable_player_input()
{


	return false;
}