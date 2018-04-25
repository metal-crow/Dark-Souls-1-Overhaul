#include "AnimationEdits.h"
#include "DllMain.h"
#include "GameData.h"
#include <unordered_map>

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
static const std::unordered_map<int32_t, float> ANIMATIONS_TO_AJUST_SPEED_RATIO = {
    { 6209, 10.0f },  { 6309, 10.0f },  //Firestorm startup
    { 6409, 1.6f },   { 6509, 1.6f },   //Firestorm main animation
    { 6215, 10.0f },  { 6315, 10.0f },  //Gravelord Sword Dance startup
    { 6415, 1.6f },   { 6515, 1.6f },   //Gravelord Sword Dance animation
    { 6203, 6.0f },   { 6303, 6.0f },   //Heal knealing startup
    { 6403, 1.0f },   { 6503, 1.0f },   //Heal knealing animation
    { 6220, 6.0f },   { 6320, 6.0f },   //Sunlight Heal knealing startup
    { 6420, 1.0f },   { 6520, 1.0f },   //Sunlight Heal knealing animation
    { 6218, 10.0f },  { 6318, 10.0f },  //lightning spear starting animation
    { 6418, 1.2f },   { 6518, 1.2f },   //lightning spear throwing animation
};


static uint32_t animation_entry_set_return;

void AnimationEdits::alter_animation_speeds()
{
    if (!print_console("    Enabling animation speed alteration injection...")) {
        Mod::startup_messages.push_back("    Enabling animation speed alteration injection...");
    }

    uint8_t *write_address = (uint8_t*)(AnimationEdits::animation_entry_set_offset + ((uint32_t)Game::ds1_base));
    set_mem_protection(write_address, 5, MEM_PROTECT_RWX);
    inject_jmp_5b(write_address, &animation_entry_set_return, 0, &animation_entry_set_injection);
}

static void __stdcall read_body_aid_injection_helper_function(int32_t* animation_id, float* speed) {
    //Since we set animation speed at the table entry level, when it gets unset the speed is automatically reset. No cleanup needed

    //If this is an animation to be changed, ajust speed while we're in it
    auto ajust_aid = ANIMATIONS_TO_AJUST_SPEED_RATIO.find(*animation_id);
    if (ajust_aid != ANIMATIONS_TO_AJUST_SPEED_RATIO.end()) {
        *speed = ajust_aid->second;
        return;
    }
}

void __declspec(naked) __stdcall AnimationEdits::animation_entry_set_injection() {
    //"DARKSOULS.exe" + 9929B6:
    //movss[ecx + 40], xmm1
    //Alt: db F3 0F 11 49 40
    _asm {
        //original code
        movss [ecx + 0x40], xmm1

        push eax
        push ecx
        push edx

        add ecx, 0x40
        push ecx //Animation entry speed ptr
        push edi //Animation aid ptr
        call read_body_aid_injection_helper_function

        pop edx
        pop ecx
        pop eax
        //restore SSM registers
        movss xmm1, DWORD PTR ds : 0x115F59C
        xorps xmm0, xmm0
        jmp animation_entry_set_return
    }
}

void AnimationEdits::disable_whiff_animations() {
    if (!print_console("    Enabling remove animation whiffs...")) {
        Mod::startup_messages.push_back("    Enabling remove animation whiffs...");
    }

    uint8_t *write_address = (uint8_t*)(AnimationEdits::animation_whiff_set_offset + ((uint32_t)Game::ds1_base));
    uint8_t nop_patch[3] = { 0x90, 0x90, 0x90 };
    apply_byte_patch(write_address, nop_patch, 3);
}
