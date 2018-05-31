/*
    CONTRIBUTORS:
        Sean Pesce

    Basic console output class
*/

#pragma once

#ifndef SP_GUI_CONSOLE_BASIC_OUTPUT_H_
#define SP_GUI_CONSOLE_BASIC_OUTPUT_H_

#include "sp/gui/console/console_output_interface.h"
#include "sp/gui/console/console_message.h"
#include "sp/utility/history.h"


__SP_NAMESPACE
namespace gui     {
namespace console {


template <typename charT = char>
class output : public sp::gui::console::output_interface<charT> {

private:

    overflow_mode _overflow_mode = sp::gui::console::default_overflow;

    size_t _width  = sp::gui::console::default_width, // Width (characters)
           _height = 25; // Height (lines)

    sp::util::history<sp::gui::console::message<charT>> _history = sp::util::history<sp::gui::console::message<charT>>(default_history_limit, true);


public:

    const static size_t default_history_limit = 200;

    // Constructors/destructors
    output() {}

    output(size_t width) : _width(width) {}

    output(overflow_mode overflow_mode) : _overflow_mode(overflow_mode) {}

    output(size_t width, size_t height, size_t history_limit = default_history_limit)
        : _width(width), _height(height)
    {
        _history.set_max_entries(history_limit);
    }

    output(size_t width, size_t height, overflow_mode overflow_mode, size_t history_limit = default_history_limit)
        : _width(width), _height(height), _overflow_mode(overflow_mode)
    {
        _history.set_max_entries(history_limit);
    }

    output& operator=(const sp::gui::console::output<charT>& other)
    {
        if (this != &other)
        {
            this->assign(other);
        }
        return *this;
    }

    inline ~output() {}


    void assign(const sp::gui::console::output<charT>& other)
    {
        this->_overflow_mode = other.overflow_mode();
        this->_width = other.width();
        this->_height = other.height();
        this->clear_history();
        this->_history.set_max_entries(other.history_limit());
        for (unsigned int i = 0; i < other.history().size(); i++)
        {
            this->_history.add(other.history()[other.history().size() - (1+i)]);
        }
    }


    ////// ACCESSORS //////
    inline const sp::util::history<sp::gui::console::message<charT>>& history() const { return this->_history; }
    inline size_t history_limit() const { return this->_history.max_entries(); }
    inline sp::gui::console::overflow_mode overflow_mode() { return this->_overflow_mode; }
    inline size_t width()  const { return this->_width;  }
    inline size_t height() const { return this->_height; }

    // Get message at given index
    inline const sp::gui::console::message<charT>& get_message(size_t index) const
    {
        return this->_history[index];
    }

    // Get line at given index
    inline std::basic_string<charT>& get_line(size_t index)
    {
        sp::gui::console::message<charT> msg;
        size_t line = 0;
        for (size_t i = 0; i < _history.size(); i++)
        {
            if (line + _history[i].line_count() > index)
            {
                msg = _history[i];
                break;
            }
            line += _history[i].line_count();
        }
        return msg[(msg.line_count() - 1) - index];
    }

    std::basic_string<charT>& operator[](size_t index)
    {
        return this->get_line(index);
    }

    inline const std::basic_string<charT>& get_line(size_t index) const
    {
        return this->get_line(index);
    }

    const std::basic_string<charT>& operator[](size_t index) const
    {
        return this->get_line(index);
    }

    // Get currently-visible output as single string
    inline std::basic_string<charT> str() const
    {
        size_t lines = 0, lines_added = 0;
        std::basic_string<charT> output;
        for (size_t i = 0; lines < _height && i < _height && i < _history.size(); i++)
        {
            output = _history[i].str(_height - lines, &lines_added) + output;
            lines += lines_added;
        }
        return output;
    }


    ////// MUTATORS //////

    inline void set_history_limit(size_t limit)
    {
        this->_history.set_max_entries(limit);
    }

    inline void set_height(size_t height)
    {
        this->_height = height;
    }

    inline void set_width(size_t width)
    {
        if (this->_width != width)
        {
            this->_width = width;
            for (size_t i = 0; i < this->_history.size(); i++)
            {
                this->_history[i].set_width(this->_width);
            }
        }
    }

    inline void set_overflow_mode(sp::gui::console::overflow_mode overflow_mode)
    {
        if (this->_overflow_mode != overflow_mode)
        {
            this->_overflow_mode = overflow_mode;
            for (size_t i = 0; i < this->_history.size(); i++)
            {
                this->_history[i].set_overflow_mode(this->_overflow_mode);
            }
        }
    }

    inline void set_width_and_overflow(size_t width, sp::gui::console::overflow_mode overflow_mode)
    {
        if (this->_width != width || this->_overflow_mode != overflow_mode)
        {
            this->_width = width;
            this->_overflow_mode = overflow_mode;
            for (size_t i = 0; i < this->_history.size(); i++)
            {
                this->_history[i].set_width_and_overflow(this->_width, this->_overflow_mode);
            }
        }
    }


    ////// ACTIONS //////

    // Append output
    inline void print(const std::basic_string<charT>& msg = std::basic_string<charT>())
    {
        _history.add(sp::gui::console::message<charT>(msg, _width, _overflow_mode));
    }

    inline void print(const charT* msg)
    {
        _history.add(sp::gui::console::message<charT>((msg != nullptr) ? msg : std::basic_string<charT>(), _width, _overflow_mode));
    }

    inline void print(const sp::gui::console::message<charT>& msg)
    {
        if (msg.overflow_mode() == this->_overflow_mode && msg.width() == this->_width)
        {
            _history.add(msg);
        }
        else
        {
            _history.add(sp::gui::console::message<charT>(msg.str_raw(), _width, _overflow_mode));
        }
    }

    inline void print(size_t count)
    {
        std::basic_string<charT> blank_msg(count, (charT)'\n');
        _history.add(sp::gui::console::message<charT>(blank_msg, _width, _overflow_mode));
    }

    inline void print(const std::basic_string<charT>& msg, size_t count)
    {
        for (size_t i = 0; i < count; i++)
        {
            _history.add(sp::gui::console::message<charT>(msg, _width, _overflow_mode));
        }
    }

    inline void print(const charT* msg, size_t count)
    {
        for (size_t i = 0; i < count; i++)
        {
            _history.add(sp::gui::console::message<charT>((msg != nullptr) ? msg : std::basic_string<charT>(), _width, _overflow_mode));
        }
    }

    inline void print(const sp::gui::console::message<charT>& msg, size_t count)
    {
        sp::gui::console::message<charT> message;
        if (msg.overflow_mode() == this->_overflow_mode && msg.width() == this->_width)
        {
            message = msg;
        }
        else
        {
            message = sp::gui::console::message<charT>(msg.str_raw(), _width, _overflow_mode);
        }
        for (size_t i = 0; i < count; i++)
        {
            _history.add(message);
        }
    }

    inline void operator+=(const std::basic_string<charT>& msg)
    {
        this->print(msg);
    }

    inline void operator+=(const charT* msg)
    {
        this->print(msg);
    }

    inline void operator+=(const sp::gui::console::message<charT>& msg)
    {
        this->print(msg);
    }

    // Clear output
    inline void cls()
    {
        this->print(this->_height);
    }

    inline void clear_history()
    {
        this->_history.clear();
    }

}; // class output


} // namespace console
} // namespace gui
__SP_NAMESPACE_CLOSE // namespace sp

#endif // SP_GUI_CONSOLE_BASIC_OUTPUT_H_
