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
};

#endif
