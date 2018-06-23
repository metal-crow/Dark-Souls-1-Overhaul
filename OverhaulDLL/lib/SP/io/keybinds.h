#pragma once

#ifndef SP_UTIL_IO_KEYBINDS_H_
#define SP_UTIL_IO_KEYBINDS_H_

#include <vector>

#include "sp/sp.h"

__SP_NAMESPACE
namespace io {

    typedef struct SP_KEY_FUNCTION {
        unsigned int key;	// Virtual key code
        int(*function)();	// Function called when key is pressed
    } SP_KEY_FUNCTION;

    #define _SP_KEY_DOWN_ 0x80000000

    class keybinds {
    private:
        // Stores all function/keybind mappings
        static std::vector<SP_KEY_FUNCTION>& keybinds_list()
        {
            static std::vector<SP_KEY_FUNCTION> theInstance;
            return theInstance;
        }

    public:
        static inline unsigned int register_hotkey_function(unsigned int vk_hotkey, int(*function)())
        {
            if (vk_hotkey > 0 && function != NULL)
            {
                SP_KEY_FUNCTION new_key_func = { vk_hotkey, function };
                keybinds_list().push_back(new_key_func);
                return vk_hotkey;
            }
            else {
                return 0;
            }
        }

        static inline void check_hotkeys() {
            SHORT keyboard_state_buffer[256];
            for (int key = 0; key < 256; key++)
            {
                keyboard_state_buffer[key] = GetAsyncKeyState(key);
            }

            std::vector<SP_KEY_FUNCTION>::const_iterator key_func_iterator;
            for (key_func_iterator = keybinds_list().begin(); key_func_iterator != keybinds_list().end(); key_func_iterator++)
            {
                if(key_func_iterator->key != 0 && (keyboard_state_buffer[key_func_iterator->key] & _SP_KEY_DOWN_))
                {
                    key_func_iterator->function();
                    break;
                }
            }
        }

    };
} // namespace io
__SP_NAMESPACE_CLOSE // namespace sp

#endif
