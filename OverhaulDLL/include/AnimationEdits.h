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

    static void start();

private:
    // Enables gesture cancelling via rolling
    static void enable_gesture_cancelling();

    static void alter_animation_speeds();

    static void disable_whiff_animations();

    static void fix_curvedsword_infinites();

    static void fix_roll_distance();

private:
    static const uint64_t animation_entry_set_offset = 0x43E281; //Triggered when an animation entry in the table has it's speed set
    static const uint64_t animation_whiff_set_offset = 0x386CD6;
    static const uint64_t TAE_GetDamageRate_StunLen_finish_offset = 0x2dd71f;
    static const uint64_t Calculate_movement_delta_offset = 0x37a22a;
};

#endif
