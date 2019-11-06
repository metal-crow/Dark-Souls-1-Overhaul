/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Sean Pesce        -  C++
*/


#pragma once

#ifndef DS1_OVERHAUL_INPUT_H_
    #define DS1_OVERHAUL_INPUT_H_

#include "XInput.h"
#define DIRECTINPUT_VERSION 0x800
#include <dinput.h>


//// ADDITIONAL BUTTON MACROS ////
//
// Generic
#define XINPUT_GAMEPAD_MENU_PRIMARY XINPUT_GAMEPAD_START
#define XINPUT_GAMEPAD_MENU_SECONDARY XINPUT_GAMEPAD_BACK
#define XINPUT_GAMEPAD_HOME 0x0400
#define XINPUT_GAMEPAD_LOGO XINPUT_GAMEPAD_HOME
#define XINPUT_GAMEPAD_MENU_TERTIARY XINPUT_GAMEPAD_HOME
#define XINPUT_GAMEPAD_UNKNOWN_BUTTON 0x0800
#define XINPUT_GAMEPAD_LEFT_BUMPER XINPUT_GAMEPAD_LEFT_SHOULDER
#define XINPUT_GAMEPAD_RIGHT_BUMPER XINPUT_GAMEPAD_RIGHT_SHOULDER
#define XINPUT_GAMEPAD_LEFT_JOYSTICK XINPUT_GAMEPAD_LEFT_THUMB
#define XINPUT_GAMEPAD_RIGHT_JOYSTICK XINPUT_GAMEPAD_RIGHT_THUMB
// Xbox
#define XINPUT_GAMEPAD_XBOX_BUTTON XINPUT_GAMEPAD_HOME
#define XINPUT_GAMEPAD_LB XINPUT_GAMEPAD_LEFT_SHOULDER
#define XINPUT_GAMEPAD_RB XINPUT_GAMEPAD_RIGHT_SHOULDER
// Playstation
#define XINPUT_GAMEPAD_PS_BUTTON XINPUT_GAMEPAD_HOME
#define XINPUT_GAMEPAD_PLAYSTATION_BUTTON XINPUT_GAMEPAD_HOME
#define XINPUT_GAMEPAD_SELECT XINPUT_GAMEPAD_BACK
#define XINPUT_GAMEPAD_OPTIONS XINPUT_GAMEPAD_BACK
#define XINPUT_GAMEPAD_L1 XINPUT_GAMEPAD_LEFT_SHOULDER
#define XINPUT_GAMEPAD_R1 XINPUT_GAMEPAD_RIGHT_SHOULDER
#define XINPUT_GAMEPAD_L3 XINPUT_GAMEPAD_LEFT_THUMB
#define XINPUT_GAMEPAD_R3 XINPUT_GAMEPAD_RIGHT_THUMB
#define XINPUT_GAMEPAD_X_PS XINPUT_GAMEPAD_A
#define XINPUT_GAMEPAD_X_PLAYSTATION XINPUT_GAMEPAD_A
#define XINPUT_GAMEPAD_CIRCLE XINPUT_GAMEPAD_B
#define XINPUT_GAMEPAD_SQUARE XINPUT_GAMEPAD_X
#define XINPUT_GAMEPAD_TRIANGLE XINPUT_GAMEPAD_Y

//// DIRECTINPUT BUTTON MAPPINGS ////
#define DINPUT_GAMEPAD_START        9
#define DINPUT_GAMEPAD_SELECT       8
#define DINPUT_GAMEPAD_TOUCH_BUTTON 13
#define DINPUT_GAMEPAD_L3           10
#define DINPUT_GAMEPAD_R3           11
#define DINPUT_GAMEPAD_L1           4
#define DINPUT_GAMEPAD_R1           5
#define DINPUT_GAMEPAD_CROSS        1
#define DINPUT_GAMEPAD_CIRCLE       2
#define DINPUT_GAMEPAD_SQUARE       0
#define DINPUT_GAMEPAD_TRIANGLE     3
#define DINPUT_GAMEPAD_L2           6
#define DINPUT_GAMEPAD_R2           7
#define DINPUT_GAMEPAD_DPAD_LEFT    27000
#define DINPUT_GAMEPAD_DPAD_RIGHT   9000
#define DINPUT_GAMEPAD_DPAD_UP      0
#define DINPUT_GAMEPAD_DPAD_DOWN    18000

namespace Input {


namespace Button {

enum Status {
    BUTTON_UP = 0,
    BUTTON_DOWN = 1,
    BUTTON_PRESSED = 2,
    BUTTON_RELEASED = 3
};

// Keyboard DirectInput check
inline bool pressed(uint8_t *old, uint8_t *current, uint16_t button);

// Joystick DirectInput check
inline bool pressed(DIJOYSTATE2 *old, DIJOYSTATE2 *current, uint16_t button);

// Joystick DirectInput check (POV specific)
inline bool POVpressed(DIJOYSTATE2 *old, DIJOYSTATE2 *current, uint16_t button);

// True if button was pressed between gamepad states
inline bool pressed(XINPUT_GAMEPAD *old, XINPUT_GAMEPAD *current, uint16_t button);

// True if button was released between gamepad states
inline bool released(XINPUT_GAMEPAD *old, XINPUT_GAMEPAD *current, uint16_t button);

// True if button is currently down
inline bool down(XINPUT_GAMEPAD *current, uint16_t button);

// Current status of button
inline Status status(XINPUT_GAMEPAD *old, XINPUT_GAMEPAD *current, uint16_t button);
} // namespace Button


extern void *base;

// Initializes pointers and other data used to monitor gamepad input
void initialize();

void handle_input(XINPUT_GAMEPAD* xold, XINPUT_GAMEPAD* xcurrent, DIJOYSTATE2* djold, DIJOYSTATE2* djcurrent, uint8_t* kbold, uint8_t* kbcurrent, bool changed, int player);

// Patches game calls to XInput API funcs, redirecting them to interceptor functions
void apply_function_intercepts();


} // namespace Input


#endif // DS1_OVERHAUL_INPUT_H_
