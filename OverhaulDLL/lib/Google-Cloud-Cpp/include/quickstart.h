#ifndef GOOGLE_CLOUD_CPP_CUSTOM_LIB_H
#define GOOGLE_CLOUD_CPP_CUSTOM_LIB_H

#include <iostream>
#include <stdbool.h>

class GoogleCloudLib
{
    public:
    static bool CreateClient(std::string const& keyfile);
    static bool UploadFile(std::string const& bucketname, std::string const& filename, std::string const& objectname);
};

#endif