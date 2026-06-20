#pragma once
#ifndef PLAYERINS_STRUCT_H
#define PLAYERINS_STRUCT_H

#include <stddef.h>
#include <stdint.h>
#include "PadManipulatorStruct.h"
#include "AttachSysSlotStructs.h"
#include "DamageManStruct.h"

typedef struct AnimationQueue_Entry_sub1_field0x10 AnimationQueue_Entry_sub1_field0x10;
typedef struct AnimationQueue_Entry_sub1 AnimationQueue_Entry_sub1;
typedef struct AnimationQueue_Entry AnimationQueue_Entry;
typedef struct AnimationQueue AnimationQueue;
typedef struct ChrCtrl_AnimationQueueEntry ChrCtrl_AnimationQueueEntry;
typedef struct ChrCtrl_AnimationQueue_field0x10 ChrCtrl_AnimationQueue_field0x10;
typedef struct ChrCtrl_AnimationQueue_field0x10_field0x10arrayelem ChrCtrl_AnimationQueue_field0x10_field0x10arrayelem;
typedef struct hkaAnimatedSkeleton hkaAnimatedSkeleton;
typedef struct hkaDefaultAnimationControl hkaDefaultAnimationControl;
typedef struct hkaAnimationControl hkaAnimationControl;
typedef struct ChrCtrl_AnimationQueue ChrCtrl_AnimationQueue;
typedef struct ChrCtrl_AnimationQueue_field0x20 ChrCtrl_AnimationQueue_field0x20;
typedef struct AnimationMediatorStateEntry AnimationMediatorStateEntry;
typedef struct AnimationMediator AnimationMediator;
typedef struct HitIns HitIns;
typedef struct HavokChara HavokChara;
typedef struct hkpCharacterProxy hkpCharacterProxy;
typedef struct EzState_detail_EzStateMachineImpl EzState_detail_EzStateMachineImpl;
typedef struct EzStateRegister EzStateRegister;
typedef struct EzStateRegisterSet EzStateRegisterSet;
typedef struct ActionCtrl_0x30Substruct ActionCtrl_0x30Substruct;
typedef struct ActionCtrl ActionCtrl;
typedef struct WalkAnim_Twist_Field0x228Elem_field0x10elem WalkAnim_Twist_Field0x228Elem_field0x10elem;
typedef struct WalkAnim_Twist_Field0x228Elem WalkAnim_Twist_Field0x228Elem;
typedef struct WalkAnim_Twist WalkAnim_Twist;
typedef struct ChrCtrl ChrCtrl;
typedef struct TurnAnim TurnAnim;
typedef struct SpinJoint SpinJoint;
typedef struct ArrowTurnAnim ArrowTurnAnim;
typedef struct PlayerCtrl PlayerCtrl;
typedef struct ChrIns_AnimationMediatorStateInfo ChrIns_AnimationMediatorStateInfo;
typedef struct ItemUsed ItemUsed;
typedef struct SpecialEffect_Info SpecialEffect_Info;
typedef struct SpecialEffect SpecialEffect;
typedef struct QwcSpEffectEquipCtrl QwcSpEffectEquipCtrl;
typedef struct ChrAttachSys ChrAttachSys;
typedef struct ThrowSelfEsc ThrowSelfEsc;
typedef struct EntityThrowAnimationStatus EntityThrowAnimationStatus;
typedef struct ChrIns_field0x2c8 ChrIns_field0x2c8;
typedef struct ChrIns_field0x18 ChrIns_field0x18;
typedef struct ChrIns ChrIns;
typedef struct PlayerGameData_AttributeInfo PlayerGameData_AttributeInfo;
typedef struct EquipInventoryData EquipInventoryData;
typedef struct EquipInventoryDataItem EquipInventoryDataItem;
typedef struct MagicSlot MagicSlot;
typedef struct EquipMagicData EquipMagicData;
typedef struct EquipItemData EquipItemData;
typedef struct EquipGameData EquipGameData;
typedef struct PlayerGameData_ChrProperties PlayerGameData_ChrProperties;
typedef struct PlayerGameData PlayerGameData;
typedef struct RingEquipCtrl RingEquipCtrl;
typedef struct WeaponEquipCtrl WeaponEquipCtrl;
typedef struct ChrAsmModel ChrAsmModel;
typedef struct ChrAsmModelRes_Elem ChrAsmModelRes_Elem;
typedef struct ChrAsmModelRes ChrAsmModelRes;
typedef struct ProEquipCtrl ProEquipCtrl;
typedef struct ChrAsm ChrAsm;
typedef struct PlayerIns PlayerIns;
typedef struct PlayerGameData PlayerGameData;
typedef struct SteamOnlineIDData SteamOnlineIDData;

struct AnimationQueue_Entry_sub1_field0x10
{
    //pointers, but to previously saved values (copied raw)
    uint64_t parent_AnimationQueue_Entry_field0x10; // 0x0 (ghidra: parent ptr)
    uint64_t unk_8;                                 // 0x8 (ghidra: pointer)
};

struct AnimationQueue_Entry_sub1
{
    uint64_t padding_1[2]; //pointer to unchanging values
    AnimationQueue_Entry_sub1_field0x10** field0x10;
    uint64_t field0x10_cap;
    uint64_t unk;
    uint64_t field0x10_len;
};

static_assert(sizeof(AnimationQueue_Entry_sub1) == 0x30);

struct AnimationQueue_Entry_field0x10
{
    uint32_t upcoming_animId;  // 0x0
    uint8_t unk_4;             // 0x4
    uint8_t unk_5[3];          // 0x5 (gap)
    float unk_8;               // 0x8
    uint32_t unk_c;            // 0xc
    uint16_t unk_10;           // 0x10
    uint8_t unk_12[2];         // 0x12 (gap)
    uint32_t unk_14;           // 0x14
    float unk_18;              // 0x18
    float unk_1c;              // 0x1c
};

static_assert(offsetof(AnimationQueue_Entry_field0x10, unk_14) == 0x14);
static_assert(sizeof(AnimationQueue_Entry_field0x10) == 0x20);

struct AnimationQueue_Entry
{
    uint16_t unk_0;            // 0x0
    uint16_t unk_2;            // 0x2
    uint8_t unk_4[4];          // 0x4 (gap)
    void* padding_0;           // 0x8 (ghidra: ChrCtrl_AnimationQueue ptr; not copied)
    AnimationQueue_Entry_field0x10 field0x10[8]; // 0x10 (former data_1)
    AnimationQueue_Entry_sub1 sub1;  // 0x110
    uint16_t unk_140;          // 0x140 (gap)
    uint16_t unk_142;          // 0x142
    float unk_144;             // 0x144
    uint64_t padding_3;        // 0x148 (ghidra: head of chained_anims; not copied)
    uint64_t* chained_animations_array_start; //i believe these are pointers to static structs (or at least static in a session) (0x150)
    uint64_t* chained_animations_array_end;   // 0x158
    uint64_t* chained_animations_array_capEnd; // 0x160
    uint32_t unk_168;          // 0x168
    uint32_t unk_16c;          // 0x16c
    float unk_170;             // 0x170
    float unk_174;             // 0x174
    float unk_178;             // 0x178
    uint8_t unk_17c[4];        // 0x17c (gap to 0x180)
};

static_assert(offsetof(AnimationQueue_Entry, padding_0) == 0x8);
static_assert(offsetof(AnimationQueue_Entry, field0x10) == 0x10);
static_assert(offsetof(AnimationQueue_Entry, sub1) == 0x110);
static_assert(offsetof(AnimationQueue_Entry, unk_140) == 0x140);
static_assert(offsetof(AnimationQueue_Entry, chained_animations_array_start) == 0x150);
static_assert(offsetof(AnimationQueue_Entry, unk_168) == 0x168);
static_assert(sizeof(AnimationQueue_Entry) == 0x180);

struct AnimationQueue
{
    void* padding_0;
    AnimationQueue_Entry* AnimationQueue_Entries[6];
    void* padding_1;
};

static_assert(offsetof(AnimationQueue, AnimationQueue_Entries) == 0x8);
static_assert(sizeof(AnimationQueue) == 0x40);

struct ChrCtrl_AnimationQueueEntry
{
    uint16_t unk_0;            // 0x0
    uint16_t unk_2;            // 0x2
    uint8_t unk_4[4];          // 0x4 (gap)
    hkaDefaultAnimationControl* defaultAnimationControl;  // 0x8
    uint64_t padding_0[2];     // 0x10 (ghidra: loopAnimCtrlListener + parent ptrs; not copied)
    uint64_t HvkAnim_AnimInfoArrayElem; //this should be a const pointer since it's to an animbnd (0x20)
    float unk_28;             // 0x28
    uint32_t unk_2c;          // 0x2c
    uint64_t unk_30;          // 0x30
    uint16_t unk_38;          // 0x38
    uint8_t unk_3a;           // 0x3a
    uint8_t unk_3b;           // 0x3b (gap)
    uint32_t unk_3c;          // 0x3c
    float unk_40;             // 0x40
    float unk_44;             // 0x44
    uint8_t unk_48;           // 0x48
    uint8_t unk_49[3];        // 0x49 (gap)
    uint32_t unk_4c;          // 0x4c
    uint8_t unk_50;           // 0x50
    uint8_t unk_51[3];        // 0x51 (gap)
    float lastFrameTime;      // 0x54
    uint32_t unk_58;          // 0x58
    float unk_5c;             // 0x5c
    uint32_t unk_60;          // 0x60
    float unk_64;             // 0x64
    float unk_68;             // 0x68
    float unk_6c;             // 0x6c
    float unk_70;             // 0x70
    uint8_t unk_74;           // 0x74
    uint8_t unk_75[3];        // 0x75 (gap to 0x78)
};

static_assert(offsetof(ChrCtrl_AnimationQueueEntry, defaultAnimationControl) == 0x8);
static_assert(offsetof(ChrCtrl_AnimationQueueEntry, HvkAnim_AnimInfoArrayElem) == 0x20);
static_assert(offsetof(ChrCtrl_AnimationQueueEntry, unk_28) == 0x28);
static_assert(offsetof(ChrCtrl_AnimationQueueEntry, lastFrameTime) == 0x54);
static_assert(sizeof(ChrCtrl_AnimationQueueEntry) == 0x78);

struct ChrCtrl_AnimationQueue_field0x10_field0x10arrayelem
{
    uint64_t rescap_anibnd;   // 0x0 (ghidra: AnibndResCap*; static/const ptr, copied wholesale)
    uint32_t unk_8;           // 0x8
    uint32_t count;           // 0xc
};

static_assert(offsetof(ChrCtrl_AnimationQueue_field0x10_field0x10arrayelem, count) == 0xc);
static_assert(sizeof(ChrCtrl_AnimationQueue_field0x10_field0x10arrayelem) == 0x10);

struct ChrCtrl_AnimationQueue_field0x10
{
    uint32_t array1_len;
    uint32_t array2_len;
    ChrCtrl_AnimationQueue_field0x10_field0x10arrayelem** arry1; //this points to elements in arry2
    ChrCtrl_AnimationQueue_field0x10_field0x10arrayelem* arry2;
    uint64_t padding_0[2]; //pointer to unchanging string (always c0000 for the PC), and ptr to parent
};

static_assert(offsetof(ChrCtrl_AnimationQueue_field0x10, array1_len) == 0x0);
static_assert(offsetof(ChrCtrl_AnimationQueue_field0x10, array2_len) == 0x4);
static_assert(offsetof(ChrCtrl_AnimationQueue_field0x10, arry1) == 0x8);
static_assert(offsetof(ChrCtrl_AnimationQueue_field0x10, arry2) == 0x10);
static_assert(sizeof(ChrCtrl_AnimationQueue_field0x10) == 0x28);

struct hkaAnimationControl
{
    uint64_t padding_0;        // 0x0 (vtable)
    uint32_t unk_8;            // 0x8
    uint32_t unk_c;            // 0xc
    float curTimeInAnimation;  // 0x10
    float weight;              // 0x14
    uint8_t* field0x18;        // 0x18
    uint32_t field0x18_len;    // 0x20
    uint32_t field0x18_cap;    // 0x24
    uint8_t* field0x28;        // 0x28
    uint32_t field0x28_len;    // 0x30
    uint32_t field0x28_cap;    // 0x34
    uint64_t HkaAnimationBinding; //static ptr (0x38)
    uint64_t padding_1;        // 0x40
    uint32_t unk_48;           // 0x48
    uint32_t unk_4c;           // 0x4c
    float unk_50;              // 0x50
};

static_assert(offsetof(hkaAnimationControl, field0x18) == 0x18);
static_assert(offsetof(hkaAnimationControl, field0x28) == 0x28);
static_assert(offsetof(hkaAnimationControl, HkaAnimationBinding) == 0x38);
static_assert(sizeof(hkaAnimationControl) == 0x58);

struct hkaDefaultAnimationControl
{
    hkaAnimationControl HkaAnimationControl;  // 0x0
    float weight;              // 0x58
    float speed;               // 0x5c
    uint32_t repetitions;      // 0x60
    uint8_t unk_64[56];        // 0x64 (floats/undefined run)
    float frameTickAnimTime;   // 0x9c
    uint8_t unk_a0[8];         // 0xa0
    uint8_t padding_0[24];     // 0xa8 (ghidra: field0xa8 ptr + len; not copied)
};

static_assert(offsetof(hkaDefaultAnimationControl, weight) == 0x58);
static_assert(offsetof(hkaDefaultAnimationControl, frameTickAnimTime) == 0x9c);
static_assert(offsetof(hkaDefaultAnimationControl, padding_0) == 0xa8);
static_assert(sizeof(hkaDefaultAnimationControl) == 0xc0);

struct hkaAnimatedSkeleton
{
    uint64_t padding_0;        // 0x0 (vtable1)
    uint32_t unk_8;            // 0x8
    uint32_t unk_c;            // 0xc (gap)
    uint64_t padding_1;        // 0x10 (vtable2)
    void* padding_HkpRigidBody; // 0x18 (ghidra: hkpRigidBody)
    hkaDefaultAnimationControl** animCtrl_list;  // 0x20
    uint32_t animCtrl_list_len; // 0x28
    uint32_t data_1;           // 0x2C
    void* padding_2; //seems to be unneeded (ghidra: hkaSkeleton) (0x30)
    float unk_38;              // 0x38
    uint32_t unk_3c;           // 0x3c
};

static_assert(offsetof(hkaAnimatedSkeleton, unk_8) == 8);
static_assert(offsetof(hkaAnimatedSkeleton, padding_HkpRigidBody) == 0x18);
static_assert(offsetof(hkaAnimatedSkeleton, animCtrl_list) == 0x20);
static_assert(offsetof(hkaAnimatedSkeleton, animCtrl_list_len) == 0x28);
static_assert(offsetof(hkaAnimatedSkeleton, data_1) == 0x2C);
static_assert(offsetof(hkaAnimatedSkeleton, padding_2) == 0x30);
static_assert(offsetof(hkaAnimatedSkeleton, unk_38) == 0x38);
static_assert(sizeof(hkaAnimatedSkeleton) == 0x40);

struct ChrCtrl_AnimationQueue_field0x20
{
    uint64_t padding_0; // 0x0 (ghidra: parent_hkaSkeleton ptr; not copied)
    void* field0x8; // len = this->parent_hkaSkeleton->field0x28_len + 3 // elem size = 0x30, all floats
    uint32_t field0x8_len;   // 0x10
    uint32_t field0x8_len2;  // 0x14
    void* field0x18; // len = this->parent_hkaSkeleton->field0x28_len + 3 // elem size = 0x30, all floats
    uint32_t field0x18_len;  // 0x20
    uint32_t field0x18_len2; // 0x24
    uint32_t* field0x28; // len = this->parent_hkaSkeleton->field0x28_len + 3 // elem size = 4
    uint32_t field0x28_len;  // 0x30
    uint32_t field0x28_len2; // 0x34
    uint8_t unk_38;          // 0x38 (bool)
    uint8_t unk_39[7];       // 0x39 (pad to 0x40)
    uint32_t* padding_field0x40; // len = this->parent_hkaSkeleton->field0x58_len + 3 // elem size = 4. Always seems to be null and length = 0
    uint32_t unk_48;         // 0x48
    uint32_t unk_4c;         // 0x4c
};

static_assert(offsetof(ChrCtrl_AnimationQueue_field0x20, field0x8) == 0x8);
static_assert(offsetof(ChrCtrl_AnimationQueue_field0x20, field0x8_len) == 0x10);
static_assert(offsetof(ChrCtrl_AnimationQueue_field0x20, field0x18) == 0x18);
static_assert(offsetof(ChrCtrl_AnimationQueue_field0x20, field0x18_len) == 0x20);
static_assert(offsetof(ChrCtrl_AnimationQueue_field0x20, field0x28) == 0x28);
static_assert(offsetof(ChrCtrl_AnimationQueue_field0x20, field0x28_len) == 0x30);
static_assert(offsetof(ChrCtrl_AnimationQueue_field0x20, padding_field0x40) == 0x40);
static_assert(offsetof(ChrCtrl_AnimationQueue_field0x20, unk_48) == 0x48);
static_assert(sizeof(ChrCtrl_AnimationQueue_field0x20) == 0x50);

struct ChrCtrl_AnimationQueue
{
    uint32_t array_length;     // 0x0
    uint32_t unk_4;            // 0x4
    ChrCtrl_AnimationQueueEntry* arry;  // 0x8
    ChrCtrl_AnimationQueue_field0x10* field0x10;  // 0x10
    hkaAnimatedSkeleton* HkaAnimatedSkeleton;  // 0x18
    ChrCtrl_AnimationQueue_field0x20* field0x20;  // 0x20
    uint64_t padding_0[7]; //unimportant pointers (ghidra: listeners + name). setting to null has no effect (0x28)
    uint32_t genderSpecificAnimationOffset; // 0x60
    uint32_t unk_64;           // 0x64
    uint32_t unk_68;           // 0x68
    uint32_t addToAnimId;      // 0x6c
    float unk_70;              // 0x70
    uint32_t unk_74;           // 0x74
    float unk_78;              // 0x78
    uint32_t unk_7c;           // 0x7c
    void* padding_2;           // 0x80 (not copied)
    uint64_t unk_88;           // 0x88 (former data_3)
    uint64_t padding_3[3];     // 0x90 (not copied)
    float unk_a8;              // 0xa8
    uint8_t unk_ac;            // 0xac
    uint8_t unk_ad;            // 0xad
    uint8_t unk_ae[2];         // 0xae
    uint8_t unk_b0[16];        // 0xb0
    uint8_t unk_c0;            // 0xc0
    uint8_t unk_c1;            // 0xc1
    uint8_t unk_c2[6];         // 0xc2
    uint64_t padding_4[2];     // 0xc8 (not copied)
    uint32_t unk_d8;           // 0xd8
    uint32_t unk_dc;           // 0xdc
    uint8_t unk_e0;            // 0xe0
    uint8_t unk_e1[15];        // 0xe1
};

static_assert(offsetof(ChrCtrl_AnimationQueue, array_length) == 0);
static_assert(offsetof(ChrCtrl_AnimationQueue, arry) == 8);
static_assert(offsetof(ChrCtrl_AnimationQueue, field0x10) == 0x10);
static_assert(offsetof(ChrCtrl_AnimationQueue, HkaAnimatedSkeleton) == 0x18);
static_assert(offsetof(ChrCtrl_AnimationQueue, genderSpecificAnimationOffset) == 0x60);
static_assert(offsetof(ChrCtrl_AnimationQueue, unk_70) == 0x70);
static_assert(offsetof(ChrCtrl_AnimationQueue, unk_88) == 0x88);
static_assert(offsetof(ChrCtrl_AnimationQueue, unk_a8) == 0xa8);
static_assert(offsetof(ChrCtrl_AnimationQueue, unk_d8) == 0xd8);
static_assert(sizeof(ChrCtrl_AnimationQueue) == 0xf0);

struct AnimationMediatorStateEntry
{
    int32_t animationId;       // 0x0
    uint32_t unk_4;            // 0x4
    uint32_t unk_8;            // 0x8
    int32_t animationId_blend_slot0; // 0xc
    int32_t animationId_blend_slot1; // 0x10
    int32_t animationId_blend_slot2; // 0x14
    uint32_t slotted_blendanim_data[3]; // 0x18
    uint8_t loop;              // 0x24 (ghidra: bool)
    uint8_t loop_blend_slot0;  // 0x25
    uint8_t loop_blend_slot1;  // 0x26
    uint8_t loop_blend_slot2;  // 0x27
    float blend_TaeEvent16;    // 0x28
    float TaeEvent16_blend_slot0; // 0x2c
    float TaeEvent16_blend_slot1; // 0x30
    float TaeEvent16_blend_slot2; // 0x34
    uint32_t unk_38;           // 0x38
    uint32_t slot0_data3;      // 0x3c
    uint32_t slot1_data3;      // 0x40
    uint32_t slot2_data3;      // 0x44
    float blend_ratio;         // 0x48
    float blend_ratio_slot0;   // 0x4c
    float blend_ratio_slot1;   // 0x50
    float blend_ratio_slot2;   // 0x54
    float blend_ratio_TaeEvent301; // 0x58
    float blend_2;             // 0x5c
    float slot0_data5;         // 0x60
    float slot1_data5;         // 0x64
    float slot2_data5;         // 0x68
    float blend_ratio2;        // 0x6c
    float blend_ratio2_slot0;  // 0x70
    float blend_ratio2_slot1;  // 0x74
    float blend_ratio2_slot2;  // 0x78
    uint8_t unk_7c;            // 0x7c
    uint8_t slot0_data7;       // 0x7d
    uint8_t slot1_data7;       // 0x7e
    uint8_t slot2_data7;       // 0x7f
    uint8_t unk_80;            // 0x80
    uint8_t unk_81[3];         // 0x81 (gap)
    uint32_t stateIndex2;      // 0x84 (ghidra: AnimationStateTypesEnum)
    uint16_t unk_88;           // 0x88
    uint8_t unk_8a[6];         // 0x8a (gap)
    uint64_t padding_0[2]; //these linked animations only point to other StateEntries in the states_list. No need to worry about them (0x90)
    uint8_t data_1[8];         // 0xa0 (tail of ghidra sub1; copied)
};

static_assert(offsetof(AnimationMediatorStateEntry, animationId) == 0);
static_assert(offsetof(AnimationMediatorStateEntry, blend_ratio) == 0x48);
static_assert(offsetof(AnimationMediatorStateEntry, stateIndex2) == 0x84);
static_assert(offsetof(AnimationMediatorStateEntry, padding_0) == 0x90);
static_assert(offsetof(AnimationMediatorStateEntry, data_1) == 0xa0);
static_assert(sizeof(AnimationMediatorStateEntry) == 0xa8);

struct AnimationMediator
{
    AnimationMediatorStateEntry states_list[31];
    void* padding_0;           // 0x1458 (ghidra: ChrCtrl_AnimationQueue ptr; not copied)
    AnimationQueue* animationQueue;  // 0x1460
    uint32_t unk_1468[7];      // 0x1468
    uint8_t unk_1484;          // 0x1484
    uint8_t unk_1485[3];       // 0x1485 (gap)
    uint64_t unk_1488;         // 0x1488
};

static_assert(offsetof(AnimationMediator, animationQueue) == 0x1460);
static_assert(offsetof(AnimationMediator, unk_1468) == 0x1468);
static_assert(sizeof(AnimationMediator) == 0x1490);

struct HitIns
{
    uint64_t padding_0;    // 0x0 (vtable)
    uint32_t unk_8;        // 0x8
    uint32_t unk_c;        // 0xc
    uint64_t padding_1; //pointer to ResCap (ghidra: HitIns_sub1::msbResCap), const ptr (0x10)
    uint16_t unk_18;       // 0x18
    uint8_t unk_1a;        // 0x1a (gap)
    uint8_t unk_1b;        // 0x1b
    uint32_t areaId;       // 0x1c
    int32_t unk_20;        // 0x20
    int32_t unk_24[11];    // 0x24
    void* unk_50;          // 0x50 (ghidra: ptr inside sub1; copied raw as part of the block)
    uint16_t unk_58;       // 0x58
    uint8_t unk_5a;        // 0x5a (gap)
    uint8_t unk_5b;        // 0x5b
    uint8_t unk_5c[4];     // 0x5c (gap)
    uint64_t padding_2[2]; //just pointers to self (0x60)
    uint16_t unk_70;       // 0x70
    uint16_t unk_72;       // 0x72
    uint32_t unk_74;       // 0x74
    uint64_t padding_3; //pointer to FileCap, const ptr (0x78)
    void* unk1; //pointer to FrpgPhysSysIns. Do we need to save? (0x80)
    uint64_t padding_4[2]; //just pointers to self (0x88)
    uint16_t unk_98;       // 0x98
    uint16_t unk_9a;       // 0x9a
    uint32_t unk_9c;       // 0x9c
    uint64_t padding_5; //pointer to FileCap, const ptr (0xa0)
    void* unk2; //pointer to FrpgPhysSysIns. Do we need to save? (0xa8)
    uint8_t unk_b0;        // 0xb0
    uint8_t BackReadState; // 0xb1
    uint8_t TargetBackreadState; // 0xb2
    uint8_t unk_b3[5];     // 0xb3 (gap)
    uint64_t padding_6; //dbgnode (0xb8)
    uint8_t IsDispHitRigid; // 0xc0
    uint8_t unk_c1[7];     // 0xc1 (gap)
};

static_assert(offsetof(HitIns, unk_8) == 8);
static_assert(offsetof(HitIns, unk_18) == 0x18);
static_assert(offsetof(HitIns, areaId) == 0x1c);
static_assert(offsetof(HitIns, unk_50) == 0x50);
static_assert(offsetof(HitIns, padding_2) == 0x60);
static_assert(offsetof(HitIns, unk_70) == 0x70);
static_assert(offsetof(HitIns, unk1) == 0x80);
static_assert(offsetof(HitIns, unk_98) == 0x98);
static_assert(offsetof(HitIns, unk2) == 0xa8);
static_assert(offsetof(HitIns, unk_b0) == 0xb0);
static_assert(offsetof(HitIns, IsDispHitRigid) == 0xc0);
static_assert(sizeof(HitIns) == 0xc8);

struct HavokChara
{
    uint8_t RotAngleUnkWep[16];        // 0x0 (ghidra: Chr_RotAngleUnkWep_data)
    float current_coords[4];           // 0x10 (ghidra: Vector4)
    float current_coords_readonly[4];  // 0x20
    uint8_t unk_30;                    // 0x30
    uint8_t unk_31;                    // 0x31
    uint8_t isLanded;                  // 0x32 (ghidra: bool)
    uint8_t unk_33;                    // 0x33
    uint8_t is_sliding;               // 0x34 (ghidra: bool)
    uint8_t unk_35[3];                // 0x35 (gap)
    hkpCharacterProxy* char_proxy;     // 0x38
    void* capsule_shape_1;             // 0x40 (ghidra: hkpCapsuleShape*; not copied)
    void* capsule_shape_2;             // 0x48 (not copied)
    FrpgPhysShapePhantomIns* physShapePhantomIns_1; // 0x50 (not copied)
    FrpgPhysShapePhantomIns* physShapePhantomIns_2; // 0x58 (not copied)
    uint64_t unk_60;                  // 0x60
    uint32_t unk_68;                  // 0x68
    uint8_t unk_6c;                   // 0x6c
    uint8_t unk_6d;                   // 0x6d
    uint8_t unk_6e[2];                // 0x6e (gap)
    float movement_delta[4];          // 0x70 (ghidra: float[4])
    float unk_80[4];                  // 0x80
    float unk_90[4];                  // 0x90
    float unk_a0[4];                  // 0xa0
    float unk_b0[4];                  // 0xb0 (ghidra: Vector4)
    uint32_t unk_c0[8];               // 0xc0 (undefined4/float run)
    float fall_height;                // 0xe0
    float attach_height;              // 0xe4
    float attach_height_no_slope_material; // 0xe8
    uint8_t unk_ec;                   // 0xec
    uint8_t unk_ed;                   // 0xed
    uint8_t unk_ee;                   // 0xee
    uint8_t unk_ef;                   // 0xef
    float unk_f0;                     // 0xf0
    uint8_t unk_f4[4];                // 0xf4 (gap)
    uint64_t padding_1;               // 0xf8 (ghidra: chrInsProxyListen ptr; not copied)
    uint32_t unk_100;                 // 0x100
    uint8_t unk_104[12];              // 0x104 (gap)
    float last_ground_coords[4];      // 0x110 (ghidra: Vector4)
    float slopeInfo[4];               // 0x120 (ghidra: float[4])
    float unk_130[3];                 // 0x130
    uint8_t unk_13c[8];               // 0x13c (gap)
    float unk_144;                    // 0x144
    uint8_t unk_148[8];               // 0x148 (gap)
    float unk_150[3];                 // 0x150
    uint8_t unk_15c[20];              // 0x15c (gap)
    int32_t unk_170[4];               // 0x170
    uint32_t unk_180[3];              // 0x180
    float unk_18c[14];                // 0x18c (incl ghidra time_spent_falling@0x1b0)
    uint8_t unk_1c4[8];               // 0x1c4 (gap)
    uint32_t unk_1cc[4];              // 0x1cc
    uint32_t hitMtrlType;             // 0x1dc
    float unk_1e0[2];                 // 0x1e0
    HitIns* padding_hitIns;           // 0x1e8 (not copied)
    uint8_t unk_1f0[4];               // 0x1f0
    uint8_t unable_to_fall;           // 0x1f4 (ghidra: bool)
    uint8_t unk_1f5;                  // 0x1f5
    uint8_t rotate_yaxis_center_appearance; // 0x1f6 (ghidra: bool)
    uint8_t rotate_yaxis_center_direction;  // 0x1f7 (ghidra: bool)
    uint8_t no_integrate;             // 0x1f8 (ghidra: bool)
    uint8_t unk_1f9;                  // 0x1f9
    uint8_t unk_1fa;                  // 0x1fa
    uint8_t unk_1fb[5];               // 0x1fb (gap)
    float unk_200[4];                 // 0x200 (ghidra: Vector4)
    float unk_210[4];                 // 0x210
    float unk_220[4];                 // 0x220
    float unk_230[4];                 // 0x230
    uint8_t unk_240;                  // 0x240
    uint8_t m_bDbgDrawProxyPos;       // 0x241 (ghidra: bool)
    uint8_t unk_242[6];               // 0x242 (gap)
    uint64_t padding_2;               // 0x248 (ghidra: HavokChara_DbgNode ptr; not copied)
    void* padding_3;                  // 0x250 (points to an offset into PlayerIns; not copied)
    uint64_t unk_258;                 // 0x258
    float unk_260[4];                 // 0x260
    uint64_t unk_270;                 // 0x270
    uint64_t unk_278;                 // 0x278
    float unk_280;                    // 0x280
    uint8_t draw_havokskel_normal;    // 0x284 (ghidra: bool)
    uint8_t unk_285[3];               // 0x285 (gap)
    uint64_t unk_288;                 // 0x288
};

static_assert(offsetof(HavokChara, current_coords) == 0x10);
static_assert(offsetof(HavokChara, char_proxy) == 0x38);
static_assert(offsetof(HavokChara, unk_60) == 0x60);
static_assert(offsetof(HavokChara, movement_delta) == 0x70);
static_assert(offsetof(HavokChara, fall_height) == 0xe0);
static_assert(offsetof(HavokChara, padding_1) == 0xf8);
static_assert(offsetof(HavokChara, unk_100) == 0x100);
static_assert(offsetof(HavokChara, last_ground_coords) == 0x110);
static_assert(offsetof(HavokChara, slopeInfo) == 0x120);
static_assert(offsetof(HavokChara, hitMtrlType) == 0x1dc);
static_assert(offsetof(HavokChara, padding_hitIns) == 0x1e8);
static_assert(offsetof(HavokChara, unk_1f0) == 0x1f0);
static_assert(offsetof(HavokChara, padding_2) == 0x248);
static_assert(offsetof(HavokChara, unk_258) == 0x258);
static_assert(sizeof(HavokChara) == 0x290);

struct hkpCharacterProxy
{
    uint64_t padding_0;    // 0x0 (vtable1)
    uint32_t unk_8;        // 0x8
    uint32_t unk_c;        // 0xc (gap)
    uint64_t padding_1[2]; // 0x10 (vtable2, vtable3)
    void* field0x20; //ghidra: m_manifold. Ignore this, seems to not be important.
    uint32_t field0x20_len;
    uint32_t field0x20_cap;
    void** field0x30; //ghidra: m_bodies. Ignore this, seems to not be important.
    uint32_t field0x30_len;
    uint32_t field0x30_cap;
    void* field0x40; //ghidra: m_phantoms. each elem is ?? size. This appears to be unused
    uint32_t field0x40_len;
    uint32_t field0x40_cap;
    void* field0x50; //ghidra: m_overlappingTriggerVolumes. each elem is ?? size. This appears to be unused
    uint32_t field0x50_len;
    uint32_t field0x50_cap;
    float m_velocity[4];        // 0x60
    float m_oldDisplacement[4]; // 0x70
    void* HkpSimpleShapePhantom; // 0x80
    float m_dynamicFriction;    // 0x88
    float m_staticFriction;     // 0x8c
    float m_up[4];              // 0x90
    float m_extraUpStaticFriction;   // 0xa0
    float m_extraDownStaticFriction; // 0xa4
    float m_keepDistance;       // 0xa8
    float m_keepContactTolerance; // 0xac
    float m_contactAngleSensitivity; // 0xb0
    int32_t m_userPlanes;       // 0xb4
    float m_maxCharacterSpeedForSolver; // 0xb8
    float m_characterStrength;  // 0xbc
    float m_characterMass;      // 0xc0
    uint32_t unk_c4;            // 0xc4 (gap)
    void** field0xc8; //ghidra: m_listeners. Ignore this, seems to not be important.
    uint32_t field0xc8_len;
    uint32_t field0xc8_cap;
    float m_maxSlopeCosine;     // 0xd8
    float m_penetrationRecoverySpeed; // 0xdc
    int32_t m_maxCastIterations; // 0xe0
    uint8_t m_refreshManifoldInCheckSupport; // 0xe4 (ghidra: bool)
    uint8_t unk_e5[11];         // 0xe5 (gap)
};

static_assert(offsetof(hkpCharacterProxy, unk_8) == 0x8);
static_assert(offsetof(hkpCharacterProxy, field0x20) == 0x20);
static_assert(offsetof(hkpCharacterProxy, field0x30) == 0x30);
static_assert(offsetof(hkpCharacterProxy, field0x40) == 0x40);
static_assert(offsetof(hkpCharacterProxy, field0x50) == 0x50);
static_assert(offsetof(hkpCharacterProxy, m_velocity) == 0x60);
static_assert(offsetof(hkpCharacterProxy, HkpSimpleShapePhantom) == 0x80);
static_assert(offsetof(hkpCharacterProxy, m_dynamicFriction) == 0x88);
static_assert(offsetof(hkpCharacterProxy, field0xc8) == 0xc8);
static_assert(offsetof(hkpCharacterProxy, m_maxSlopeCosine) == 0xd8);
static_assert(sizeof(hkpCharacterProxy) == 0xf0);

struct EzStateRegister
{
    uint8_t data_0[0x10];
};

static_assert(sizeof(EzStateRegister) == 0x10);

struct EzStateRegisterSet
{
    uint64_t padding_0[2];
    EzStateRegister* arry;
    uint64_t arry_cur;
    uint64_t arry_end;
};

static_assert(sizeof(EzStateRegisterSet) == 0x28);

struct EzState_detail_EzStateMachineImpl
{
    uint8_t padding_0[0x18];
    uint8_t data_0[16]; //the cur_state pointer in this is just a pointer to const data
    uint64_t padding_1;
    uint64_t unk2; //always null?
    uint64_t padding_unk3[4]; //always null?
    EzStateRegisterSet EzStateRegisterSet1;
    uint64_t unk4; //always null?
    EzStateRegisterSet EzStateRegisterSet2;
    uint8_t data_1[16];
    uint64_t padding_unk5[4]; //nothing in the class it points to
    uint64_t padding_MessageQueue[6]; //always null?
    uint64_t padding_ActionEnv;
    uint8_t data_2[24];
};

static_assert(offsetof(EzState_detail_EzStateMachineImpl, data_0) == 0x18);
static_assert(offsetof(EzState_detail_EzStateMachineImpl, padding_1) == 0x28);
static_assert(offsetof(EzState_detail_EzStateMachineImpl, unk2) == 0x30);
static_assert(offsetof(EzState_detail_EzStateMachineImpl, padding_unk3) == 0x38);
static_assert(offsetof(EzState_detail_EzStateMachineImpl, EzStateRegisterSet1) == 0x58);
static_assert(offsetof(EzState_detail_EzStateMachineImpl, unk4) == 0x80);
static_assert(offsetof(EzState_detail_EzStateMachineImpl, EzStateRegisterSet2) == 0x88);
static_assert(offsetof(EzState_detail_EzStateMachineImpl, data_1) == 0xb0);
static_assert(offsetof(EzState_detail_EzStateMachineImpl, padding_unk5) == 0xc0);
static_assert(offsetof(EzState_detail_EzStateMachineImpl, padding_MessageQueue) == 0xe0);
static_assert(offsetof(EzState_detail_EzStateMachineImpl, padding_ActionEnv) == 0x110);
static_assert(offsetof(EzState_detail_EzStateMachineImpl, data_2) == 0x118);
static_assert(sizeof(EzState_detail_EzStateMachineImpl) == 0x130);

struct ActionCtrl_0x30Substruct
{
    EzState_detail_EzStateMachineImpl* EzStateMachineImpl;  // 0x0
    uint8_t actionIsValid;     // 0x8 (ghidra: bool)
    uint8_t unk_9[3];          // 0x9 (gap)
    int32_t animType;          // 0xc
    uint32_t count;            // 0x10
    uint16_t unk_14;           // 0x14
    uint8_t unk_16;            // 0x16
    uint8_t unk_17;            // 0x17 (gap)
    uint32_t unk_18;           // 0x18
    uint32_t unk_1c;           // 0x1c (gap)
};

static_assert(offsetof(ActionCtrl_0x30Substruct, EzStateMachineImpl) == 0);
static_assert(offsetof(ActionCtrl_0x30Substruct, actionIsValid) == 8);
static_assert(offsetof(ActionCtrl_0x30Substruct, animType) == 0xc);
static_assert(sizeof(ActionCtrl_0x30Substruct) == 0x20);

struct ChrIns_AnimationMediatorStateInfo
{
    int32_t animationId;
    uint32_t stateIndex;
};

static_assert(offsetof(ChrIns_AnimationMediatorStateInfo, animationId) == 0x0);
static_assert(offsetof(ChrIns_AnimationMediatorStateInfo, stateIndex) == 0x4);
static_assert(sizeof(ChrIns_AnimationMediatorStateInfo) == 0x8);

struct ActionCtrl
{
    uint8_t padding_0[8];      // 0x0 (vtable)
    uint64_t unk_8;            // 0x8 (former data_0)
    ChrCtrl* chrctrl_parent;   // 0x10
    uint8_t padding_1[24];     // 0x18 (ghidra: esdResCap/esdStateEvent/esdStateEnv ptrs; not copied)
    ActionCtrl_0x30Substruct passive_state;  // 0x30
    ActionCtrl_0x30Substruct active_state;   // 0x50
    uint32_t ezStatePassiveState; // 0x70
    uint32_t ezStateActiveState;  // 0x74
    ChrIns_AnimationMediatorStateInfo upperBodyStateInfo; // 0x78
    ChrIns_AnimationMediatorStateInfo lowerBodyStateInfo; // 0x80
    ChrIns_AnimationMediatorStateInfo secondaryStateInfo; // 0x88
    uint8_t unk_90[0x154];     // 0x90 (bitfields + ChrManipulator_ActionInputted blobs)
    uint32_t recievedDamageType; // 0x1e4
    uint32_t damage_direction; // 0x1e8
    uint32_t dmgLv_cur;        // 0x1ec
    uint32_t guardLv_cur;      // 0x1f0
    uint32_t atkAttr_cur;      // 0x1f4
    uint32_t spAttr_cur;       // 0x1f8
    uint8_t unk_1fc[0x2e4];    // 0x1fc (throw allow types, Action_* input blobs, ActionInputs_TimeHeld, etc.)
    float throw_position[4];   // 0x4e0 (ghidra: Vector4)
    uint8_t unk_4f0[0x38];     // 0x4f0 (throwrotation etc.)
    int32_t actionType;        // 0x528
    uint8_t unk_52c[0x14];     // 0x52c
    uint8_t padding_2[8];      // 0x540 (ghidra: dbgNode ptr; not copied)
    uint8_t unk_548;           // 0x548
    uint8_t map_action_test_is_valid; // 0x549 (ghidra: bool)
    uint8_t unk_54a[2];        // 0x54a (gap)
    int32_t actionNumber;      // 0x54c
    uint32_t ezState;          // 0x550
    uint8_t unk_554[12];       // 0x554 (gap)
};

static_assert(offsetof(ActionCtrl, passive_state) == 0x30);
static_assert(offsetof(ActionCtrl, active_state) == 0x50);
static_assert(offsetof(ActionCtrl, ezStatePassiveState) == 0x70);
static_assert(offsetof(ActionCtrl, recievedDamageType) == 0x1e4);
static_assert(offsetof(ActionCtrl, throw_position) == 0x4e0);
static_assert(offsetof(ActionCtrl, actionType) == 0x528);
static_assert(offsetof(ActionCtrl, padding_2) == 0x540);
static_assert(offsetof(ActionCtrl, actionNumber) == 0x54c);
static_assert(sizeof(ActionCtrl) == 0x560);

struct WalkAnim_Twist_Field0x228Elem_field0x10elem
{
    uint32_t unk_0;   // 0x0
    uint32_t unk_4;   // 0x4
    uint32_t unk_8;   // 0x8
    uint32_t unk_c;   // 0xc
};

static_assert(sizeof(WalkAnim_Twist_Field0x228Elem_field0x10elem) == 0x10);

struct WalkAnim_Twist_Field0x228Elem
{
    uint64_t padding_1[2]; //pointer to unchanging values
    WalkAnim_Twist_Field0x228Elem_field0x10elem** field0x10;
    uint64_t field0x10_cap;
    uint64_t unk;
    uint64_t field0x10_len;
};

static_assert(offsetof(WalkAnim_Twist_Field0x228Elem, field0x10) == 0x10);
static_assert(sizeof(WalkAnim_Twist_Field0x228Elem) == 0x30);

struct WalkAnim_Twist
{
    uint8_t padding_0[8];      // 0x0 (vtable)
    uint64_t unk_8;            // 0x8 (former data_0)
    uint8_t padding_1[8];      // 0x10 (ghidra: parent_player ptr; not copied)
    uint32_t walkF;            // 0x18
    uint32_t dashF;            // 0x1c
    uint32_t unk_20[2];        // 0x20
    uint32_t walkB;            // 0x28
    uint32_t dashB;            // 0x2c
    uint32_t unk_30[2];        // 0x30
    uint32_t walkL;            // 0x38
    uint32_t dashL;            // 0x3c
    uint32_t unk_40[2];        // 0x40
    uint32_t walkR;            // 0x48
    uint32_t dashR;            // 0x4c
    uint32_t unk_50[2];        // 0x50
    uint32_t analogWalkF;      // 0x58
    uint32_t analogDashF;      // 0x5c
    uint32_t unk_60[2];        // 0x60
    uint32_t analogWalkB;      // 0x68
    uint32_t analogDashB;      // 0x6c
    uint32_t unk_70[2];        // 0x70
    uint32_t analogWalkL;      // 0x78
    uint32_t analogDashL;      // 0x7c
    uint32_t unk_80[2];        // 0x80
    uint32_t analogWalkR;      // 0x88
    uint32_t analogDashR;      // 0x8c
    uint32_t unk_90[2];        // 0x90
    int32_t stayAnimId;        // 0x98
    uint32_t unk_9c;           // 0x9c
    int32_t superDashAnimId;   // 0xa0
    int32_t analogSuperDashAnimId; // 0xa4
    int32_t flyWalkF;          // 0xa8
    uint8_t unk_ac[12];        // 0xac
    int32_t flyWalkFL;         // 0xb8
    uint8_t unk_bc[12];        // 0xbc
    int32_t flyWalkFR;         // 0xc8
    uint8_t unk_cc[12];        // 0xcc
    int32_t flyWalkFL2;        // 0xd8
    uint8_t unk_dc[12];        // 0xdc
    int32_t flyWalkFR2;        // 0xe8
    uint8_t unk_ec[76];        // 0xec (gap)
    int32_t flyStay;           // 0x138
    uint8_t unk_13c[100];      // 0x13c (floats region)
    ChrIns_AnimationMediatorStateInfo stateInfo;  // 0x1a0
    ChrIns_AnimationMediatorStateInfo stateInfo2; // 0x1a8
    uint8_t unk_1b0;           // 0x1b0
    uint8_t unk_1b1[3];        // 0x1b1 (gap)
    uint32_t moveParamRowId;   // 0x1b4
    float unk_1b8;             // 0x1b8
    float running_time_to_turn;// 0x1bc
    uint32_t unk_1c0;          // 0x1c0
    uint8_t unk_1c4[12];       // 0x1c4 (gap)
    uint8_t padding_2[8];      // 0x1d0 (ghidra: dbgNode ptr; not copied)
    uint64_t unk_1d8;          // 0x1d8
    ChrIns_AnimationMediatorStateInfo animStateInfo; // 0x1e0
    SpinJoint* Upper_Root_Joint;  // 0x1e8
    SpinJoint* master_joint;      // 0x1f0
    SpinJoint* neck_joint;        // 0x1f8
    uint8_t turn_lower_body;   // 0x200 (ghidra: bool)
    uint8_t unk_201[3];        // 0x201 (gap)
    float unk_204;             // 0x204
    float turn_gain;           // 0x208
    float forward_limit_angle; // 0x20c
    float forward_margin_angle;// 0x210
    float back_limit_angle;    // 0x214
    float back_margin_angle;   // 0x218
    float dash_threshold;      // 0x21c
    float dash_threshold_margin; // 0x220
    uint32_t unk_224;          // 0x224 (gap)
    WalkAnim_Twist_Field0x228Elem walkAnim_Twist_Field0x228Elem;  // 0x228
    uint8_t unk_258[8];        // 0x258 (gap)
    float super_dash_counter;  // 0x260
    float limit_super_dash_counter; // 0x264
    float unk_268;             // 0x268
    float unk_26c;             // 0x26c
    float dash_to_walk_limit_time; // 0x270
    uint32_t unk_274;          // 0x274
    float unk_278;             // 0x278
    uint32_t unk_27c;          // 0x27c
    float unk_280;             // 0x280
    uint32_t unk_284;          // 0x284 (gap)
    float upperBody_rotation_rate; // 0x288
    float unk_28c;             // 0x28c
    float max_equip_load;      // 0x290
    uint8_t unk_294[12];       // 0x294 (gap)
};

static_assert(offsetof(WalkAnim_Twist, walkF) == 0x18);
static_assert(offsetof(WalkAnim_Twist, stayAnimId) == 0x98);
static_assert(offsetof(WalkAnim_Twist, flyStay) == 0x138);
static_assert(offsetof(WalkAnim_Twist, stateInfo) == 0x1a0);
static_assert(offsetof(WalkAnim_Twist, padding_2) == 0x1d0);
static_assert(offsetof(WalkAnim_Twist, Upper_Root_Joint) == 0x1e8);
static_assert(offsetof(WalkAnim_Twist, turn_lower_body) == 0x200);
static_assert(offsetof(WalkAnim_Twist, walkAnim_Twist_Field0x228Elem) == 0x228);
static_assert(offsetof(WalkAnim_Twist, super_dash_counter) == 0x260);
static_assert(sizeof(WalkAnim_Twist) == 0x2a0);

struct ChrCtrl
{
    uint8_t padding_0[8];      // 0x0 (vtable)
    uint64_t unk_8;            // 0x8 (former data_0)
    ChrIns* chrins;            // 0x10 (ghidra: EntityIns)
    ChrCtrl_AnimationQueue* animationQueue;  // 0x18
    AnimationMediator* animationMediator;    // 0x20
    HavokChara* havokChara;    // 0x28
    uint8_t padding_2[24];     // 0x30 (ghidra: RagDoll/hkxPwv/aniSkl ptrs; not copied)
    ActionCtrl* actionctrl;    // 0x48
    uint8_t padding_3[0x30];   // 0x50 (ghidra: chrTaeAnimEvent ptr + name string; not copied)
    uint64_t unk_80;           // 0x80 (former data_1)
    uint8_t padding_4[8]; //this is just another pointer to the padMan, can ignore (0x88)
    uint32_t unk_90;           // 0x90
    uint32_t unk_94;           // 0x94
    uint8_t unk_98[8];         // 0x98 (gap)
    uint8_t FollowupBullet[48];// 0xa0 (ghidra: BulletIns_FollowupBullet)
    float unk_d0[12];          // 0xd0
    uint8_t enable;            // 0x100 (ghidra: byte)
    uint8_t unk_101[2];        // 0x101 (gap)
    uint8_t CompletelyNoMove;  // 0x103 (ghidra: byte)
    uint32_t unk_104;          // 0x104
    uint8_t WarpActivate;      // 0x108 (ghidra: byte)
    uint8_t unk_109[3];        // 0x109 (gap)
    uint32_t unk_10c[13];      // 0x10c (undefined4/float run)
    uint8_t movementActionDeltas[48]; // 0x140 (ghidra: MovementActionDeltas)
    uint8_t unk_170[56];       // 0x170 (floats + DrawRagdoll flags)
    WalkAnim_Twist* walkAnim_Twist;  // 0x1a8
    uint8_t movement_enabled;  // 0x1b0 (ghidra: bool)
    uint8_t is_allow_turn_during_attack; // 0x1b1 (ghidra: bool)
    uint8_t unk_1b2[2];        // 0x1b2 (gap)
    uint32_t foot_ik_adjustment; // 0x1b4
    uint8_t unk_1b8[8];        // 0x1b8
    uint32_t MoveParamIDs[20]; // 0x1c0 (ghidra: uint32_t[20])
    uint32_t MoveType;         // 0x210
    uint8_t unk_214[28];       // 0x214 (floats/flags incl damageTotalTime/damageDecTime)
    uint8_t unk_230[96];       // 0x230 (mostly undefined + ChrCtrl_field0x258_struct)
    uint8_t padding_5[16];     // 0x290 (ghidra: dbgNode + DbgChrManipulator ptrs; not copied)
    uint16_t MapHitDisableDebugging; // 0x2a0
    uint8_t is_allowed_move;   // 0x2a2 (ghidra: bool)
    uint8_t unk_2a3;           // 0x2a3
    uint32_t dmgLv_max;        // 0x2a4
    uint32_t guardLv_max;      // 0x2a8
    uint32_t atkAttr_max;      // 0x2ac
    uint32_t spAttr_max;       // 0x2b0
    float knockback_speed;     // 0x2b4
    float knockback_time;      // 0x2b8
    float knockback_decrease_time; // 0x2bc
    uint8_t unk_2c0[64];       // 0x2c0 (damage_level/guard_level/is_on_fire/anim_test_id etc.)
};

static_assert(offsetof(ChrCtrl, animationQueue) == 0x18);
static_assert(offsetof(ChrCtrl, animationMediator) == 0x20);
static_assert(offsetof(ChrCtrl, havokChara) == 0x28);
static_assert(offsetof(ChrCtrl, unk_80) == 0x80);
static_assert(offsetof(ChrCtrl, unk_90) == 0x90);
static_assert(offsetof(ChrCtrl, movementActionDeltas) == 0x140);
static_assert(offsetof(ChrCtrl, walkAnim_Twist) == 0x1a8);
static_assert(offsetof(ChrCtrl, movement_enabled) == 0x1b0);
static_assert(offsetof(ChrCtrl, MoveParamIDs) == 0x1c0);
static_assert(offsetof(ChrCtrl, padding_5) == 0x290);
static_assert(offsetof(ChrCtrl, MapHitDisableDebugging) == 0x2a0);
static_assert(offsetof(ChrCtrl, dmgLv_max) == 0x2a4);
static_assert(sizeof(ChrCtrl) == 0x300);

struct TurnAnim
{
    uint8_t padding_0[8];      // 0x0 (vtable)
    uint64_t unk_8;            // 0x8
    uint8_t padding_1[8];      // 0x10 (ghidra: parent_playerctrl ptr; not copied)
    uint32_t turnL;            // 0x18
    uint32_t turnR;            // 0x1c
    uint32_t largeTurnL;       // 0x20
    uint32_t largeTurnR;       // 0x24
    uint32_t turn_right_animID;// 0x28
    uint32_t unk_2c;           // 0x2c
    uint32_t unk_30;           // 0x30
    uint32_t unk_34;           // 0x34
    uint32_t unk_38;           // 0x38 (gap)
    float rotation_speed;      // 0x3c
    SpinJoint* joint_UpperRoot;   // 0x40
    SpinJoint* joint_LowerRoot;   // 0x48
    SpinJoint* joint_spine1_1;    // 0x50
    SpinJoint* joint_spine_1;     // 0x58
    SpinJoint* joint_master;      // 0x60
    uint8_t padding_2[8];      // 0x68 (ghidra: comboSpinJoints ptr; not copied)
    float unk_70;              // 0x70
    float playerRotation;      // 0x74
    float unk_78;              // 0x78
    float unk_7c;              // 0x7c
    float master_rotation_gain;// 0x80
    float unk_84;              // 0x84
    uint8_t unk_88;            // 0x88
    uint8_t unk_89[3];         // 0x89 (gap)
    float unk_8c;              // 0x8c
    float reference_frames_for_turn_anim[30]; // 0x90 (ghidra: float[30])
    float unk_108;             // 0x108
    float unk_10c;             // 0x10c
    uint8_t unk_110[112];      // 0x110 (gap)
    uint32_t next_ref_frame_for_turn_anim;     // 0x180
    int32_t reference_frames_for_turn_anim_len;// 0x184
    uint8_t draw_direction;    // 0x188 (ghidra: bool)
    uint8_t unk_189[7];        // 0x189 (gap)
    uint8_t padding_3[8];      // 0x190 (ghidra: dbgNode ptr; not copied)
    uint64_t unk_198;          // 0x198
};

static_assert(offsetof(TurnAnim, unk_8) == 0x8);
static_assert(offsetof(TurnAnim, turnL) == 0x18);
static_assert(offsetof(TurnAnim, rotation_speed) == 0x3c);
static_assert(offsetof(TurnAnim, joint_UpperRoot) == 0x40);
static_assert(offsetof(TurnAnim, unk_70) == 0x70);
static_assert(offsetof(TurnAnim, reference_frames_for_turn_anim) == 0x90);
static_assert(offsetof(TurnAnim, padding_3) == 0x190);
static_assert(offsetof(TurnAnim, unk_198) == 0x198);
static_assert(sizeof(TurnAnim) == 0x1a0);

struct SpinJoint
{
    uint8_t padding_0[8];      // 0x0 (vtable)
    uint64_t unk_8;            // 0x8
    uint8_t padding_1[8];      // 0x10 (ghidra: ChrCtrlParent_AnimationQueue ptr; not copied)
    uint32_t spin_bone_index;  // 0x18
    uint32_t axis_bone_index;  // 0x1c
    uint32_t unk_20;           // 0x20
    uint8_t unk_24[12];        // 0x24 (gap)
    float gain;                // 0x30
    uint8_t unk_34[12];        // 0x34 (gap)
    uint32_t unk_40[14];       // 0x40 (undefined4/float run)
    uint8_t padding_2[8];      // 0x78 (ghidra: dbgNode ptr; not copied)
    uint8_t disableUpdate;     // 0x80 (ghidra: bool)
    uint8_t unk_81[7];         // 0x81 (gap)
    uint8_t padding_3[8];      // 0x88 (ghidra: joint_name ptr; not copied)
};

static_assert(offsetof(SpinJoint, unk_8) == 8);
static_assert(offsetof(SpinJoint, padding_1) == 0x10);
static_assert(offsetof(SpinJoint, spin_bone_index) == 0x18);
static_assert(offsetof(SpinJoint, gain) == 0x30);
static_assert(offsetof(SpinJoint, padding_2) == 0x78);
static_assert(offsetof(SpinJoint, disableUpdate) == 0x80);
static_assert(offsetof(SpinJoint, padding_3) == 0x88);
static_assert(sizeof(SpinJoint) == 0x90);

struct ArrowTurnAnim
{
    TurnAnim turnAnim;         // 0x0 (ghidra: base TurnAnim)
    uint8_t padding_0[8];      // 0x1a0 (ghidra: comboSpinJoints_arrow ptr; not copied)
    SpinJoint* joint_spine_2;  // 0x1a8
    SpinJoint* joint_spine1_2; // 0x1b0
    uint8_t unk_1b8[4];        // 0x1b8
    uint32_t unk_1bc;          // 0x1bc (gap)
};

static_assert(offsetof(ArrowTurnAnim, padding_0) == 0x1a0);
static_assert(offsetof(ArrowTurnAnim, joint_spine_2) == 0x1a8);
static_assert(offsetof(ArrowTurnAnim, joint_spine1_2) == 0x1b0);
static_assert(offsetof(ArrowTurnAnim, unk_1b8) == 0x1b8);
static_assert(sizeof(ArrowTurnAnim) == 0x1c0);

struct PlayerCtrl
{
    ChrCtrl chrCtrl;           // 0x0
    uint32_t unk_300;          // 0x300
    uint32_t unk_304;          // 0x304
    TurnAnim* turnAnim;        // 0x308
    ArrowTurnAnim* arrowTurnAnim; // 0x310
    uint8_t padding_0[24];     // 0x318 (ghidra: footIK ptrs; not copied)
    float unk_330;             // 0x330
    uint16_t unk_334;          // 0x334
    uint16_t unk_336;          // 0x336
    uint8_t padding_1[32];     // 0x338 (ghidra: footIK/raycast ptrs; not copied)
    uint8_t movement_related_flags; // 0x358
    uint8_t unk_359[3];        // 0x359 (gap)
    uint32_t recordCtrl_fileNum; // 0x35c
    uint8_t feetIK;            // 0x360 (ghidra: bool)
    uint8_t unk_361[3];        // 0x361 (gap)
    float damping_force_without_ik; // 0x364
    uint32_t feetIK_InterpolationType_ForBonfire; // 0x368
    uint32_t unk_36c;          // 0x36c
};

static_assert(offsetof(PlayerCtrl, unk_300) == 0x300);
static_assert(offsetof(PlayerCtrl, turnAnim) == 0x308);
static_assert(offsetof(PlayerCtrl, arrowTurnAnim) == 0x310);
static_assert(offsetof(PlayerCtrl, padding_0) == 0x318);
static_assert(offsetof(PlayerCtrl, unk_330) == 0x330);
static_assert(offsetof(PlayerCtrl, padding_1) == 0x338);
static_assert(offsetof(PlayerCtrl, movement_related_flags) == 0x358);
static_assert(sizeof(PlayerCtrl) == 0x370);


struct ItemUsed
{
    int32_t itemId;
    int32_t amountUsed;
};

static_assert(offsetof(ItemUsed, itemId) == 0x0);
static_assert(offsetof(ItemUsed, amountUsed) == 0x4);
static_assert(sizeof(ItemUsed) == 0x8);

struct SpecialEffect_Info
{
    union
    {
        uint8_t data_0[0x38];
        struct SpecialEffect_Info_data_0
        {
            float life;
            float intervalLength;
            float intervalCountdownTimer;
            float unk0;
            float unk1;
            uint32_t unk2;
            uint8_t unk3;
            uint8_t unk4;
            uint8_t unk5;
            uint8_t unk6;
            uint8_t unk7;
            uint8_t unk8;
            uint8_t unk9;
            uint8_t unk10;
            uint32_t target;
            uint32_t attacker;
            uint8_t unk11;
            bool isDead;
            uint8_t unk12;
            uint8_t unk13;
            uint32_t unk14;
            uint32_t speffect_id;
            uint32_t unk15;
        } data_0_struct;
    };
    uint64_t paramRowBytes; //this is a pointer to a const struct, so just read/write the pointer itself
    SpecialEffect_Info* next;
    SpecialEffect_Info* prev;
};

static_assert(offsetof(SpecialEffect_Info, data_0) == 0x0);
static_assert(offsetof(SpecialEffect_Info, paramRowBytes) == 0x38);
static_assert(offsetof(SpecialEffect_Info, next) == 0x40);
static_assert(offsetof(SpecialEffect_Info, prev) == 0x48);
static_assert(sizeof(SpecialEffect_Info) == 0x50);

struct SpecialEffect
{
    uint8_t padding_0[8];      // 0x0 (vtable)
    SpecialEffect_Info* specialEffect_Info;  // 0x8
    uint8_t speffectIsBeingRun; // 0x10 (ghidra: bool)
    uint8_t unk_11[3];         // 0x11 (gap)
    float unk_14;              // 0x14
    uint8_t padding_2[8];      // 0x18 (ghidra: PlayerIns_parent ptr; not copied)
    uint32_t flags;            // 0x20 (ghidra: flags1-10 bitfields)
    uint32_t unk_24;           // 0x24
    uint8_t padding_3[8];      // 0x28 (ghidra: dbgNode ptr; not copied)
    int32_t debugActivateSpEffect; // 0x30
    uint32_t unk_34;           // 0x34
};

static_assert(offsetof(SpecialEffect, specialEffect_Info) == 0x8);
static_assert(offsetof(SpecialEffect, speffectIsBeingRun) == 0x10);
static_assert(offsetof(SpecialEffect, flags) == 0x20);
static_assert(offsetof(SpecialEffect, debugActivateSpEffect) == 0x30);
static_assert(sizeof(SpecialEffect) == 0x38);

struct QwcSpEffectEquipCtrl
{
    uint8_t padding_0[0x18];  // 0x0 (vtable, ChrIns_parent, chrins_spEffectList ptrs; not copied)
    uint32_t* arry;           // 0x18 (ghidra: array)
    uint32_t arry_len;        // 0x20
    uint32_t unk_24;          // 0x24
    uint32_t unk_28;          // 0x28
    uint32_t unk_2c;          // 0x2c
};

static_assert(offsetof(QwcSpEffectEquipCtrl, arry) == 0x18);
static_assert(offsetof(QwcSpEffectEquipCtrl, arry_len) == 0x20);
static_assert(offsetof(QwcSpEffectEquipCtrl, unk_24) == 0x24);
static_assert(sizeof(QwcSpEffectEquipCtrl) == 0x30);

struct ChrAttachSys
{
    uint8_t padding_0[8];
    AttachSysSlotBaseImpl* SysSlots;
    uint8_t padding_1[8];
};

static_assert(sizeof(ChrAttachSys) == 0x18);

struct ThrowSelfEsc
{
    EntityThrowAnimationStatus* parent;  // 0x0
    float m_cycleTime;                 // 0x8
    float m_cycleRemainTime;           // 0xc
    uint8_t m_cycleInputCounterMax;    // 0x10
    uint8_t self_esc_dmg;              // 0x11
    uint8_t unk_12[2];                 // 0x12 (gap)
    float m_thresholdStickMaxDist;     // 0x14
    float m_thresholdStickMinDist;     // 0x18
    float m_currentDeclineRemainTime;  // 0x1c
    void* padding_0;                   // 0x20 (ghidra: dbgNode; not copied)
};

static_assert(offsetof(ThrowSelfEsc, m_cycleTime) == 0x8);
static_assert(offsetof(ThrowSelfEsc, padding_0) == 0x20);
static_assert(sizeof(ThrowSelfEsc) == 0x28);

struct EntityThrowAnimationStatus
{
    uint8_t padding_0[8];      // 0x0 (ghidra: dbgMenu ptr; not copied)
    uint64_t playerins_parent; // 0x8 (ghidra: chrIns_parent)
    uint64_t throw_paramdef; //this is a pointer to a const struct, so just read/write the pointer itself (ghidra: ThrowParamdefAndIDLink details) (0x10)
    uint32_t unk_18;           // 0x18 (tail of details)
    uint32_t unk_1c;           // 0x1c (gap)
    uint8_t throwState;        // 0x20 (ghidra: ThrowState_1Byte)
    uint8_t unk_21;            // 0x21
    uint16_t ThrowEscHp;       // 0x22
    uint32_t ThrowPairHandle;  // 0x24
    uint8_t unk_28;            // 0x28
    uint8_t m_needAdjustMoveToPairDmy; // 0x29 (ghidra: bool)
    uint8_t m_needAdjustTurnToPairDir; // 0x2a (ghidra: bool)
    uint8_t unk_2b[5];         // 0x2b (gap)
    uint32_t unk_30;           // 0x30
    uint32_t unk_34;           // 0x34
    uint32_t unk_38;           // 0x38
    float unk_3c;              // 0x3c
    uint32_t unk_40;           // 0x40
    uint32_t unk_44;           // 0x44
    uint32_t unk_48;           // 0x48
    uint32_t unk_4c;           // 0x4c
    uint8_t unk_50;            // 0x50
    uint8_t unk_51;            // 0x51
    uint8_t unk_52;            // 0x52
    uint8_t unk_53[5];         // 0x53 (gap)
    ThrowSelfEsc* throwSelfEsc; // 0x58
    uint32_t unk_60;           // 0x60
    float Desire_Weight_of_ResistanceAnim;  // 0x64
    float Current_Weight_of_ResistanceAnim; // 0x68
    uint8_t unk_6c[4];         // 0x6c (gap)
    float starting_position_self[4];   // 0x70
    float starting_position_self_[4];  // 0x80
    float starting_position_other[4];  // 0x90
    uint8_t m_dbgIsIgnoreNet;  // 0xa0
    uint8_t unk_a1[3];         // 0xa1 (gap)
    uint32_t throwMask;        // 0xa4
    uint8_t throwLoggingEnable; // 0xa8 (ghidra: bool)
    uint8_t unk_a9[3];         // 0xa9 (gap)
    uint32_t dbg_drawFlags;    // 0xac
};

static_assert(offsetof(EntityThrowAnimationStatus, throw_paramdef) == 0x10);
static_assert(offsetof(EntityThrowAnimationStatus, unk_18) == 0x18);
static_assert(offsetof(EntityThrowAnimationStatus, throwSelfEsc) == 0x58);
static_assert(offsetof(EntityThrowAnimationStatus, unk_60) == 0x60);
static_assert(offsetof(EntityThrowAnimationStatus, starting_position_self) == 0x70);
static_assert(sizeof(EntityThrowAnimationStatus) == 0xb0);

struct ChrIns_field0x2c8
{
    uint64_t padding_0;    // 0x0 (ghidra: NaviMeshParts1 ptr; not copied)
    uint32_t unk_8;        // 0x8
    uint32_t unk_c;        // 0xc
    float unk_10;          // 0x10
    float unk_14;          // 0x14
    uint32_t unk_18;       // 0x18
    uint8_t unk_1c[4];     // 0x1c (gap)
    uint64_t padding_1;    // 0x20 (ghidra: NaviMeshParts2 ptr; not copied)
    uint32_t unk_28;       // 0x28
    uint32_t unk_2c;       // 0x2c (gap)
    uint64_t padding_2;    // 0x30 (ghidra: pointer-to-stack-thing; not copied)
    uint32_t unk_38;       // 0x38
    uint8_t unk_3c[4];     // 0x3c (gap)
    uint64_t unk_40;       // 0x40
    uint64_t padding_3;    // 0x48 (ghidra: parent ChrIns*; not copied)
    uint64_t unk_50;       // 0x50
};

static_assert(offsetof(ChrIns_field0x2c8, unk_8) == 8);
static_assert(offsetof(ChrIns_field0x2c8, unk_28) == 0x28);
static_assert(offsetof(ChrIns_field0x2c8, unk_38) == 0x38);
static_assert(offsetof(ChrIns_field0x2c8, unk_50) == 0x50);
static_assert(sizeof(ChrIns_field0x2c8) == 0x58);

struct ChrIns_field0x18
{
    uint32_t throw_animId;    // 0x0
    uint32_t animId;          // 0x4
    uint8_t IsEnableAnimLoop; // 0x8 (ghidra: bool)
    uint8_t unk_9[0x77];      // 0x9 (remaining; ghidra: mostly undefined, packed)
};

static_assert(offsetof(ChrIns_field0x18, animId) == 0x4);
static_assert(sizeof(ChrIns_field0x18) == 0x80);

struct ChrIns
{
    uint8_t padding_0[8 + 0x18];
    uint64_t field0x18;
    uint8_t padding_0a[0x38];
    void* chrModel;
    PlayerCtrl* playerCtrl;
    union
    {
        PadManipulator* padManipulator;
        void* netManipulator;
    };
    uint8_t padding_1[0x54];
    int32_t CharaInitParamID;
    uint8_t padding_3[0x9c];
    ChrIns_AnimationMediatorStateInfo unk_16c;  // 0x16c
    ChrIns_AnimationMediatorStateInfo unk_174;  // 0x174
    ChrIns_AnimationMediatorStateInfo lowerThrowAnim;
    ChrIns_AnimationMediatorStateInfo upperThrowAnim;
    uint8_t padding_4a[12];
    uint32_t* player_handing_state;
    uint8_t padding_4b[176];
    float curToughness;
    float maxToughness;
    float toughnessUnk1;
    float toughnessRefillTimer;
    uint32_t toughnessUnk2;
    int32_t curSelectedMagicId;
    ItemUsed curUsedItem;
    uint32_t itemid;
    uint32_t override_equipped_magicId;
    SpecialEffect* specialEffects;
    QwcSpEffectEquipCtrl* qwcSpEffectEquipCtrl;
    uint32_t unk_288;          // 0x288
    float unk_28c;             // 0x28c
    uint8_t unk_290;           // 0x290
    uint8_t unk_291[7];        // 0x291 (gap)
    uint64_t unk_298;          // 0x298
    uint32_t unk_2a0;          // 0x2a0
    uint8_t unk_2a4;           // 0x2a4
    uint8_t unk_2a5;           // 0x2a5
    uint8_t unk_2a6;           // 0x2a6 (ghidra: inBowPrecisionShoot)
    uint8_t unk_2a7;           // 0x2a7 (ghidra: noDead)
    uint8_t unk_2a8;           // 0x2a8
    uint8_t unk_2a9;           // 0x2a9
    uint8_t unk_2aa;           // 0x2aa
    uint8_t unk_2ab;           // 0x2ab (gap)
    uint32_t unk_2ac;          // 0x2ac
    uint32_t entityId;         // 0x2b0
    uint8_t unk_2b4[12];       // 0x2b4 (gap)
    uint32_t unk_2c0;          // 0x2c0
    uint32_t unk_2c4;          // 0x2c4
    uint32_t unk_2c8;          // 0x2c8
    uint32_t unk_2cc;          // 0x2cc
    ChrIns_field0x2c8* field0x2c8;  // 0x2d0
    uint64_t unk_2d8;          // 0x2d8
    uint8_t unk_2e0[64];       // 0x2e0 (gap)
    uint32_t unk_320;          // 0x320
    float unk_324;             // 0x324
    float unk_328;             // 0x328
    float unk_32c;             // 0x32c
    float unk_330;             // 0x330
    uint32_t unk_334[8];       // 0x334
    uint32_t onlineAreaId;     // 0x354
    uint32_t areaId;           // 0x358
    uint16_t hitMtrlId;        // 0x35c
    uint16_t defenseMaterialId;    // 0x35e
    uint16_t defenseMaterialSfxId; // 0x360
    uint16_t unk_362;          // 0x362 (gap)
    uint8_t unk_364;           // 0x364
    uint8_t unk_365[3];        // 0x365 (gap)
    uint64_t unk_368;          // 0x368
    HitIns* hitins_1;          // 0x370
    HitIns* hitins_2;          // 0x378
    float unk_380;             // 0x380
    uint16_t unk_384;          // 0x384
    uint8_t unk_386;           // 0x386
    uint8_t unk_387;           // 0x387 (gap)
    ChrAttachSys chrattachsys; // 0x388
    uint8_t padding_7[0x48];
    uint32_t curHp;
    uint32_t maxHp;
    uint8_t padding_8[8];
    uint32_t curSp;
    uint32_t maxSp;
    float damage_taken_scalar;
    uint8_t padding_9[20];
    uint32_t PoisonResist;
    uint32_t ToxicResist;
    uint32_t BleedResist;
    uint32_t CurseResist;
    uint32_t resistPoisonTotal;
    uint32_t resistPlagueTotal;
    uint32_t resistBleedingTotal;
    uint32_t resistCurseTotal;
    uint8_t padding_10[0x10];
    EntityThrowAnimationStatus* throw_animation_info;
    float unk_450;             // 0x450
    uint32_t unk_454;          // 0x454 (gap)
    float frame_delta_amount;  // 0x458
    float frame_delta_amount2; // 0x45c
    uint8_t unk_460;           // 0x460
    uint8_t unk_461[3];        // 0x461 (gap)
    uint32_t unk_464;          // 0x464
    uint64_t padding_self;     // 0x468
    uint64_t unk_470;          // 0x470
    float unk_478;             // 0x478
    uint8_t unk_47c;           // 0x47c
    uint8_t unk_47d[3];        // 0x47d (gap)
    uint32_t unk_480;          // 0x480
    uint32_t unk_484;          // 0x484
    uint32_t unk_488;          // 0x488
    uint32_t unk_48c;          // 0x48c
    uint64_t unk_490;          // 0x490
    uint64_t unk_498;          // 0x498
    uint64_t unk_4a0;          // 0x4a0
    uint32_t timestamp_when_joined_session; // 0x4a8
    uint32_t frame_time_delta; // 0x4ac
    uint32_t unk_4b0;          // 0x4b0
    uint8_t MenuLockout;       // 0x4b4 (ghidra: bool)
    uint8_t unk_4b5[3];        // 0x4b5 (gap)
    uint64_t unk_4b8;          // 0x4b8
    uint64_t padding_dbgmenu[3]; // 0x4c0
    uint64_t unk_4d8;          // 0x4d8
    void* unk_4e0;             // 0x4e0 (ghidra: hitIns ptr; copied raw with the block)
    uint64_t unk_4e8;          // 0x4e8
    uint8_t padding_str[0x28]; // 0x4f0
    uint64_t unk_518;          // 0x518
    uint32_t unk_520;          // 0x520
    uint8_t unk_524;           // 0x524
    uint8_t unk_525;           // 0x525
    uint8_t chrReset_flag;     // 0x526
    uint8_t unk_527;           // 0x527
    uint64_t unk_528;          // 0x528
    uint64_t unk_530;          // 0x530
    uint64_t unk_538;          // 0x538
    uint64_t unk_540;          // 0x540
    uint32_t unk_548;          // 0x548
    uint32_t unk_54c;          // 0x54c (gap)
    uint64_t unk_550;          // 0x550
    uint32_t unk_558;          // 0x558 (gap)
    uint32_t unk_55c;          // 0x55c
    uint64_t unk_560;          // 0x560
    uint64_t unk_568;          // 0x568
};

static_assert(offsetof(ChrIns, field0x18) == 0x18+8);
static_assert(offsetof(ChrIns, playerCtrl) == 0x60+8);
static_assert(offsetof(ChrIns, padManipulator) == 0x68+8);
static_assert(offsetof(ChrIns, CharaInitParamID) == 0xc4+8);
static_assert(offsetof(ChrIns, unk_16c) == 0x164 + 8);
static_assert(offsetof(ChrIns, lowerThrowAnim) == 0x174+8);
static_assert(offsetof(ChrIns, upperThrowAnim) == 0x17c+8);
static_assert(offsetof(ChrIns, player_handing_state) == 0x190+8);
static_assert(offsetof(ChrIns, curToughness) == 0x248 + 8);
static_assert(offsetof(ChrIns, maxToughness) == 0x24C + 8);
static_assert(offsetof(ChrIns, curSelectedMagicId) == 0x25c+8);
static_assert(offsetof(ChrIns, curUsedItem) == 0x260+8);
static_assert(offsetof(ChrIns, specialEffects) == 0x270+8);
static_assert(offsetof(ChrIns, qwcSpEffectEquipCtrl) == 0x278+8);
static_assert(offsetof(ChrIns, unk_288) == 0x280+8);
static_assert(offsetof(ChrIns, entityId) == 0x2a8+8);
static_assert(offsetof(ChrIns, field0x2c8) == 0x2c8+8);
static_assert(offsetof(ChrIns, unk_2d8) == 0x2d0+8);
static_assert(offsetof(ChrIns, areaId) == 0x350+8);
static_assert(offsetof(ChrIns, hitins_1) == 0x368+8);
static_assert(offsetof(ChrIns, hitins_2) == 0x370+8);
static_assert(offsetof(ChrIns, unk_380) == 0x378+8);
static_assert(offsetof(ChrIns, chrattachsys) == 0x380+8);
static_assert(offsetof(ChrIns, curHp) == 0x3e0+8);
static_assert(offsetof(ChrIns, maxHp) == 0x3e4+8);
static_assert(offsetof(ChrIns, curSp) == 0x3f0+8);
static_assert(offsetof(ChrIns, maxSp) == 0x3f4+8);
static_assert(offsetof(ChrIns, damage_taken_scalar) == 0x3f8+8);
static_assert(offsetof(ChrIns, PoisonResist) == 0x410+8);
static_assert(offsetof(ChrIns, ToxicResist) == 0x414+8);
static_assert(offsetof(ChrIns, BleedResist) == 0x418+8);
static_assert(offsetof(ChrIns, CurseResist) == 0x41c+8);
static_assert(offsetof(ChrIns, resistPoisonTotal) == 0x420+8);
static_assert(offsetof(ChrIns, resistPlagueTotal) == 0x424+8);
static_assert(offsetof(ChrIns, resistBleedingTotal) == 0x428+8);
static_assert(offsetof(ChrIns, resistCurseTotal) == 0x42c+8);
static_assert(offsetof(ChrIns, throw_animation_info) == 0x440+8);
static_assert(offsetof(ChrIns, unk_450) == 0x448+8);
static_assert(offsetof(ChrIns, unk_470) == 0x468+8);
static_assert(offsetof(ChrIns, unk_4d8) == 0x4d0+8);
static_assert(offsetof(ChrIns, unk_518) == 0x510+8);
static_assert(sizeof(ChrIns) == 0x570);

#pragma pack(push, 1)
struct PlayerGameData_AttributeInfo
{
    int32_t player_number_1indexed; // 0x0
    int32_t Hp;                      // 0x4
    int32_t MaxHp;                   // 0x8
    int32_t BaseMaxHp;               // 0xc
    int32_t Mp;                      // 0x10
    int32_t MaxMp;                   // 0x14
    int32_t BaseMaxMp;               // 0x18
    int32_t unk_1c;                  // 0x1c
    int32_t Sp;                      // 0x20
    int32_t MaxSp;                   // 0x24
    int32_t BaseMaxSp;               // 0x28
    int32_t unk_2c;                  // 0x2c
    int32_t Vitality;                // 0x30
    int32_t unk_34;                  // 0x34
    int32_t Attunement;              // 0x38
    int32_t unk_3c;                  // 0x3c
    int32_t Endurance;               // 0x40
    int32_t unk_44;                  // 0x44
    int32_t Strength;                // 0x48
    int32_t unk_4c;                  // 0x4c
    int32_t Dexterity;               // 0x50
    int32_t unk_54;                  // 0x54
    int32_t Intellect;               // 0x58
    int32_t unk_5c;                  // 0x5c
    int32_t Force;                   // 0x60 (Faith)
    int32_t unk_64;                  // 0x64
    int32_t Luck;                    // 0x68
    int32_t unk_6c;                  // 0x6c
    int32_t BaseHumanity;            // 0x70
    int32_t Humanity;                // 0x74
    int32_t Resistance;              // 0x78
    int32_t unk_7c;                  // 0x7c
    int32_t SoulLevel;               // 0x80
    int32_t soul_count;              // 0x84
    int32_t SoulMemory;              // 0x88
    int32_t TotalAddParam;           // 0x8c
    int32_t TotalLevelsGained;       // 0x90
    int32_t ChrType;                 // 0x94
    uint16_t characterName1[16];     // 0x98 (ghidra: wchar_t[16])
    uint8_t unk_b8[2];               // 0xb8 (gap)
    uint8_t player_sex;              // 0xba (ghidra: CHARACTER_INIT_SEX)
    uint8_t unk_bb;                  // 0xbb (gap)
    uint16_t ShopLevel;              // 0xbc
    uint8_t Archetype;               // 0xbe
    uint8_t Appearance;              // 0xbf
    uint8_t Gift;                    // 0xc0
    uint8_t unk_c1[3];               // 0xc1 (gap)
    int32_t MultiplayCount;          // 0xc4
    int32_t CoopSuccessCount;        // 0xc8
    int32_t ThiefInvadeSuccessCount; // 0xcc
    int32_t RankSCount;              // 0xd0
    int32_t RankACount;              // 0xd4
    int32_t IzarisRankingPoint;      // 0xd8
    uint8_t RankCCount;              // 0xdc
    uint8_t Convenant_3_Rank;        // 0xdd
    uint8_t Convenant_4_Rank;        // 0xde
    uint8_t Convenant_5_Rank;        // 0xdf
    uint8_t Convenant_6_Rank;        // 0xe0
    uint8_t Convenant_7_Rank;        // 0xe1
    uint8_t Convenant_8_Rank;        // 0xe2
    uint8_t Convenant_9_Rank;        // 0xe3
    int32_t BountyRankingPoint;      // 0xe4
    float BlockClearBonus;           // 0xe8
    int32_t EggSouls;                // 0xec
    float PoisonResist;              // 0xf0
    float BleedResist;               // 0xf4
    float ToxicResist;               // 0xf8
    float CurseResist;               // 0xfc
    uint8_t ClearItem;               // 0x100
    uint8_t ResvSoulSteal;           // 0x101
    uint8_t ResvSoulPenalty;         // 0x102
    uint8_t covenantId;              // 0x103
    uint8_t faceType;                // 0x104
    uint8_t hairStyle;               // 0x105
    uint8_t hairColor;               // 0x106
    uint8_t CurseLv;                 // 0x107
    uint8_t InvadeType;              // 0x108
    uint8_t MaxWeaponLevel;          // 0x109
    uint8_t EstusLevel;              // 0x10a
    uint8_t unk_10b;                 // 0x10b (gap)
    uint32_t RankingRegistration_Value; // 0x10c
    uint32_t LastClearCovenant;      // 0x110
    uint32_t unk_114;                // 0x114
    uint8_t unk_118[4];              // 0x118 (gap)
    uint16_t characterName2[16];     // 0x11c (ghidra: wchar_t[16])
    uint8_t unk_13c[2];              // 0x13c (gap)
    uint8_t unk_13e;                 // 0x13e
    uint8_t unk_13f;                 // 0x13f (gap)
    uint64_t unk_140;                // 0x140
    uint64_t save_version_time;      // 0x148
    uint16_t unk_150;                // 0x150
    uint64_t unk_152;                // 0x152
    uint64_t unk_15a;                // 0x15a
    uint64_t unk_162;                // 0x162
    uint64_t unk_16a;                // 0x16a
    uint64_t unk_172;                // 0x172
    uint64_t unk_17a;                // 0x17a
    uint64_t unk_182;                // 0x182
    uint64_t unk_18a;                // 0x18a
    uint64_t unk_192;                // 0x192
    uint32_t unk_19a;                // 0x19a
    uint16_t unk_19e;                // 0x19e
    uint8_t unk_1a0;                 // 0x1a0
    uint8_t unk_1a1[3];              // 0x1a1 (gap)
};
#pragma pack(pop)

static_assert(offsetof(PlayerGameData_AttributeInfo, Hp) == 0x4);
static_assert(offsetof(PlayerGameData_AttributeInfo, Vitality) == 0x30);
static_assert(offsetof(PlayerGameData_AttributeInfo, SoulLevel) == 0x80);
static_assert(offsetof(PlayerGameData_AttributeInfo, characterName1) == 0x98);
static_assert(offsetof(PlayerGameData_AttributeInfo, PoisonResist) == 0xf0);
static_assert(offsetof(PlayerGameData_AttributeInfo, save_version_time) == 0x148);
static_assert(sizeof(PlayerGameData_AttributeInfo) == 0x1a4);

struct EquipInventoryDataItem
{
    uint32_t item_category;
    uint32_t item_id;
    uint8_t padding_0[20];
};

static_assert(sizeof(EquipInventoryDataItem) == 0x1c);

struct EquipInventoryData
{
    //this only deals with what stuff is in your inventory, not equipped.
    uint8_t padding_0[16];
    uint32_t itemList2_len;
    uint32_t padding_1;
    EquipInventoryDataItem* itemlist_general; //usually null
    uint32_t itemList1_len;
    uint32_t padding_2;
    uint64_t padding_3;
    EquipInventoryDataItem* itemlist1; //points to same location as list2
    EquipInventoryDataItem* itemlist2;
    uint32_t itemCount;
    uint32_t keyCount;
    uint8_t padding_4[48];
};

static_assert(sizeof(EquipInventoryData) == 0x78);

struct MagicSlot
{
    int32_t magic_id;
    uint32_t count;
};

static_assert(offsetof(MagicSlot, magic_id) == 0x0);
static_assert(offsetof(MagicSlot, count) == 0x4);
static_assert(sizeof(MagicSlot) == 0x8);

struct EquipMagicData
{
    uint8_t padding_0[0x18];
    MagicSlot equippedMagicList[12];
    int32_t curSelectedMagicSlot;
};

static_assert(offsetof(EquipMagicData, equippedMagicList) == 0x18);
static_assert(offsetof(EquipMagicData, curSelectedMagicSlot) == 0x78);
static_assert(sizeof(EquipMagicData) == 0x7c);

struct EquipItemData
{
    uint8_t padding_0[0x18];
    uint32_t quickbar[5];
    uint32_t selectedQuickbarItem;
};

static_assert(offsetof(EquipItemData, quickbar) == 0x18);
static_assert(sizeof(EquipItemData) == 0x30);

struct ChrAsm
{
    uint8_t padding_0[8];      // 0x0 (vtable)
    uint32_t equipped_weapon_style; // 0x8
    uint32_t l_hand_equipped_index; // 0xc
    uint32_t r_hand_equipped_index; // 0x10
    uint32_t l_arrow_equipped_index; // 0x14
    uint32_t r_arrow_equipped_index; // 0x18
    uint32_t l_bolt_equipped_index;  // 0x1c
    uint32_t r_bolt_equipped_index;  // 0x20
    uint32_t equip_items[20];  // 0x24
    int32_t transformProtectorId;          // 0x74
    int32_t transformProtectorId_HalfCamo; // 0x78
    uint32_t unk_7c;           // 0x7c (gap)
};

static_assert(offsetof(ChrAsm, equipped_weapon_style) == 0x8);
static_assert(offsetof(ChrAsm, l_arrow_equipped_index) == 0x14);
static_assert(offsetof(ChrAsm, equip_items) == 0x24);
static_assert(offsetof(ChrAsm, transformProtectorId) == 0x74);
static_assert(sizeof(ChrAsm) == 0x80);

struct EquipGameData
{
    uint8_t padding_0[0x24];
    uint32_t EquipItemToInventoryIndexMap[20];
    uint8_t EquipItemToInventoryIndexMap_index_updated[6];
    uint8_t padding_1[6];
    ChrAsm chrasm;
    ChrAsm* chrasm_alt;
    uint8_t padding_2[24];
    EquipInventoryData equippedInventory;
    EquipMagicData* equipMagicData;
    EquipItemData equippedItemsInQuickbar;
    void* gestureEquipData;
    uint32_t amountOfItemUsedFromInventory;
    uint32_t itemInventoryIdCurrentlyBeingUsedFromInventory;
    void* playerGameData_parent;
    uint8_t padding_3[48];
};

static_assert(offsetof(EquipGameData, EquipItemToInventoryIndexMap) == 0x24);
static_assert(offsetof(EquipGameData, chrasm) == 0x80);
static_assert(offsetof(EquipGameData, equippedInventory) == 0x120);
static_assert(offsetof(EquipGameData, equipMagicData) == 0x198);
static_assert(offsetof(EquipGameData, equippedItemsInQuickbar) == 0x1a0);
static_assert(offsetof(EquipGameData, itemInventoryIdCurrentlyBeingUsedFromInventory) == 0x1dc);
static_assert(sizeof(EquipGameData) == 0x218);

struct PlayerGameData_ChrProperties
{
    int32_t defPhysicalTotal;       // 0x0
    int32_t attackLeftWep1;         // 0x4
    int32_t attackRightWep1;        // 0x8
    int32_t attackLeftWep2;         // 0xc
    int32_t attackRightWep2;        // 0x10
    int32_t defMagicTotal;          // 0x14
    int32_t unkValue;               // 0x18
    float unk_1c;                   // 0x1c
    float unk_20;                   // 0x20
    float MaxEquipLoad;             // 0x24
    int32_t unkValue2;              // 0x28
    int32_t defCutTotal;            // 0x2c
    int32_t defBluntTotal;          // 0x30
    int32_t defStabTotal;           // 0x34
    int32_t defFireTotal;           // 0x38
    int32_t defThunderboltTotal;    // 0x3c
    int32_t resistPoisonCurrent;    // 0x40
    int32_t resistToxicCurrent;     // 0x44
    int32_t resistBleedCurrent;     // 0x48
    int32_t resistCurseCurrent;     // 0x4c
    float unk_50[8];                // 0x50
    uint32_t unk_70;                // 0x70 (gap)
    int32_t resistPoisonTotal;      // 0x74
    int32_t resistPlagueTotal;      // 0x78
    int32_t resistBleedingTotal;    // 0x7c
    int32_t resistCurseTotal;       // 0x80
    int32_t defSAToughnessTotal;    // 0x84
    uint32_t unk_88;                // 0x88
    int32_t ItemDiscoveryRate;      // 0x8c
};

static_assert(offsetof(PlayerGameData_ChrProperties, MaxEquipLoad) == 0x24);
static_assert(offsetof(PlayerGameData_ChrProperties, resistPoisonTotal) == 0x74);
static_assert(sizeof(PlayerGameData_ChrProperties) == 0x90);

struct PlayerGameData
{
    uint8_t padding_0[0x10];
    PlayerGameData_AttributeInfo attribs;
    uint8_t padding_1[0xcc];
    EquipGameData equipGameData;
    uint8_t padding_2[0xd8];
    PlayerGameData_ChrProperties ChrProperties;
    uint8_t padding_3[0x60];
};

static_assert(offsetof(PlayerGameData, attribs) == 0x10);
static_assert(offsetof(PlayerGameData, equipGameData) == 0x280);
static_assert(offsetof(PlayerGameData, ChrProperties) == 0x570);
static_assert(sizeof(PlayerGameData) == 0x660);

struct RingEquipCtrl
{
    uint8_t padding_0[0x10];
    SpecialEffect* spEffectList;
    uint32_t* equipped_rings_ids; //index is slot num, elem is id
    uint32_t array_len;
    uint8_t padding_1[0xc];
};

static_assert(offsetof(RingEquipCtrl, spEffectList) == 0x10);
static_assert(offsetof(RingEquipCtrl, equipped_rings_ids) == 0x18);
static_assert(offsetof(RingEquipCtrl, array_len) == 0x20);
static_assert(sizeof(RingEquipCtrl) == 0x30);

struct WeaponEquipCtrl
{
    uint8_t padding_0[0x10];
    SpecialEffect* spEffectList;
    uint32_t* equipped_weapons_ids; //index is slot num, elem is id
    uint32_t array_len;
    uint8_t padding_1[0xc];
};

static_assert(offsetof(WeaponEquipCtrl, spEffectList) == 0x10);
static_assert(offsetof(WeaponEquipCtrl, equipped_weapons_ids) == 0x18);
static_assert(offsetof(WeaponEquipCtrl, array_len) == 0x20);
static_assert(sizeof(WeaponEquipCtrl) == 0x30);

struct ProEquipCtrl
{
    uint8_t padding_0[0x10];
    SpecialEffect* spEffectList;
    uint32_t* equipped_armors_ids; //index is slot num, elem is id
    uint32_t array_len;
    uint8_t padding_1[0xc];
};

static_assert(offsetof(ProEquipCtrl, spEffectList) == 0x10);
static_assert(offsetof(ProEquipCtrl, equipped_armors_ids) == 0x18);
static_assert(offsetof(ProEquipCtrl, array_len) == 0x20);
static_assert(sizeof(ProEquipCtrl) == 0x30);

struct ChrAsmModelRes_Elem
{
    uint32_t curModelId;   // 0x0
    uint32_t newModelId;   // 0x4
    void* PartsbndFileCap1; // 0x8
    void* PartsbndFileCap2; //this is only used for intermediate storage (0x10)
    uint64_t unk_18;       // 0x18
    uint16_t unk_20;       // 0x20
    uint16_t unk_22;       // 0x22 (gap)
    float unk_24[8];       // 0x24
    uint32_t unk_44;       // 0x44 (gap)
    float unk_48;          // 0x48
    uint32_t unk_4c;       // 0x4c (gap)
};

static_assert(offsetof(ChrAsmModelRes_Elem, PartsbndFileCap1) == 0x8);
static_assert(offsetof(ChrAsmModelRes_Elem, unk_18) == 0x18);
static_assert(sizeof(ChrAsmModelRes_Elem) == 0x50);

struct ChrAsmModelRes
{
    uint64_t padding_0;    // 0x0 (vtable)
    uint32_t magic_state;  // 0x8
    uint32_t unk_c;        // 0xc (gap)
    uint64_t unk_10;       // 0x10
    uint8_t unk_18[16];    // 0x18 (gap)
    uint8_t unk_28;        // 0x28
    uint8_t unk_29[3];     // 0x29 (gap)
    uint8_t unk_2c;        // 0x2c
    uint8_t unk_2d[3];     // 0x2d (gap)
    ChrAsmModelRes_Elem arry[14];  // 0x30
};

static_assert(offsetof(ChrAsmModelRes, magic_state) == 8);
static_assert(offsetof(ChrAsmModelRes, arry) == 0x30);
static_assert(sizeof(ChrAsmModelRes) == 0x490);

struct ChrAsmModel
{
    uint64_t padding_0;    // 0x0 (vtable)
    uint8_t unk_8[8];      // 0x8 (gap)
    uint32_t unk_10;       // 0x10
    uint32_t unk_14;       // 0x14 (gap)
    uint64_t padding_1;    // 0x18 (ghidra: FlverModel ptr; not copied)
    uint8_t fieldE0[48];   // 0x20 (ghidra: NS_FRPG_BulletIns_Field0x90_Field0xE0)
    uint8_t padding_2[0x18]; // 0x50 (ghidra: MdlMtx; not copied)
    float unk_68[4];       // 0x68
    uint32_t field_0x80_len; // 0x78
    uint32_t unk_7c;       // 0x7c (gap)
    uint64_t unk1;         // 0x80 (ghidra: field_0x80_array ptr; not copied)
    uint64_t unk2;         // 0x88 (not copied)
    uint64_t unk3;         // 0x90 (not copied)
    uint32_t data_3a;      // 0x98
    uint32_t updatePhantomColor; // 0x9c (not copied)
    uint32_t unk_a0;       // 0xa0
    uint32_t unk_a4;       // 0xa4
    uint8_t unk_a8;        // 0xa8
    uint8_t phantomColor;  // 0xa9
    uint16_t unk_aa;       // 0xaa (gap)
    uint16_t unk_ac;       // 0xac
    uint16_t unk_ae;       // 0xae
    float unk_b0;          // 0xb0
    float unk_b4;          // 0xb4
    uint8_t unk_b8[12];    // 0xb8 (gap)
    float unk_c4;          // 0xc4
    uint64_t unk4;         // 0xc8 (not copied)
    float hairColor[4];    // 0xd0
    uint8_t unk_e0[16];    // 0xe0 (gap)
    float unk_f0[4];       // 0xf0
    uint8_t unk_100[8];    // 0x100 (gap)
    uint32_t unk_108;      // 0x108
    uint32_t unk_10c;      // 0x10c
    uint8_t unk_110[64];   // 0x110 (gap)
    uint64_t padding_3; //duplicate pointer from parent (ghidra: ChrAsmModelRes ptr) (0x150)
    uint64_t filecap_array[13]; //pointer to FileCaps, const ptrs (0x158)
    uint64_t padding_4;    // 0x1c0 (ghidra: FgFlverModel ptr; not copied)
    uint8_t unk_1c8;       // 0x1c8
    uint8_t unk_1c9[7];    // 0x1c9 (gap)
};

static_assert(offsetof(ChrAsmModel, unk_8) == 0x8);
static_assert(offsetof(ChrAsmModel, padding_1) == 0x18);
static_assert(offsetof(ChrAsmModel, fieldE0) == 0x20);
static_assert(offsetof(ChrAsmModel, padding_2) == 0x50);
static_assert(offsetof(ChrAsmModel, unk_68) == 0x68);
static_assert(offsetof(ChrAsmModel, unk1) == 0x80);
static_assert(offsetof(ChrAsmModel, data_3a) == 0x98);
static_assert(offsetof(ChrAsmModel, updatePhantomColor) == 0x9c);
static_assert(offsetof(ChrAsmModel, unk_a0) == 0xa0);
static_assert(offsetof(ChrAsmModel, unk4) == 0xc8);
static_assert(offsetof(ChrAsmModel, hairColor) == 0xd0);
static_assert(offsetof(ChrAsmModel, padding_3) == 0x150);
static_assert(offsetof(ChrAsmModel, filecap_array) == 0x158);
static_assert(offsetof(ChrAsmModel, padding_4) == 0x1c0);
static_assert(offsetof(ChrAsmModel, unk_1c8) == 0x1c8);
static_assert(sizeof(ChrAsmModel) == 0x1d0);

struct SteamOnlineIDData
{
    uint8_t padding_0[0x48];
    uint64_t steam_id;
    uint8_t padding_1[0x20];
};
static_assert(offsetof(SteamOnlineIDData, steam_id) == 0x48);
static_assert(sizeof(SteamOnlineIDData) == 0x70);

struct SteamPlayerData
{
    uint8_t padding_0[0x18];
    SteamOnlineIDData* steamOnlineIDData;
    uint8_t padding_1[0x78];
};
static_assert(offsetof(SteamPlayerData, steamOnlineIDData) == 0x18);
static_assert(sizeof(SteamPlayerData) == 0x98);

struct PlayerIns
{
    ChrIns chrins;
    uint8_t padding_0[8];
    PlayerGameData* playergamedata;
    uint8_t padding_1a[0x10];
    SteamPlayerData* steamPlayerData;
    uint8_t padding_1ab[0x38];
    uint8_t data_0[0x1d0];
    uint64_t padding_1b;       // 0x7a0 (ghidra: ReplayRecorder ptr; not copied)
    uint64_t unk_7a8;          // 0x7a8
    float TimeDelayForRollRecalc; // 0x7b0
    uint32_t unk_7b4;          // 0x7b4
    uint8_t padding_1c[24];    // 0x7b8 (ghidra: 3 pointers; not copied)
    uint8_t unk_7d0;           // 0x7d0
    uint8_t unk_7d1[3];        // 0x7d1 (gap)
    float unk_7d4;             // 0x7d4
    uint64_t padding_1d[2];    // 0x7d8 (ghidra: FgModel + ptr; not copied)
    RingEquipCtrl* ringequipctrl;
    WeaponEquipCtrl* weaponequipctrl;
    ProEquipCtrl* proequipctrl;
    uint8_t padding_2[8];
    int32_t curSelectedMagicId;
    ItemUsed curUsedItem;
    uint32_t override_itemId;
    uint32_t override_equipped_magicId;
    uint32_t using_override;
    uint8_t padding_3[0x20];
    ChrAsm* chrasm;
    ChrAsmModelRes* chrAsmModelRes;
    ChrAsmModel* chrAsmModel;
    uint64_t padding_4;
    float headSize;            // 0x860
    float upperBodySize;       // 0x864
    float lowerBodySize;       // 0x868
    float armSize;             // 0x86c
    float legSize;             // 0x870
    uint32_t unk_874;          // 0x874 (gap)
    uint64_t padding_5;        // 0x878 (ghidra: chrasm_copy ptr; not copied)
    uint8_t unk_880[48];       // 0x880 (gap)
    float unk_8b0;             // 0x8b0
    uint8_t equipSlotToEquipIndexMap[8]; // 0x8b4 (ghidra: byte[8])
    uint32_t unk_8bc;          // 0x8bc
    uint8_t unk_8c0[16];       // 0x8c0 (gap)
    uint8_t unk_BulletIns_FollowupBullet[0x30];
    uint8_t data_5[0x49];
    uint8_t unk_flags;
    uint8_t data_5a[0x66];
    uint64_t padding_6;
    uint8_t data_6[24];
};

static_assert(offsetof(PlayerIns, chrins) == 0x0);
static_assert(offsetof(PlayerIns, playergamedata) == 0x578);
static_assert(offsetof(PlayerIns, padding_1a) == 0x580);
static_assert(offsetof(PlayerIns, steamPlayerData) == 0x590);
static_assert(offsetof(PlayerIns, data_0) == 0x5d0);
static_assert(offsetof(PlayerIns, padding_1b) == 0x7a0);
static_assert(offsetof(PlayerIns, unk_7a8) == 0x7a8);
static_assert(offsetof(PlayerIns, TimeDelayForRollRecalc) == 0x7b0);
static_assert(offsetof(PlayerIns, padding_1c) == 0x7b8);
static_assert(offsetof(PlayerIns, unk_7d0) == 0x7d0);
static_assert(offsetof(PlayerIns, padding_1d) == 0x7d8);
static_assert(offsetof(PlayerIns, ringequipctrl) == 0x7e8);
static_assert(offsetof(PlayerIns, weaponequipctrl) == 0x7f0);
static_assert(offsetof(PlayerIns, proequipctrl) == 0x7f8);
static_assert(offsetof(PlayerIns, curSelectedMagicId) == 0x808);
static_assert(offsetof(PlayerIns, curUsedItem) == 0x80c);
static_assert(offsetof(PlayerIns, override_itemId) == 0x814);
static_assert(offsetof(PlayerIns, override_equipped_magicId) == 0x818);
static_assert(offsetof(PlayerIns, chrasm) == 0x840);
static_assert(offsetof(PlayerIns, chrAsmModelRes) == 0x848);
static_assert(offsetof(PlayerIns, chrAsmModel) == 0x850);
static_assert(offsetof(PlayerIns, padding_4) == 0x858);
static_assert(offsetof(PlayerIns, headSize) == 0x860);
static_assert(offsetof(PlayerIns, padding_5) == 0x878);
static_assert(offsetof(PlayerIns, unk_880) == 0x880);
static_assert(offsetof(PlayerIns, equipSlotToEquipIndexMap) == 0x8b4);
static_assert(offsetof(PlayerIns, unk_BulletIns_FollowupBullet) == 0x8d0);
static_assert(offsetof(PlayerIns, data_5) == 0x900);
static_assert(offsetof(PlayerIns, unk_flags) == 0x949);
static_assert(offsetof(PlayerIns, data_5a) == 0x94a);
static_assert(offsetof(PlayerIns, padding_6) == 0x9b0);
static_assert(offsetof(PlayerIns, data_6) == 0x9b8);

static_assert(sizeof(PlayerIns) == 0x9d0);

#endif
