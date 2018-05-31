/*
    CONTRIBUTORS:
        Sean Pesce

*/

#pragma once

#ifndef SP_UTILITY_SYSTEM_PROCESS_H_
#define SP_UTILITY_SYSTEM_PROCESS_H_

#include "sp/sp.h"

#include <vector>


__SP_NAMESPACE
namespace sys  {

namespace proc {
struct io_handles {
    HANDLE stdin_read   = NULL;
    HANDLE stdin_write  = NULL;
    HANDLE stdout_read  = NULL;
    HANDLE stdout_write = NULL;
};
} // namespace proc

class process {
protected:
    std::string _app_name;
    std::string _command;

public:
    sp::sys::proc::io_handles io;

    process()
    {
    }

    process(const process&) = delete; // Disable copy constructor
}; // class process


} // namespace sys
__SP_NAMESPACE_CLOSE // namespace sp

#endif // SP_UTILITY_SYSTEM_PROCESS_H_
