/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Sean Pesce        -  C++
        Zullie The Witch  -  Original CE script
*/

#include "Challenge/BlackPhantomEnemies.h"

#define DS1_CHALLENGE_BPE_SP_EFFECT 7100

namespace Challenge {
namespace BlackPhantomEnemies {

bool active = false;
uint8_t DRAW_TYPE = DRAW_TYPE_DEFAULT;

// Stores original special effects and draw types for all NPCs affected by this challenge mod
std::vector<OriginalNpcData> original_values = std::vector<OriginalNpcData>();

// Applies black phantom visual effects and stat buffs to all enemy NPCs
//     NOTE: Params::Npc().init() must be called before this mod can be enabled
void enable()
{
    if (Params::Npc().base == NULL) {
        // NPC parameters not loaded yet
        print_console("ERROR: Failed to enable Black Phantom Enemies (wait for NPC params to load)");
        SetLastError(ERROR_FILE_NOT_FOUND);
        return;
    }
    if (original_values.empty()) {
        // First time enabling; store unmodified NPC param values
        // @TODO: What if other mods are present and active? Incorrect values could be stored. Maybe read original values from game files instead
        OriginalNpcData original_params;
        for (int i = 0; i < (int)Params::Npc().param_count; i++) {
            original_params.sp_effect_id0 = Params::Npc().get(i)->spEffectID0;
            original_params.sp_effect_id1 = Params::Npc().get(i)->spEffectID1;
            original_params.sp_effect_id2 = Params::Npc().get(i)->spEffectID2;
            original_params.sp_effect_id3 = Params::Npc().get(i)->spEffectID3;
            original_params.sp_effect_id4 = Params::Npc().get(i)->spEffectID4;
            original_params.sp_effect_id5 = Params::Npc().get(i)->spEffectID5;
            original_params.sp_effect_id6 = Params::Npc().get(i)->spEffectID6;
            original_params.sp_effect_id7 = Params::Npc().get(i)->spEffectID7;
            original_params.draw_type = Params::Npc().get(i)->drawType;
            original_params.changed = false;
            original_values.push_back(original_params);
        }
    }
    if (!active) {
        print_console("Activating Black Phantom Enemies challenge mod");
        // Set black phantom draw type and special effect for all enemy NPCs
        for (int i = 0; i < (int)Params::Npc().param_count; i++) {
            if (Params::Npc().get(i)->npcType != 2
                    && Params::Npc().get(i)->npcType != 8
                    && Params::Npc().get(i)->teamType != 2
                    && Params::Npc().get(i)->teamType != 8
                    && Params::Npc().get(i)->teamType != 7
                    && (Params::Npc().get(i)->npcType == 0
                    || Params::Npc().get(i)->npcType  == 1
                    || Params::Npc().get(i)->teamType == 0
                    || Params::Npc().get(i)->teamType == 1
                    || Params::Npc().get(i)->teamType == 6
                    || Params::Npc().get(i)->teamType == 9
                    // Values below may be unnecessary
                    || Params::Npc().get(i)->teamType == 10
                    || Params::Npc().get(i)->teamType == 11
                    || Params::Npc().get(i)->teamType == 13)) {
                // Find unused Special Effect ID slot for this NPC and apply stat buff
                if (Params::Npc().get(i)->spEffectID5 == 0) {
                    Params::Npc().get(i)->spEffectID5 = DS1_CHALLENGE_BPE_SP_EFFECT;
                } else if (Params::Npc().get(i)->spEffectID6 == 0) {
                    Params::Npc().get(i)->spEffectID6 = DS1_CHALLENGE_BPE_SP_EFFECT;
                } else if (Params::Npc().get(i)->spEffectID4 == 0) {
                    Params::Npc().get(i)->spEffectID4 = DS1_CHALLENGE_BPE_SP_EFFECT;
                } else if (Params::Npc().get(i)->spEffectID2 == 0) {
                    Params::Npc().get(i)->spEffectID2 = DS1_CHALLENGE_BPE_SP_EFFECT;
                } else if (Params::Npc().get(i)->spEffectID1 == 0) {
                    Params::Npc().get(i)->spEffectID1 = DS1_CHALLENGE_BPE_SP_EFFECT;
                } else {
                    // No unused SpEffect ID slot for this NPC
                    //print_console("Warning: NPC at index " + std::to_string(i) + " has no unused SpEffect ID slots");
                }
                // Apply phantom draw type
                Params::Npc().get(i)->drawType = DRAW_TYPE;
                original_values.at(i).changed = true;
            }
        }
        active = true;
    } else {
        SetLastError(ERROR_BAD_ENVIRONMENT);
    }
}

// Restores visual effects and stats to all NPCs affected by this challenge mod
void disable()
{
    if (active) {
        print_console("Deactivating Black Phantom Enemies challenge mod");
        for (int i = 0; i < (int)Params::Npc().param_count; i++) {
            // Restore original NPC param data
            if (original_values.at(i).changed) {
                if (Params::Npc().get(i)->spEffectID0 == DS1_CHALLENGE_BPE_SP_EFFECT) {
                    Params::Npc().get(i)->spEffectID0 = original_values.at(i).sp_effect_id0;
                }
                if (Params::Npc().get(i)->spEffectID1 == DS1_CHALLENGE_BPE_SP_EFFECT) {
                    Params::Npc().get(i)->spEffectID1 = original_values.at(i).sp_effect_id1;
                }
                if (Params::Npc().get(i)->spEffectID2 == DS1_CHALLENGE_BPE_SP_EFFECT) {
                    Params::Npc().get(i)->spEffectID2 = original_values.at(i).sp_effect_id2;
                }
                if (Params::Npc().get(i)->spEffectID3 == DS1_CHALLENGE_BPE_SP_EFFECT) {
                    Params::Npc().get(i)->spEffectID3 = original_values.at(i).sp_effect_id3;
                }
                if (Params::Npc().get(i)->spEffectID4 == DS1_CHALLENGE_BPE_SP_EFFECT) {
                    Params::Npc().get(i)->spEffectID4 = original_values.at(i).sp_effect_id4;
                }
                if (Params::Npc().get(i)->spEffectID5 == DS1_CHALLENGE_BPE_SP_EFFECT) {
                    Params::Npc().get(i)->spEffectID5 = original_values.at(i).sp_effect_id5;
                }
                if (Params::Npc().get(i)->spEffectID6 == DS1_CHALLENGE_BPE_SP_EFFECT) {
                    Params::Npc().get(i)->spEffectID6 = original_values.at(i).sp_effect_id6;
                }
                if (Params::Npc().get(i)->spEffectID7 == DS1_CHALLENGE_BPE_SP_EFFECT) {
                    Params::Npc().get(i)->spEffectID7 = original_values.at(i).sp_effect_id7;
                }
                if (Params::Npc().get(i)->drawType == DRAW_TYPE) {
                    Params::Npc().get(i)->drawType = original_values.at(i).draw_type;
                }
                original_values.at(i).changed = false;
            }
        }
        active = false;
    } else {
        SetLastError(ERROR_BAD_ENVIRONMENT);
    }
}


// Toggles BlackPhantomEnemies challenge mod
void toggle()
{
    if (active) {
        disable();
    } else {
        enable();
    }
}


} // namespace BlackPhantomEnemies
} // namespace Challenge

#undef DS1_CHALLENGE_BPE_SP_EFFECT
