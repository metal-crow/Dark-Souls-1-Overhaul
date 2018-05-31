/*
    CONTRIBUTORS:
        Sean Pesce

    Console text output interface
*/

#pragma once

#ifndef SP_GUI_CONSOLE_OUTPUT_INTERFACE_H_
#define SP_GUI_CONSOLE_OUTPUT_INTERFACE_H_

#include "sp/sp.h"
#include "sp/gui/console.h"

#include <string>


__SP_NAMESPACE
namespace gui     {
namespace console {


template <typename charT = char>
class output_interface {

public:

    // Constructors/destructors
    output_interface() {}
    virtual inline ~output_interface() {}

    // Append output
    virtual inline void print(const std::basic_string<charT>& msg) = 0;

    // Clear output
    virtual inline void cls() = 0;

}; // class output_interface


} // namespace console
} // namespace gui
__SP_NAMESPACE_CLOSE // namespace sp

#endif // SP_GUI_CONSOLE_OUTPUT_INTERFACE_H_
