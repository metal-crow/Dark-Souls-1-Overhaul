#pragma once
#ifndef PADMANIPULATOR_STRUCT_H
#define PADMANIPULATOR_STRUCT_H

//NOTE: Anything marked "data_x" is a bunch of non-pointer data that can be saved/loaded without really knowing what it is.
// See the ghidra repo for more specific info about what is in that blob, if it's even known

#include <stddef.h>
#include <stdint.h>

#pragma pack(push, 1)

typedef struct PadManipulator PadManipulator;
typedef struct ChrManipulator ChrManipulator;
typedef struct ChrManipulator_ActionInputted ChrManipulator_ActionInputted;
typedef struct ChrManipulator_ActionInputtedTimeHeld ChrManipulator_ActionInputtedTimeHeld;
typedef struct PadManipulatorPacked PadManipulatorPacked;

struct PadManipulatorPacked
{
    //the bitfields must all be together
    unsigned int r1_weapon_attack_input : 1;
    unsigned int l1_input : 1;
    unsigned int r1_magic_attack_input : 1;
    unsigned int l1_magic_attack_input : 1;
    unsigned int r2_input : 1;
    unsigned int backstep_input : 1;
    unsigned int roll_forward_input : 1;
    unsigned int lefthand_weapon_attack : 1;
    unsigned int parry_input : 1;
    unsigned int block_input : 1;
    unsigned int jump_input : 1;
    unsigned int l1_weapon_attack : 1;
    unsigned int l2_weapon_attack : 1;
    unsigned int left_hand_slot_selected : 1;
    unsigned int right_hand_slot_selected : 1;
    unsigned int isSprintingAnim : 1;
    unsigned int not_getting_movement_input : 1;

    float camera_x_direction_movement_input_amount;
    float camera_z_direction_movement_input_amount;
    float camera_y_direction_movement_input_amount;
    float continuous_weapon_controlled_angle;
    float delta_pc_rotation_perframe;
    float weapon_controlled_angle;
    float pc_rotation;
    float delta_camera_y_rotation_perframe;
    float delta_camera_x_rotation_perframe;
    float camera_y_rotation;
    float camera_x_rotation;
    float movement_velocity[4];
    uint32_t LockonTargetHandle;
    uint8_t movement_related_flags;
    float y_movement_input;
    float x_movement_input;
    uint32_t cur_movement_input_index_to_use;
};

struct ChrManipulator_ActionInputtedTimeHeld
{
    float time_weapon_r1_held_1;
    float time_l1_held;
    float time_magic_r1_held;
    float time_magic_l1_held;
    float field4_0x10;
    float time_r2_held;
    float field6_0x18;
    float time_weapon_r1_held_2;
    float field8_0x20;
    float field9_0x24;
    float time_backstep_held;
    float field11_0x2c;
    float field12_0x30;
    float field13_0x34;
    float field14_0x38;
    float field15_0x3c;
    float field16_0x40;
    float field17_0x44;
    float field18_0x48;
    float field19_0x4c;
    float time_parry_held;
    float time_block_held;
    float field22_0x58;
    float field23_0x5c;
    float field24_0x60;
    float field25_0x64;
    float field26_0x68;
    float field27_0x6c;
    float field28_0x70;
    float field29_0x74;
    float field30_0x78;
    float field31_0x7c;
    float field32_0x80;
    float field33_0x84;
    float field34_0x88;
    float field35_0x8c;
    float field36_0x90;
    float field37_0x94;
    float field38_0x98;
    float field39_0x9c;
    float field40_0xa0;
    float field41_0xa4;
    float field42_0xa8;
    float field43_0xac;
    float field44_0xb0;
    float field45_0xb4;
    float field46_0xb8;
    float field47_0xbc;
    float field48_0xc0;
    float field49_0xc4;
    float field50_0xc8;
    float field51_0xcc;
};

static_assert(sizeof(ChrManipulator_ActionInputtedTimeHeld) == 0xd0);

struct ChrManipulator_ActionInputted
{
    bool r1_weapon_attack_input_1;
    bool l1_input;
    bool r1_magic_attack_input;
    bool l1_magic_attack_input;
    bool field4_0x4;
    bool r2_input;
    bool field6_0x6;
    bool r1_weapon_attack_input_2;
    bool field8_0x8;
    bool field9_0x9;
    bool backstep_input;
    bool field11_0xb;
    bool field12_0xc;
    bool field13_0xd;
    bool field14_0xe;
    bool roll_forward_input;
    bool field16_0x10;
    bool field17_0x11;
    bool field18_0x12;
    bool lefthand_weapon_attack;
    bool parry_input;
    bool block_input;
    bool field22_0x16;
    bool field23_0x17;
    bool field24_0x18;
    bool field25_0x19;
    bool field26_0x1a;
    bool field27_0x1b;
    bool field28_0x1c;
    bool field29_0x1d;
    bool field30_0x1e;
    bool field31_0x1f;
    bool field32_0x20;
    bool field33_0x21;
    bool field34_0x22;
    bool field35_0x23;
    bool field36_0x24;
    bool field37_0x25;
    bool field38_0x26;
    bool field39_0x27;
    bool field40_0x28;
    bool field41_0x29;
    bool jump_input;
    bool field43_0x2b;
    bool field44_0x2c;
    bool field45_0x2d;
    bool field46_0x2e;
    bool field47_0x2f;
    bool field48_0x30;
    bool field49_0x31;
    bool field50_0x32;
    bool l1_weapon_attack;
    bool l2_weapon_attack;
};

static_assert(sizeof(ChrManipulator_ActionInputted) == 0x35);

struct ChrManipulator
{
    void* vtable;
    uint8_t field1_0x8;
    uint8_t field2_0x9;
    uint8_t field3_0xa;
    uint8_t field4_0xb;
    uint8_t field5_0xc;
    uint8_t field6_0xd;
    uint8_t field7_0xe;
    uint8_t field8_0xf;
    float camera_x_direction_movement_input_amount;
    float camera_z_direction_movement_input_amount;
    float camera_y_direction_movement_input_amount;
    float camera_w_direction_movement_input_amount;
    float continuous_weapon_controlled_angle;
    float delta_pc_rotation_perframe;
    float field15_0x28;
    float field16_0x2c;
    float weapon_controlled_angle;
    float pc_rotation;
    uint32_t field19_0x38;
    uint32_t field20_0x3c;
    float delta_camera_y_rotation_perframe;
    float delta_camera_x_rotation_perframe;
    uint32_t field23_0x48;
    uint32_t field24_0x4c;
    float camera_y_rotation;
    float camera_x_rotation;
    float field27_0x58;
    uint32_t field28_0x5c;
    float camera_x_direction_movement_input_amount_alt;
    float camera_z_direction_movement_input_amount_alt;
    float camera_y_direction_movement_input_amount_alt;
    float camera_w_direction_movement_input_amount_alt;
    float movement_velocity[4];
    uint32_t field34_0x80;
    ChrManipulator_ActionInputted CurrentFrame_ActionInputs;
    ChrManipulator_ActionInputted PrevFrame_ActionInputs;
    uint8_t field37_0xee;
    uint8_t field38_0xef;
    ChrManipulator_ActionInputtedTimeHeld ActionInputtedTimeHeld;
    uint8_t field40_0x1c0;
    uint8_t field41_0x1c1;
    uint8_t field42_0x1c2;
    uint8_t field43_0x1c3;
    bool AnyActionInputted;
    uint8_t field45_0x1c5;
    uint8_t field46_0x1c6;
    uint8_t field47_0x1c7;
    uint32_t field48_0x1c8;
    uint32_t left_hand_slot_selected;
    uint32_t right_hand_slot_selected;
    uint32_t field51_0x1d4;
    uint32_t field52_0x1d8;
    uint32_t field53_0x1dc;
    uint32_t field54_0x1e0;
    bool isSprintingAnim;
    uint8_t field56_0x1e5;
    uint8_t field57_0x1e6;
    uint8_t field58_0x1e7;
    uint32_t field59_0x1e8;
    uint32_t ezStateActiveState;
    uint32_t ezStatePassiveState;
    uint8_t field62_0x1f4;
    uint8_t field63_0x1f5;
    uint8_t field64_0x1f6;
    uint8_t field65_0x1f7;
    uint8_t field66_0x1f8;
    uint8_t field67_0x1f9;
    uint8_t field68_0x1fa;
    uint8_t field69_0x1fb;
    uint8_t field70_0x1fc;
    uint8_t field71_0x1fd;
    uint8_t field72_0x1fe;
    uint8_t field73_0x1ff;
    float field74_0x200;
    float field75_0x204;
    float field76_0x208;
    float field77_0x20c;
    float field78_0x210[4];
    uint32_t LockonTargetHandle;
    uint32_t field80_0x224;
    uint8_t field81_0x228;
    uint8_t field82_0x229;
    uint8_t field83_0x22a;
    uint8_t field84_0x22b;
    uint8_t field85_0x22c;
    uint8_t field86_0x22d;
    uint8_t field87_0x22e;
    uint8_t field88_0x22f;
};

static_assert(sizeof(ChrManipulator) == 0x230);

struct PadManipulator
{
    ChrManipulator chrManipulator;
    void* SendGeneralPlayerData;
    uint32_t ticks_remaining_before_packetsent;
    uint8_t movement_related_flags;
    bool not_getting_movement_input;
    uint8_t field5_0x23e;
    uint8_t field6_0x23f;
    void* heap;
    uint64_t* arry_start;
    uint64_t* arry_end;
    uint64_t field10_0x258;
    float DashInputTimer;
    float y_movement_input[6]; //all the same copies
    uint8_t field13_0x27c;
    uint8_t field14_0x27d;
    uint8_t field15_0x27e;
    uint8_t field16_0x27f;
    uint32_t field17_0x280;
    uint8_t field18_0x284;
    uint8_t field19_0x285;
    uint8_t field20_0x286;
    uint8_t field21_0x287;
    uint8_t field22_0x288;
    uint8_t field23_0x289;
    uint8_t field24_0x28a;
    uint8_t field25_0x28b;
    float field26_0x28c;
    uint8_t field27_0x290;
    uint8_t field28_0x291;
    uint8_t field29_0x292;
    uint8_t field30_0x293;
    uint8_t field31_0x294;
    uint8_t field32_0x295;
    uint8_t field33_0x296;
    uint8_t field34_0x297;
    uint8_t field35_0x298;
    uint8_t field36_0x299;
    uint8_t field37_0x29a;
    uint8_t field38_0x29b;
    uint8_t field39_0x29c;
    uint8_t field40_0x29d;
    uint8_t field41_0x29e;
    uint8_t field42_0x29f;
    uint8_t field43_0x2a0;
    uint8_t field44_0x2a1;
    uint8_t field45_0x2a2;
    uint8_t field46_0x2a3;
    uint8_t field47_0x2a4;
    uint8_t field48_0x2a5;
    uint8_t field49_0x2a6;
    uint8_t field50_0x2a7;
    uint8_t field51_0x2a8;
    uint8_t field52_0x2a9;
    uint8_t field53_0x2aa;
    uint8_t field54_0x2ab;
    uint8_t field55_0x2ac;
    uint8_t field56_0x2ad;
    uint8_t field57_0x2ae;
    uint8_t field58_0x2af;
    uint8_t field59_0x2b0;
    uint8_t field60_0x2b1;
    uint8_t field61_0x2b2;
    uint8_t field62_0x2b3;
    float x_movement_input[6]; //all the same copies
    uint8_t field64_0x2cc;
    uint8_t field65_0x2cd;
    uint8_t field66_0x2ce;
    uint8_t field67_0x2cf;
    uint8_t field68_0x2d0;
    uint8_t field69_0x2d1;
    uint8_t field70_0x2d2;
    uint8_t field71_0x2d3;
    uint8_t field72_0x2d4;
    uint8_t field73_0x2d5;
    uint8_t field74_0x2d6;
    uint8_t field75_0x2d7;
    uint8_t field76_0x2d8;
    uint8_t field77_0x2d9;
    uint8_t field78_0x2da;
    uint8_t field79_0x2db;
    uint8_t field80_0x2dc;
    uint8_t field81_0x2dd;
    uint8_t field82_0x2de;
    uint8_t field83_0x2df;
    uint8_t field84_0x2e0;
    uint8_t field85_0x2e1;
    uint8_t field86_0x2e2;
    uint8_t field87_0x2e3;
    uint8_t field88_0x2e4;
    uint8_t field89_0x2e5;
    uint8_t field90_0x2e6;
    uint8_t field91_0x2e7;
    uint8_t field92_0x2e8;
    uint8_t field93_0x2e9;
    uint8_t field94_0x2ea;
    uint8_t field95_0x2eb;
    uint8_t field96_0x2ec;
    uint8_t field97_0x2ed;
    uint8_t field98_0x2ee;
    uint8_t field99_0x2ef;
    uint8_t field100_0x2f0;
    uint8_t field101_0x2f1;
    uint8_t field102_0x2f2;
    uint8_t field103_0x2f3;
    uint8_t field104_0x2f4;
    uint8_t field105_0x2f5;
    uint8_t field106_0x2f6;
    uint8_t field107_0x2f7;
    uint8_t field108_0x2f8;
    uint8_t field109_0x2f9;
    uint8_t field110_0x2fa;
    uint8_t field111_0x2fb;
    uint8_t field112_0x2fc;
    uint8_t field113_0x2fd;
    uint8_t field114_0x2fe;
    uint8_t field115_0x2ff;
    uint8_t field116_0x300;
    uint8_t field117_0x301;
    uint8_t field118_0x302;
    uint8_t field119_0x303;
    uint32_t cur_movement_input_index_to_use;
    float time_spend_forward_strafing;
    float time_spend_back_strafing;
    float time_spend_left_strafing;
    float time_spend_right_strafing;
    float camera_xaxis_rotation;
    uint8_t field126_0x31c;
    uint8_t field127_0x31d;
    uint8_t field128_0x31e;
    uint8_t field129_0x31f;
    uint32_t field130_0x320;
    float Backstep_timer;
    bool EnableBackStep;
    bool EnableBackStep_forward;
    uint8_t field134_0x32a;
    uint8_t field135_0x32b;
    uint8_t field136_0x32c;
    uint8_t field137_0x32d;
    uint8_t field138_0x32e;
    uint8_t field139_0x32f;
    float x_movement_input_reversed;
    float field141_0x334;
    float y_movement_input_reversed;
    float field143_0x33c;
    float jump_trigger_time_remaining;
    uint32_t field145_0x344;
    void* dbgNode;
    bool field147_0x350;
    uint8_t field148_0x351;
    uint8_t field149_0x352;
    uint8_t field150_0x353;
    uint8_t field151_0x354;
    uint8_t field152_0x355;
    uint8_t field153_0x356;
    uint8_t field154_0x357;
    uint8_t field155_0x358;
    uint8_t field156_0x359;
    uint8_t field157_0x35a;
    uint8_t field158_0x35b;
    uint8_t field159_0x35c;
    uint8_t field160_0x35d;
    uint8_t field161_0x35e;
    uint8_t field162_0x35f;
};

static_assert(sizeof(PadManipulator) == 0x360);

#pragma pack(pop)

#endif
