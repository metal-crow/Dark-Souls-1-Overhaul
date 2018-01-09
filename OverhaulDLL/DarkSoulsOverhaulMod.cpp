/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Metal-Crow  -  C++
        Sean Pesce  -  C++

*/

#include "DllMain.h"
#include "Files.h"
#include "AntiCheat.h"
#include "BloodborneRallySystem.h"
#include "XInputUtil.h"


/*
    Called from DllMain when the plugin DLL is first loaded into memory (PROCESS_ATTACH case).
    NOTE: This function runs in the same thread as DllMain, so code implemented here will halt
    game loading. Code in this function should be limited to tasks that absolutely MUST be
    executed before the game loads. For less delicate startup tasks, use on_process_attach_async()
    or initialize_plugin().
*/
void on_process_attach()
{
    // Initialize file I/O monitoring data structs
    Files::init_io_monitors();

    // Load startup preferences from settings file
    Mod::get_startup_preferences();

    // Check if game version is supported
    if (!game_version_is_supported)
    {
        // @TODO: Handle wrong game version: Show dialog box and continue loading without applying mod

        // Placeholder handling of wrong game version:
        Mod::startup_messages.push_back(Mod::output_prefix + "WARNING: Unsupported game version detected.");
        MessageBox(NULL, std::string("Invalid game version detected. Change to supported game version, or disable the Dark Souls Overhaul Mod.").c_str(),
                   "ERROR: Dark Souls Overhaul Mod - Wrong game version", NULL);

        exit(0);
    }

    // Apply increased memory limit patch
    Game::set_memory_limit(Game::memory_limit);

    // Change game version number
    Files::apply_function_intercepts();

    // Check for existence of non-default game files
    Files::check_custom_archive_file_path();
    Files::check_custom_save_file_path();
    Files::check_custom_game_config_file_path();

    if (!Mod::legacy_mode)
    {
        // Change game version number
        Game::set_game_version(DS1_VERSION_OVERHAUL);

        // Apply first part of phantom limit patch
        Game::increase_phantom_limit1();
    }
}

/*
    Called from DllMain when the plugin DLL is first loaded into memory (PROCESS_ATTACH case).
    This function runs in a separate thread from DllMain, so code implemented here does NOT
    pause game loading. Code that must be executed before the game loads should be implemented
    in on_process_attach() instead of this function.
*/
DWORD WINAPI on_process_attach_async(LPVOID lpParam)
{
    // Start anti-cheat
    AntiCheat::start();

    // Allow modded effectIDs
    Game::unrestrict_network_synced_effectids();

    // Enable Bloodborne rally system
    if (!Mod::legacy_mode)
    {
        BloodborneRally::start();
    }

    // Initialize XInput hooks
    XInput::initialize();
    return 0;
}

// Called from DllMain when the plugin DLL is unloaded (PROCESS_DETACH case)
void on_process_detach()
{
    // Exit tasks should be performed here

    // Cancel any unfinished tasks
    Mod::deferred_tasks_complete = true;
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
    if (Mod::legacy_mode)
    {
        set_text_feed_title("[Dark Souls Overhaul Mod (Legacy Mode)]");
    }
    else
    {
        set_text_feed_title("[Dark Souls Overhaul Mod]");
    }
    print("-------------DARK SOULS OVERHAUL TEST BUILD-------------", 0, false, SP_D3D9O_TEXT_COLOR_ORANGE);

    // Print startup messages
    for (std::string msg : Mod::startup_messages)
        print_console(msg.c_str());

    // Load user preferences & keybinds from settings file
    Mod::get_user_preferences();
    Mod::get_user_keybinds();

    // Register console commands
    Mod::register_console_commands();

    // Initialize pointers
    Game::init_pointers();

    if (!Mod::legacy_mode)
        // Apply secondary phantom limit patch
        Game::increase_phantom_limit2();

    if (Mod::disable_low_fps_disconnect)
        // Disable "Framerate insufficient for online play" error
        Game::enable_low_fps_disconnect(false);

    // Start thread for deferred tasks
    if (!CreateThread(NULL, 0, deferred_tasks, NULL, 0, NULL))
        ; // Error creating new thread

    Mod::initialized = true; // Should be the last statement in this function
}

/*
    Continuously called from the main thread loop in the overlay DLL.
*/
__declspec(dllexport) void __stdcall main_loop()
{
    // Use this function for code that must be called rerpeatedly, such as checking flags or waiting for values to change

    if (Mod::initialized)
    {
        // Check for pending save file index changes
        if (Files::save_file_index_pending_set_next) {
            if (Files::saves_menu_is_open()) {
                Files::set_save_file_next();
            }
            Files::save_file_index_pending_set_next = false;
        }
        if (Files::save_file_index_pending_set_prev) {
            if (Files::saves_menu_is_open()) {
                Files::set_save_file_prev();
            }
            Files::save_file_index_pending_set_prev = false;
        }

        // Update multiplayer node graph HUD element display status
        Hud::set_show_node_graph(Hud::get_show_node_graph());

        // Update multiplayer node count
        Game::node_count = Game::get_node_count();

        // If cheats are enabled, make sure saveing and multiplayer are disabled
        if (Mod::cheats)
        {
            if (*(bool*)Game::saves_enabled.resolve())
                // Cheats are on, but saving is enabled; disable saving
                Game::saves_enabled.write(false);

            if (Game::get_game_version() != DS1_VERSION_OVERHAUL_CHEATS)
                // Change to network for players that have cheats enabled
                Game::set_game_version(DS1_VERSION_OVERHAUL_CHEATS);

            // @TODO: Check that multiplayer is disabled? (first figure out how to disable it)
        }

        // Check if the player is stuck at the bonfire, and if so, automatically apply the bonfire input fix
        Game::check_bonfire_input_bug();
    }
}

/*
    Called from initialize_plugin() after the DLL is loaded and initialized. This function is for
    tasks  that can only be  executed AFTER certain events have  occurred, but don't need to be
    executed BEFORE any specific time or event.
*/
DWORD WINAPI deferred_tasks(LPVOID lpParam)
{
    // Sleep time (in milliseconds) between loop iterations
    int wait_time = 500;

    // Wait for event: first character loaded in this instance of the game
    int char_status = DS1_PLAYER_STATUS_LOADING;
    while ((!Mod::deferred_tasks_complete) && char_status == DS1_PLAYER_STATUS_LOADING)
    {
        Game::player_char_status.read(&char_status);
        Sleep(wait_time);
    }

    // Perform tasks that rely on a character being loaded
    Game::on_first_character_loaded();

    ////////// Implement additional wait conditions here //////////

    // Finished deferred tasks
    Mod::deferred_tasks_complete = true;
    return 0;
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

        NOTE: Use the _d3d9_dev macro to obtain the game's IDirect3DDevice9 for drawing new elements.

        WARNING: This function is called from inside the DirectX9 Device Present() function. Keep
        code in this function as optimized as possible to avoid lowering FPS.
    */

    if (Mod::show_node_count)
    {
        // Show node count in text feed info header
        if (Game::node_count > -1)
            text_feed_info_header->append("[Nodes: ").append(std::to_string(Game::node_count)).append("]  ");
        else
            text_feed_info_header->append("[Nodes: --]  ");
    }
}

/*
    Called every frame from the Present() member function of either the D3D9 device or SwapChain (depends on the game's rendering method; Dark Souls uses Device).
    If called from IDirect3DDevice9::Present (rather than IDirect3DSwapChain9::Present), dwFlags is always zero.

        Microsoft documentation for IDirect3DDevice9::Present():
        https://msdn.microsoft.com/en-us/library/windows/desktop/bb174423(v=vs.85).aspx

*/
__declspec(dllexport) void __stdcall present(const RECT *pSourceRect, const RECT *pDestRect, HWND hDestWindowOverride, const RGNDATA *pDirtyRegion, DWORD dwFlags)
{
    /*
        NOTE: Use the _d3d9_dev macro to obtain the game's IDirect3DDevice9 for drawing new elements.

        WARNING: This function is called from inside the DirectX9 Device Present() function. Keep
        code in this function as optimized as possible to avoid performance issues.
    */
}

/*
    Called from the EndScene() member function of the D3D9 device.
    Note: This function isn't necessarily called exactly once per frame (could be more, less, or not at all).

        Microsoft documentation for IDirect3DDevice9::EndScene():
        https://msdn.microsoft.com/en-us/library/windows/desktop/bb174375(v=vs.85).aspx

*/
__declspec(dllexport) void __stdcall end_scene()
{
    /*
        NOTE: Use the _d3d9_dev macro to obtain the game's IDirect3DDevice9 for drawing new elements.

        WARNING: This function is called from inside the DirectX9 Device EndScene() function. Keep
        code in this function as optimized as possible to avoid performance issues.
    */
}

/*
    Called every time GetRawInputData() is called with a non-null pData argument.

    Use this function to read player input, if needed.

        Microsoft documentation for GetRawInputData:
        https://msdn.microsoft.com/en-us/library/windows/desktop/ms645596(v=vs.85).aspx

    @return     True if the given input message should be cancelled before being sent to
                the game, thereby disabling game input. Otherwise, returns false.
*/
__declspec(dllexport) bool __stdcall get_raw_input_data(RAWINPUT *pData, PUINT pcbSize)
{
    // Mouse cursor position
    static POINT cursor_position; // Static variable; saved between calls

    // Determine input device type
    switch (pData->header.dwType)
    {
        case RIM_TYPEMOUSE:

            // Handle mouse input

            // Mouse button press
            switch (pData->data.mouse.usButtonFlags)
            {
                case RI_MOUSE_LEFT_BUTTON_DOWN:

                    break;

                case RI_MOUSE_LEFT_BUTTON_UP:

                    break;

                case RI_MOUSE_RIGHT_BUTTON_DOWN:

                    break;

                case RI_MOUSE_RIGHT_BUTTON_UP:

                    break;

                case RI_MOUSE_MIDDLE_BUTTON_DOWN:

                    break;

                case RI_MOUSE_MIDDLE_BUTTON_UP:

                    break;

                case RI_MOUSE_WHEEL:
                    if (pData->data.mouse.usButtonData == 120)
                    {
                        // Scrolling up
                    }
                    else if (pData->data.mouse.usButtonData == 65416)
                    {
                        // Scrolling down
                    }
                    break;

                    // Additional mouse buttons
                case RI_MOUSE_BUTTON_4_DOWN:
                case RI_MOUSE_BUTTON_4_UP:
                case RI_MOUSE_BUTTON_5_DOWN:
                case RI_MOUSE_BUTTON_5_UP:
                    break;
            } // switch (pData->data.mouse.usButtonFlags)

            // Mouse movement
            switch (pData->data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE)
            {
                case MOUSE_MOVE_ABSOLUTE:
                    // Mouse movement data is based on absolute position
                    cursor_position.x = pData->data.mouse.lLastX;
                    cursor_position.y = pData->data.mouse.lLastY;
                    break;
                case MOUSE_MOVE_RELATIVE:
                    // Mouse movement data is relative (based on last known position)
                    cursor_position.x += pData->data.mouse.lLastX;
                    cursor_position.y += pData->data.mouse.lLastY;

                    // Keep cursor on-screen
                    RECT window_rect;
                    if (!GetClientRect(_game_window, &window_rect))
                    {
                        // Handle error
                        break;
                    }
                    if (cursor_position.x < 0) // Too far left
                    {
                        cursor_position.x = 0;
                    }
                    else if (cursor_position.x > window_rect.right) // Too far right
                    {
                        cursor_position.x = window_rect.right;
                    }
                    if (cursor_position.y < 0) // Too far up
                    {
                        cursor_position.y = 0;
                    }
                    else if (cursor_position.y > window_rect.bottom) // Too far down
                    {
                        cursor_position.x = window_rect.bottom;
                    }
                    break;
            }

            break; // case RIM_TYPEMOUSE

        case RIM_TYPEKEYBOARD:

            // Handle keyboard input

            // Determine type of keyboard input
            switch (pData->data.keyboard.Message)
            {
                case WM_KEYDOWN: // A nonsystem key is a key that is pressed when the ALT key is not pressed

                    // Determine which key was pressed
                    switch (pData->data.keyboard.VKey)
                    {
                        // Handle key press

                        default:
                            break;
                    }

                case WM_SYSKEYDOWN: // A system key is a key that is pressed when the ALT key is also down

                    // Determine which syskey was pressed
                    switch (pData->data.keyboard.VKey)
                    {
                        // Handle key press (when ALT is down)

                        default:
                            break;
                    }
                    break; // case  WM_KEYDOWN || WM_SYSKEYDOWN

                case WM_KEYUP:

                    // Determine which key was released
                    switch (pData->data.keyboard.VKey)
                    {
                        // Handle key release (key is no longer being pressed)

                        default:
                            break;
                    }
                    break; // case WM_KEYUP
            } // switch (pData->data.keyboard.Message)
            break; // case RIM_TYPEKEYBOARD

        case RIM_TYPEHID:

            // Handle input from some other type of hardware input device

            break; // case RIM_TYPEHID
    } // switch (pData->header.dwType)

    // False = don't disable game input (allow this input to reach the game)
    return false;
}
