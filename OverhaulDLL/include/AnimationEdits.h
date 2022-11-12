/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Metal-Crow  -  Reverse engineering, C++

*/
#pragma once

#ifndef DS1_OVERHAUL_ANIMATION_EDITS_H_
#define DS1_OVERHAUL_ANIMATION_EDITS_H_

#include <cstdint>
#include <unordered_map>
#include "GameData.h"

typedef struct {
    int32_t animation_id;
    float duration;
    uint8_t unknown[52];
    float weight;
    float speed;
    uint32_t count;
} AnimationEntry;

typedef struct
{
    void* animationMediatorPtr;
    uint16_t animationState;
    uint64_t timeAnimationTriggered;
    uint32_t frameStart;
} SetAnimationTimeOffsetArg;

class AnimationEdits
{
public:
    // Animation IDs for the default set of gesture animations in the game
    const static uint32_t gesture_anim_ids[15];

    static void start();

private:
    static void alter_animation_parameters();

    static void disable_whiff_animations();

private:
    static const uint64_t animation_entry_set_offset = 0x43ed81; //Triggered when an animation entry in the table has it's speed set
    static const uint64_t animation_whiff_set_offset = 0x386384;
};

#endif
