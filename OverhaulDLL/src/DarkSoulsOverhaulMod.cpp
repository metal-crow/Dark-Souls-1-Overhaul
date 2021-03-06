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
    global::cmd_out << DS1_OVERHAUL_TXT_INTRO "\n\n";
    d3d11_module = h_module;

    // Check if game version is supported
    /*if (Game::get_game_version() != DS1_GAME_VERSION_ENUM::DS1_VERSION_RELEASE)
    {
    Mod::startup_messages.push_back(Mod::output_prefix + "WARNING: Unsupported game version detected.");
    MessageBox(NULL, std::string("Invalid game version detected. Change to supported game version, or disable the Dark Souls Overhaul Mod.").c_str(),
    "ERROR: Dark Souls Overhaul Mod - Wrong game version", NULL);
    exit(0);
    }*/

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
    Game::unrestrict_network_synced_effectids();
    BloodborneRally::start();
    DeadAngles::start();
    EquipmentUnlock::start();
    InvisBackstabFix::start();
    AnimationEdits::start();
    SpellDesync::start();
    PhantomUnshackle::start();
    FixAnkles::start();
    ModNetworking::start();
    if (Mod::disable_low_fps_disconnect) {
        Game::disable_low_fps_disconnect(true);
    }

    global::cmd_out << (Mod::output_prefix + "All initial loading finished!\n");

    // Start dedicated thread for first character loading injections
    CreateThread(NULL, 0, wait_for_first_char_load, NULL, 0, NULL);

    //All actions finished, this now serves as a Main Loop which continues to run in background
    bool waiting_for_load = false;
    while (true)
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
        if (Files::save_file_index_make_new) {
            if (Files::saves_menu_is_open()) {
                Files::create_new_save_file();
            }
            Files::save_file_index_make_new = false;
        }

        // Check if the character is loading, and apply actions that need to be _reapplied_ after every loading screen
        int32_t char_status = Game::get_player_char_status();
        if (char_status == DS1_PLAYER_STATUS_LOADING) {
            waiting_for_load = true;
        }
        if (waiting_for_load == true && char_status != DS1_PLAYER_STATUS_LOADING) {
            Game::on_reloaded();
            waiting_for_load = false;
        }

        Sleep(150);
    }

    return 0;
}


DWORD WINAPI wait_for_first_char_load(LPVOID lpParam) {
    // Wait for event: first character loaded in this instance of the game
    int char_status = DS1_PLAYER_STATUS_LOADING;
    while (char_status == DS1_PLAYER_STATUS_LOADING) {
        char_status = Game::get_player_char_status();
        Sleep(500);
    }

    // Perform tasks that rely on a character being loaded
    Game::on_first_character_loaded();

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
