/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Metal-Crow  -  C++
        Sean Pesce  -  C++

*/

#include "SP/io/keybinds.h"

#include "DarkSoulsOverhaulMod.h"
#include "AntiAntiCheat.h"
#include "AntiCheat.h"
#include "BloodborneRallySystem.h"
#include "DeadAngles.h"
#include "EquipmentUnlock.h"
#include "AnimationEdits.h"
#include "PhantomUnshackle.h"
#include "InvisibleBackstabAttackFix.h"
#include "Menu/SavedCharacters.h"
#include "Files.h"
#include "InputUtil.h"
#include "SpellDesyncFixes.h"
#include "FixAnkles.h"
#include "ModNetworking.h"
#include "MainLoop.h"
#include "FileReloading.h"

HMODULE d3d11_module;

/*
    Called from DllMain when the plugin DLL is first loaded into memory (PROCESS_ATTACH case).
    This means this code will be run before _any_ Dark Souls code is.
    NOTE: This function runs in the same thread as DllMain, so code implemented here will halt
    game loading. Code in this function should be limited to tasks that absolutely MUST be
    executed before the game loads. For less delicate startup tasks, use on_process_attach_async()
    or initialize_plugin().
*/
BOOL on_process_attach(HMODULE h_module, LPVOID lp_reserved)
{
    ConsoleWrite("%s\n\n",DS1_OVERHAUL_TXT_INTRO);
    d3d11_module = h_module;

    Game::init();
    AntiAntiCheat::start();

    CreateThread(NULL,  // Default security attributes
                 0,     // Use default stack size
                 on_process_attach_async, // Thread function name
                 NULL,  // Argument to thread function
                 0,     // Use default creation flags
                 NULL); // Optionally returns the thread identifier

    // Load user preferences & keybinds from settings file
    Mod::get_init_preferences();
    Mod::get_user_keybinds();

    // Initialize XInput hook (used for intercepting button to change save files)
    Input::initialize();

    // Apply increased memory limit patch
    Game::set_memory_limit();

    // Load alternate files
    Files::apply_function_intercepts();

    // Check for existence of non-default game files
    Files::check_custom_archive_file_path();
    Files::check_custom_save_file_path();
    Files::check_custom_game_config_file_path();

    // start callback handler
    MainLoop::start();

    return TRUE;
}

/*
    Called from on_process_attach when the DLL is first loaded into memory (PROCESS_ATTACH case).
    This function runs in a separate thread from DllMain, so code implemented here does NOT
    pause game loading. Code that must be executed before the game loads should be implemented
    in on_process_attach() instead of this function.
*/
DWORD WINAPI on_process_attach_async(LPVOID lpParam)
{
    set_crash_handlers();

    Menu::Saves::init_custom_saves();
    AntiCheat::start();
    Game::increase_gui_hpbar_max();
    BloodborneRally::start();
    DeadAngles::start();
    EquipmentUnlock::start();
    InvisBackstabFix::start();
    AnimationEdits::start();
    SpellDesync::start();
    PhantomUnshackle::start();
    FixAnkles::start();
    ModNetworking::start();
    FileReloading::start();
    Game::disable_low_fps_disconnect(Mod::disable_low_fps_disconnect);

    ConsoleWrite("%s All initial loading finished!", Mod::output_prefix);

    // Start callback for first character loading injections
    MainLoop::setup_mainloop_callback(Game::on_character_load, NULL, "on_character_load");

    //start callback for setting the preferred mode
    MainLoop::setup_mainloop_callback(Mod::set_preferred_mode, NULL, "set_preferred_mode");

    return 0;
}

// Called from DllMain when the plugin DLL is unloaded (PROCESS_DETACH case)
BOOL on_process_detach(HMODULE h_module, LPVOID lp_reserved)
{
    // Exit tasks should be performed here
    return TRUE;
}


// Called from DllMain (THREAD_ATTACH case)
BOOL on_thread_attach(HMODULE h_module, LPVOID lp_reserved)
{
    return TRUE;
}

// Called from DllMain (THREAD_DETACH case)
BOOL on_thread_detach(HMODULE h_module, LPVOID lp_reserved)
{
    return TRUE;
}
