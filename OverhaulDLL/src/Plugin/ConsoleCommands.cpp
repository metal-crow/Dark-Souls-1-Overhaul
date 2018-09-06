/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Sean Pesce  -  C++, DirectX9 overlay
        Wulf2k      -  Reverse engineering (multiplayer network data)


    This file defines custom console commands that will
    be registered with the in-game console at runtime.
*/

#include "DllMain.h"
#include "Plugin/ConsoleCommands.h"
#include "AntiCheat.h"
#include "Save/Sl2.h"
#include "Challenge/AggressiveAi.h"
#include "Challenge/BlackPhantomEnemies.h"
#include "Challenge/GravelordPhantoms.h"
#include "Menu/SavedCharacters.h"
#include "LadderFix.h"
#include "Updates.h"
#include "DurabilityBars.h"
#include "Menu/Dialog.h"
#include "MultiTribute.h"
#include "MultiConsume.h"
#include "AnimationEdits.h"
#include "L3Jump.h"


///////////////////////////////////////////////////////////
//////////////// Console command functions ////////////////
///////////////////////////////////////////////////////////



// Plays the mod credits (lists the people who helped create the mod)
int cc_credits(std::vector<std::string> args, std::string *output)
{
    // @TODO
    //output->append("\n\n");
    //output->append("    ///////////////////////////////////////////////////////////\n");
    //output->append("    ///////////////// Dark Souls Overhaul Mod /////////////////\n");
    //output->append("    ///////////////////////////////////////////////////////////\n");
    //output->append("\n                ...credits coming eventually...\n\n");

    CreateThread(NULL,				// Default security attributes
                 0,					// Use default stack size
                 Mod::play_credits, // Thread function name
                 NULL,				// Argument to thread function
                 0,					// Use default creation flags
                 NULL);             // Returns the thread identifier

    return CONSOLE_COMMAND_SUCCESS;
}


// Plays the detailed mod credits (lists the people who helped create the mod, as well as their roles in development)
int cc_credits_detailed(std::vector<std::string> args, std::string *output)
{
    CreateThread(NULL,				// Default security attributes
                 0,					// Use default stack size
                 Mod::play_credits_detailed, // Thread function name
                 NULL,				// Argument to thread function
                 0,					// Use default creation flags
                 NULL);             // Returns the thread identifier

    return CONSOLE_COMMAND_SUCCESS;
}


// Prints build title and compile time for this build of the Dark Souls Overhaul project
int cc_build(std::vector<std::string> args, std::string *output)
{
    output->append(std::string(DS1_OVERHAUL_TXT_INTRO "\nCompiled:  ") + Updates::VERSION);
    return CONSOLE_COMMAND_SUCCESS;
}


// Prints the Overhaul Message of the Day
int cc_overhaul_motd(std::vector<std::string> args, std::string *output)
{
    if (!Updates::motd().empty())
    {
        output->append("MESSAGE OF THE DAY:\n\n" + Updates::motd());
    }
    else
    {
        output->append("No Message of the Day.");
    }
    return CONSOLE_COMMAND_SUCCESS;
}


// Prints the latest known Dark Souls Overhaul version and a link to download it (if available)
int cc_overhaul_update(std::vector<std::string> args, std::string *output)
{
    if (!Updates::latest().empty())
    {
        if (Updates::latest() == std::string(Updates::VERSION))
        {
            output->append("    No new updates.");
        }
        else if (Updates::compare_versions() > 0)
        {
            output->append("\n    UPDATE AVAILABLE:\n    Local  build: " + std::string(Updates::VERSION) + "\n    Remote build: " + std::string(Updates::latest())
                          + "\n\n    " + (Updates::download_url().empty() ? "Download URL unknown; try here:\n    " DS1_OVERHAUL_DEFAULT_DOWNLOAD_URL_
                                          : ("Download the latest version here:\n    " + Updates::download_url())));
        }
        else
        {
            // Local is somehow newer than remote...
            output->append("\n    WARNING: VERSION MISMATCH (Remote build may be older than local build)\n\n    POSSIBLE UPDATE AVAILABLE:\n    Local  build: " + std::string(Updates::VERSION) + "\n    Remote build: " + std::string(Updates::latest())
                          + "\n\n    " + (Updates::download_url().empty() ? "Download URL unknown; try here:\n    " DS1_OVERHAUL_DEFAULT_DOWNLOAD_URL_
                                          : ("Download the latest version here:\n    " + Updates::download_url())));
        }
    }
    else
    {
        output->append("    No version info.");
    }
    return CONSOLE_COMMAND_SUCCESS;
}


// Prints the download URL for the latest known Dark Souls Overhaul version
int cc_overhaul_download_url(std::vector<std::string> args, std::string *output)
{
    if (!Updates::download_url().empty())
    {
        output->append("Download the latest version here:\n    " + Updates::download_url());
    }
    else
    {
        output->append("Latest download URL unknown; try here:\n    " DS1_OVERHAUL_DEFAULT_DOWNLOAD_URL_);
    }
    return CONSOLE_COMMAND_SUCCESS;
}



// Prints all remote hosts being used for updates/version information/MoTD
int cc_overhaul_remote_update_hosts(std::vector<std::string> args, std::string *output)
{
    if (Updates::sources().empty())
    {
        output->append("No remote hosts");
    }
    else
    {
        output->append(std::to_string(Updates::sources().size()) + " remote host" + (Updates::sources().size() == 1 ? std::string() : std::string("s")) + ":\n");
    }

    for (unsigned int i = 0; i < Updates::sources().size(); i++)
    {
        output->append("\n" + std::to_string(i) + (Updates::skip_source[i] ? ": [DISABLED] " : ":            ") + Updates::sources()[i]);
    }
    return CONSOLE_COMMAND_SUCCESS;
}


// Enables/disables mouse input
int cc_mouse_input(std::vector<std::string> args, std::string *output)
{
    int ret_val = CONSOLE_COMMAND_SUCCESS;
    if (args.size() > 0) {
        switch (parse_toggle_arg(args.at(0).c_str()))
        {
            case 0:
                Mod::mouse_input = false;
                break;
            case 1:
                Mod::mouse_input = true;
                break;
            default:
                output->append(ERROR_INVALID_BOOL_ARGUMENT + "\n");
                ret_val = ERROR_INVALID_PARAMETER;
                break;
        }
    }
    if (Mod::mouse_input) {
        output->append("Mouse input = enabled");
    } else {
        output->append("Mouse input = disabled");
    }
    return ret_val;
}


// Enables/disables camera lock when console is open
int cc_console_lock_cam(std::vector<std::string> args, std::string *output)
{
    int ret_val = CONSOLE_COMMAND_SUCCESS;
    if (args.size() > 0) {
        switch (parse_toggle_arg(args.at(0).c_str()))
        {
            case 0:
                Mod::console_lock_camera = false;
                break;
            case 1:
                Mod::console_lock_camera = true;
                break;
            default:
                output->append(ERROR_INVALID_BOOL_ARGUMENT + "\n");
                ret_val = ERROR_INVALID_PARAMETER;
                break;
        }
    }
    if (Mod::console_lock_camera) {
        output->append("Camera lock when console is open = enabled");
    } else {
        output->append("Camera lock when console is open = disabled");
    }
    return ret_val;
}


// Checks if Legacy Mode is enabled
int cc_legacy_mode(std::vector<std::string> args, std::string *output)
{
    if (Mod::legacy_mode)
        output->append("Legacy Mode is enabled.");
    else
        output->append("Legacy Mode is disabled.");
    return CONSOLE_COMMAND_SUCCESS;
}


// Enables cheats until game is restarted (also disables saving and @TODO multiplayer)
int cc_cheats(std::vector<std::string> args, std::string *output)
{
    int return_val = CONSOLE_COMMAND_SUCCESS;

    if (args.size() > 0) {
        switch (parse_toggle_arg(args.at(0).c_str()))
        {
            case 0:
                if (Mod::cheats)
                    output->append("Restart the game to disable cheats and re-enable saving and multiplayer.");
                else
                    output->append("Cheats = disabled");
                break;
            case 1:
                if (!Mod::cheats)
                {
                    Game::saves_enabled.write(false);
                    Game::set_game_version(DS1_VERSION_OVERHAUL_CHEATS);
                    // @TODO: Disable multiplayer
                    if (Mod::cheats_warning)
                        print("WARNING: Cheats enabled. Saving and multiplayer functions disabled. Restart game to disable cheats", 0, false, SP_D3D9O_TEXT_COLOR_RED);
                    else
                        print_console("WARNING: Cheats enabled. Saving and multiplayer functions disabled. Restart game to disable cheats");
                }
                else
                    output->append("Cheats = enabled. Saving and multiplayer functions have been disabled. Restart the game to disable cheats.");
                Mod::cheats = true;
                break;
            default:
                output->append(ERROR_INVALID_BOOL_ARGUMENT + "\n");
                if (Mod::cheats)
                    output->append("Cheats = enabled. Saving and multiplayer functions have been disabled. Restart the game to disable cheats.");
                else
                    output->append("Cheats = disabled");
                return_val = ERROR_INVALID_PARAMETER;
                break;
        }
    }
    else
    {
        if (Mod::cheats)
            output->append("Cheats = enabled. Saving and multiplayer functions have been disabled. Restart the game to disable cheats.");
        else
            output->append("Cheats = disabled");
    }
    return return_val;
}


// Sets the current save file index (must be called while viewing saved characters menu)
int cc_save_file_index(std::vector<std::string> args, std::string *output)
{
    int ret_val = ERROR_SUCCESS;
    int new_index = -1;
    if (args.size() > 0) {
        if (args.at(0) == "next") {
            SetLastError(ERROR_SUCCESS);
            Files::set_save_file_next();
            ret_val = GetLastError();
        } else if (args.at(0) == "prev" || args.at(0) == "previous") {
            SetLastError(ERROR_SUCCESS);
            Files::set_save_file_prev();
            ret_val = GetLastError();
        } else {
            // Index specified
            try {
                new_index = std::stoi(args.at(0).c_str(), NULL);
            }
            catch (std::invalid_argument) {
                ret_val = ERROR_BAD_ARGUMENTS;
            }
            catch (std::out_of_range) {
                ret_val = ERROR_RANGE_NOT_FOUND;
            }
        }
        
        if (new_index >= 0) {
            SetLastError(ERROR_SUCCESS);
            Files::set_save_file_index(new_index);
            ret_val = GetLastError();
            if (ret_val != ERROR_SUCCESS)
            {
                output->append("ERROR: Failed to update index\n");
            }
        } else if (ret_val != ERROR_SUCCESS) {
            output->append("ERROR: Invalid argument (Index must be a positive integer)\n");
        }
    }
    output->append("Save file index = " + std::to_string(Files::save_file_index));
    return ret_val;
}


// Sets the current save file index to the next save file (must be called while viewing saved characters menu)
int cc_save_file_next(std::vector<std::string> args, std::string *output)
{
    Files::set_save_file_next();
    int ret_val = GetLastError();
    output->append("Save file index = " + std::to_string(Files::save_file_index));
    return ret_val;
}


// Sets the current save file index to the previous save file (must be called while viewing saved characters menu)
int cc_save_file_prev(std::vector<std::string> args, std::string *output)
{
    Files::set_save_file_prev();
    int ret_val = GetLastError();
    output->append("Save file index = " + std::to_string(Files::save_file_index));
    return ret_val;
}


// Creates a new save file with 10 empty character slots (Existing save files are not modified)
int cc_save_file_create(std::vector<std::string> args, std::string *output)
{
    std::string file, out;
    if (Mod::custom_save_file_path.length() > 0) {
        if (string_wide_to_mb((wchar_t*)Mod::custom_save_file_path.c_str(), file)) {
            output->append("ERROR: Failed to create new save file (Conversion error)");
            return ERROR_CLUSTER_INVALID_STRING_FORMAT;
        }
        if (SaveFile::generate_empty_save_file(file.c_str())) {
            output->append("ERROR: Failed to create new save file (Write error)");
            return ERROR_WRITE_FAULT;
        }
    } else if (Files::default_save_file_path.length() > 0) {
        file = Files::default_save_file_path;
        if (SaveFile::generate_empty_save_file(Files::default_save_file_path.c_str())) {
            output->append("ERROR: Failed to create new save file (Write error)");
            return ERROR_WRITE_FAULT;
        }
    } else {
        file = Sl2::FILE_NAME_DEFAULT;
        if (SaveFile::generate_empty_save_file()) {
            output->append("ERROR: Failed to create new save file (Write error)");
            return ERROR_WRITE_FAULT;
        }
    }
    int count = Sl2::get_save_file_count(file.c_str());
    std::wstring custom_header =
        L"                                            Save file "
        + std::to_wstring(Files::save_file_index + 1)
        + L"/" + std::to_wstring(count);
    Menu::Saves::set_custom_header_msgs(L"Select data to load." + custom_header, L"Select data to delete." + custom_header);
    out = "Created new save file: " + file;
    if (count < 11) {
        out += "_0" + std::to_string(count - 1);
    } else {
        out += "_" + std::to_string(count - 1);
    }
    output->append(out);
    return ERROR_SUCCESS;
}

// Creates a copy of the current save file (Existing save files are not modified)
int cc_save_file_copy(std::vector<std::string> args, std::string *output)
{
    std::string file, out;
    if (Mod::custom_save_file_path.length() > 0) {
        if (string_wide_to_mb((wchar_t*)Mod::custom_save_file_path.c_str(), file)) {
            output->append("ERROR: Failed to create new save file (Conversion error)");
            return ERROR_CLUSTER_INVALID_STRING_FORMAT;
        }
        if (SaveFile::copy_save_file(file.c_str(), Files::save_file_index)) {
            output->append("ERROR: Failed to create new save file (Write error)");
            return ERROR_WRITE_FAULT;
        }
    } else if (Files::default_save_file_path.length() > 0) {
        file = Files::default_save_file_path;
        if (SaveFile::copy_save_file(file.c_str(), Files::save_file_index)) {
            output->append("ERROR: Failed to create new save file (Write error)");
            return ERROR_WRITE_FAULT;
        }
    } else {
        file = Sl2::FILE_NAME_DEFAULT;
        if (SaveFile::copy_save_file(Sl2::FILE_NAME_DEFAULT, Files::save_file_index)) {
            output->append("ERROR: Failed to create new save file (Write error)");
            return ERROR_WRITE_FAULT;
        }
    }
    int count = Sl2::get_save_file_count(file.c_str());
    std::wstring custom_header =
        L"                                            Save file "
        + std::to_wstring(Files::save_file_index + 1)
        + L"/" + std::to_wstring(count);
    Menu::Saves::set_custom_header_msgs(L"Select data to load." + custom_header, L"Select data to delete." + custom_header);
    out = "Created save file copy: " + file;
    if (count < 11) {
        out += "_0" + std::to_string(count - 1);
    } else {
        out += "_" + std::to_string(count - 1);
    }
    output->append(out);
    return ERROR_SUCCESS;
}

// Sets the character name for the currently-loaded Dark Souls save game
int cc_game_character_name(std::vector<std::string> args, std::string *output)
{
    int ret_val = CONSOLE_COMMAND_SUCCESS;
    uint32_t* player_status = static_cast<uint32_t*>(Game::player_char_status.resolve());
    if (player_status && (*player_status != DS1_PLAYER_STATUS_LOADING) && Game::player_char_name.resolve()) {
        if (!args.empty())
        {
            Game::set_character_name(args[0]);
        }
        std::string name = Game::get_character_name();
        if (!name.empty())
        {
            output->append("Character name = " + name);
        }
    }
    else
    {
        output->append("ERROR: No character loaded.");
    }
    return ret_val;
}

// Enables/disables "Aggressive AI" challenge mod
int cc_challenge_aggressive_ai(std::vector<std::string> args, std::string *output)
{
    int ret_val = CONSOLE_COMMAND_SUCCESS;
    if (args.size() > 0) {
        switch (parse_toggle_arg(args.at(0).c_str()))
        {
            case 0:
                if (Challenge::AggressiveAi::active()) {
                    Challenge::AggressiveAi::disable();
                }
                break;
            case 1:
                if (!Challenge::AggressiveAi::active()) {
                    Challenge::AggressiveAi::enable();
                }
                break;
            default:
                output->append(ERROR_INVALID_BOOL_ARGUMENT + "\n");
                ret_val = ERROR_INVALID_PARAMETER;
                break;
        }
    }
    if (Challenge::AggressiveAi::active()) {
        output->append("\"Aggressive AI\" challenge mod = enabled");
    } else {
        output->append("\"Aggressive AI\" challenge mod = disabled");
    }
    return ret_val;
}

// Sets smell distance for NPCs modified by the "Aggressive AI" challenge mod
int cc_challenge_aggressive_ai_nose_distance(std::vector<std::string> args, std::string *output)
{
    int ret_val = ERROR_SUCCESS;
    int new_nose_dist = -1;
    if (args.size() > 0) {
        if (args.at(0) == "default") {
            new_nose_dist = Challenge::AggressiveAi::DEFAULT_NOSE_DISTANCE;
        } else {
            // Nose distance specified
            try {
                new_nose_dist = std::stoi(args.at(0).c_str(), NULL);
            } catch (std::invalid_argument) {
                ret_val = ERROR_BAD_ARGUMENTS;
                new_nose_dist = -1;
            } catch (std::out_of_range) {
                ret_val = ERROR_RANGE_NOT_FOUND;
                new_nose_dist = -1;
            }
        }
        if (new_nose_dist >= 0 && new_nose_dist <= 65535) {
            bool reapply = (new_nose_dist != Challenge::AggressiveAi::nose_distance);
            Challenge::AggressiveAi::nose_distance = new_nose_dist;
            if (Challenge::AggressiveAi::active() && reapply) {
                Challenge::AggressiveAi::disable();
                Challenge::AggressiveAi::enable();
            }
        } else {
            if (ret_val == ERROR_SUCCESS) {
                ret_val = ERROR_INVALID_PARAMETER;
            }
            output->append("ERROR: Invalid argument (Distance value must be an integer between 0 and 65535)\n");
        }
    }
    output->append("(Aggressive AI challenge mod) AI smell distance = " + std::to_string(Challenge::AggressiveAi::nose_distance));
    return ret_val;
}

// Sets hearing distance for NPCs modified by the "Aggressive AI" challenge mod
int cc_challenge_aggressive_ai_ear_distance(std::vector<std::string> args, std::string *output)
{
    int ret_val = ERROR_SUCCESS;
    int new_ear_dist = -1;
    if (args.size() > 0) {
        if (args.at(0) == "default") {
            new_ear_dist = Challenge::AggressiveAi::DEFAULT_EAR_DISTANCE;
        } else {
            // Nose distance specified
            try {
                new_ear_dist = std::stoi(args.at(0).c_str(), NULL);
            } catch (std::invalid_argument) {
                ret_val = ERROR_BAD_ARGUMENTS;
                new_ear_dist = -1;
            } catch (std::out_of_range) {
                ret_val = ERROR_RANGE_NOT_FOUND;
                new_ear_dist = -1;
            }
        }
        if (new_ear_dist >= 0 && new_ear_dist <= 65535) {
            bool reapply = (new_ear_dist != Challenge::AggressiveAi::ear_distance);
            Challenge::AggressiveAi::ear_distance = new_ear_dist;
            if (Challenge::AggressiveAi::active() && reapply) {
                Challenge::AggressiveAi::disable();
                Challenge::AggressiveAi::enable();
            }
        } else {
            if (ret_val == ERROR_SUCCESS) {
                ret_val = ERROR_INVALID_PARAMETER;
            }
            output->append("ERROR: Invalid argument (Distance value must be an integer between 0 and 65535)\n");
        }
    }
    output->append("(Aggressive AI challenge mod) AI hearing distance = " + std::to_string(Challenge::AggressiveAi::ear_distance));
    return ret_val;
}

// Enables/disables "Black Phantom Enemies" challenge mod
int cc_challenge_bp_enemies(std::vector<std::string> args, std::string *output)
{
    int ret_val = CONSOLE_COMMAND_SUCCESS;
    if (args.size() > 0) {
        switch (parse_toggle_arg(args.at(0).c_str()))
        {
            case 0:
                if (Challenge::BlackPhantomEnemies::active) {
                    Challenge::BlackPhantomEnemies::disable();
                }
                break;
            case 1:
                if (!Challenge::BlackPhantomEnemies::active) {
                    Challenge::BlackPhantomEnemies::enable();
                }
                break;
            default:
                output->append(ERROR_INVALID_BOOL_ARGUMENT + "\n");
                ret_val = ERROR_INVALID_PARAMETER;
                break;
        }
    }
    if (Challenge::BlackPhantomEnemies::active) {
        output->append("\"Black Phantom Enemies\" challenge mod = enabled");
    } else {
        output->append("\"Black Phantom Enemies\" challenge mod = disabled");
    }
    return ret_val;
}

// Sets draw type for NPCs modified by the "Black Phantom Enemies" challenge mod
int cc_challenge_bp_enemy_draw_type(std::vector<std::string> args, std::string *output)
{
    int ret_val = ERROR_SUCCESS;
    int new_draw_type = -1;
    if (args.size() > 0) {
        if (args.at(0) == "default") {
            new_draw_type = Challenge::BlackPhantomEnemies::DRAW_TYPE_DEFAULT;
        } else {
            // Draw type specified
            try {
                new_draw_type = std::stoi(args.at(0).c_str(), NULL);
            } catch (std::invalid_argument) {
                ret_val = ERROR_BAD_ARGUMENTS;
                new_draw_type = -1;
            } catch (std::out_of_range) {
                ret_val = ERROR_RANGE_NOT_FOUND;
                new_draw_type = -1;
            }
        }
        if (new_draw_type >= 0 && new_draw_type <= 255) {
            bool reapply = (new_draw_type != Challenge::BlackPhantomEnemies::DRAW_TYPE);
            Challenge::BlackPhantomEnemies::DRAW_TYPE = new_draw_type;
            if (Challenge::BlackPhantomEnemies::active && reapply) {
                Challenge::BlackPhantomEnemies::disable();
                Challenge::BlackPhantomEnemies::enable();
            }
        } else {
            if (ret_val == ERROR_SUCCESS) {
                ret_val = ERROR_INVALID_PARAMETER;
            }
            output->append("ERROR: Invalid argument (Draw type must be an integer between 0 and 255)\n");
        }
    }
    output->append("(Black Phantom Enemies challenge mod) Enemy draw type = " + std::to_string(Challenge::BlackPhantomEnemies::DRAW_TYPE));
    return ret_val;
}


// Enables/disables "Gravelord Phantoms" challenge mod
int cc_challenge_gravelord_phantoms(std::vector<std::string> args, std::string *output)
{
    int ret_val = CONSOLE_COMMAND_SUCCESS;
    if (args.size() > 0) {
        switch (parse_toggle_arg(args.at(0).c_str()))
        {
            case 0:
                if (Challenge::GravelordPhantoms::active) {
                    Challenge::GravelordPhantoms::disable();
                }
                break;
            case 1:
                if (!Challenge::GravelordPhantoms::active) {
                    Challenge::GravelordPhantoms::enable();
                }
                break;
            default:
                output->append(ERROR_INVALID_BOOL_ARGUMENT + "\n");
                ret_val = ERROR_INVALID_PARAMETER;
                break;
        }
    }
    if (Challenge::GravelordPhantoms::active) {
        output->append("\"Gravelord Phantoms\" challenge mod = enabled");
    } else {
        output->append("\"Gravelord Phantoms\" challenge mod = disabled");
    }
    return ret_val;
}


// De-spawns existing Gravelord phantoms, but only if current character is in NG+0
int cc_gravelord_phantoms_despawn(std::vector<std::string> args, std::string *output)
{
    Challenge::GravelordPhantoms::despawn_gravelord_phantoms();
    return CONSOLE_COMMAND_SUCCESS;
}


// Enables/disables anti-cheat protection against forced binoculars effect on hit
int cc_binocs_trigger_block(std::vector<std::string> args, std::string *output)
{
    int ret_val = CONSOLE_COMMAND_SUCCESS;
    if (args.size() > 0) {
        switch (parse_toggle_arg(args.at(0).c_str()))
        {
            case 0:
                if (AntiCheat::BinocsTriggerBlock::active) {
                    AntiCheat::BinocsTriggerBlock::disable();
                }
                break;
            case 1:
                if (!AntiCheat::BinocsTriggerBlock::active) {
                    AntiCheat::BinocsTriggerBlock::enable();
                }
                break;
            default:
                output->append(ERROR_INVALID_BOOL_ARGUMENT + "\n");
                ret_val = ERROR_INVALID_PARAMETER;
                break;
        }
    }
    if (AntiCheat::BinocsTriggerBlock::active) {
        output->append("BinocsTriggerBlock = enabled");
    } else {
        output->append("BinocsTriggerBlock = disabled");
    }
    return ret_val;
}

// Enables/disables anti-cheat protection against forced dragonification effect on hit
int cc_dragon_trigger_block(std::vector<std::string> args, std::string *output)
{
    int ret_val = CONSOLE_COMMAND_SUCCESS;
    if (args.size() > 0) {
        switch (parse_toggle_arg(args.at(0).c_str()))
        {
            case 0:
                if (AntiCheat::DragonTriggerBlock::active) {
                    AntiCheat::DragonTriggerBlock::disable();
                }
                break;
            case 1:
                if (!AntiCheat::DragonTriggerBlock::active) {
                    AntiCheat::DragonTriggerBlock::enable();
                }
                break;
            default:
                output->append(ERROR_INVALID_BOOL_ARGUMENT + "\n");
                ret_val = ERROR_INVALID_PARAMETER;
                break;
        }
    }
    if (AntiCheat::DragonTriggerBlock::active) {
        output->append("DragonTriggerBlock = enabled");
    } else {
        output->append("DragonTriggerBlock = disabled");
    }
    return ret_val;
}


// Enables/disables dim lava effects
int cc_dim_lava(std::vector<std::string> args, std::string *output)
{
    int ret_val = CONSOLE_COMMAND_SUCCESS;
    if (args.size() > 0) {
        switch (parse_toggle_arg(args.at(0).c_str()))
        {
            case 0:
                Game::enable_dim_lava(false);
                break;
            case 1:
                Game::enable_dim_lava(true);
                break;
            default:
                output->append(ERROR_INVALID_BOOL_ARGUMENT + "\n");
                ret_val = ERROR_INVALID_PARAMETER;
                break;
        }
    }
    if (Game::dim_lava_enabled()) {
        output->append("Dim lava = enabled");
    } else {
        output->append("Dim lava = disabled");
    }
    return ret_val;
}


// Enables/disables ladder fix
int cc_ladder_fix(std::vector<std::string> args, std::string *output)
{
    int ret_val = CONSOLE_COMMAND_SUCCESS;
    if (args.size() > 0)
    {
        switch (parse_toggle_arg(args.at(0).c_str()))
        {
            case 0:
                LadderFix::unpatch();
                break;
            case 1:
                LadderFix::apply();
                break;
            default:
                output->append(ERROR_INVALID_BOOL_ARGUMENT + "\n");
                ret_val = ERROR_INVALID_PARAMETER;
                break;
        }
    }
    if (LadderFix::is_active())
    {
        output->append("Ladder fix = enabled");
    }
    else
    {
        output->append("Ladder fix = disabled");
    }
    return ret_val;
}


// Overrides the default rung index at which the player will exit a ladder while sliding down
int cc_ladder_fix_override(std::vector<std::string> args, std::string *output)
{
    int ret_val = ERROR_SUCCESS;
    uint32_t new_rung = -1;
    if (args.size() > 0) {
        if (args.at(0) == "default") {
            new_rung = DS1_DEFAULT_LADDER_FIX_EXIT_RUNG_;
        } else {
            // Index specified
            try {
                new_rung = std::stoi(args.at(0).c_str(), NULL);
            } catch (std::invalid_argument) {
                ret_val = ERROR_BAD_ARGUMENTS;
                new_rung = -1;
            } catch (std::out_of_range) {
                ret_val = ERROR_RANGE_NOT_FOUND;
                new_rung = -1;
            }
        }
        LadderFix::set_exit_rung(new_rung);
    }
    output->append("Ladder fix exit rung = " + std::to_string(LadderFix::exit_rung()));
    return ret_val;
}


// Enables/disables multi-tribute
int cc_multi_tribute(std::vector<std::string> args, std::string *output)
{
    int ret_val = CONSOLE_COMMAND_SUCCESS;
    if (args.size() > 0)
    {
        switch (parse_toggle_arg(args.at(0).c_str()))
        {
            case 0:
                MultiTribute::unpatch();
                break;
            case 1:
                MultiTribute::apply();
                break;
            default:
                output->append(ERROR_INVALID_BOOL_ARGUMENT + "\n");
                ret_val = ERROR_INVALID_PARAMETER;
                break;
        }
    }
    if (MultiTribute::is_active())
    {
        output->append("Multi-tribute = enabled");
    }
    else
    {
        output->append("Multi-tribute = disabled");
    }
    return ret_val;
}


// Enables/disables multi-consumption
int cc_multi_consume(std::vector<std::string> args, std::string *output)
{
    int ret_val = CONSOLE_COMMAND_SUCCESS;
    if (args.size() > 0)
    {
        switch (parse_toggle_arg(args.at(0).c_str()))
        {
            case 0:
                MultiConsume::unpatch();
                break;
            case 1:
                MultiConsume::apply();
                break;
            default:
                output->append(ERROR_INVALID_BOOL_ARGUMENT + "\n");
                ret_val = ERROR_INVALID_PARAMETER;
                break;
        }
    }
    if (MultiConsume::is_active())
    {
        output->append("Multi-consume = enabled");
    }
    else
    {
        output->append("Multi-consume = disabled");
    }
    return ret_val;
}


// Enables/disables omni-directional dodging
int cc_omni_roll(std::vector<std::string> args, std::string *output)
{
    int ret_val = CONSOLE_COMMAND_SUCCESS;
    if (args.size() > 0)
    {
        switch (parse_toggle_arg(args.at(0).c_str()))
        {
            case 0:
                AnimationEdits::omni_directional_dodge = 0;
                break;
            case 1:
                AnimationEdits::omni_directional_dodge = 1;
                break;
            default:
                output->append(ERROR_INVALID_BOOL_ARGUMENT + "\n");
                ret_val = ERROR_INVALID_PARAMETER;
                break;
        }
    }
    if (!!AnimationEdits::omni_directional_dodge)
    {
        output->append("Omni-directional dodging = enabled");
    }
    else
    {
        output->append("Omni-directional dodging = disabled");
    }
    return ret_val;
}


// Enables/disables jumping with L3 instead of B
int cc_l3_jump(std::vector<std::string> args, std::string *output)
{
    int ret_val = CONSOLE_COMMAND_SUCCESS;
    if (args.size() > 0)
    {
        switch (parse_toggle_arg(args.at(0).c_str()))
        {
            case 0:
                L3Jump::unpatch();
                break;
            case 1:
                L3Jump::apply();
                break;
            default:
                output->append(ERROR_INVALID_BOOL_ARGUMENT + "\n");
                ret_val = ERROR_INVALID_PARAMETER;
                break;
        }
    }
    if (L3Jump::is_active())
    {
        output->append("L3 jump = enabled");
    }
    else
    {
        output->append("L3 jump = disabled");
    }
    return ret_val;
}


// Enables/disables "Framerate too low for online play" disconnect
int cc_low_fps_disconnect(std::vector<std::string> args, std::string *output)
{
    int ret_val = CONSOLE_COMMAND_SUCCESS;
    if (args.size() > 0)
    {
        switch (parse_toggle_arg(args.at(0).c_str()))
        {
            case 0:
                Game::enable_low_fps_disconnect(false);
                break;
            case 1:
                Game::enable_low_fps_disconnect(true);
                break;
            default:
                output->append(ERROR_INVALID_BOOL_ARGUMENT + "\n");
                ret_val = ERROR_INVALID_PARAMETER;
                break;
        }
    }
    
    return ret_val;
}


// Enables/disables radial compass HUD element
int cc_hud_compass_radial(std::vector<std::string> args, std::string *output)
{
    int ret_val = CONSOLE_COMMAND_SUCCESS;
    if (args.size() > 0) {
        switch (parse_toggle_arg(args.at(0).c_str()))
        {
            case 0:
                Hud::set_show_compass_radial(false);
                break;
            case 1:
                Hud::set_show_compass_radial(true);
                break;
            default:
                output->append(ERROR_INVALID_BOOL_ARGUMENT + "\n");
                ret_val = ERROR_INVALID_PARAMETER;
                break;
        }
    }
    if (Hud::get_show_compass_radial()) {
        output->append("Radial compass HUD element = enabled");
    } else {
        output->append("Radial compass HUD element = disabled");
    }
    return ret_val;
}


// Enables/disables bar compass HUD element (Skyrim-styled compass)
int cc_hud_compass_bar(std::vector<std::string> args, std::string *output)
{
    int ret_val = CONSOLE_COMMAND_SUCCESS;
    if (args.size() > 0) {
        switch (parse_toggle_arg(args.at(0).c_str()))
        {
            case 0:
                Hud::set_show_compass_bar(false);
                break;
            case 1:
                Hud::set_show_compass_bar(true);
                break;
            default:
                output->append(ERROR_INVALID_BOOL_ARGUMENT + "\n");
                ret_val = ERROR_INVALID_PARAMETER;
                break;
        }
    }
    if (Hud::get_show_compass_bar()) {
        output->append("Bar compass HUD element = enabled");
    } else {
        output->append("Bar compass HUD element = disabled");
    }
    return ret_val;
}


// Enables/disables elevation meter HUD element
int cc_hud_elevation_meter(std::vector<std::string> args, std::string *output)
{
    int ret_val = CONSOLE_COMMAND_SUCCESS;
    if (args.size() > 0) {
        switch (parse_toggle_arg(args.at(0).c_str()))
        {
            case 0:
                Hud::set_show_elevation_meter(false);
                break;
            case 1:
                Hud::set_show_elevation_meter(true);
                break;
            default:
                output->append(ERROR_INVALID_BOOL_ARGUMENT + "\n");
                ret_val = ERROR_INVALID_PARAMETER;
                break;
        }
    }
    if (Hud::get_show_elevation_meter()) {
        output->append("Elevation meter HUD element = enabled");
    } else {
        output->append("Elevation meter HUD element = disabled");
    }
    return ret_val;
}


// Enables/disables multiplayer node graph HUD element
int cc_hud_node_graph(std::vector<std::string> args, std::string *output)
{
    int ret_val = CONSOLE_COMMAND_SUCCESS;
    if (args.size() > 0) {
        switch (parse_toggle_arg(args.at(0).c_str()))
        {
            case 0:
                Hud::set_show_node_graph(false, false);
                break;
            case 1:
                Hud::set_show_node_graph(true, false);
                break;
            default:
                output->append(ERROR_INVALID_BOOL_ARGUMENT + "\n");
                ret_val = ERROR_INVALID_PARAMETER;
                break;
        }
    }
    if (Hud::get_show_node_graph()) {
        output->append("Multiplayer node graph HUD element = enabled");
    } else {
        output->append("Multiplayer node graph HUD element = disabled");
    }
    return ret_val;
}


// Enables/disables weapon durability HUD elements
int cc_hud_weapon_durability_bars(std::vector<std::string> args, std::string *output)
{
    int ret_val = CONSOLE_COMMAND_SUCCESS;
    if (args.size() > 0)
    {
        switch (parse_toggle_arg(args.at(0).c_str()))
        {
            case 0:
                DurabilityBars::enable_pref = false;
                break;
            case 1:
                DurabilityBars::enable_pref = true;
                break;
            default:
                output->append(ERROR_INVALID_BOOL_ARGUMENT + "\n");
                ret_val = ERROR_INVALID_PARAMETER;
                break;
        }
    }
    if (DurabilityBars::enable_pref)
    {
        output->append("Weapon durability bar HUD elements = enabled");
    }
    else
    {
        output->append("Weapon durability bar HUD elements = disabled");
    }
    return ret_val;
}


// Sets horizontal offset for left-side durability bar HUD element
int cc_hud_weapon_durability_x_offset_l(std::vector<std::string> args, std::string *output)
{
    int ret_val = ERROR_SUCCESS;
    long new_offset = 0;
    if (args.size() > 0)
    {
        if (args.at(0) == "default")
        {
            new_offset = 0L;
        }
        else
        {
            // Offset specified
            try
            {
                new_offset = std::stoi(args.at(0).c_str(), NULL);
            }
            catch (const std::invalid_argument&)
            {
                ret_val = ERROR_BAD_ARGUMENTS;
                new_offset = DurabilityBars::render_data.offset_left_x;
            }
            catch (const std::out_of_range&)
            {
                ret_val = ERROR_RANGE_NOT_FOUND;
                new_offset = DurabilityBars::render_data.offset_left_x;
            }
        }
        DurabilityBars::render_data.offset_left_x = new_offset;
    }
    output->append("Horizontal offset for left-hand weapon durability meter HUD element = " + std::to_string(DurabilityBars::render_data.offset_left_x) + " pixels");
    return ret_val;
}


// Sets horizontal offset for right-side durability bar HUD element
int cc_hud_weapon_durability_x_offset_r(std::vector<std::string> args, std::string *output)
{
    int ret_val = ERROR_SUCCESS;
    long new_offset = 0;
    if (args.size() > 0)
    {
        if (args.at(0) == "default")
        {
            new_offset = 0L;
        }
        else
        {
            // Offset specified
            try
            {
                new_offset = std::stoi(args.at(0).c_str(), NULL);
            }
            catch (const std::invalid_argument&)
            {
                ret_val = ERROR_BAD_ARGUMENTS;
                new_offset = DurabilityBars::render_data.offset_right_x;
            }
            catch (const std::out_of_range&)
            {
                ret_val = ERROR_RANGE_NOT_FOUND;
                new_offset = DurabilityBars::render_data.offset_right_x;
            }
        }
        DurabilityBars::render_data.offset_right_x = new_offset;
    }
    output->append("Horizontal offset for right-hand weapon durability meter HUD element = " + std::to_string(DurabilityBars::render_data.offset_right_x) + " pixels");
    return ret_val;
}


// Sets vertical offset for durability bar HUD elements
int cc_hud_weapon_durability_y_offset(std::vector<std::string> args, std::string *output)
{
    int ret_val = ERROR_SUCCESS;
    long new_offset = 0;
    if (args.size() > 0)
    {
        if (args.at(0) == "default")
        {
            new_offset = 0L;
        }
        else
        {
            // Offset specified
            try
            {
                new_offset = std::stoi(args.at(0).c_str(), NULL);
            }
            catch (const std::invalid_argument&)
            {
                ret_val = ERROR_BAD_ARGUMENTS;
                new_offset = DurabilityBars::render_data.offset_y;
            }
            catch (const std::out_of_range&)
            {
                ret_val = ERROR_RANGE_NOT_FOUND;
                new_offset = DurabilityBars::render_data.offset_y;
            }
        }
        DurabilityBars::render_data.offset_y = new_offset;
    }
    output->append("Vertical offset for weapon durability meter HUD elements = " + std::to_string(DurabilityBars::render_data.offset_y) + " pixels");
    return ret_val;
}


// Sets scale of durability bar HUD elements
int cc_hud_weapon_durability_scale(std::vector<std::string> args, std::string *output)
{
    int ret_val = ERROR_SUCCESS;
    float new_scale = 1.0f;
    if (args.size() > 0)
    {
        if (args.at(0) == "default")
        {
            new_scale = 1.0f;
        }
        else
        {
            // Scale specified
            try
            {
                new_scale = std::stof(args.at(0).c_str(), NULL);
                if (new_scale < 0.0f)
                {
                    new_scale = DurabilityBars::render_data.scale;
                }
            }
            catch (const std::invalid_argument&)
            {
                ret_val = ERROR_BAD_ARGUMENTS;
                new_scale = DurabilityBars::render_data.scale;
            }
            catch (const std::out_of_range&)
            {
                ret_val = ERROR_RANGE_NOT_FOUND;
                new_scale = DurabilityBars::render_data.scale;
            }
        }
        DurabilityBars::render_data.scale = new_scale;
    }
    output->append("Scaling for weapon durability meter HUD elements = " + std::to_string(DurabilityBars::render_data.scale));
    return ret_val;
}


// Enables/disables automatic equipping of items on pickup
int cc_auto_equip(std::vector<std::string> args, std::string *output)
{
    int ret_val = CONSOLE_COMMAND_SUCCESS;
    if (args.size() > 0) {
        switch (parse_toggle_arg(args.at(0).c_str()))
        {
            case 0:
                Game::set_item_auto_equip(false);
                break;
            case 1:
                Game::set_item_auto_equip(true);
                break;
            default:
                output->append(ERROR_INVALID_BOOL_ARGUMENT + "\n");
                ret_val = ERROR_INVALID_PARAMETER;
                break;
        }
    }
    if (Game::item_auto_equip_enabled()) {
        output->append("Item auto-equip = enabled");
    } else {
        output->append("Item auto-equip = disabled");
    }
    return ret_val;
}


// Enables/disables armor sound effects
int cc_armor_sfx(std::vector<std::string> args, std::string *output)
{
    int ret_val = CONSOLE_COMMAND_SUCCESS;
    if (args.size() > 0) {
        switch (parse_toggle_arg(args.at(0).c_str()))
        {
            case 0:
                Game::enable_armor_sfx(false);
                break;
            case 1:
                Game::enable_armor_sfx(true);
                break;
            default:
                output->append(ERROR_INVALID_BOOL_ARGUMENT + "\n");
                ret_val = ERROR_INVALID_PARAMETER;
                break;
        }
    }
    if (Game::armor_sfx_enabled()) {
        output->append("Armor sfx = enabled");
    } else {
        output->append("Armor sfx = disabled");
    }
    return ret_val;
}



// Applies the Bonfire input fix (Probably unnecessary now that the fix is automatic)
int cc_fix_bonfire_input(std::vector<std::string> args, std::string *output)
{
    // Apply fix and print message to console (and not to text feed)
    return Game::fix_bonfire_input(false, true);
}


// Prints information on every player in the user's multiplayer node network
int cc_multiplayer_network(std::vector<std::string> args, std::string *output)
{
    if (Game::node_count < 0) {
        output->append("Multiplayer network is unavailable.");
    } else if (Game::node_count == 0) {
        output->append("No players in multiplayer network.");
    } else {
        output->append("\n");
        std::string header = "     Steam64 ID         |    Steam64 ID (Hex)   |   Ping    ";
        output->append(header);
        output->append("\n");
        for (int i = 0; i < (int)header.length(); i++) {
            output->append("-");
        }
        output->append("\n");
        SpPointer connection_list = SpPointer((uint8_t*)Game::ds1_base + 0xF62D24, { 0x54 });
        SpPointer entry = SpPointer(NULL);
        for (int i = 0; i < Game::node_count && connection_list.resolve() != entry.resolve(); i++) {
            if (i == 0)
                entry = SpPointer(connection_list.resolve(), { 0x0 });

            SpPointer status_ptr = SpPointer(entry.resolve(), { 0x8, 0x8 });
            SpPointer steam_id_ptr = SpPointer(entry.resolve(), { 0x8, 0x170 });
            SpPointer ping_ptr = SpPointer(entry.resolve(), { 0x8, 0x178 });

            int status;
            status_ptr.read(&status);
            if (status > 2) {
                output->append("  ");

                // Get Steam64 ID as decimal string
                unsigned long long steam64_id_val;
                steam_id_ptr.read(&steam64_id_val);
                std::string steam64_id = std::to_string(steam64_id_val);
                for (int j = 0, len = steam64_id.length(); j < (22 - len); j++) // 20 = largest possible length of a Steam64 ID string
                    steam64_id += ' ';
                output->append(steam64_id);
                output->append("|    ");

                // Get Steam64 ID as hex string
                uint8_t steam_id_bytes[8];
                memcpy_s(steam_id_bytes, 8, steam_id_ptr.resolve(), 8);
                for (int j = 0; j < 8; j++) {
                    uint8_t b = steam_id_bytes[7 - j];
                    std::stringstream hex_stream;
                    hex_stream << std::hex << (int)b; // Convert Virtual-key code to hex string
                    if (b < 16)
                        output->append("0");
                    output->append(hex_stream.str());
                }
                output->append("   |   ");

                // Get ping
                int ping;
                ping_ptr.read(&ping);
                if (ping > 999)
                    ping = 999;
                output->append(std::to_string(ping));
                if (ping < 10)
                    output->append("  ");
                else if (ping < 100)
                    output->append(" ");
                output->append("\n");
            }
            // Get next connection
            entry = SpPointer(entry.resolve(), { 0x0 });
        }
    }

    return CONSOLE_COMMAND_SUCCESS;
}


// Enables/disables multiplayer node count element of overlay text feed
int cc_text_feed_node_count(std::vector<std::string> args, std::string *output)
{
    int return_val = CONSOLE_COMMAND_SUCCESS;
    if (args.size() > 0) {
        switch (parse_toggle_arg(args.at(0).c_str()))
        {
            case 0:
                Mod::show_node_count = false;
                break;
            case 1:
                Mod::show_node_count = true;
                break;
            default:
                output->append("ERROR: Assigned value must be either 1 or 0 (1 = enabled, 0 = disabled)\n");
                return_val = ERROR_INVALID_PARAMETER;
                break;
        }
    } else {
        if (Game::node_count < 0) {
            output->append("Multiplayer network is unavailable.\n");
        } else {
            output->append("Current node count: ").append(std::to_string(Game::node_count)).append("\n");
        }
    }
    // If no argument is specified, simply print the status
    if (Mod::show_node_count) {
        output->append("Display multiplayer node count = enabled");
    } else {
        output->append("Display multiplayer node count = disabled");
    }
    return return_val;
}


// Enables/disables in-game time (IGT) element of overlay text feed
int cc_text_feed_play_time(std::vector<std::string> args, std::string *output)
{
    int return_val = CONSOLE_COMMAND_SUCCESS;
    if (args.size() > 0) {
        switch (parse_toggle_arg(args.at(0).c_str()))
        {
            case 0:
                Mod::hud_play_time_pref = false;
                Game::play_time_str = "[IGT: --:--:--:--]   ";
                break;
            case 1:
                Mod::hud_play_time_pref = true;
                break;
            default:
                output->append("ERROR: Assigned value must be either 1 or 0 (1 = enabled, 0 = disabled)\n");
                return_val = ERROR_INVALID_PARAMETER;
                break;
        }
    } else {
        if (Game::play_time_str.empty()) {
            output->append("IGT is unavailable.\n");
        } else {
            output->append(Game::play_time_str + "\n");
        }
    }
    // If no argument is specified, simply print the status
    if (Mod::hud_play_time_pref) {
        output->append("Display IGT = enabled");
    } else {
        output->append("Display IGT = disabled");
    }
    return return_val;
}



/*
                        /////////////////////////////
                        ///// FOR DEVELOPER USE /////
                        /////////////////////////////

    Placeholder console command for devs to test/debug various data at runtime.

    NOTE:
     Code implemented here is for developer use only; changes to this function
     should NOT be pushed to the repository.

*/
int cc_developer_debug(std::vector<std::string> args, std::string *output)
{
    

    return CONSOLE_COMMAND_SUCCESS;
}




/*
    Called once at startup; registers all the above commands
    for use with the in-game console.
*/
void Mod::register_console_commands()
{
    register_console_command(ccn_developer_debug, cc_developer_debug, chm_developer_debug);
    register_console_command(ccn_overhaul_build, cc_build, chm_overhaul_build);
    register_console_command(ccn_overhaul_motd, cc_overhaul_motd, chm_overhaul_motd);
    register_console_alias(cca_overhaul_message_of_the_day, ccn_overhaul_motd);
    register_console_command(ccn_overhaul_update, cc_overhaul_update, chm_overhaul_update);
    register_console_alias(cca_overhaul_latest, ccn_overhaul_update);
    register_console_command(ccn_overhaul_download_url, cc_overhaul_download_url, chm_overhaul_download_url);
    register_console_alias(cca_overhaul_download, ccn_overhaul_download_url);
    register_console_command(ccn_overhaul_remote_update_hosts, cc_overhaul_remote_update_hosts, chm_overhaul_remote_update_hosts);
    register_console_command(ccn_mouse_input, cc_mouse_input, chm_mouse_input);
    register_console_command(ccn_console_lock_cam, cc_console_lock_cam, chm_console_lock_cam);
    register_console_alias(cca_console_lock_cam, ccn_console_lock_cam);
    register_console_command(ccn_credits, cc_credits, chm_credits);
    register_console_command(ccn_credits_detailed, cc_credits_detailed, chm_credits_detailed);
    register_console_command(ccn_dim_lava, cc_dim_lava, chm_dim_lava);
    register_console_alias(cca_lava_brightness_fix, ccn_dim_lava);
    register_console_command(ccn_ladder_fix, cc_ladder_fix, chm_ladder_fix);
    register_console_command(ccn_auto_equip, cc_auto_equip, chm_auto_equip);
    register_console_command(ccn_ladder_fix_override, cc_ladder_fix_override, chm_ladder_fix_override);
    register_console_command(ccn_multi_consume, cc_multi_consume, chm_multi_consume);
    register_console_command(ccn_multi_tribute, cc_multi_tribute, chm_multi_tribute);
    register_console_command(ccn_omni_roll, cc_omni_roll, chm_omni_roll);
    register_console_command(ccn_l3_jump, cc_l3_jump, chm_l3_jump);
    register_console_command(ccn_low_fps_disconnect, cc_low_fps_disconnect, chm_low_fps_disconnect);
    register_console_command(ccn_fix_bonfire_input, cc_fix_bonfire_input, chm_fix_bonfire_input);
    register_console_command(ccn_text_feed_node_count, cc_text_feed_node_count, chm_text_feed_node_count);
    register_console_alias(cca_node_count, ccn_text_feed_node_count);
    register_console_command(ccn_text_feed_play_time, cc_text_feed_play_time, chm_text_feed_play_time);
    register_console_alias(cca_play_time, ccn_text_feed_play_time);
    register_console_alias(cca_igt, ccn_text_feed_play_time);
    register_console_command(ccn_multiplayer_network, cc_multiplayer_network, chm_multiplayer_network);
    register_console_command(ccn_game_character_name, cc_game_character_name, chm_game_character_name);
    register_console_command(ccn_hud_compass_radial, cc_hud_compass_radial, chm_hud_compass_radial);
    register_console_command(ccn_hud_compass_bar, cc_hud_compass_bar, chm_hud_compass_bar);
    register_console_command(ccn_hud_elevation_meter, cc_hud_elevation_meter, chm_hud_elevation_meter);
    register_console_command(ccn_hud_node_graph, cc_hud_node_graph, chm_hud_node_graph);
    register_console_command(ccn_hud_weapon_durability_bars, cc_hud_weapon_durability_bars, chm_hud_weapon_durability_bars);
    register_console_command(ccn_hud_weapon_durability_x_offset_l, cc_hud_weapon_durability_x_offset_l, chm_hud_weapon_durability_x_offset_l);
    register_console_command(ccn_hud_weapon_durability_x_offset_r, cc_hud_weapon_durability_x_offset_r, chm_hud_weapon_durability_x_offset_r);
    register_console_command(ccn_hud_weapon_durability_y_offset, cc_hud_weapon_durability_y_offset, chm_hud_weapon_durability_y_offset);
    register_console_command(ccn_hud_weapon_durability_scale, cc_hud_weapon_durability_scale, chm_hud_weapon_durability_scale);
    register_console_command(ccn_binocs_trigger_block, cc_binocs_trigger_block, chm_binocs_trigger_block);
    register_console_command(ccn_dragon_trigger_block, cc_dragon_trigger_block, chm_dragon_trigger_block);
    register_console_command(ccn_save_file_index, cc_save_file_index, chm_save_file_index);
    register_console_command(ccn_save_file_next, cc_save_file_next, chm_save_file_next);
    register_console_command(ccn_save_file_prev, cc_save_file_prev, chm_save_file_prev);
    register_console_command(ccn_save_file_create, cc_save_file_create, chm_save_file_create);
    register_console_command(ccn_save_file_copy, cc_save_file_copy, chm_save_file_copy);
    register_console_command(ccn_challenge_aggressive_ai, cc_challenge_aggressive_ai, chm_challenge_aggressive_ai);
    register_console_command(ccn_challenge_aggressive_ai_ear_distance, cc_challenge_aggressive_ai_ear_distance, chm_challenge_aggressive_ai_ear_distance);
    register_console_command(ccn_challenge_aggressive_ai_nose_distance, cc_challenge_aggressive_ai_nose_distance, chm_challenge_aggressive_ai_nose_distance);
    register_console_command(ccn_challenge_bp_enemies, cc_challenge_bp_enemies, chm_challenge_bp_enemies);
    register_console_command(ccn_challenge_bp_enemy_draw_type, cc_challenge_bp_enemy_draw_type, chm_challenge_bp_enemy_draw_type);
    register_console_command(ccn_challenge_gravelord_phantoms, cc_challenge_gravelord_phantoms, chm_challenge_gravelord_phantoms);
    register_console_command(ccn_gravelord_phantoms_despawn, cc_gravelord_phantoms_despawn, chm_gravelord_phantoms_despawn);


#ifndef DS1_OVERHAUL_QOL_PREVIEW
    register_console_command(ccn_cheats, cc_cheats, chm_cheats);
    register_console_command(ccn_legacy_mode, cc_legacy_mode, chm_legacy_mode);
    register_console_command(ccn_armor_sfx, cc_armor_sfx, chm_armor_sfx);
    register_console_alias(cca_armor_sounds, ccn_armor_sfx);
#endif // DS1_OVERHAUL_QOL_PREVIEW


#ifdef _DS1_OVERHAUL_MOD_DBG_

    Mod::register_console_commands_debug();

#endif // _DS1_OVERHAUL_MOD_DBG_
}