#include "Files.h"
#include "Shlobj.h"
#include "Menu/SavedCharacters.h"
#include "Save/Sl2.h"
#include "SP/memory/injection/asm/x64.h"
#include <algorithm>
#include <iterator>


int Files::save_file_index = 0;
bool Files::save_file_index_pending_set_next = false;
bool Files::save_file_index_pending_set_prev = false;
bool Files::save_file_index_make_new = false;
bool first_save_load = true;
std::string Files::save_file;

//Make sure file names are lowercase
//These have edits to existing entries, and thus are overhaul only
std::set<std::wstring> Files::files_to_intercept_loading = {
    L"gameparam.parambnd.dcx",
    L"c0000.esd.dcx",
    L"c0000.anibnd.dcx",
};

//These don't have any edits, just additions to support changes
std::set<std::wstring> Files::files_to_always_intercept_loading = {
    L"frpg_sfxbnd_commoneffects.ffxbnd.dcx",
    L"fdlc_main.fev",
    L"fdlc_main.fsb",
};

bool Files::UseOverhaulFiles = false;

int Files::string_wide_to_mb(wchar_t *in_string, std::string &out_string)
{
    if (in_string == NULL)
        return ERROR_INVALID_ADDRESS;

    int size_needed = WideCharToMultiByte(CP_UTF8, 0, in_string, -1, NULL, 0, NULL, NULL);
    char* out_c_str = new char[size_needed + 1];
    int ret_val = WideCharToMultiByte(CP_UTF8, 0, in_string, -1, out_c_str, size_needed, NULL, NULL);

    if (ret_val == 0)
        return ERROR_BAD_FORMAT;
    else {
        out_string = out_c_str;
        return 0;
    }
}

int Files::string_mb_to_wide(char *in_string, std::wstring &out_string)
{
    if (in_string == NULL)
        return ERROR_INVALID_ADDRESS;

    int size_needed = MultiByteToWideChar(CP_UTF8, 0, in_string, -1, NULL, 0);
    wchar_t* out_c_str = new wchar_t[size_needed + 1];
    int ret_val = MultiByteToWideChar(CP_UTF8, 0, in_string, -1, out_c_str, size_needed);

    if (ret_val == 0)
        return ERROR_BAD_FORMAT;
    else {
        out_string = out_c_str;
        return 0;
    }
}


extern "C" {
    HANDLE WINAPI intercept_create_file_w(LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);
}

// Called when the game attempts to call CreateFileW
HANDLE WINAPI intercept_create_file_w(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
                                      LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition,
                                      DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
    std::wstring load_file;
    std::wstring filename;

    if (lpFileName != NULL)
    {
        load_file = lpFileName;
        if ((int)load_file.length() >= 4)
        {
            size_t backslash_i = load_file.find_last_of(L"\\");
            if (backslash_i != -1)
                filename = load_file.substr(backslash_i+1, load_file.length());

            // Intercept save file load (.sl2)
            if (load_file.substr(load_file.length() - 4) == SaveFile::FILE_EXT_W)
            {
                // Parse the save file on first read
                if (first_save_load) {
                    // If not set by user, store default save file
                    if (Files::save_file.length() == 0) {
                        if (Files::string_wide_to_mb((wchar_t*)lpFileName, Files::save_file)) {
                            FATALERROR("Unable to convert in game save file to char");
                        }
                    }

                    // Count save files/characters and find first free save slot
                    int saved_char_count = 0;
                    std::tuple<int, int> first_free(-1, -1);
                    int save_file_count = Sl2::get_save_file_count(Files::save_file.c_str(), &saved_char_count, &first_free);

                    std::string free_slot_msg = Mod::output_prefix + "Found " + std::to_string(saved_char_count) + " characters in " + std::to_string(save_file_count) + " save files";
                    free_slot_msg += " (first free slot: " + std::to_string(std::get<0>(first_free)) + "," + std::to_string(std::get<1>(first_free)) + ")\n";
                    ConsoleWrite(free_slot_msg.c_str());

                    // If all save files are full, generate new (empty) save file
                    if (std::get<0>(first_free) < 0 && std::get<1>(first_free) < 0) {
                        std::string current_save_file;
                        if (save_file_count < 10) {
                            current_save_file = Files::save_file + "_0" + std::to_string(save_file_count);
                        }
                        else {
                            current_save_file = Files::save_file + "_" + std::to_string(save_file_count);
                        }
                        ConsoleWrite("No free character slots; generating next empty save file: \"%s\"", current_save_file.c_str());

                        Sl2::generate_empty_save_file(current_save_file.c_str());
                    }

                    // Update menu text
                    std::wstring custom_header =
                        L"                                            Save file "
                        + std::to_wstring(Files::save_file_index + 1)
                        + L"/" + std::to_wstring(save_file_count);
                    std::wstring custom_buttons = L"<?conclusion?>:Enter <?cancel?>:Back <?viewChange?>:Toggle Display <?commando?>:Delete  <?categoryChangeL?>/<?categoryChangeR?>:Change save file  <?startMenuSwitch?>Create New Save File";
                    std::wstring custom_header1 = L"Select data to load." + custom_header;
                    std::wstring custom_header2 = L"Select data to delete." + custom_header;
                    Menu::Saves::set_custom_header_msgs(custom_header1, custom_header2);
                    std::wstring custom_buttons1 = L"<?selectUD?>:Select " + custom_buttons;
                    std::wstring custom_buttons2 = L"<?selectUD?>:Select " + custom_buttons;
                    Menu::Saves::set_custom_buttons_msgs(custom_buttons1, custom_buttons, custom_buttons2);

                    first_save_load = false;
                }

                load_file = Files::get_save_file_path();
            }

            else if (
                (Files::files_to_intercept_loading.count(filename) && Mod::custom_game_archive_path.length() > 0 && Files::UseOverhaulFiles) ||
                (Files::files_to_always_intercept_loading.count(filename) && Mod::custom_game_archive_path.length() > 0)
            ){
                load_file = Mod::custom_game_archive_path + filename;

                std::string filename_str(filename.begin(), filename.end());
                std::string loadfile_str(load_file.begin(), load_file.end());
                std::string load_file_msg = "Loading custom file \"" + filename_str + "\" as \"" + loadfile_str + "\"";
                ConsoleWrite(load_file_msg.c_str());
            }
        }
        // Call original function
        return CreateFileW(load_file.c_str(), dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
    }
    else
    {
        // Call original function with original arguments
        return CreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
    }
}


extern "C" {
    UINT WINAPI intercept_get_private_profile_int_w(LPCWSTR, LPCWSTR, INT, LPCWSTR);
}

// Called when the game attempts to call GetPrivateProfileIntW
UINT WINAPI intercept_get_private_profile_int_w(LPCWSTR lpAppName, LPCWSTR lpKeyName, INT nDefault, LPCWSTR lpFileName)
{
    std::wstring load_file = lpFileName;
    if ((int)Mod::custom_config_file_path.length() > 0)
    {
        // Intercept config file load (.ini)
        load_file = Mod::custom_config_file_path;
    }
    // Call original function
    return GetPrivateProfileIntW(lpAppName, lpKeyName, nDefault, load_file.c_str());
}


extern "C" {
    BOOL WINAPI intercept_write_private_profile_section_w(LPCWSTR, LPCWSTR, LPCWSTR);
}

// Called when the game attempts to call WritePrivateProfileSectionW
BOOL WINAPI intercept_write_private_profile_section_w(LPCWSTR lpAppName, LPCWSTR lpString, LPCWSTR lpFileName)
{
    std::wstring load_file = lpFileName;
    if ((int)Mod::custom_config_file_path.length() > 0)
    {
        // Intercept config file load (.ini)
        load_file = Mod::custom_config_file_path;
    }
    // Call original function
    return WritePrivateProfileSectionW(lpAppName, lpString, load_file.c_str());
}


#if 0
// Called when the game attempts to call ReadFile
BOOL WINAPI Files::intercept_read_file(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped)
{
    BOOL return_val;
    std::string fn_buff;
    int32_t file_pos_offset = 0;
    IoMonitor *io_monitor = Files::io_monitor_from_handle(hFile);
    // Call original function
    return_val = ReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);

    if (lpOverlapped == NULL && lpNumberOfBytesRead != NULL) {
        file_pos_offset = *lpNumberOfBytesRead;
    }

    if (io_monitor != NULL) {
        fn_buff = io_monitor->default_filename;
    }
    if ((return_val || (lpNumberOfBytesRead && *lpNumberOfBytesRead)) && fn_buff.length() > 0) {
        if (lpOverlapped != NULL) {
            fn_buff += " (WARNING: Async read operation)";
        }
        if (io_monitor->monitor) {
            std::string out = "Reading " + std::to_string(nNumberOfBytesToRead) + " bytes from " + fn_buff + " at offset " + std::to_string(io_monitor->io_pos) + " (" + FileUtil::to_hex_string(io_monitor->io_pos, fn_buff, true) + ") and storing in buffer at " + FileUtil::to_hex_string((uint32_t)lpBuffer, fn_buff, true);
            if (Files::io_output_filter.length() == 0 || out.find(Files::io_output_filter) != std::string::npos) {
                print_console(out);
                // Log packed file name:
                //try {
                //    out = bdt_off_to_fn.at(io_monitor->io_pos);
                //} catch (const std::out_of_range&) {
                //    out = "";
                //}
                //if (out.length() > 0) {
                //    print_console("    File: " + out);
                //}
            }
        }
        io_monitor->io_pos += file_pos_offset;
    }
    return return_val;
}
#endif

#if 0
BOOL WINAPI Files::intercept_write_file(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped)
{
    BOOL return_val;
    std::string fn_buff;
    int32_t file_pos_offset = 0;
    IoMonitor *io_monitor = Files::io_monitor_from_handle(hFile);
    // Call original function
    return_val = WriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);

    if (lpOverlapped == NULL && lpNumberOfBytesWritten != NULL) {
        file_pos_offset = *lpNumberOfBytesWritten;
    }

    if (io_monitor != NULL) {
        fn_buff = io_monitor->default_filename;
    }
    if ((return_val || (lpNumberOfBytesWritten && *lpNumberOfBytesWritten)) && fn_buff.length() > 0) {
        if (lpOverlapped != NULL) {
            fn_buff += " (WARNING: Async write operation)";
        }
        if (io_monitor->monitor) {
            std::string out = "Writing " + std::to_string(nNumberOfBytesToWrite) + " bytes to " + fn_buff + " at offset " + std::to_string(io_monitor->io_pos) + " (" + FileUtil::to_hex_string(io_monitor->io_pos, fn_buff, true) + ")";
            if (Files::io_output_filter.length() == 0 || out.find(Files::io_output_filter) != std::string::npos) {
                print_console(out);
            }
        }
        io_monitor->io_pos += file_pos_offset;
    }
    return return_val;
}
#endif

#if 0
// Called when the game attempts to call CloseHandle
BOOL WINAPI Files::intercept_close_handle(HANDLE hObject)
{
    BOOL return_val;
    std::string fn_buff;
    IoMonitor *io_monitor = Files::io_monitor_from_handle(hObject);
    // Call original function
    return_val = CloseHandle(hObject);

    if (io_monitor != NULL) {
        fn_buff = io_monitor->default_filename;
    }
    if (return_val && fn_buff.length() > 0) {
        io_monitor->io_pos = 0;
        io_monitor->handle = NULL;
        if (io_monitor->monitor) {
            std::string out = "Closed file handle: " + fn_buff;
            if (Files::io_output_filter.length() == 0 || out.find(Files::io_output_filter) != std::string::npos) {
                print_console(out);
            }
        }
    }
    return return_val;
}
#endif

#if 0
// Called when the game attempted to call SetFilePointer
DWORD WINAPI Files::intercept_set_file_pointer(HANDLE hFile, LONG lDistanceToMove, PLONG lpDistanceToMoveHigh, DWORD dwMoveMethod)
{
    DWORD return_val;
    std::string fn_buff;
    IoMonitor *io_monitor = Files::io_monitor_from_handle(hFile);
    // Call original function
    return_val = SetFilePointer(hFile, lDistanceToMove, lpDistanceToMoveHigh, dwMoveMethod);

    if (io_monitor != NULL) {
        fn_buff = io_monitor->default_filename;
    }
    if ((return_val != INVALID_SET_FILE_POINTER) && fn_buff.length() > 0) {
        if (io_monitor->monitor) {
            std::string out = "New file pointer position for " + fn_buff + ": " + std::to_string(return_val) + " (" + FileUtil::to_hex_string(return_val, fn_buff, true) + ")";
            if (Files::io_output_filter.length() == 0 || out.find(Files::io_output_filter) != std::string::npos) {
                print_console(out);
            }
        }
        io_monitor->io_pos = return_val;
    }
    return return_val;
}
#endif

static const uint64_t archives_n_savegame_write_offset = 0xD015FF;
static const uint64_t savegame_n_config_write_offset = 0xD009DF;

static const uint64_t private_profile_get_offset_1 = 0x1B5119;
static const uint64_t private_profile_get_offset_2 = 0x1B5267;

static const uint64_t private_profile_write_offset = 0x1B55D6;

extern "C" {
    uint64_t archives_n_savegame_write_inject_return;
    void archives_n_savegame_write_inject();
    uint64_t savegame_n_config_write_inject_return;
    void savegame_n_config_write_inject();

    uint64_t private_profile_get1_inject_return;
    void private_profile_get1_inject();
    uint64_t private_profile_get2_inject_return;
    void private_profile_get2_inject();

    uint64_t private_profile_write_inject_return;
    void private_profile_write_inject();
}

// Patches game calls to various shared library functions, redirecting them to Game::intercept_create_file()
void Files::apply_function_intercepts()
{
    // CreateFileW
    void *write_address = (uint8_t*)Game::ds1_base + archives_n_savegame_write_offset; // Game archives and savegame file
    sp::mem::code::x64::inject_jmp_14b(write_address, &archives_n_savegame_write_inject_return, 0, &archives_n_savegame_write_inject);
    write_address = (uint8_t*)Game::ds1_base + savegame_n_config_write_offset; // Savegame file and config directory
    sp::mem::code::x64::inject_jmp_14b(write_address, &savegame_n_config_write_inject_return, 0, &savegame_n_config_write_inject);

    // GetPrivateProfileIntW
    write_address = (uint8_t*)Game::ds1_base + private_profile_get_offset_1;
    sp::mem::code::x64::inject_jmp_14b(write_address, &private_profile_get1_inject_return, 3, &private_profile_get1_inject);
    write_address = (uint8_t*)Game::ds1_base + private_profile_get_offset_2;
    sp::mem::code::x64::inject_jmp_14b(write_address, &private_profile_get2_inject_return, 2, &private_profile_get2_inject);

    // WritePrivateProfileIntW
    write_address = (uint8_t*)Game::ds1_base + private_profile_write_offset;
    sp::mem::code::x64::inject_jmp_14b(write_address, &private_profile_write_inject_return, 3, &private_profile_write_inject);
}

// Checks if custom archive files exist. If one or more are missing, custom archives are not loaded, and default game files are loaded instead.
void Files::check_custom_archive_file_path()
{
    if ((int)Mod::custom_game_archive_path.length() == 0)
        return;

    ConsoleWrite("Checking if custom game archive files exist...");

    // Get char* strings for printing console messages
    std::string archive_name_ch = "";
    if (string_wide_to_mb((wchar_t*)Mod::custom_game_archive_path.c_str(), archive_name_ch))
    {
        // Error converting from wide char to char
        ConsoleWrite("ERROR: Unable to parse custom archive file name. Using default archive files instead.");
        Mod::custom_game_archive_path = L"";
        return;
    }

    // Check that custom game archive files exist
    std::set<std::wstring> custom_files;
    std::set_union(Files::files_to_intercept_loading.begin(), Files::files_to_intercept_loading.end(),
                   Files::files_to_always_intercept_loading.begin(), Files::files_to_always_intercept_loading.end(),
                   std::inserter(custom_files, custom_files.begin()));
    for (auto custom_file : custom_files)
    {
        std::wstring filepath = std::wstring(Mod::custom_game_archive_path).append(custom_file);
        std::string custom_file_str;
        string_wide_to_mb((wchar_t*)custom_file.c_str(), custom_file_str);

        if (!FileUtil::file_exists(filepath.c_str()))
        {
            // Custom file doesn't exist
            ConsoleWrite("ERROR: The file \"%s%s\" could not be found. Using default files instead.", archive_name_ch.c_str(), custom_file_str.c_str());
            Mod::custom_game_archive_path = L"";
            return;
        }
        else
            ConsoleWrite("Found %s%s",archive_name_ch.c_str(),custom_file_str.c_str());
    }
    ConsoleWrite("SUCCESS: Custom game archive files will be loaded (\"%s\"", archive_name_ch.c_str());
}


// Checks if custom save file (.sl2) exists
void Files::check_custom_save_file_path()
{
    if ((int)Files::save_file.length() == 0)
        return;

    ConsoleWrite("Checking if custom save file exists...");

    if (FileUtil::file_exists(Files::save_file.c_str()))
    {
        ConsoleWrite("SUCCESS: Custom save file will be loaded (\"%s\"",Files::save_file.c_str());
    }
    else
    {
        // Custom save file doesn't exist
        FATALERROR("WARNING: Custom save file was not found (\"%s\")", Files::save_file.c_str());
    }
}


// Checks if custom game config file (.ini) exists
void Files::check_custom_game_config_file_path()
{
    if ((int)Mod::custom_config_file_path.length() == 0)
        return;

    ConsoleWrite("Checking if custom game config file exists...");

    // Get char* strings for printing console messages
    std::string filename_ch;
    if (string_wide_to_mb((wchar_t*)Mod::custom_config_file_path.c_str(), filename_ch))
    {
        // Error converting from wide char to char
        FATALERROR("Unable to convert game config file to char");
    }

    if (FileUtil::file_exists(Mod::custom_config_file_path.c_str()))
    {
        ConsoleWrite("SUCCESS: Custom game config file will be loaded (\"%s\"",filename_ch.c_str());
    }
    else
    {
        // Custom config file doesn't exist
        FATALERROR("WARNING: Custom config file was not found (\"%s\")", filename_ch.c_str());
    }
}

// Returns the full file path of the current save file (with index)
std::wstring Files::get_save_file_path()
{
    // Set base file path/name
    std::string cur_save_file = Files::save_file;

    // Append index
    if (Files::save_file_index > 0) {
        if (Files::save_file_index < 10) {
            cur_save_file += "_0" + std::to_string(Files::save_file_index);
        }
        else {
            cur_save_file += "_" + std::to_string(Files::save_file_index);
        }
    }

    std::wstring buffer;
    if (string_mb_to_wide((char*)cur_save_file.c_str(), buffer))
        FATALERROR("Unable to convert save file %s into wstring.", cur_save_file.c_str());

    return buffer;
}

static const bool debug_save_print_output = true;

// Changes the current save file index (and writes all corresponding data)
void Files::set_save_file_index(int unsigned index)
{
    std::string save_path = Files::save_file;
    int save_file_count = 0;
    uint8_t saved_chars_menu_flag = 0;
    bool return_to_saves_screen = false;
    if (index > 99) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return;
    }
    // Check if index is out of range
    save_file_count = Sl2::get_save_file_count(save_path.c_str());
    if ((int)index >= save_file_count) {
        SetLastError(ERROR_RANGE_NOT_FOUND);
        if (debug_save_print_output) {
            ConsoleWrite("ERROR: Failed to set save file index (out of range)");
        }
        return;
    }
    // Check if character is loaded
    if (Game::playerchar_is_loaded()) {
        SetLastError(ERROR_BAD_ENVIRONMENT);
        if (debug_save_print_output) {
            ConsoleWrite("ERROR: Failed to set save file index (Character is loaded)");
        }
        return;
    }
    // Check if viewing saves menu
    if (!Game::get_saved_chars_menu_flag().has_value() || (*Game::get_saved_chars_menu_flag().value() != 4 && *Game::get_saved_chars_menu_flag().value() != 3)) {
        SetLastError(ERROR_BAD_ENVIRONMENT);
        if (debug_save_print_output) {
            ConsoleWrite("ERROR: Failed to set save file index (Must be viewing saved characters)");
        }
        return;
    }
    if (!Game::get_saved_chars_menu_flag().has_value())
    {
        ConsoleWrite("ERROR: Failed to set get_saved_chars_menu_flag to 0");
    }
    else
    {
        *Game::get_saved_chars_menu_flag().value() = 0;
    }
    Files::save_file_index = index;
    // Get full filename (with index)
    if (Files::save_file_index > 0) {
        if (Files::save_file_index < 10) {
            save_path += "_0" + std::to_string(Files::save_file_index);
        }
        else {
            save_path += "_" + std::to_string(Files::save_file_index);
        }
    }
    // Overwrite character preview data
    if (Sl2::read_character_preview_data_from_file(save_path.c_str(), Game::get_saved_chars_preview_data().value_or((uint8_t*)NULL)) != ERROR_SUCCESS) {
        if (debug_save_print_output) {
            ConsoleWrite("ERROR: Failed to set save file index (I/O error)");
        }
        if (!Game::get_saved_chars_menu_flag().has_value())
        {
            ConsoleWrite("ERROR: Failed to get_saved_chars_menu_flag");
        }
        else
        {
            *Game::get_saved_chars_menu_flag().value() = 3;
        }
        return;
    }
    // Update menu text
    std::wstring custom_header =
        L"                                            Save file "
        + std::to_wstring(Files::save_file_index + 1)
        + L"/" + std::to_wstring(save_file_count);
    std::wstring custom_header1 = L"Select data to load." + custom_header;
    std::wstring custom_header2 = L"Select data to delete." + custom_header;
    Menu::Saves::set_custom_header_msgs(custom_header1, custom_header2);

    // Re-load saved characters menu
    if (!Game::get_saved_chars_menu_flag().has_value())
    {
        ConsoleWrite("ERROR: Failed to reload due to get_saved_chars_menu_flag");
    }
    else
    {
        *Game::get_saved_chars_menu_flag().value() = 3;
    }
    if (debug_save_print_output) {
        ConsoleWrite("Save file index changed to %d",index);
    }
    SetLastError(ERROR_SUCCESS);
}

// Changes the to the next save file (if current save file is the last one, new file is first save file)
void Files::set_save_file_next()
{
    std::string file = Files::save_file;
    int count = Sl2::get_save_file_count(file.c_str());
    if (Files::save_file_index == (count - 1)) {
        Files::set_save_file_index(0);
    }
    else {
        Files::set_save_file_index(Files::save_file_index + 1);
    }
}

// Changes the to the previous save file (if current save file is the first one, new file is last save file)
void Files::set_save_file_prev()
{
    std::string file = Files::save_file;
    int count = Sl2::get_save_file_count(file.c_str());
    if (Files::save_file_index == 0) {
        Files::set_save_file_index(count - 1);
    }
    else {
        Files::set_save_file_index(Files::save_file_index - 1);
    }
}


// Checks if the saved characters menu is currently open
bool Files::saves_menu_is_open()
{
    if (!Game::get_saved_chars_menu_flag().has_value())
    {
        ConsoleWrite("ERROR: Failed to get_saved_chars_menu_flag in saves_menu_is_open");
        return false;
    }

    uint8_t flag_value = *Game::get_saved_chars_menu_flag().value();
    // Check if viewing saves menu
    if (flag_value != 4 && flag_value != 3) {
        return false;
    }
    // Check if character is loaded
    return !Game::playerchar_is_loaded();
}

void Files::create_new_save_file()
{
    SaveFile::generate_empty_save_file(Files::save_file.c_str());
    Files::set_save_file_index(Files::save_file_index + 1);
}
