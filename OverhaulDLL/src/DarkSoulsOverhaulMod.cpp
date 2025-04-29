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
#include "CustomInvasionTypes.h"
#include "UIFeatures.h"
#include "Rollback.h"
#include "PlayerVisualsValidationFix.h"
#include "ServerMonitor.h"

HMODULE d3d11_module;
FILE* logfile = NULL;

//VERY BASIC update check
void Update_Check()
{
    char cmd[] = "Powershell.exe -Command \"Invoke-WebRequest https://raw.githubusercontent.com/metal-crow/Dark-Souls-1-Overhaul/master/VERSION -O VERSION\"";

    //download the new version info
    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    bool err = CreateProcess(NULL, cmd, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
    if (err == 0)
    {
        ConsoleWrite("Update_Check failed:%d", GetLastError());
    }
    WaitForSingleObject(pi.hProcess, 5 * 1000); // Wait 5 sec max
    TerminateProcess(pi.hProcess, 0);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    //read the new version info
    FILE *fp = NULL;
    if (fopen_s(&fp, "./VERSION", "r") != 0 || fp == NULL)
    {
        ConsoleWrite("Update_Check failed: unable to open file.");
        return;
    }
    uint64_t new_version;
    fscanf_s(fp, "%llu", &new_version);
    fclose(fp);

    //basic sanity check new version
    if (new_version < 2000000000 || new_version >= 3000000000)
    {
        ConsoleWrite("Update_Check failed: got bad new version. %llu", new_version);
        return;
    }

    if (new_version > VERSION_RAW)
    {
        MessageBox(NULL,
            std::string("There is a new update available! Please go to the nexus page and download the newest version!").c_str(),
            std::string("Update").c_str(),
            MB_OK);
        ConsoleWrite("Found new update, version %llu is > our version %llu", new_version, VERSION_RAW);
    }
    else
    {
        ConsoleWrite("No updates found");
    }
}

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
#ifndef ANTIBAN_ONLY
    Game::injections_init(); //only do injections after we disable the built in code checking

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

#ifndef DEBUG
    set_crash_handlers();
#endif
#endif

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
    Menu::Saves::init_custom_saves();
    AntiCheat::start();
    Game::set_gui_hpbar_max();
    Game::stopDurabilityDamage(Mod::stop_durability_damage);
    BloodborneRally::start();
    DeadAngles::start();
    EquipmentUnlock::start();
    InvisBackstabFix::start();
    AnimationEdits::start();
    SpellDesync::start();
    PhantomUnshackle::start();
    FixAnkles::start();
    ModNetworking::start();
    ServerMonitor::start();
    FileReloading::start();
    CustomInvasionTypes::start();
    //UIFeatures::start();
    //Rollback::start();
    Game::disable_low_fps_disconnect(true);
    PlayerVisualsValidationFix::start();

    ConsoleWrite("All initial loading finished!");

    // Start callback for first character loading injections
    MainLoop::setup_mainloop_callback(Game::on_character_load, NULL, "on_character_load");

    //start callback for setting the preferred mode
    MainLoop::setup_mainloop_callback(Mod::mode_setting_process, NULL, "mode_setting_process");
    CreateThread(NULL, 0, mod_mode_visuals_setting_thread, NULL,  0, NULL);

    Update_Check();

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
