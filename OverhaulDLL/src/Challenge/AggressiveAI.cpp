/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        RavagerChris  -  Aggressive AI technique
        Sean Pesce    -  C++
*/

#include "Challenge/AggressiveAI.h"

namespace Challenge {
namespace AggressiveAi {

bool is_active = false;

// Stores original special effects and draw types for all NPCs affected by this challenge mod
std::vector<OriginalAiData>         original_ai_values          = std::vector<OriginalAiData>();
std::vector<OriginalNpcThoughtData> original_npc_thought_values = std::vector<OriginalNpcThoughtData>();

// Default NPC detection sense values when Aggressive AI mod is enabled
const uint16_t DEFAULT_EYE_DISTANCE  = 65535; // Max = 65535
const uint16_t DEFAULT_EAR_DISTANCE  = 10;    // Max = 65535
const uint16_t DEFAULT_NOSE_DISTANCE = 10;    // Max = 65535

// NPC detection sense values when Aggressive AI mod is enabled
uint16_t eye_distance  = DEFAULT_EYE_DISTANCE;
uint16_t ear_distance  = DEFAULT_EAR_DISTANCE;
uint16_t nose_distance = DEFAULT_NOSE_DISTANCE;

// Returns whether the Aggressive AI challenge mod is currently enabled
bool active()
{
    return is_active;
}

// Activates aggressive AI (increased aggro range)
//     NOTE: Params::AiStandardInfo().init() and Params::NpcThink().init() must be called before this mod can be enabled
void enable()
{
    if (Params::AiStandardInfo().base == NULL) {
        // AI parameters not loaded yet
        print_console("ERROR: Failed to enable Aggressive AI (wait for AI params to load)");
        SetLastError(ERROR_FILE_NOT_FOUND);
        return;
    }
    if (Params::NpcThink().base == NULL) {
        // NPC thought parameters not loaded yet
        print_console("ERROR: Failed to enable Aggressive AI (wait for NPC thought params to load)");
        SetLastError(ERROR_FILE_NOT_FOUND);
        return;
    }
    if (original_ai_values.empty()) {
        // First time enabling; store unmodified param values
        // @TODO: What if other mods are present and active? Incorrect values could be stored. Maybe read original values from game files instead
        OriginalAiData original_ai_params;
        for (int i = 0; i < (int)Params::AiStandardInfo().param_count; i++) {
            original_ai_params.radar_range = Params::AiStandardInfo().get(i)->RadarRange;
            original_ai_params.radar_angle_x = Params::AiStandardInfo().get(i)->RadarAngleX;
            original_ai_params.radar_angle_y = Params::AiStandardInfo().get(i)->RadarAngleY;
            original_ai_params.territory_size = Params::AiStandardInfo().get(i)->TerritorySize;
            original_ai_params.changed = false;
            original_ai_values.push_back(original_ai_params);
        }
    }
    if (original_npc_thought_values.empty()) {
        OriginalNpcThoughtData original_npc_thought_params;
        for (int i = 0; i < (int)Params::NpcThink().param_count; i++) {
            //original_npc_thought_params.out_distance = Params::NpcThink().get(i)->outDist;
            original_npc_thought_params.back_home_life_on_hit_enemy_wall = Params::NpcThink().get(i)->BackHomeLife_OnHitEneWal;
            original_npc_thought_params.eye_distance = Params::NpcThink().get(i)->eye_dist;
            original_npc_thought_params.ear_distance = Params::NpcThink().get(i)->ear_dist;
            //original_npc_thought_params.ear_soundcut_distance = Params::NpcThink().get(i)->ear_soundcut_dist;
            original_npc_thought_params.nose_distance = Params::NpcThink().get(i)->nose_dist;
            original_npc_thought_params.max_back_home_distance = Params::NpcThink().get(i)->maxBackhomeDist;
            original_npc_thought_params.back_home_distance = Params::NpcThink().get(i)->backhomeDist;
            original_npc_thought_params.back_home_battle_distance = Params::NpcThink().get(i)->backhomeBattleDist;
            original_npc_thought_params.battle_start_distance = Params::NpcThink().get(i)->BattleStartDist;
            original_npc_thought_params.eye_angle_x = Params::NpcThink().get(i)->eye_angX;
            original_npc_thought_params.eye_angle_y = Params::NpcThink().get(i)->eye_angY;
            original_npc_thought_params.ear_angle_x = Params::NpcThink().get(i)->ear_angX;
            original_npc_thought_params.ear_angle_y = Params::NpcThink().get(i)->ear_angY;
            original_npc_thought_params.changed = false;
            original_npc_thought_values.push_back(original_npc_thought_params);
        }
    }
    if (!is_active) {
        print_console("Activating Aggressive AI challenge mod");
        // Set black phantom draw type and special effect for all enemy NPCs
        for (int i = 0; i < (int)Params::AiStandardInfo().param_count; i++) {
            Params::AiStandardInfo().get(i)->RadarRange  = 30000; // Max = 30000
            Params::AiStandardInfo().get(i)->RadarAngleX = 90;  // Max = 90
            Params::AiStandardInfo().get(i)->RadarAngleY = 180; // Max = 180
            Params::AiStandardInfo().get(i)->TerritorySize = 30000; // Max = 30000
            original_ai_values.at(i).changed = true;
        }
        for (int i = 0; i < (int)Params::NpcThink().param_count; i++) {
            //Params::NpcThink().get(i)->BackHomeLife_OnHitEneWal = ; // Max = 999
            Params::NpcThink().get(i)->eye_dist = eye_distance; // Max = 65535
            if (Params::NpcThink().get(i)->ear_dist < ear_distance) {
                Params::NpcThink().get(i)->ear_dist = ear_distance; // Max = 65535
            }
            if (Params::NpcThink().get(i)->nose_dist < nose_distance) {
                Params::NpcThink().get(i)->nose_dist = nose_distance; // Max = 65535
            }
            Params::NpcThink().get(i)->maxBackhomeDist = 30000; // Max = 65535
            Params::NpcThink().get(i)->backhomeDist = 30000; // Max = 65535
            Params::NpcThink().get(i)->backhomeBattleDist = 30000; // Max = 65535
            Params::NpcThink().get(i)->BattleStartDist = 30000; // Max = 65534
            Params::NpcThink().get(i)->eye_angX = 180; // Max = 180
            Params::NpcThink().get(i)->eye_angY = 180; // Max = 180
            Params::NpcThink().get(i)->ear_angX = 90;  // Max = 90
            Params::NpcThink().get(i)->ear_angY = 180; // Max = 180
            original_npc_thought_values.at(i).changed = true;
        }
        is_active = true;
    } else {
        SetLastError(ERROR_BAD_ENVIRONMENT);
    }
}

// Restores visual effects and stats to all NPCs affected by this challenge mod
void disable()
{
    if (is_active) {
        print_console("Deactivating Aggressive AI challenge mod");
        for (int i = 0; i < (int)Params::AiStandardInfo().param_count; i++) {
            // Restore original NPC param data
            if (original_ai_values.at(i).changed) {
                Params::AiStandardInfo().get(i)->RadarRange = original_ai_values.at(i).radar_range;
                Params::AiStandardInfo().get(i)->RadarAngleX = original_ai_values.at(i).radar_angle_x;
                Params::AiStandardInfo().get(i)->RadarAngleY = original_ai_values.at(i).radar_angle_y;
                Params::AiStandardInfo().get(i)->TerritorySize = original_ai_values.at(i).territory_size;
                original_ai_values.at(i).changed = false;
            }
        }
        for (int i = 0; i < (int)Params::NpcThink().param_count; i++) {
            if (original_npc_thought_values.at(i).changed) {
                //Params::NpcThink().get(i)->outDist = original_npc_thought_values.at(i).out_distance;
                Params::NpcThink().get(i)->BackHomeLife_OnHitEneWal = original_npc_thought_values.at(i).back_home_life_on_hit_enemy_wall;
                Params::NpcThink().get(i)->eye_dist = original_npc_thought_values.at(i).eye_distance;
                Params::NpcThink().get(i)->ear_dist = original_npc_thought_values.at(i).ear_distance;
                //Params::NpcThink().get(i)->ear_soundcut_dist = original_npc_thought_values.at(i).ear_soundcut_distance;
                Params::NpcThink().get(i)->nose_dist = original_npc_thought_values.at(i).nose_distance;
                Params::NpcThink().get(i)->maxBackhomeDist = original_npc_thought_values.at(i).max_back_home_distance;
                Params::NpcThink().get(i)->backhomeDist = original_npc_thought_values.at(i).back_home_distance;
                Params::NpcThink().get(i)->backhomeBattleDist = original_npc_thought_values.at(i).back_home_battle_distance;
                Params::NpcThink().get(i)->BattleStartDist = original_npc_thought_values.at(i).battle_start_distance;
                Params::NpcThink().get(i)->eye_angX = original_npc_thought_values.at(i).eye_angle_x;
                Params::NpcThink().get(i)->eye_angY = original_npc_thought_values.at(i).eye_angle_y;
                Params::NpcThink().get(i)->ear_angX = original_npc_thought_values.at(i).ear_angle_x;
                Params::NpcThink().get(i)->ear_angY = original_npc_thought_values.at(i).ear_angle_y;
                original_npc_thought_values.at(i).changed = false;
            }
        }
        is_active = false;
    } else {
        SetLastError(ERROR_BAD_ENVIRONMENT);
    }
}


// Toggles Aggressive AI challenge mod
void toggle()
{
    if (is_active) {
        disable();
    } else {
        enable();
    }
}


} // namespace AggressiveAi
} // namespace Challenge

