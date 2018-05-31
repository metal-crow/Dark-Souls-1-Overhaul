/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Sean Pesce        -  C++
*/

#include "DllMain_Legacy.h"
#include "XInputUtil.h"


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
XInputGetStateFunc XInputGetState_original = NULL;

// Initializes pointers and other data used to monitor gamepad input
void initialize() {
    base = NULL;
    while (base == NULL) {
        // Wait for XInput DLL to load, then save base address
        base = GetModuleHandle("XINPUT1_3.dll");
        Sleep(500);
    }
    if (!print_console(Mod::output_prefix + "Initializing XInput hook..."))
        Mod::startup_messages.push_back(Mod::output_prefix + "Initializing XInput hook...");
    apply_function_intercepts();
}


// Returns pointer to the gamepad state data structure
inline XINPUT_STATE *get_state() {
    SpPointer state = SpPointer((void*)((uint32_t)base + 0x10C44), { 0x0, 0x24 });
    return (XINPUT_STATE*)state.resolve();
}


// Called when the game attemps to call XInputGetState
DWORD WINAPI intercept_xinput_get_state(DWORD dwUserIndex, XINPUT_STATE *pState) {
    // Packet number value changes every time the state of the controller has changed
    static uint32_t packet_number[4] = { 0, 0, 0, 0 };
    static XINPUT_GAMEPAD old_state[4];
    // Call original function
    DWORD result = XInputGetState_original(dwUserIndex, pState);
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


// 
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

        // B (Xbox) or Circle (Playstation)
        if (Button::pressed(old, current, XINPUT_GAMEPAD_B)) {
            if (console_is_open()) {
                execute_console_command("close");
                current.wButtons &= (~XINPUT_GAMEPAD_B);
            }
        }
        

        // Test code for omni-directional rolling while locked:
        //if (Button::pressed(old, current, XINPUT_GAMEPAD_B)) {
        //    print_console("Player " + std::to_string(player) + " pressed B");
        //    if (Game::player_is_locked_on()) {
        //        print_console("Locked on? Yes");
        //        Game::allow_rotation_when_locked_on(true);
        //        reset_lock_rotation = true;
        //    } else {
        //        print_console("Locked on? No");
        //    }
        //}
        //// Player dodge animation started; fix body rotation
        //int32_t player_body_anim = Game::get_player_body_anim_id();
        //if (reset_lock_rotation && player_body_anim >= 680 && player_body_anim <= 738) {
        //    Game::allow_rotation_when_locked_on(false);
        //    reset_lock_rotation = false;
        //}
        
    } // if(changed)
}


// Patches game calls to XInput API funcs, redirecting them to interceptor functions
void apply_function_intercepts() {
    // XInputGetState
    //    Docs: https://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.reference.xinputgetstate(v=vs.85).aspx
    static uint32_t xinput_get_state_func = (uint32_t)&intercept_xinput_get_state;
    static uint32_t *xinput_get_state_func_ptr = &xinput_get_state_func;
    uint8_t *xinput_get_state_func_b = (uint8_t *)&xinput_get_state_func_ptr;
    uint8_t patch_xinput_get_state[4] = { xinput_get_state_func_b[0], xinput_get_state_func_b[1], xinput_get_state_func_b[2], xinput_get_state_func_b[3] };
    void *write_address = (uint8_t*)Game::ds1_base + 0x6BA544;
    XInputGetState_original = *(XInputGetStateFunc*)*(void**)write_address;
    apply_byte_patch(write_address, patch_xinput_get_state, 4);
}


} // namespace XInput
