/*
    CONTRIBUTORS:
        Sean Pesce

    Console output message class
*/

#pragma once

#ifndef SP_GUI_CONSOLE_OUTPUT_MESSAGE_H_
#define SP_GUI_CONSOLE_OUTPUT_MESSAGE_H_

#include "sp/sp.h"
#include "sp/gui/console.h"

#include <string>
#include <vector>


__SP_NAMESPACE
namespace gui     {
namespace console {


template <typename charT = char>
class message {

protected:

    //static const charT[3] BAD_NEWLINE = { (charT)'\r', (charT)'\n', (charT)'\0' };
    overflow_mode _overflow_mode = sp::gui::console::default_overflow;
    size_t _width = sp::gui::console::default_width; // Width (characters)

    std::basic_string<charT> _message;
    std::vector<std::basic_string<charT>> _lines = std::vector<std::basic_string<charT>>();

    // Break message into lines
    void _parse_lines()
    {
        _lines.clear();

        if (_width < 1)
        {
            _width = 1;
        }

        size_t start = 0,
               pos   = _message.find((charT)'\n'),
               len   = _message.length();

        if (_overflow_mode == overflow_mode::hide)
        {
            while (pos != std::string::npos)
            {
                // Remove overflowing chars
                _lines.push_back(_message.substr(start, (pos-start <= _width) ? pos-start : _width));
                start = pos + 1;
                if (start < len)
                {
                    pos = _message.find((charT)'\n', start);
                }
                else
                {
                    break;
                }
            }
            if (start < len)
            {
                _lines.push_back(_message.substr(start, (len-start <= _width) ? len-start : _width));
            }
        }
        else if (_overflow_mode == overflow_mode::wrap)
        {
            while (pos != std::string::npos)
            {
                if (pos - start <= _width)
                {
                    _lines.push_back(_message.substr(start, pos-start));
                }
                else
                {
                    do
                    {
                        // Wrap overflowing chars to next line
                        size_t w = (pos-start) <= _width ? pos-start : _width;
                        _lines.push_back(_message.substr(start, w));
                        start += w;
                    } while (start < pos);
                }
                start = pos + 1;
                if (start < len)
                {
                    pos = _message.find((charT)'\n', start);
                }
                else
                {
                    break;
                }
            }
            if (start < len)
            {
                do
                {
                    // Wrap overflowing chars to next line
                    size_t w = (len-start) <= _width ? len-start : _width;
                    _lines.push_back(_message.substr(start, w));
                    start += w;
                } while (start < len);
            }
        }
        else // _overflow_mode == overflow_mode::overflow
        {
            while (pos != std::string::npos)
            {
                _lines.push_back(_message.substr(start, pos-start));
                start = pos + 1;
                if (start < len)
                {
                    pos = _message.find((charT)'\n', start);
                }
                else
                {
                    break;
                }
            }
            if (start < len)
            {
                _lines.push_back(_message.substr(start));
            }
        }

        if (_message.empty() || _message[len-1] == (charT)'\n')
        {
            _lines.push_back(std::basic_string<charT>());
        }
    }

public:

    // Constructors/destructors
    message() : _message()
    {
        _parse_lines();
    }

    message(const std::basic_string<charT>& str,
            size_t width = sp::gui::console::default_width,
            overflow_mode overflow = sp::gui::console::default_overflow)
                : _message(str),
                  _width(width),
                  _overflow_mode(overflow)
    {
        _parse_lines();
    }

    message(const charT* char_str,
            size_t width = sp::gui::console::default_width,
            overflow_mode overflow = sp::gui::console::default_overflow)
                : _message(char_str),
                  _width(width),
                  _overflow_mode(overflow)
    {
        _parse_lines();
    }

    message(const std::basic_string<charT>& str,
            overflow_mode overflow)
                : _message(str),
                  _overflow_mode(overflow)
    {
        _parse_lines();
    }

    message(const charT* char_str,
            overflow_mode overflow)
                : _message(char_str),
                  _overflow_mode(overflow)
    {
        _parse_lines();
    }

    message(const sp::gui::console::message<charT>& msg) : _message(msg.str_raw()),
                                                           _width(msg.width()),
                                                           _overflow_mode(msg.overflow_mode())
    {
        _parse_lines();
    }

    message& operator=(const sp::gui::console::message<charT>& msg)
    {
        if (this != &msg)
        {
            this->assign(msg);
        }
        return *this;
    }

    message& operator=(const std::basic_string<charT>& msg)
    {
        this->_message = msg;
        this->_parse_lines();
        return *this;
    }

    message& operator=(const charT* msg)
    {
        this->_message = msg;
        this->_parse_lines();
        return *this;
    }

    inline ~message() {}


    inline void assign(const sp::gui::console::message<charT>& other)
    {
        this->_message = other.str_raw();
        this->_overflow_mode = other.overflow_mode();
        this->_width = other.width();
        this->_parse_lines();
    }


    ////// ACCESSORS //////
    inline const std::vector<std::basic_string<charT>>& lines() const { return this->_lines; }
    inline size_t line_count() const { return this->_lines.size(); }
    inline const std::basic_string<charT>& str_raw() const { return this->_message; }
    inline sp::gui::console::overflow_mode overflow_mode() const { return this->_overflow_mode; }
    inline size_t width() const { return this->_width; }

    inline std::basic_string<charT> str() const
    {
        std::basic_string<charT> s;
        for (std::basic_string<charT> line : _lines)
        {
            s += line + (charT)'\n';
        }
        return s;
    }

    inline std::basic_string<charT> str(size_t max_lines) const
    {
        std::basic_string<charT> s;
        max_lines = (max_lines <= _lines.size()) ? max_lines : _lines.size();
        for (size_t i = 0; i < max_lines; i++)
        {
            s += _lines[i] + (charT)'\n';
        }
        return s;
    }

    inline std::basic_string<charT> str(size_t max_lines, size_t* line_count) const
    {
        if (line_count == nullptr)
        {
            return this->str(max_lines);
        }
        std::basic_string<charT> s;
        max_lines = (max_lines <= _lines.size()) ? max_lines : _lines.size();
        *line_count = max_lines;
        for (size_t i = 0; i < max_lines; i++)
        {
            s += _lines[i] + (charT)'\n';
        }
        return s;
    }

    // Get line at given index
    inline const std::basic_string<charT>& get_line(size_t index) const
    {
        return this->_lines[index];
    }
    
    inline const std::basic_string<charT>& operator[](size_t index) const
    {
        return this->_lines[index];
    }

    inline std::basic_string<charT>& get_line(size_t index)
    {
        return this->_lines[index];
    }

    inline std::basic_string<charT>& operator[](size_t index)
    {
        return this->_lines[index];
    }

    inline bool operator==(const sp::gui::console::message<charT>& other)
    {
        return this->_message == other._message;
    }

    inline bool operator!=(const sp::gui::console::message<charT>& other)
    {
        return this->_message != other._message;
    }


    ////// MUTATORS //////

    inline void set_message(const std::basic_string<charT>& msg)
    {
        if (this->_message != msg)
        {
            this->_message = msg;
            this->_parse_lines();
        }
    }

    inline void set_overflow_mode(sp::gui::console::overflow_mode overflow_mode)
    {
        if (this->_overflow_mode != overflow_mode)
        {
            this->_overflow_mode = overflow_mode;
            this->_parse_lines();
        }
    }

    inline void set_width(size_t width)
    {
        if (this->_width != width)
        {
            this->_width = width;
            if (this->_overflow_mode != sp::gui::console::overflow_mode::overflow)
            {
                this->_parse_lines();
            }
        }
    }

    inline void set_width_and_overflow(size_t width, sp::gui::console::overflow_mode overflow_mode)
    {
        if (this->_width != width || this->_overflow_mode != overflow_mode)
        {
            this->_width = width;
            this->_overflow_mode = overflow_mode;
            this->_parse_lines();
        }
    }


}; // class message


} // namespace console
} // namespace gui
__SP_NAMESPACE_CLOSE // namespace sp

#endif // SP_GUI_CONSOLE_OUTPUT_MESSAGE_H_
