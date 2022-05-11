/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Sean Pesce        -  C++
*/

#include "DarkSoulsOverhaulMod.h"
#include "InputUtil.h"
#include "Files.h"
#include "sp/memory/injection/asm/x64.h"
#include "ModNetworking.h"
#include "Rollback.h"

namespace Input {



namespace Button {

// Keyboard DirectInput check
inline bool pressed(uint8_t *old, uint8_t *current, uint16_t button) {
    if (old != NULL && current != NULL) {
        return (!old[button] && current[button]);
    }
    return false;
}

// Joystick DirectInput check
inline bool pressed(DIJOYSTATE2 *old, DIJOYSTATE2 *current, uint16_t button) {
    if (old != NULL && current != NULL) {
        return (!old->rgbButtons[button] && current->rgbButtons[button]);
    }
    return false;
}

// Joystick DirectInput check (POV specific)
inline bool POVpressed(DIJOYSTATE2 *old, DIJOYSTATE2 *current, uint16_t button) {
    if (old != NULL && current != NULL) {
        return ((old->rgdwPOV[0] != button) && (current->rgdwPOV[0] == button));
    }
    return false;
}

// True if button was pressed between gamepad states
inline bool pressed(XINPUT_GAMEPAD *old, XINPUT_GAMEPAD *current, uint16_t button) {
    if (old != NULL && current != NULL) {
        return (!(old->wButtons & button) && (current->wButtons & button));
    }
    return false;
}
// True if button was released between gamepad states
inline bool released(XINPUT_GAMEPAD *old, XINPUT_GAMEPAD *current, uint16_t button) {
    if (old != NULL && current != NULL) {
        return ((old->wButtons & button) && !(current->wButtons & button));
    }
    return false;
}
// True if button is currently down
inline bool down(XINPUT_GAMEPAD *current, uint16_t button) {
    if (current != NULL) {
        return ((current->wButtons & button) != 0);
    }
    return false;
}
// Current status of button
inline Status status(XINPUT_GAMEPAD *old, XINPUT_GAMEPAD *current, uint16_t button) {
    if (old != NULL && current != NULL) {
        if (pressed(old, current, button)) {
            return BUTTON_PRESSED;
        }
        else if (down(current, button)) {
            return BUTTON_DOWN;
        }
        else if (released(old, current, button)) {
            return BUTTON_RELEASED;
        }
        else {
            return BUTTON_UP;
        }
    }
    return BUTTON_UP;
}

} // namespace Button


static const uint64_t XInput_Get_State_offset = 0x82AE70;
static const uint64_t IDirectInputDevice8GetDeviceState_Keyboard_offset = 0xe27300;
static const uint64_t IDirectInputDevice8GetDeviceState_DIJOYSTATE2_offset = 0xc9ad8e;

extern "C" {
    uint64_t XInput_Get_State_inject_return;
    void XInput_Get_State_inject();
    DWORD WINAPI intercept_xinput_get_state(DWORD, XINPUT_STATE *);

    uint64_t IDirectInputDevice8GetDeviceState_Keyboard_injection_return;
    void IDirectInputDevice8GetDeviceState_Keyboard_injection();
    void intercept_IDirectInputDevice8GetDeviceState_Keyboard(LPVOID);

    uint64_t IDirectInputDevice8GetDeviceState_DIJOYSTATE2_injection_return;
    void IDirectInputDevice8GetDeviceState_DIJOYSTATE2_injection();
    void intercept_IDirectInputDevice8GetDeviceState_DIJOYSTATE2(LPVOID);
}

// Initializes pointers and other data used to monitor gamepad input
void initialize() {
    ConsoleWrite("Initializing Input hooks...");

    // Patches game calls to XInput and DirectInput API funcs, redirecting them to interceptor functions

    // XInputGetState
    //    Docs: https://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.reference.xinputgetstate(v=vs.85).aspx
    void *write_address = (uint8_t*)Game::ds1_base + XInput_Get_State_offset;
    sp::mem::code::x64::inject_jmp_14b(write_address, &XInput_Get_State_inject_return, 0, &XInput_Get_State_inject);

    // IDirectInputDevice8GetDeviceState call for the keyboard
    write_address = (uint8_t*)Game::ds1_base + IDirectInputDevice8GetDeviceState_Keyboard_offset;
    sp::mem::code::x64::inject_jmp_14b(write_address, &IDirectInputDevice8GetDeviceState_Keyboard_injection_return, 0, &IDirectInputDevice8GetDeviceState_Keyboard_injection);

    // IDirectInputDevice8GetDeviceState call for the joystick
    write_address = (uint8_t*)Game::ds1_base + IDirectInputDevice8GetDeviceState_DIJOYSTATE2_offset;
    sp::mem::code::x64::inject_jmp_14b(write_address, &IDirectInputDevice8GetDeviceState_DIJOYSTATE2_injection_return, 3, &IDirectInputDevice8GetDeviceState_DIJOYSTATE2_injection);
}

static uint8_t keyboard_old_state[256];

void intercept_IDirectInputDevice8GetDeviceState_Keyboard(LPVOID lpvData)
{
    uint8_t* data = (uint8_t*)lpvData;
    handle_input(NULL, NULL, NULL, NULL, keyboard_old_state, data, true, 0);
    memcpy(keyboard_old_state, data, 256);
}

static DIJOYSTATE2 DIJOYSTATE2_old_state;

void intercept_IDirectInputDevice8GetDeviceState_DIJOYSTATE2(LPVOID lpvData)
{
    DIJOYSTATE2 * data = (DIJOYSTATE2*)lpvData;
    handle_input(NULL, NULL, &DIJOYSTATE2_old_state, data, NULL, NULL, true, 0);
    DIJOYSTATE2_old_state = *data;
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
            handle_input(&old_state[dwUserIndex], &pState->Gamepad, NULL, NULL, NULL, NULL, (packet_number[dwUserIndex] != pState->dwPacketNumber), dwUserIndex);
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

void handle_input(XINPUT_GAMEPAD* xold, XINPUT_GAMEPAD* xcurrent, DIJOYSTATE2* djold, DIJOYSTATE2* djcurrent, uint8_t* kbold, uint8_t* kbcurrent, bool changed, int player) {
    static bool reset_lock_rotation = false;
    if (changed)
    {

        if (Button::pressed(xold, xcurrent, XINPUT_GAMEPAD_DPAD_LEFT) ||
            Button::POVpressed(djold, djcurrent, DINPUT_GAMEPAD_DPAD_LEFT) ||
            Button::pressed(kbold, kbcurrent, DIK_LEFT))
        {
            if (Files::saves_menu_is_open())
            {
                Files::save_file_index_pending_set_prev = true;
            }
        }

        if (Button::pressed(xold, xcurrent, XINPUT_GAMEPAD_DPAD_RIGHT) ||
            Button::POVpressed(djold, djcurrent, DINPUT_GAMEPAD_DPAD_RIGHT) ||
            Button::pressed(kbold, kbcurrent, DIK_RIGHT))
        {
            if (Files::saves_menu_is_open())
            {
                Files::save_file_index_pending_set_next = true;
            }
        }

        // Left bumper
        if (Button::pressed(xold, xcurrent, XINPUT_GAMEPAD_LEFT_SHOULDER) ||
            Button::pressed(djold, djcurrent, DINPUT_GAMEPAD_L2))
        {
            if (Files::saves_menu_is_open())
            {
                Files::save_file_index_pending_set_prev = true;
            }
        }

        // Right bumper
        if (Button::pressed(xold, xcurrent, XINPUT_GAMEPAD_RIGHT_SHOULDER) ||
            Button::pressed(djold, djcurrent, DINPUT_GAMEPAD_R2))
        {
            if (Files::saves_menu_is_open())
            {
                Files::save_file_index_pending_set_next = true;
            }
        }

        // Start
        if (Button::pressed(xold, xcurrent, XINPUT_GAMEPAD_START) ||
            Button::pressed(djold, djcurrent, DINPUT_GAMEPAD_START) ||
            Button::pressed(kbold, kbcurrent, DIK_ESCAPE))
        {
            if (Files::saves_menu_is_open())
            {
                Files::save_file_index_make_new = true;
            }
        }

        // --------------------------
        // TEMP: change the game mode and connection settings via button press
        // --------------------------
        if (Button::pressed(kbold, kbcurrent, user_setting<uint8_t>(_DS1_OVERHAUL_KEYBINDS_SECTION_, _DS1_OVERHAUL_KEYBIND_NONMOD_HOST_CONNECT_, DIK_F1).read()))
        {
            ModNetworking::allow_connect_with_non_mod_host = !ModNetworking::allow_connect_with_non_mod_host;
            ConsoleWrite("Set allow_connect_with_non_mod_host=%d", ModNetworking::allow_connect_with_non_mod_host);
        }
        if (Button::pressed(kbold, kbcurrent, user_setting<uint8_t>(_DS1_OVERHAUL_KEYBINDS_SECTION_, _DS1_OVERHAUL_KEYBIND_LEGACYMODE_HOST_CONNECT_, DIK_F2).read()))
        {
            ModNetworking::allow_connect_with_legacy_mod_host = !ModNetworking::allow_connect_with_legacy_mod_host;
            ConsoleWrite("Set allow_connect_with_legacy_mod_host=%d", ModNetworking::allow_connect_with_legacy_mod_host);
        }
        if (Button::pressed(kbold, kbcurrent, user_setting<uint8_t>(_DS1_OVERHAUL_KEYBINDS_SECTION_, _DS1_OVERHAUL_KEYBIND_OVERHAULMODE_HOST_CONNECT_, DIK_F3).read()))
        {
            ModNetworking::allow_connect_with_overhaul_mod_host = !ModNetworking::allow_connect_with_overhaul_mod_host;
            ConsoleWrite("Set allow_connect_with_overhaul_mod_host=%d", ModNetworking::allow_connect_with_overhaul_mod_host);
        }
        if (Button::pressed(kbold, kbcurrent, user_setting<uint8_t>(_DS1_OVERHAUL_KEYBINDS_SECTION_, _DS1_OVERHAUL_KEYBIND_NONMOD_GUEST_CONNECT_, DIK_F4).read()))
        {
            ModNetworking::allow_connect_with_non_mod_guest = !ModNetworking::allow_connect_with_non_mod_guest;
            ConsoleWrite("Set allow_connect_with_non_mod_guest=%d", ModNetworking::allow_connect_with_non_mod_guest);
        }
        if (Button::pressed(kbold, kbcurrent, user_setting<uint8_t>(_DS1_OVERHAUL_KEYBINDS_SECTION_, _DS1_OVERHAUL_KEYBIND_TOGGLEMODE_, DIK_F5).read()))
        {
            if (Mod::user_selected_default_mode == ModMode::Legacy)
            {
                Mod::user_selected_default_mode = ModMode::Overhaul;
                ConsoleWrite("Set prefer Overhaul mode");
            }
            else if (Mod::user_selected_default_mode == ModMode::Overhaul)
            {
                Mod::user_selected_default_mode = ModMode::Legacy;
                ConsoleWrite("Set prefer Legacy mode");
            }
        }

        if (Button::pressed(kbold, kbcurrent, DIK_F8))
        {
            Rollback::bsave = true;
        }
        if (Button::pressed(kbold, kbcurrent, DIK_F9))
        {
            Rollback::bload = true;
        }
    }
}

} // namespace Input
