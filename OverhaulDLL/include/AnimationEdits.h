#pragma once

#ifndef DS1_OVERHAUL_ANIMATION_EDITS_H_
#define DS1_OVERHAUL_ANIMATION_EDITS_H_

#include <cstdint>

class AnimationEdits
{
public:
    // Gesture cancelling enabled/disabled
    static bool gesture_cancelling;

    // Animation IDs for the default set of gesture animations in the game
    const static uint32_t gesture_anim_ids[15];

    // Enables gesture cancelling via rolling
    static bool enable_gesture_cancelling();

    static void alter_animation_speeds();

private:
    static const uint32_t read_upper_body_aid_offset = 0xA2BEB9;
    static void __stdcall read_upper_body_aid_injection();
    static const uint32_t read_lower_body_aid_offset = 0xA2B2CF;
    static void __stdcall read_lower_body_aid_injection();
};

#endif
