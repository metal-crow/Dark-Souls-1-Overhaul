/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Sean Pesce        -  C++
        Zullie The Witch  -  Original CE script
*/

#include "Challenge/BlackPhantomEnemies.h"

#define DS1_CHALLENGE_BPE_SP_EFFECT 7100
#define DS1_CHALLENGE_BPE_DRAW_TYPE 2

namespace Challenge {
namespace BlackPhantomEnemies {

bool active = false;

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
        for (int i = 0; i < (int)Params::Npc().param_count; i++) {
            original_values.push_back({ Params::Npc().get(i)->spEffectID4, Params::Npc().get(i)->drawType });
        }
    }
    if (!active) {
        print_console("Activating Black Phantom Enemies challenge mod");
        // Set black phantom draw type and special effect for all enemy NPCs
        for (int i = 0; i < (int)Params::Npc().param_count; i++) {
            if (Params::Npc().get(i)->spEffectID4 >= 7001 && Params::Npc().get(i)->spEffectID4 <= 7415) {
                Params::Npc().get(i)->spEffectID4 = DS1_CHALLENGE_BPE_SP_EFFECT;
                Params::Npc().get(i)->drawType = DS1_CHALLENGE_BPE_DRAW_TYPE;
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
            if (Params::Npc().get(i)->spEffectID4 == DS1_CHALLENGE_BPE_SP_EFFECT) {
                Params::Npc().get(i)->spEffectID4 = original_values.at(i).sp_effect_id4;
            }
            if (Params::Npc().get(i)->drawType == DS1_CHALLENGE_BPE_DRAW_TYPE) {
                Params::Npc().get(i)->drawType = original_values.at(i).draw_type;
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
#undef DS1_CHALLENGE_BPE_DRAW_TYPE
