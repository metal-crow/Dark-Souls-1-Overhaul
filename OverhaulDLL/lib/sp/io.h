/*
    CONTRIBUTORS:
        Sean Pesce

*/

#pragma once

#ifndef SP_UTILITY_INPUT_OUTPUT_MAIN_H_
#define SP_UTILITY_INPUT_OUTPUT_MAIN_H_

#include "sp/sp.h"


__SP_NAMESPACE
namespace io {

extern uint32_t BEEP_FREQ_DEFAULT,
                BEEP_DUR_DEFAULT;

bool beep(uint32_t frequency = BEEP_FREQ_DEFAULT, uint32_t duration = BEEP_DUR_DEFAULT, bool async = true);


/**
    get_async_keyboard_state(unsigned short*)
    
    Gets the async key state for all 256 virtual keys and stores them in the given buffer.
    Note: The buffer must be at least the size of an array of 256 * sizeof(unsigned short).
    
    @param keyboard_state_buffer Buffer where async key state data will be stored
 */
#ifdef _WIN32
inline void get_async_keyboard_state(unsigned short(&keyboard_state_buffer)[256])
{
    for (int key = 0; key < 256; key++)
    {
        keyboard_state_buffer[key] = GetAsyncKeyState(key);
    }
}
#endif // _WIN32

} // namespace io
__SP_NAMESPACE_CLOSE // namespace sp

#endif // SP_UTILITY_INPUT_OUTPUT_MAIN_H_
