/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Metal-Crow  -  Reverse engineering, C++

*/
#pragma once

#ifndef DS1_OVERHAUL_ANIMATION_EDITS_H_
#define DS1_OVERHAUL_ANIMATION_EDITS_H_

#include <cstdint>

typedef struct {
    int32_t animation_id;
    float duration;
    uint8_t unknown[52];
    float weight;
    float speed;
    uint32_t count;
} AnimationEntry;

class AnimationEdits
{
public:
    // Gesture cancelling enabled/disabled
    static bool gesture_cancelling;

    // Animation IDs for the default set of gesture animations in the game
    const static uint32_t gesture_anim_ids[15];

    // Enables gesture cancelling via rolling
    static void enable_gesture_cancelling();

    static void alter_animation_speeds();

    static void disable_whiff_animations();

    static void enable_cast_and_move();

private:
    static const uint64_t animation_entry_set_offset = 0x43E281; //Triggered when an animation entry in the table has it's speed set
    static const uint64_t animation_whiff_set_offset = 0x386CD6;
    //in-exe checks to prevent movement that must be moddified
    static const uint64_t walkfb_check_1 = 0x382CD6;
    static const uint64_t walkfb_check_2 = 0x3A6114;
    static const uint64_t walkfb_check_3 = 0x3A5EC0;
};

#endif
