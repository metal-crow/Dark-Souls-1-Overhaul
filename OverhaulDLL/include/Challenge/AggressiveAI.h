/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        RavagerChris  -  Aggressive AI technique
        Sean Pesce    -  C++
*/


#pragma once

#ifndef DS1_CHALLENGE_AGGRESSIVE_AI_H_
    #define DS1_CHALLENGE_AGGRESSIVE_AI_H_

#include <vector>
#include "Param/Params.h"

namespace Challenge {
namespace AggressiveAi {

// Stores original AI data for an NPC affected by this challenge mod
struct OriginalAiData {
    int16_t radar_range;
    int8_t  radar_angle_x;
    int8_t  radar_angle_y;
    int16_t territory_size;
    bool changed = false;
};

// Stores original NPC thought data for an NPC affected by this challenge mod
struct OriginalNpcThoughtData {
    //float   out_distance;
    float   back_home_life_on_hit_enemy_wall;
    int16_t eye_distance;
    int16_t ear_distance;
    //int16_t ear_soundcut_distance;
    int16_t nose_distance;
    int16_t max_back_home_distance;
    int16_t back_home_distance;
    int16_t back_home_battle_distance;
    int16_t battle_start_distance;
    int8_t  eye_angle_x;
    int8_t  eye_angle_y;
    int8_t  ear_angle_x;
    int8_t  ear_angle_y;
    bool changed = false;
};

// Stores original parameter data for all NPCs affected by this challenge mod
extern std::vector<OriginalAiData> original_ai_values;
extern std::vector<OriginalNpcThoughtData> original_npc_thought_values;

// Default NPC detection sense values when Aggressive AI mod is enabled
extern const uint16_t DEFAULT_EYE_DISTANCE;
extern const uint16_t DEFAULT_EAR_DISTANCE;
extern const uint16_t DEFAULT_NOSE_DISTANCE;

// NPC detection sense values when Aggressive AI mod is enabled
extern uint16_t eye_distance;
extern uint16_t ear_distance;
extern uint16_t nose_distance;

// Returns whether the Aggressive AI challenge mod is currently enabled
bool active();

// Activates Aggressive AI
void enable();

// Restores normal AI
void disable();

// Toggles Aggressive AI challenge mod
void toggle();


} // namespace AggressiveAi
} // namespace Challenge

#endif // DS1_CHALLENGE_AGGRESSIVE_AI_H_