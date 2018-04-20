#include "AnimationEdits.h"
#include "DllMain.h"
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

// List of animations and their new speed ratio. Any large number (>10) does a frame 1 skip
static const std::tuple<uint32_t, float> ANIMATIONS_TO_AJUST_SPEED_RATIO[] = {
    { 6209, 10.0f },  { 6309, 10.0f },  //Firestorm startup
    { 6409, 1.6f },   { 6509, 1.6f },   //Firestorm main animation
    { 6215, 10.0f },  { 6315, 10.0f },  //Gravelord Sword Dance startup
    { 6415, 1.6f },   { 6515, 1.6f },   //Gravelord Sword Dance animation
    { 6203, 6.0f },   { 6303, 6.0f },   //Heal knealing startup
    { 6403, 1.0f },   { 6503, 1.0f },   //Heal knealing animation
    { 6220, 6.0f },   { 6320, 6.0f },   //Sunlight Heal knealing startup
    { 6420, 1.0f },   { 6520, 1.0f },   //Sunlight Heal knealing animation
    { 6218, 1.8f },   { 6318, 1.8f },   //lightning spear starting animation
    { 6518, 1.2f },   { 6518, 1.2f },   //lightning spear throwing animation
};


static uint32_t read_upper_body_aid_return;
static uint32_t read_lower_body_aid_return;

void AnimationEdits::alter_animation_speeds()
{
    if (!print_console("    Enabling animation speed alteration injection...")) {
        Mod::startup_messages.push_back("    Enabling animation speed alteration injection...");
    }

    uint8_t *write_address = (uint8_t*)(AnimationEdits::read_upper_body_aid_offset + ((uint32_t)Game::ds1_base));
    set_mem_protection(write_address, 6, MEM_PROTECT_RWX);
    inject_jmp_5b(write_address, &read_upper_body_aid_return, 1, &read_upper_body_aid_injection);

    write_address = (uint8_t*)(AnimationEdits::read_lower_body_aid_offset + ((uint32_t)Game::ds1_base));
    set_mem_protection(write_address, 6, MEM_PROTECT_RWX);
    inject_jmp_5b(write_address, &read_lower_body_aid_return, 1, &read_lower_body_aid_injection);
}

static void __stdcall read_body_aid_injection_helper_function(uint32_t animation_id, uint32_t lowerbdy) {
    //Set the animation speed change back when the animation sequence finishes (both upper and lower body are -1)
    if (
        (lowerbdy && animation_id == -1 && Game::get_player_upper_body_anim_id() == -1) ||
        (!lowerbdy && animation_id == -1 && Game::get_player_lower_body_anim_id() == -1)
        )
    {
        Game::set_current_player_animation_speed(1);
        return;
    }

    //If this is an animation to be changed, ajust speed while we're in it
    for (int i = 0; i < sizeof(ANIMATIONS_TO_AJUST_SPEED_RATIO) / sizeof(std::tuple<uint32_t, float>); i++) {
        std::tuple<uint32_t, float> ajust_aid = ANIMATIONS_TO_AJUST_SPEED_RATIO[i];
        if (animation_id == std::get<0>(ajust_aid)) {
            Game::set_current_player_animation_speed(std::get<1>(ajust_aid));
            return;
        }
    }
}

void __declspec(naked) __stdcall AnimationEdits::read_upper_body_aid_injection() {
    //"DARKSOULS.exe" + A2BEB9:
    //mov[esi + 58], ecx
    //mov edx, [eax + 04]
    //Alt: db 89 4E 58 8B 50 04
    _asm {
        push eax
        push ecx
        push edx

        push 0
        push ecx //Animation id arg
        call read_body_aid_injection_helper_function

        pop edx
        pop ecx
        pop eax
        //original code
        mov [esi + 0x58], ecx
        mov edx, [eax + 04]
        jmp read_upper_body_aid_return
    }
}

void __declspec(naked) __stdcall AnimationEdits::read_lower_body_aid_injection() {
    //"DARKSOULS.exe" + A2B2CF:
    //mov[esi + 60], ecx
    //mov edx, [eax + 04]
    //Alt: db 89 4E 60 8B 50 04
    _asm {
        push eax
        push ecx
        push edx

        push 1
        push ecx //Animation id arg
        call read_body_aid_injection_helper_function

        pop edx
        pop ecx
        pop eax
        //original code
        mov [esi + 0x60], ecx
        mov edx, [eax + 04]
        jmp read_lower_body_aid_return
    }
}
