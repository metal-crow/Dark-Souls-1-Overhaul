#include "AnimationEdits.h"
#include "GameData.h"

// Animation IDs for the default set of gesture animations in the game
const uint32_t AnimationEdits::gesture_anim_ids[15] = { 6800, 6801, 6802, 6803, 6804, 6805, 6806, 6807, 6808, 6809, 6810, 6815, 6820, 6825, 6830 };

// Gesture cancelling enabled/disabled
bool AnimationEdits::gesture_cancelling = true;

// Enables gesture cancelling via rolling
bool AnimationEdits::enable_gesture_cancelling()
{
    int gestures_changed = 0;
    if (Game::characters_loaded && Game::player_tae.is_initialized())
    {
        for (uint32_t id : gesture_anim_ids) {
            int n_events = Game::player_tae.get_event_count_by_id(id);
            for (int i = 0; i < n_events; i++) {
                bool anim_updated = false;
                if (Game::player_tae.get_event_type_by_id(id, i) == 0 &&
                    (
                        Game::player_tae.get_event_param_by_id(id, i, 0) == TAE_type0_param_values::allow_animation_cancel_events ||
                        Game::player_tae.get_event_param_by_id(id, i, 0) == TAE_type0_param_values::cancel_by_rolling_or_backstepping
                        )
                    )
                {
                    Game::player_tae.set_event_start_by_id(id, i, 0.0f);
                    //if (!print_console("Updated gesture " + std::to_string(id) + ", event " + std::to_string(i) + " to allow cancelling"))
                    //    Mod::startup_messages.push_back("Updated gesture " + std::to_string(id) + ", event " + std::to_string(i) + " to allow cancelling");
                    if (!anim_updated) {
                        anim_updated = true;
                        gestures_changed++;
                    }
                }
            }
        }
        return (gestures_changed >= 15);
    }
    else {
        return false;
    }
}
