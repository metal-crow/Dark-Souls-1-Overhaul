/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Metal-Crow  -  Reverse engineering, C++

*/

#include "AnimationEdits.h"
#include "DarkSoulsOverhaulMod.h"
#include "SP/memory/injection/asm/x64.h"
#include <set>
#include "MainLoop.h"
#include "ModNetworking.h"

void AnimationEdits::start()
{
    AnimationEdits::alter_animation_parameters();
    AnimationEdits::disable_whiff_animations();
}

/*
 * ========== ADJUST ANIMATION PARAMETERS
 */

// key - animation id
// value - new speed ratio (Any large number ~>10 does a frame 1 skip), and point to start speed ajustment in seconds
static const std::unordered_map<int32_t, std::tuple<float, float>> ANIMATIONS_TO_AJUST_SPEED_RATIO = {
    { 6215, {10.0f, 0.0f}}, { 6315, {10.0f, 0.0f}},  //Gravelord Sword Dance startup
    { 6415, {1.6f,  0.0f}}, { 6515, {1.6f,  0.0f}},  //Gravelord Sword Dance animation
    { 6203, {6.0f,  0.0f}}, { 6303, {6.0f,  0.0f}},  //Heal knealing startup
    { 6403, {1.0f,  0.0f}}, { 6503, {1.0f,  0.0f}},  //Heal knealing animation
    { 6220, {6.0f,  0.0f}}, { 6320, {6.0f,  0.0f}},  //Sunlight Heal knealing startup
    { 6420, {1.0f,  0.0f}}, { 6520, {1.0f,  0.0f}},  //Sunlight Heal knealing animation
    { 6218, {10.0f, 0.0f}}, { 6318, {10.0f, 0.0f}},  //lightning spear starting animation
    { 6418, {1.2f,  0.0f}}, { 6518, {1.2f,  0.0f}},  //lightning spear throwing animation
};


extern "C" {
    uint64_t animation_entry_set_return;
    void animation_entry_set_injection();
    void read_body_aid_injection_helper_function(int32_t*, float*);
}

void AnimationEdits::alter_animation_parameters()
{
    ConsoleWrite("Enabling animation parameter alteration injection...");

    uint8_t *write_address = (uint8_t*)(AnimationEdits::animation_entry_set_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &animation_entry_set_return, 1, &animation_entry_set_injection);
}

typedef struct SpeedAlterStruct_ {
    float new_animation_speed;
    float time_to_adjust_speed_at;
    float* animation_entry_speed_ptr;
} SpeedAlterStruct;

//data = speed ratio, start time (absolute, not relative), speed ajustment ptr
bool DelayAnimationSpeedAjustment(void* thread_data_arg) {
    SpeedAlterStruct* thread_data = (SpeedAlterStruct*)thread_data_arg;
    auto cur_time_o = Game::get_game_time_ms();
    if (!cur_time_o.has_value())
    {
        ConsoleWrite("Unable to get_game_time_ms for cur_time in %s", __FUNCTION__);
        return true;
    }
    uint32_t cur_time = *cur_time_o.value();

    //Wait till animation reaches desired point
    float adjust_time = thread_data->time_to_adjust_speed_at;
    if (cur_time >= adjust_time)
    {
        //set speed
        float* speed_ptr = thread_data->animation_entry_speed_ptr;
        *speed_ptr = thread_data->time_to_adjust_speed_at;

        free(thread_data);

        return false;
    }

    //time to adjust not yet reached
    return true;
}

void read_body_aid_injection_helper_function(int32_t* animation_id, float* speed) {
    //If feature disabled, don't do anything
    if (Mod::get_mode() != ModMode::Overhaul) {
        return;
    }

    //Since we set animation speed at the table entry level, when it gets unset the speed is automatically reset. No cleanup needed
    auto ajust_aid = ANIMATIONS_TO_AJUST_SPEED_RATIO.find(*animation_id);
    if (ajust_aid != ANIMATIONS_TO_AJUST_SPEED_RATIO.end()) {
        //if this is a instant speed ajustment
        if (std::get<1>(ajust_aid->second) == 0) {
            *speed = std::get<0>(ajust_aid->second);
            return;
        }
        //if delayed speed ajustment
        else {
            //need the args to be on the heap. It will get cleaned up by the using function
            SpeedAlterStruct* thread_data = (SpeedAlterStruct*)malloc(sizeof(SpeedAlterStruct));

            thread_data->new_animation_speed = std::get<0>(ajust_aid->second);

            //compute the absolute time this anim should be changed at. cur_time + offset_time_to_adjust
            auto cur_time_o = Game::get_game_time_ms();
            if (!cur_time_o.has_value())
            {
                ConsoleWrite("Unable to get_game_time_ms for start_time in %s", __FUNCTION__);
                free(thread_data);
                return;
            }
            thread_data->time_to_adjust_speed_at = std::get<1>(ajust_aid->second)*1000 + *cur_time_o.value();

            thread_data->animation_entry_speed_ptr = speed;

            MainLoop::setup_mainloop_callback(DelayAnimationSpeedAjustment, thread_data, "DelayAnimationSpeedAjustment");
            return;
        }
    }
}

/*
 * ========== DISABLE WHIFF ANIMATIONS
 */

extern "C" {
    uint64_t disable_whiff_animations_injection_return;
    void disable_whiff_animations_injection();
    uint8_t disable_whiff_animations_injection_helper(uint8_t);
}

void AnimationEdits::disable_whiff_animations() {
    ConsoleWrite("Enabling remove animation whiffs...");

    //make jump over setting whiff unconditional
    uint8_t *write_address = (uint8_t*)(AnimationEdits::animation_whiff_set_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &disable_whiff_animations_injection_return, 2, &disable_whiff_animations_injection);
}

uint8_t disable_whiff_animations_injection_helper(uint8_t whiff) {
    //Disable feature
    if (Mod::get_mode() != ModMode::Overhaul) {
        return whiff;
    }

    return 0; //disable whiff
}
