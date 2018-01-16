/*
    DARK SOULS GAME FILE C++ LIBRARY

    Contributors to this file:
        Sean Pesce  -  C++


    Archive/Archive.h

*/

#pragma once

#ifndef DS1_FILE_LIB_GAME_ARCHIVE_FILE_H_
    #define DS1_FILE_LIB_GAME_ARCHIVE_FILE_H_

#define CURL_STATICLIB

#include "Archive/Header.h"
#include "PluginImports.h"
#include <map>
#include <curl/curl.h>
#include "SpSearchStringSet.h"


// Fix for compatibility issues between SeQan and Windows API
#ifdef min
    #undef min
#endif // min
#ifdef max
    #undef max
#endif // max

#ifdef _MSC_VER
    #pragma warning( disable : 4390) // Disable "';': empty controlled statement found; is this the intent?" warnings
    #pragma warning( push )
    #pragma warning( disable : 4521 4800 4244 4996 4503) // Disable "multiple copy constructors", "data loss", and "name truncated" warnings from SeQan library
#endif // _MSC_VER
#include "seqan/index.h" // String, StringSet, Index, Finder
#ifdef _MSC_VER
    #pragma warning( pop )
#endif // _MSC_VER



typedef enum ARCHIVE_FILE_TYPE {
    BDT = 0,
    BHD5 = 1,
    BND3 = 2,
    BND4 = 3,
    DCX = 4
} ARCHIVE_TYPE;


// Ancestor class for all Dark Souls archive file classes
typedef class ArchiveFile {
public:

    // Storage object for the names of files in the game's .bdt containers
    static SpSearchStringSetA bdt_file_name_set;

    // Flag to denote that the list of packed .bdt files is finished loading
    static bool bdt_file_name_set_loaded;

    // Initialize string formatting for .bdt files list
    static void ArchiveFile::init_bdt_file_name_set_format()
    {
        // Strings are automatically formatted as they are added to the SearchStringSet object

        // Remove tabs, new lines, and return carriage characters
        Archive::bdt_file_name_set.invalid_chars.value = "\t\r\n";
        Archive::bdt_file_name_set.invalid_chars.act = SSS_CONFORM;

        // Make searches case-insensitive
        Archive::bdt_file_name_set.case_distinction = SSS_CASE_INSENSITIVE;

        // Replace backslashes with forward slashes
        Archive::bdt_file_name_set.replacements.push_back(std::make_pair("\\", "/"));
    }

    // Obtains the list of filenames for files that can be extracted from .bdt files
    static int ArchiveFile::load_bdt_file_list(const char *source, const char *list_file, bool local_file)
    {
        static const char *default_url = "https://raw.githubusercontent.com/metal-crow/Dark-Souls-1-Overhaul/master/OverhaulDLL/rsrc/bdt_file_list.txt";
        static const char *default_file = "bdt_files.txt";
        static const char *error_curl = "    CURL ERROR: ";
        static const char *error_opening_file_write = "    WRITE ERROR: Failed to create or open ";
        static const char *error_closing_file[2] = { "    WRITE ERROR: Problem when attempting to close ", " after writing data" };
        static const char *error_opening_file_read = "    READ ERROR: Failed to open ";
        static const char *error_empty_list = "    ERROR: List was empty";
        static const char *success_message[3] = { "SUCCESS: ", " file names were loaded from ", "\n    File names will now be indexed for faster searches (Note: This might take a while)" };
        static const char *finished_indexing = " Finished indexing the list of archived .bdt files.";

        // Holds the error code returned by this function
        int error_code = ERROR_SUCCESS;

        // For storing extended CURL error messages:
        char error_msg_buffer[CURL_ERROR_SIZE];
        error_msg_buffer[0] = '\0';
        error_msg_buffer[CURL_ERROR_SIZE - 1] = '\0';

        // Use default URL if one was not specified
        if (source == NULL || std::string(source).length() == 0)
            source = default_url;
        // Use default output file if one was not specified
        if (list_file == NULL || std::string(list_file).length() == 0)
            list_file = default_file;

        // Build output string
        std::string info_message = __output_prefix + "Loading list of archived .bdt files...\n    Source: ";
        if (!local_file && (!FileUtil::file_exists(list_file) || FileUtil::file_size(list_file) == 0))
            info_message += source + std::string("\n    Saving to: ") + list_file;
        else
            info_message += list_file;

        if (!print_console(info_message))
#ifdef __DS1_OVERHAUL_MOD
            Mod::startup_messages.push_back(info_message);
#else
            ;
#endif // __DS1_OVERHAUL_MOD

        if (!local_file) // File list will be obtained from a remote source using CURL (default source is the Overhaul repository)
        {
            // Initialize file handle for the file that the web data will be written to
            FILE *out_file;
            if (fopen_s(&out_file, list_file, "w+") || out_file == NULL)
            {
                error_code = ERROR_WRITE_FAULT; // Unable to open file for writing
                if (!print_console(std::string(error_opening_file_write) + list_file))
#ifdef __DS1_OVERHAUL_MOD
                    Mod::startup_messages.push_back(std::string(error_opening_file_write) + list_file);
#else
                    ;
#endif // __DS1_OVERHAUL_MOD
            }

            // Initialize CURL
            CURL* curl;
            CURLcode result;
            curl = curl_easy_init();
            curl_easy_setopt(curl, CURLOPT_URL, source);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)out_file);
            curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, error_msg_buffer);
            curl_easy_setopt(curl, CURLOPT_USERAGENT, "DS-Overhaul/1.0");

            // Attempt to get the data from the web
            if ((result = curl_easy_perform(curl)) != CURLE_OK)
            {
                // Encountered a network error
                error_code = ERROR_UNEXP_NET_ERR;
                if (std::string(error_msg_buffer).length() > 0)
                {
                    if (!print_console(std::string(error_curl) + error_msg_buffer))
#ifdef __DS1_OVERHAUL_MOD
                        Mod::startup_messages.push_back(std::string(error_curl) + error_msg_buffer);
#else
                        ;
#endif // __DS1_OVERHAUL_MOD
                }
                else if (std::string(curl_easy_strerror(result)).length() > 0)
                {
                    if (!print_console(std::string(error_curl) + curl_easy_strerror(result)))
#ifdef __DS1_OVERHAUL_MOD
                        Mod::startup_messages.push_back(std::string(error_curl) + curl_easy_strerror(result));
#else
                        ;
#endif // __DS1_OVERHAUL_MOD
                }
            }

            curl_easy_cleanup(curl); // CURL cleanup

            // Close the file handle that the data was written with
            if (fclose(out_file) && error_code != ERROR_WRITE_FAULT)
            {
                // Error closing file handle
                if (!print_console(std::string(error_closing_file[0]) + list_file + error_closing_file[1]))
#ifdef __DS1_OVERHAUL_MOD
                    Mod::startup_messages.push_back(std::string(error_closing_file[0]) + list_file + error_closing_file[1]);
#else
                    ;
#endif // __DS1_OVERHAUL_MOD
                if (error_code == ERROR_SUCCESS)
                    error_code = ERROR_WRITE_FAULT;
            }
        }

        bdt_file_name_set_loaded = false; // Set "loaded" flag

        // Read list from local file
        if (error_code == ERROR_SUCCESS)
        {
            std::ifstream input_stream(list_file);

            if (!input_stream.good()) // Error opening file for reading
            {
                error_code = ERROR_READ_FAULT;
                if (!print_console(std::string(error_opening_file_read) + list_file))
#ifdef __DS1_OVERHAUL_MOD
                    Mod::startup_messages.push_back(std::string(error_opening_file_read) + list_file);
#else
                    ;
#endif // __DS1_OVERHAUL_MOD
                return error_code;
            }

            // Read file line by line
            std::string line;
            while (std::getline(input_stream, line))
            {
                ArchiveFile::bdt_file_name_set.append(line, false);
            }
            input_stream.close();
        }

        if (error_code == ERROR_SUCCESS && ArchiveFile::bdt_file_name_set.size() > 0) // Successfully loaded filename list
        {
            if (!print_console(__output_prefix + success_message[0] + std::to_string(ArchiveFile::bdt_file_name_set.size()) + success_message[1] + list_file + success_message[2]))
#ifdef __DS1_OVERHAUL_MOD
                Mod::startup_messages.push_back(Mod::output_prefix + success_message[0] + std::to_string(ArchiveFile::bdt_file_name_set.size()) + success_message[1] + list_file + success_message[2]);
#else
                ;
#endif // __DS1_OVERHAUL_MOD
            // Build search index
            ArchiveFile::bdt_file_name_set.build_index();

            // Notify user that indexing is complete
            if (!print_console(__output_prefix + finished_indexing))
#ifdef __DS1_OVERHAUL_MOD
                Mod::startup_messages.push_back(Mod::output_prefix + finished_indexing);
#else
                ;
#endif // __DS1_OVERHAUL_MOD

            bdt_file_name_set_loaded = true;
        }
        else if (error_code == ERROR_SUCCESS)
        {
            // Filename list was empty, but no error was detected
            if (!print_console(error_empty_list))
#ifdef __DS1_OVERHAUL_MOD
                Mod::startup_messages.push_back(error_empty_list);
#else
                ;
#endif // __DS1_OVERHAUL_MOD
            error_code = ERROR_SOURCE_ELEMENT_EMPTY;
        }
        return error_code;
    }
} Archive;


#endif // DS1_FILE_LIB_GAME_ARCHIVE_FILE_H_
