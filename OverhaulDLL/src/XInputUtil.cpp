/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Sean Pesce        -  C++
*/

#include "DarkSoulsOverhaulMod.h"
#include "XInputUtil.h"
#include "Files.h"
#include "sp/memory/injection/asm/x64.h"

namespace XInput {



namespace Button {

// True if button was pressed between gamepad states
inline bool pressed(XINPUT_GAMEPAD &old, XINPUT_GAMEPAD &current, uint16_t button) {
    return (!(old.wButtons & button) && (current.wButtons & button));
}
// True if button was released between gamepad states
inline bool released(XINPUT_GAMEPAD &old, XINPUT_GAMEPAD &current, uint16_t button) {
    return ((old.wButtons & button) && !(current.wButtons & button));
}
// True if button is currently down
inline bool down(XINPUT_GAMEPAD &current, uint16_t button) {
    return ((current.wButtons & button) != 0);
}
// Current status of button
inline Status status(XINPUT_GAMEPAD &old, XINPUT_GAMEPAD &current, uint16_t button) {
    if (pressed(old, current, button)) {
        return BUTTON_PRESSED;
    } else if (down(current, button)) {
        return BUTTON_DOWN;
    } else if (released(old, current, button)) {
        return BUTTON_RELEASED;
    } else {
        return BUTTON_UP;
    }
}

} // namespace Button



void *base = NULL;

// Initializes pointers and other data used to monitor gamepad input
void initialize() {
    global::cmd_out << (Mod::output_prefix + "Initializing XInput hook...\n");

    base = GetModuleHandle("XINPUT1_3.dll");
    while (base == NULL) {
        // Wait for XInput DLL to load, then save base address
        base = GetModuleHandle("XINPUT1_3.dll");
        Sleep(500);
    }

    apply_function_intercepts();
}

extern "C" {
    DWORD WINAPI intercept_xinput_get_state(DWORD, XINPUT_STATE *);
}

// Called when the game attemps to call XInputGetState
DWORD WINAPI intercept_xinput_get_state(DWORD dwUserIndex, XINPUT_STATE *pState) {
    // Packet number value changes every time the state of the controller has changed
    static uint32_t packet_number[4] = { 0, 0, 0, 0 };
    static XINPUT_GAMEPAD old_state[4];
    // Call original function
    DWORD result = XInputGetState(dwUserIndex, pState);
    switch (result) {
        case ERROR_SUCCESS:
            handle_input(old_state[dwUserIndex], pState->Gamepad, (packet_number[dwUserIndex] != pState->dwPacketNumber), dwUserIndex);
            packet_number[dwUserIndex] = pState->dwPacketNumber;
            old_state[dwUserIndex] = pState->Gamepad;
            break;
        case ERROR_DEVICE_NOT_CONNECTED:
            // No controller connected for this player
            break;
        default:
            // XInputGetState failed
            break;
    } // switch (result)
    return result;
}


void handle_input(XINPUT_GAMEPAD &old, XINPUT_GAMEPAD &current, bool changed, int player) {
    static bool reset_lock_rotation = false;
    if (changed) {

        if (Button::pressed(old, current, XINPUT_GAMEPAD_DPAD_LEFT)) {
            if (Files::saves_menu_is_open()) {
                Files::save_file_index_pending_set_prev = true;
            }
        }

        if (Button::pressed(old, current, XINPUT_GAMEPAD_DPAD_RIGHT)) {
            if (Files::saves_menu_is_open()) {
                Files::save_file_index_pending_set_next = true;
            }
        }

        // Left bumper
        if (Button::pressed(old, current, XINPUT_GAMEPAD_LEFT_SHOULDER)) {
            if (Files::saves_menu_is_open()) {
                Files::save_file_index_pending_set_prev = true;
            }
        }

        // Right bumper
        if (Button::pressed(old, current, XINPUT_GAMEPAD_RIGHT_SHOULDER)) {
            if (Files::saves_menu_is_open()) {
                Files::save_file_index_pending_set_next = true;
            }
        }

        // Start
        if (Button::pressed(old, current, XINPUT_GAMEPAD_START)) {
            if (Files::saves_menu_is_open()) {
                Files::save_file_index_make_new = true;
            }
        }

        // B (Xbox) or Circle (Playstation)
        /*if (Button::pressed(old, current, XINPUT_GAMEPAD_B)) {
            if (console_is_open()) {
                execute_console_command("close");
                current.wButtons &= (~XINPUT_GAMEPAD_B);
            }
        }*/
    }
}

static const uint64_t XInput_Get_State_offset = 0x82AE70;
extern "C" {
    uint64_t XInput_Get_State_inject_return;
    void XInput_Get_State_inject();
}

// Patches game calls to XInput API funcs, redirecting them to interceptor functions
void apply_function_intercepts() {
    // XInputGetState
    //    Docs: https://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.reference.xinputgetstate(v=vs.85).aspx
    void *write_address = (uint8_t*)Game::ds1_base + XInput_Get_State_offset;
    sp::mem::code::x64::inject_jmp_14b(write_address, &XInput_Get_State_inject_return, 0, &XInput_Get_State_inject);
}


} // namespace XInput
