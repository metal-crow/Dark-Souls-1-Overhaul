/*
    CONTRIBUTORS:
        Sean Pesce

*/

#pragma once

#ifndef SP_UTILITY_SYSTEM_PROCESS_CHILD_H_
#define SP_UTILITY_SYSTEM_PROCESS_CHILD_H_

#include "sp/sp.h"
#include "../process.h"

#include <string>


__SP_NAMESPACE
namespace sys  {
namespace proc {


class child : public sp::sys::process {
protected:
    bool     _inherit_handles;
    uint32_t _flags;

public:
    STARTUPINFO startup;
    PROCESS_INFORMATION info;
    SECURITY_ATTRIBUTES security;


    child(const std::string &app_name,
            const std::string &command,
            bool inherit_handles,
            uint32_t flags)
    {
        _initialize();
        _app_name = app_name;
        _command  = command;
        _inherit_handles = inherit_handles;
        _flags  = flags;
    }

    child(const child&) = delete; // Disable copy constructor

    
    bool start()
    {
        // Command-line command to launch the external console output window
        if (!running())
        {
            const char* app_name = _app_name.length() > 0 ? _app_name.c_str() : NULL;
            const char* command  = _command.length()  > 0 ? _command.c_str()  : NULL;
            startup.cb = sizeof(startup);
            startup.hStdInput = io.stdin_read;
            startup.dwFlags  |= STARTF_USESTDHANDLES;
            if (!CreateProcess(app_name,
                            (char*)command,
                            NULL,     // Process security attributes
                            NULL,     // Primary thread security attributes
                            (int)_inherit_handles,
                            _flags,   // Creation flags
                            NULL,     // Use parent environment
                            NULL,     // Use parent current directory
                            &startup,
                            &info))
            {
                // Handle error
                return false;
            }
            else
            {
                return true;
            }
        }
        else
        {
            return true;
        }
    }


    bool terminate()
    {
        if (running())
        {
            if (info.hProcess != NULL)
            {
                //system(std::string("TASKKILL /PID ").append(std::to_string(info.dwProcessId)).append(" /F").c_str());
                if (!TerminateProcess(info.hProcess, EXIT_SUCCESS))
                {
                    // Handle error
                    return false;
                }
                if (!CloseHandle(info.hThread))
                {
                    // Handle error
                    return false;
                }
                if (!CloseHandle(io.stdin_read))
                {
                    // Handle error
                    return false;
                }
                if (!CloseHandle(io.stdin_write))
                {
                    // Handle error
                    return false;
                }
                if (!CloseHandle(info.hProcess))
                {
                    // Handle error
                    return false;
                }
            }
            _initialize();
            return true;
        }
        else
        {
            return true;
        }
    }


    inline bool restart()
    {
        if (running())
        {
            return (terminate() && start());
        }
        return false;
    }


    inline bool running() const
    {
        uint32_t exit_code = 0;
        return (GetExitCodeProcess(info.hProcess, reinterpret_cast<DWORD*>(&exit_code)) && exit_code == STILL_ACTIVE);
    }

protected:
    bool _initialize()
    {
        if (running() && !terminate())
        {
            return false;
        }
        memset(&io, 0, sizeof(io));
        memset(&startup, 0, sizeof(startup));
        memset(&info, 0, sizeof(info));
        memset(&security, 0, sizeof(security));
        // Set pipe handles to be inherited
        security.nLength = sizeof(security);
        security.bInheritHandle = TRUE;
        security.lpSecurityDescriptor = NULL;

        // Create pipe for child process stdin
        if (!CreatePipe(&io.stdin_read, &io.stdin_write, &security, 0))
        {
            // Handle error
            return false;
        }

        // Make sure stdin pipe write handle is not inherited
        if (!SetHandleInformation(io.stdin_write, HANDLE_FLAG_INHERIT, 0)) // Allow writing to child process stdin
        {
            // Handle error
            return false;
        }
        return true;
    }
}; // class child


} // namespace proc
} // namespace sys
__SP_NAMESPACE_CLOSE // namespace sp

#endif // SP_UTILITY_SYSTEM_PROCESS_CHILD_H_
