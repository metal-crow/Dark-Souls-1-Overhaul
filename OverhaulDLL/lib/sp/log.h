/*
    CONTRIBUTORS:
        Sean Pesce

*/

#pragma once

#ifndef SP_UTILITY_LOG_H_
#define SP_UTILITY_LOG_H_

#include "sp/sp.h"
#include "sp/io/cmd_ostream.h"
#include "sp/file.h"
#include <string>
#include <iostream>

#ifdef _DEBUG
#define SP_LOG_DBG(format_str, ...) sp::log::write(sp::str::format(format_str, __VA_ARGS__))
#define SP_LOG_DBG_ELSE_PRINT(format_str, ...) sp::log::write(sp::str::format(format_str, __VA_ARGS__)) // Writes log in debug builds. If release build, only prints to console instead
#else
#define SP_LOG_DBG(format_str, ...)
#define SP_LOG_DBG_ELSE_PRINT(format_str, ...) sp::log::print_ostream(sp::str::format(format_str, __VA_ARGS__))
#endif // _DEBUG

#define SP_LOG(format_str, ...) sp::log::write(sp::str::format(format_str, __VA_ARGS__))




__SP_NAMESPACE


class log {
private:
    log() = delete;

    static bool _initialized;
    static bool _overwrite_on_init; // Overwrite log file during initialization
    static std::string _file;
    static io::cmd_ostream _ostream;


public:
    class printer; // Member class allowing use of "<<" operator
    static const printer out;


    static bool initialize(bool open_output_window = false);

    static inline bool initialized() { return log::_initialized; }
    static inline bool overwrite_on_init() { return log::_overwrite_on_init; }
    static inline const std::string& file() { return log::_file; }
    static inline io::cmd_ostream& ostream() { return log::_ostream; }


    static inline void set_overwrite_on_init(bool overwrite)
    {
        log::_overwrite_on_init = overwrite;
    }


    //static inline void set_file(const std::string& file, bool reinitialize = false, bool open_output_window = false)
    //{
    //    log::_file = file;
    //    if (reinitialize || !sp::file::exists(log::_file))
    //    {
    //        log::initialize(open_output_window);
    //    }
    //}


    static inline bool write(const std::string& message, bool print_to_ostream = true, bool print_to_stdout = true)
    {
        if (print_to_stdout)
        {
            std::cout << message;
        }
        if (print_to_ostream)
        {
            log::print_ostream(message);
        }
        return log::write_file(message);
    }


    static inline bool write_file(const std::string& message, bool overwrite = false)
    {
        if (log::_file.empty())
        {
            return true;
        }

        long err = SP_ERR_SUCCESS;
        if (overwrite)
        {
            err = sp::file::write_text(log::_file, message);
        }
        else
        {
            err = sp::file::append_text(log::_file, message);
        }
        return (err == SP_ERR_SUCCESS);
    }


    static inline bool print_ostream(const std::string& message)
    {
        return log::_ostream.print(message);
    }


    static inline bool show()
    {
        return log::_ostream.start();
    }


    static inline bool hide()
    {
        return log::_ostream.terminate();
    }


    // Member class allowing use of "<<" operator
    class printer {
    public:
        printer() {}
    };


    class defaults {
    public:
        static constexpr const bool overwrite_on_init = true;
        static constexpr const char* file = "SP_log.log";
    private:
        defaults() = delete;
    };

}; // class log


__SP_NAMESPACE_CLOSE // namespace sp



inline const sp::log::printer& operator<< (const sp::log::printer& printer, const std::string& message)
{
    sp::log::write(message);
    return printer;
}

inline const sp::log::printer& operator<< (const sp::log::printer& printer, char message)
{
    sp::log::write(std::string(1, message));
    return printer;
}



#endif // SP_UTILITY_LOG_H_
