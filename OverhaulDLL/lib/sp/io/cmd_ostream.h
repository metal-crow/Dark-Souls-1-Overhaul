/*
    CONTRIBUTORS:
        Sean Pesce

*/
#ifdef _WIN32
#pragma once

#ifndef SP_INPUT_OUTPUT_CMD_OUTPUT_STREAM_H_
#define SP_INPUT_OUTPUT_CMD_OUTPUT_STREAM_H_

#include "sp/sp.h"
#include "sp/environment.h"
#include "sp/system/process/child.h"

#include <string>


#define SP_IO_CMD_OUT_LAUNCH_CMD_ "cmd /V:ON /C \"@ECHO OFF&TITLE " + _title + "&SET \"sp_log_output_line= \"&FOR /l %x IN (1, 0, 1) DO (SET /p sp_log_output_line=\"\"" \
                                  " & ECHO:!sp_log_output_line! & SET \"sp_log_output_line= \" & TASKLIST /FI \"IMAGENAME EQ " + sp::env::exe_name() +             \
                                  "\" 2>NUL | FIND /I /N \"" + sp::env::exe_name() + "\">NUL & IF \"!ERRORLEVEL!\"==\"1\" EXIT 0)\""


__SP_NAMESPACE
namespace io {


// Class for printing messages to external cmd window
class cmd_ostream : public sp::sys::proc::child {
private:
    // Window title
    std::string _title;

public:
    class defaults;

    cmd_ostream(const std::string& title = cmd_ostream::defaults::title)
            : sp::sys::proc::child("", "", true, 0)
    {
        _title = title;
        _command = SP_IO_CMD_OUT_LAUNCH_CMD_;
    }

    cmd_ostream(const sp::io::cmd_ostream&) = delete; // Disable copy constructor
    ~cmd_ostream() { terminate(); }


    bool print(const std::string& message) const
    {
        uint32_t bytes_written = 0;
        if (running())
        {
            if (!WriteFile(io.stdin_write,
                            message.c_str(),
                            static_cast<DWORD>(message.length()),
                            reinterpret_cast<DWORD*>(&bytes_written),
                            NULL))
            {
                // Handle error
                return false;
            }
        }
        else
        {
            return false;
        }
        return true;
    }


    inline const std::string& title() const
    {
        return _title;
    }


    inline void set_title(const std::string& new_title)
    {
        if (_title != new_title)
        {
            _title = new_title;
            restart();
        }
    }

    class defaults {
    public:
        static constexpr const char* title = "CMD Output";
    private:
        defaults() = delete;
    };

}; // class cmd_ostream


} // namespace io
__SP_NAMESPACE_CLOSE // namespace sp


inline const sp::io::cmd_ostream& operator<< (const sp::io::cmd_ostream& os, const std::string& message)
{
    os.print(message);
    return os;
}

inline const sp::io::cmd_ostream& operator<< (const sp::io::cmd_ostream& os, char message)
{
    os.print(std::string(1, message));
    return os;
}


#undef SP_IO_CMD_OUT_LAUNCH_CMD_

#endif // SP_INPUT_OUTPUT_CMD_OUTPUT_STREAM_H_


#endif // _WIN32
