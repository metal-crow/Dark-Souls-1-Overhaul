/*
    CONTRIBUTORS:
        Sean Pesce

*/

#pragma once

#ifndef SP_UTILITY_FILE_PATH_H_
#define SP_UTILITY_FILE_PATH_H_

#include "sp/sp.h"
#include "sp/file.h"

#include <string>


__SP_NAMESPACE
namespace file {


template <typename charT>
class path {
protected:
    std::basic_string<charT> _dir;
    std::basic_string<charT> _name_no_ext;
    std::basic_string<charT> _ext;

public:
    path(const std::basic_string<charT> &file_name)
        : _dir(sp::file::dir(file_name)), _name_no_ext(sp::file::name_no_ext(file_name)), _ext(sp::file::ext(file_name))
    {
    }

    inline std::basic_string<charT> get()  const { return _dir + _name_no_ext + _ext; }
    inline std::basic_string<charT> dir()  const { return _dir; }
    inline std::basic_string<charT> name_no_ext() const { return _name_no_ext; }
    inline std::basic_string<charT> ext()  const { return _ext; }

    inline void set(const std::basic_string<charT> &file_name) {
        _dir  = sp::file::dir(file_name);
        _name_no_ext = sp::file::name_no_ext(file_name);
        _ext  = sp::file::ext(file_name);
    }

    inline void set_dir(const std::basic_string<charT> &new_dir) {
        _dir = new_dir;
    }

    inline void set_name_no_ext(const std::basic_string<charT> &new_name_no_ext) {
        _name_no_ext = new_name_no_ext;
    }

    inline void set_ext(const std::basic_string<charT> &new_dir) {
        _ext = new_ext;
    }
};


} // namespace file
__SP_NAMESPACE_CLOSE // namespace sp

#endif // SP_UTILITY_FILE_PATH_H_
