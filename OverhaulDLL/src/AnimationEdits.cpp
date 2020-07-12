/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Metal-Crow  -  Reverse engineering, C++

*/

#include "AnimationEdits.h"
#include "DarkSoulsOverhaulMod.h"
#include "SP/memory/injection/asm/x64.h"
#include <unordered_map>
#include <set>

void AnimationEdits::start()
{
    AnimationEdits::alter_animation_speeds();
    AnimationEdits::disable_whiff_animations();
    AnimationEdits::fix_curvedsword_infinites();
    //AnimationEdits::fix_roll_distance();
}


// key - animation id
// value - new speed ratio (Any large number ~>10 does a frame 1 skip), and point to start speed ajustment
static const std::unordered_map<int32_t, std::tuple<float, float>> ANIMATIONS_TO_AJUST_SPEED_RATIO = {
    { 6209, {10.0f, 0.0f}}, { 6309, {10.0f, 0.0f}},  //Firestorm startup
    { 6409, {1.6f,  0.0f}}, { 6509, {1.6f,  0.0f}},  //Firestorm main animation
    { 6215, {10.0f, 0.0f}}, { 6315, {10.0f, 0.0f}},  //Gravelord Sword Dance startup
    { 6415, {1.6f,  0.0f}}, { 6515, {1.6f,  0.0f}},  //Gravelord Sword Dance animation
    { 6203, {6.0f,  0.0f}}, { 6303, {6.0f,  0.0f}},  //Heal knealing startup
    { 6403, {1.0f,  0.0f}}, { 6503, {1.0f,  0.0f}},  //Heal knealing animation
    { 6220, {6.0f,  0.0f}}, { 6320, {6.0f,  0.0f}},  //Sunlight Heal knealing startup
    { 6420, {1.0f,  0.0f}}, { 6520, {1.0f,  0.0f}},  //Sunlight Heal knealing animation
    { 6218, {10.0f, 0.0f}}, { 6318, {10.0f, 0.0f}},  //lightning spear starting animation
    { 6418, {1.2f,  0.0f}}, { 6518, {1.2f,  0.0f}},  //lightning spear throwing animation
    { 9000, {1.25f, 3.0f}}, { 9420, {1.25f, 3.0f}},  //getting backstabbed (total times 5.9 and 5.766667)
};


extern "C" {
    uint64_t animation_entry_set_return;
    void animation_entry_set_injection();
    void read_body_aid_injection_helper_function(int32_t*, float*);
}

void AnimationEdits::alter_animation_speeds()
{
    global::cmd_out << Mod::output_prefix << ("Enabling animation speed alteration injection...\n");

    uint8_t *write_address = (uint8_t*)(AnimationEdits::animation_entry_set_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &animation_entry_set_return, 1, &animation_entry_set_injection);
}

typedef struct SpeedAlterStruct_ {
    float new_animation_speed;
    float time_to_adjust_speed_at;
    float* animation_entry_speed_ptr;
} SpeedAlterStruct;

//Thread data = speed ratio, start time, speed ajustment ptr
static DWORD WINAPI DelayAnimationSpeedAjustment(void* thread_data_arg) {
    SpeedAlterStruct* thread_data = (SpeedAlterStruct*)thread_data_arg;
    uint32_t start = *Game::get_game_time_ms();
    uint32_t cur = start;

    //Wait till animation reaches desired point
    float adjust_time = thread_data->time_to_adjust_speed_at;

    while (cur < start+adjust_time*1000) {
        cur = *Game::get_game_time_ms();
        Sleep(1);
    }

    //set speed
    float* speed_ptr = thread_data->animation_entry_speed_ptr;
    *speed_ptr = thread_data->time_to_adjust_speed_at;

    free(thread_data);
    return 0;
}

void read_body_aid_injection_helper_function(int32_t* animation_id, float* speed) {
    //Since we set animation speed at the table entry level, when it gets unset the speed is automatically reset. No cleanup needed

    //If this is an animation to be changed, ajust speed while we're in it
    auto ajust_aid = ANIMATIONS_TO_AJUST_SPEED_RATIO.find(*animation_id);
    if (ajust_aid != ANIMATIONS_TO_AJUST_SPEED_RATIO.end()) {
        //if this is a instant speed ajustment
        if (std::get<1>(ajust_aid->second) == 0) {
            *speed = std::get<0>(ajust_aid->second);
            return;
        }
        //if delayed speed ajustment
        else {
            //need the args to be on the heap
            SpeedAlterStruct* thread_data = (SpeedAlterStruct*)malloc(sizeof(SpeedAlterStruct));
            thread_data->new_animation_speed = std::get<0>(ajust_aid->second);
            thread_data->time_to_adjust_speed_at = std::get<1>(ajust_aid->second);
            thread_data->animation_entry_speed_ptr = speed;

            CreateThread(NULL, 0, DelayAnimationSpeedAjustment, thread_data, 0, NULL);
            return;
        }
    }

    //handle backstabING detection (b/c it's a ton of diff animations)
    if (*animation_id > 200000) {
        if (*animation_id % 1000 == 400 || *animation_id % 1000 == 401) {
            SpeedAlterStruct* thread_data = (SpeedAlterStruct*)malloc(sizeof(SpeedAlterStruct));
            thread_data->new_animation_speed = 1.25f;
            thread_data->time_to_adjust_speed_at = 3.0f;
            thread_data->animation_entry_speed_ptr = speed;

            CreateThread(NULL, 0, DelayAnimationSpeedAjustment, thread_data, 0, NULL);
            return;
        }
    }
}

void AnimationEdits::disable_whiff_animations() {
    global::cmd_out << Mod::output_prefix << ("Enabling remove animation whiffs...\n");

    //make jump over setting whiff unconditional
    uint8_t *write_address = (uint8_t*)(AnimationEdits::animation_whiff_set_offset + Game::ds1_base);
    uint8_t jmp_patch[2] = { 0xEB, 0x54 };
    sp::mem::patch_bytes(write_address, jmp_patch, 2);
}

extern "C" {
    uint64_t TAE_GetDamageRate_StunLen_finish_return;
    void TAE_GetDamageRate_StunLen_finish_injection();
    float TAE_GetDamageRate_StunLen_finish_helper_function(float);
}

void AnimationEdits::fix_curvedsword_infinites() {
    global::cmd_out << Mod::output_prefix << ("Fix curved swords stun time...\n");

    uint8_t *write_address = (uint8_t*)(AnimationEdits::TAE_GetDamageRate_StunLen_finish_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &TAE_GetDamageRate_StunLen_finish_return, 0, &TAE_GetDamageRate_StunLen_finish_injection);
}

//decrease the stun length so that defender can roll escape
float TAE_GetDamageRate_StunLen_finish_helper_function(float current_stun)
{
    uint32_t weaponid = Game::get_last_attack_weapon_id();

    //if this is a curved sword (except QFS)
    if ((weaponid >= 400000 && weaponid < 406000) || weaponid == 9010000) {
        return (current_stun - 0.15f);
    }
    return current_stun;
}

extern "C" {
    uint64_t Calculate_movement_delta_return;
    void Calculate_movement_delta_injection();
    void Calculate_movement_delta_helper_function(uint64_t, float*);
}

void AnimationEdits::fix_roll_distance() {
    global::cmd_out << Mod::output_prefix << ("Correct roll distance...\n");

    uint8_t *write_address = (uint8_t*)(AnimationEdits::Calculate_movement_delta_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &Calculate_movement_delta_return, 2, &Calculate_movement_delta_injection);
}

static const std::unordered_map<int32_t, const std::vector<float>> hka_reference_frames_distances = {
    {700, {}}, //Roll forward(fast)
    {701, {}}, //Roll backward(fast)
    {702, {}}, //Roll left(fast)
    {703, {}}, //Roll right(fast)

    {710, {}}, //Roll forward(mid)
    {711, {}}, //Roll backward(mid)
    {712, {}}, //Roll left(mid)
    {713, {}}, //Roll right(mid)

    {720, {}}, //Roll forward(fat / slow)
    {721, {}}, //Roll backward(fat / slow)
    {722, {}}, //Roll left(fat / slow)
    {723, {}}, //Roll right(fat / slow)

    {735, {}}, //Roll forward(fast, Dark Wood Grain Ring)
    {736, {}}, //Roll backward(fast, Dark Wood Grain Ring)
    {737, {}}, //Roll left(fast, Dark Wood Grain Ring)
    {738, {}}, //Roll right(fast, Dark Wood Grain Ring)
};

void Calculate_movement_delta_helper_function(uint64_t playerCtrl, float* movement_delta)
{
}
