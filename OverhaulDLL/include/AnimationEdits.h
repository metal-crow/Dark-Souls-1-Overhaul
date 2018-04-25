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
    static bool enable_gesture_cancelling();

    static void alter_animation_speeds();

    static void disable_whiff_animations();

private:
    static const uint32_t animation_entry_set_offset = 0x9929B6; //Triggered when an animation entry in the table has it's speed set
    static void __stdcall animation_entry_set_injection();
    static const uint32_t animation_whiff_set_offset = 0xA2B860;
};

#endif
