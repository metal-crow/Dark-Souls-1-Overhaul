/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Sean Pesce  -  C++


*/
#pragma once

#ifndef DS1_OVERHAUL_UPDATES_H_
    #define DS1_OVERHAUL_UPDATES_H_

#define CURL_STATICLIB


#include "DllMain.h"


namespace Updates {

constexpr const char* VERSION = __DATE__ "  " __TIME__;

#define DS1_OVERHAUL_UPDATE_SOURCE_COUNT_ 3

#define DS1_OVERHAUL_DEFAULT_DOWNLOAD_URL_ "https://github.com/metal-crow/Dark-Souls-1-Overhaul/releases"

// Returns string containing the Overhaul message of the day (MotD)
std::string motd();

// Returns string containing latest Overhaul DLL build version
std::string latest();

// Returns URL to latest Overhaul DLL download
std::string download_url();

// Checks online for a new Overhaul MotD
void check_motd();

// Checks online for the latest Overhaul version string
void check_latest();

// Checks online for the latest Overhaul download URL
void check_download_url();

// Retrieves data from the specified remote URL and saves it to a local file
long fetch_data(const std::string& source, const std::string& output_file, bool silent = false);

// Compares the local version string to the remote version string and returns 1 if the remote version string is newer, 0 if they are the same, or -1 if local is newer
int compare_versions();

// Flags to skip checking remote data sources
extern bool skip_source[DS1_OVERHAUL_UPDATE_SOURCE_COUNT_];

extern bool keep_temp_files;

} // namespace Updates


#endif // DS1_OVERHAUL_UPDATES_H_
