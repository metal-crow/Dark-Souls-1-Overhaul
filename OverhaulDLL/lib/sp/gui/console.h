/*
    CONTRIBUTORS:
        Sean Pesce

*/

#pragma once

#ifndef SP_GUI_CONSOLE_MAIN_H_
#define SP_GUI_CONSOLE_MAIN_H_

#include "sp/sp.h"


__SP_NAMESPACE
namespace gui     {
namespace console {


enum overflow_mode {
    hide,    // Hide overflowing chars
    wrap,    // Print overflowing chars on new line
    overflow // Print overflowing chars on same line
};

const static overflow_mode default_overflow = overflow_mode::hide;
const static size_t default_width = 100; // Width (characters)

} // namespace console
} // namespace gui
__SP_NAMESPACE_CLOSE // namespace sp

#endif // SP_GUI_CONSOLE_MAIN_H_
