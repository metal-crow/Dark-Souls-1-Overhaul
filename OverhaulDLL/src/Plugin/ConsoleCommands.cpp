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
#include "Challenge/BlackPhantomEnemies.h"
#include "Challenge/GravelordPhantoms.h"


///////////////////////////////////////////////////////////
//////////////// Console command functions ////////////////
///////////////////////////////////////////////////////////



// Plays the mod credits (lists the people who helped create the mod)
int cc_credits(std::vector<std::string> args, std::string *output)
{
    // @TODO
    output->append("\n\n");
    output->append("    ///////////////////////////////////////////////////////////\n");
    output->append("    ///////////////// Dark Souls Overhaul Mod /////////////////\n");
    output->append("    ///////////////////////////////////////////////////////////\n");
    output->append("\n                ...credits coming eventually...\n\n");

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
        if (SaveFile::generate_empty_save_file(Files::default_save_file_path.c_str())) {
            output->append("ERROR: Failed to create new save file (Write error)");
            return ERROR_WRITE_FAULT;
        }
    } else {
        if (SaveFile::generate_empty_save_file()) {
            output->append("ERROR: Failed to create new save file (Write error)");
            return ERROR_WRITE_FAULT;
        }
    }
    int count = Sl2::get_save_file_count(file.c_str());
    out = "Created new save file: " + file;
    if (count < 11) {
        out += "_0" + std::to_string(count - 1);
    } else {
        out += "_" + std::to_string(count - 1);
    }
    output->append(out);
    return ERROR_SUCCESS;
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
            output->append("ERROR: Invalid argument (Index must be an integer between 0 and 255)\n");
        }
    }
    output->append("Black Phantom Enemies challenge mod enemy draw type = " + std::to_string(Challenge::BlackPhantomEnemies::DRAW_TYPE));
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
                if (BinocsTriggerBlock::active) {
                    BinocsTriggerBlock::disable();
                }
                break;
            case 1:
                if (!BinocsTriggerBlock::active) {
                    BinocsTriggerBlock::enable();
                }
                break;
            default:
                output->append(ERROR_INVALID_BOOL_ARGUMENT + "\n");
                ret_val = ERROR_INVALID_PARAMETER;
                break;
        }
    }
    if (BinocsTriggerBlock::active) {
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
                if (DragonTriggerBlock::active) {
                    DragonTriggerBlock::disable();
                }
                break;
            case 1:
                if (!DragonTriggerBlock::active) {
                    DragonTriggerBlock::enable();
                }
                break;
            default:
                output->append(ERROR_INVALID_BOOL_ARGUMENT + "\n");
                ret_val = ERROR_INVALID_PARAMETER;
                break;
        }
    }
    if (DragonTriggerBlock::active) {
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
    register_console_command(ccn_mouse_input, cc_mouse_input, chm_mouse_input);
    register_console_command(ccn_console_lock_cam, cc_console_lock_cam, chm_console_lock_cam);
    register_console_alias(cca_console_lock_cam, ccn_console_lock_cam);
    register_console_command(ccn_credits, cc_credits, chm_credits);
    register_console_command(ccn_legacy_mode, cc_legacy_mode, chm_legacy_mode);
    register_console_command(ccn_armor_sfx, cc_armor_sfx, chm_armor_sfx);
    register_console_alias(cca_armor_sounds, ccn_armor_sfx);
    register_console_command(ccn_dim_lava, cc_dim_lava, chm_dim_lava);
    register_console_alias(cca_lava_brightness_fix, ccn_dim_lava);
    register_console_command(ccn_fix_bonfire_input, cc_fix_bonfire_input, chm_fix_bonfire_input);
    register_console_command(ccn_text_feed_node_count, cc_text_feed_node_count, chm_text_feed_node_count);
    register_console_alias(cca_node_count, ccn_text_feed_node_count);
    register_console_command(ccn_cheats, cc_cheats, chm_cheats);
    register_console_command(ccn_multiplayer_network, cc_multiplayer_network, chm_multiplayer_network);
    register_console_command(ccn_hud_compass_radial, cc_hud_compass_radial, chm_hud_compass_radial);
    register_console_command(ccn_hud_compass_bar, cc_hud_compass_bar, chm_hud_compass_bar);
    register_console_command(ccn_hud_elevation_meter, cc_hud_elevation_meter, chm_hud_elevation_meter);
    register_console_command(ccn_hud_node_graph, cc_hud_node_graph, chm_hud_node_graph);
    register_console_command(ccn_binocs_trigger_block, cc_binocs_trigger_block, chm_binocs_trigger_block);
    register_console_command(ccn_dragon_trigger_block, cc_dragon_trigger_block, chm_dragon_trigger_block);
    register_console_command(ccn_save_file_index, cc_save_file_index, chm_save_file_index);
    register_console_command(ccn_save_file_next, cc_save_file_next, chm_save_file_next);
    register_console_command(ccn_save_file_prev, cc_save_file_prev, chm_save_file_prev);
    register_console_command(ccn_save_file_create, cc_save_file_create, chm_save_file_create);
    register_console_command(ccn_challenge_bp_enemies, cc_challenge_bp_enemies, chm_challenge_bp_enemies);
    register_console_command(ccn_challenge_bp_enemy_draw_type, cc_challenge_bp_enemy_draw_type, chm_challenge_bp_enemy_draw_type);
    register_console_command(ccn_challenge_gravelord_phantoms, cc_challenge_gravelord_phantoms, chm_challenge_gravelord_phantoms);
    register_console_command(ccn_gravelord_phantoms_despawn, cc_gravelord_phantoms_despawn, chm_gravelord_phantoms_despawn);


#ifdef _DS1_OVERHAUL_MOD_DBG_

    Mod::register_console_commands_debug();

#endif // _DS1_OVERHAUL_MOD_DBG_
}