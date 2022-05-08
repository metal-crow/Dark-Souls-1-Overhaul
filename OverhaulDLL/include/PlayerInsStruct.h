#pragma once
#ifndef PLAYERINS_STRUCT_H
#define PLAYERINS_STRUCT_H

//NOTE: Anything marked "data_x" is a bunch of non-pointer data that can be saved/loaded without really knowing what it is.
// See the ghidra repo for more specific info about what is in that blob, if it's even known

#include <stddef.h>

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
typedef struct hkaAnimationBinding hkaAnimationBinding;
typedef struct ChrCtrl_AnimationQueue ChrCtrl_AnimationQueue;
typedef struct ChrCtrl_AnimationQueue_field0x20 ChrCtrl_AnimationQueue_field0x20;
typedef struct AnimationMediatorStateEntry AnimationMediatorStateEntry;
typedef struct AnimationMediator AnimationMediator;
typedef struct hkpCharacterProxy_field0x20elem hkpCharacterProxy_field0x20elem;
typedef struct hkpRigidBody hkpRigidBody;
typedef struct hkpSimpleShapePhantom hkpSimpleShapePhantom;
typedef struct ChrInsProxyListener ChrInsProxyListener;
typedef struct hkpCharacterProxy hkpCharacterProxy;
typedef struct HitIns HitIns;
typedef struct HavokChara HavokChara;
typedef struct EzState_detail_EzStateMachineImpl EzState_detail_EzStateMachineImpl;
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
typedef struct ChrManipulator ChrManipulator;
typedef struct NetworkManipulator NetworkManipulator;
typedef struct PadManipulator PadManipulator;
typedef struct ChrIns_AnimationMediatorStateInfo ChrIns_AnimationMediatorStateInfo;
typedef struct ItemUsed ItemUsed;
typedef struct SpecialEffect_Info SpecialEffect_Info;
typedef struct SpecialEffect SpecialEffect;
typedef struct QwcSpEffectEquipCtrl QwcSpEffectEquipCtrl;
typedef struct ChrAttachSys ChrAttachSys;
typedef struct EntityThrowAnimationStatus EntityThrowAnimationStatus;
typedef struct ChrIns_field0x2c8 ChrIns_field0x2c8;
typedef struct ChrIns ChrIns;
typedef struct PlayerGameData_AttributeInfo PlayerGameData_AttributeInfo;
typedef struct EquipInventoryData EquipInventoryData;
typedef struct MagicSlot MagicSlot;
typedef struct EquipMagicData EquipMagicData;
typedef struct EquipItemData EquipItemData;
typedef struct EquipGameData EquipGameData;
typedef struct PlayerGameData_ChrProperties PlayerGameData_ChrProperties;
typedef struct PlayerGameData PlayerGameData;
typedef struct RingEquipCtrl RingEquipCtrl;
typedef struct WeaponEquipCtrl WeaponEquipCtrl;
typedef struct ProEquipCtrl ProEquipCtrl;
typedef struct ChrAsm ChrAsm;
typedef struct PlayerIns PlayerIns;

struct AnimationQueue_Entry_sub1_field0x10
{
    uint64_t data_0[2]; //pointers, but to previously saved values
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

struct AnimationQueue_Entry
{
    uint8_t data_0[8];
    void* padding_0;
    uint8_t data_1[0x100];
    AnimationQueue_Entry_sub1 sub1;
    uint64_t data_2[1];
    uint64_t padding_3;
    uint64_t* chained_animations_array_start; //i believe these are pointers to static structs (or at least static in a session)
    uint64_t* chained_animations_array_end;
    uint64_t* chained_animations_array_capEnd;
    uint8_t data_3[24];
};

static_assert(offsetof(AnimationQueue_Entry, data_0) == 0x0);
static_assert(offsetof(AnimationQueue_Entry, data_1) == 0x10);
static_assert(offsetof(AnimationQueue_Entry, sub1) == 0x110);
static_assert(offsetof(AnimationQueue_Entry, data_2) == 0x140);
static_assert(offsetof(AnimationQueue_Entry, data_3) == 0x168);
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
    uint8_t data_0[8];
    uint64_t padding_0[3];
    uint64_t HvkAnim_AnimInfoArrayElem; //this should be a const pointer since it's to an animbnd
    uint8_t data_1[0x50];
};

static_assert(offsetof(ChrCtrl_AnimationQueueEntry, HvkAnim_AnimInfoArrayElem) == 0x20);
static_assert(offsetof(ChrCtrl_AnimationQueueEntry, data_1) == 0x28);
static_assert(sizeof(ChrCtrl_AnimationQueueEntry) == 0x78);

struct ChrCtrl_AnimationQueue_field0x10_field0x10arrayelem
{
    uint8_t data_0[0x10];
};

static_assert(sizeof(ChrCtrl_AnimationQueue_field0x10_field0x10arrayelem) == 0x10);

struct ChrCtrl_AnimationQueue_field0x10
{
    uint32_t array1_len;
    uint32_t array2_len;
    ChrCtrl_AnimationQueue_field0x10_field0x10arrayelem** arry1; //this points to elements in arry2
    ChrCtrl_AnimationQueue_field0x10_field0x10arrayelem* arry2;
    uint64_t padding_0[2];
};

static_assert(offsetof(ChrCtrl_AnimationQueue_field0x10, array1_len) == 0x0);
static_assert(offsetof(ChrCtrl_AnimationQueue_field0x10, array2_len) == 0x4);
static_assert(offsetof(ChrCtrl_AnimationQueue_field0x10, arry1) == 0x8);
static_assert(offsetof(ChrCtrl_AnimationQueue_field0x10, arry2) == 0x10);
static_assert(sizeof(ChrCtrl_AnimationQueue_field0x10) == 0x28);

struct hkaAnimationBinding
{
    uint64_t padding_0;
    uint8_t data_0[16];
    uint64_t padding_1;
};

static_assert(sizeof(hkaAnimationBinding) == 0x20);

struct hkaAnimationControl
{
    uint64_t padding_0;
    uint8_t data_0[16];
    uint8_t* field0x18;
    uint32_t field0x18_len;
    uint32_t field0x18_cap;
    uint8_t* field0x28;
    uint32_t field0x28_len;
    uint32_t field0x28_cap;
    hkaAnimationBinding* HkaAnimationBinding;
    uint64_t padding_1;
    uint32_t data_1[3];
};

static_assert(offsetof(hkaAnimationControl, field0x18) == 0x18);
static_assert(offsetof(hkaAnimationControl, field0x28) == 0x28);
static_assert(offsetof(hkaAnimationControl, HkaAnimationBinding) == 0x38);
static_assert(sizeof(hkaAnimationControl) == 0x58);

struct hkaDefaultAnimationControl
{
    hkaAnimationControl HkaAnimationControl;
    uint8_t data_0[0x50];
    uint8_t padding_0[24];
};

static_assert(offsetof(hkaDefaultAnimationControl, data_0) == 0x58);
static_assert(sizeof(hkaDefaultAnimationControl) == 0xc0);

struct hkaAnimatedSkeleton
{
    uint64_t padding_0;
    uint64_t data_0;
    uint64_t padding_1;
    void* padding_HkpRigidBody;
    hkaDefaultAnimationControl** animCtrl_list;
    uint32_t animCtrl_list_len;
    uint32_t data_1;
    void* padding_2; //seems to be unneeded
    uint64_t data_2;
};

static_assert(offsetof(hkaAnimatedSkeleton, data_0) == 8);
static_assert(offsetof(hkaAnimatedSkeleton, padding_HkpRigidBody) == 0x18);
static_assert(offsetof(hkaAnimatedSkeleton, animCtrl_list) == 0x20);
static_assert(offsetof(hkaAnimatedSkeleton, animCtrl_list_len) == 0x28);
static_assert(offsetof(hkaAnimatedSkeleton, data_1) == 0x2C);
static_assert(offsetof(hkaAnimatedSkeleton, padding_2) == 0x30);
static_assert(offsetof(hkaAnimatedSkeleton, data_2) == 0x38);
static_assert(sizeof(hkaAnimatedSkeleton) == 0x40);

struct ChrCtrl_AnimationQueue_field0x20
{
    uint64_t padding_0;
    void* field0x8; // len = this->parent_hkaSkeleton->field0x28_len + 3 // elem size = 0x30, all floats
    uint32_t data_0[2];
    void* field0x18; // len = this->parent_hkaSkeleton->field0x28_len + 3 // elem size = 0x30, all floats
    uint32_t data_1[2];
    uint32_t* field0x28; // len = this->parent_hkaSkeleton->field0x28_len + 3 // elem size = 4
    uint32_t data_2[4];
    uint32_t* padding_field0x40; // len = this->parent_hkaSkeleton->field0x58_len + 3 // elem size = 4. Always seems to be null and length = 0
    uint32_t data_3[2];
};

static_assert(offsetof(ChrCtrl_AnimationQueue_field0x20, field0x8) == 0x8);
static_assert(offsetof(ChrCtrl_AnimationQueue_field0x20, data_0) == 0x10);
static_assert(offsetof(ChrCtrl_AnimationQueue_field0x20, field0x18) == 0x18);
static_assert(offsetof(ChrCtrl_AnimationQueue_field0x20, data_1) == 0x20);
static_assert(offsetof(ChrCtrl_AnimationQueue_field0x20, field0x28) == 0x28);
static_assert(offsetof(ChrCtrl_AnimationQueue_field0x20, data_2) == 0x30);
static_assert(offsetof(ChrCtrl_AnimationQueue_field0x20, padding_field0x40) == 0x40);
static_assert(offsetof(ChrCtrl_AnimationQueue_field0x20, data_3) == 0x48);
static_assert(sizeof(ChrCtrl_AnimationQueue_field0x20) == 0x50);

struct ChrCtrl_AnimationQueue
{
    uint32_t array_length;
    uint32_t data_0;
    ChrCtrl_AnimationQueueEntry* arry;
    ChrCtrl_AnimationQueue_field0x10* field0x10;
    hkaAnimatedSkeleton* HkaAnimatedSkeleton;
    ChrCtrl_AnimationQueue_field0x20* field0x20;
    uint64_t padding_0[7]; //unimportant pointers. setting to null has no effect
    uint8_t data_1[0x10];
    uint8_t data_2[0x10];
    void* padding_2;
    uint64_t data_3;
    uint64_t padding_3[3];
    uint8_t data_4[0x20];
    uint64_t padding_4[2];
    uint8_t data_5[0x18];
};

static_assert(offsetof(ChrCtrl_AnimationQueue, array_length) == 0);
static_assert(offsetof(ChrCtrl_AnimationQueue, data_0) == 4);
static_assert(offsetof(ChrCtrl_AnimationQueue, arry) == 8);
static_assert(offsetof(ChrCtrl_AnimationQueue, field0x10) == 0x10);
static_assert(offsetof(ChrCtrl_AnimationQueue, HkaAnimatedSkeleton) == 0x18);
static_assert(offsetof(ChrCtrl_AnimationQueue, data_1) == 0x60);
static_assert(offsetof(ChrCtrl_AnimationQueue, data_2) == 0x70);
static_assert(offsetof(ChrCtrl_AnimationQueue, data_3) == 0x88);
static_assert(offsetof(ChrCtrl_AnimationQueue, data_4) == 0xa8);
static_assert(offsetof(ChrCtrl_AnimationQueue, data_5) == 0xd8);
static_assert(sizeof(ChrCtrl_AnimationQueue) == 0xf0);

struct AnimationMediatorStateEntry
{
    uint8_t data_0[0x90];
    uint64_t padding_0[2]; //these linked animations only point to other StateEntries in the states_list. No need to worry about them
    uint8_t data_1[8];
};

static_assert(offsetof(AnimationMediatorStateEntry, data_0) == 0);
static_assert(offsetof(AnimationMediatorStateEntry, padding_0) == 0x90);
static_assert(offsetof(AnimationMediatorStateEntry, data_1) == 0xa0);
static_assert(sizeof(AnimationMediatorStateEntry) == 0xa8);

struct AnimationMediator
{
    AnimationMediatorStateEntry states_list[31];
    void* padding_0;
    AnimationQueue* animationQueue;
    uint8_t data_0[0x28];
};

static_assert(offsetof(AnimationMediator, animationQueue) == 0x1460);
static_assert(offsetof(AnimationMediator, data_0) == 0x1468);
static_assert(sizeof(AnimationMediator) == 0x1490);

struct hkpCharacterProxy_field0x20elem
{
    uint8_t data_0[0x20];
    void* pointer_0; //this seems to be a static pointer
    uint64_t data_1;
    void* pointer_1; //this seems to be a static pointer
    uint64_t data_2;
};

static_assert(sizeof(hkpCharacterProxy_field0x20elem) == 0x40);

struct hkpRigidBody
{
    uint8_t padding_0[0x2c0];
};

static_assert(sizeof(hkpRigidBody) == 0x2c0);

struct hkpSimpleShapePhantom
{
    uint8_t padding_0[0x1c0];
};

static_assert(sizeof(hkpSimpleShapePhantom) == 0x1c0);

struct ChrInsProxyListener
{
    uint8_t padding_0[0x30];
};

static_assert(sizeof(ChrInsProxyListener) == 0x30);

struct hkpCharacterProxy
{
    uint64_t padding_0;
    uint64_t data_0;
    uint64_t padding_1[2];
    hkpCharacterProxy_field0x20elem* field0x20; //Ignore this, seems to not be important.
    uint32_t field0x20_len;
    uint32_t field0x20_cap;
    hkpRigidBody** field0x30; //Ignore this, seems to not be important.
    uint32_t field0x30_len;
    uint32_t field0x30_cap;
    void* field0x40; //each elem is ?? size. This appears to be unused
    uint32_t field0x40_len;
    uint32_t field0x40_cap;
    void* field0x50; //each elem is ?? size. This appears to be unused
    uint32_t field0x50_len;
    uint32_t field0x50_cap;
    uint8_t data_1[0x20];
    hkpSimpleShapePhantom* HkpSimpleShapePhantom;
    uint8_t data_2[0x40];
    ChrInsProxyListener** field0xc8; //Ignore this, seems to not be important.
    uint32_t field0xc8_len;
    uint32_t field0xc8_cap;
    uint8_t data_3[24];
};

static_assert(offsetof(hkpCharacterProxy, data_0) == 0x8);
static_assert(offsetof(hkpCharacterProxy, field0x20) == 0x20);
static_assert(offsetof(hkpCharacterProxy, field0x30) == 0x30);
static_assert(offsetof(hkpCharacterProxy, field0x40) == 0x40);
static_assert(offsetof(hkpCharacterProxy, field0x50) == 0x50);
static_assert(offsetof(hkpCharacterProxy, data_1) == 0x60);
static_assert(offsetof(hkpCharacterProxy, HkpSimpleShapePhantom) == 0x80);
static_assert(offsetof(hkpCharacterProxy, data_2) == 0x88);
static_assert(offsetof(hkpCharacterProxy, field0xc8) == 0xc8);
static_assert(offsetof(hkpCharacterProxy, data_3) == 0xd8);
static_assert(sizeof(hkpCharacterProxy) == 0xf0);

struct HitIns
{
    uint64_t padding_0;
    uint64_t data_0;
    uint64_t padding_1; //pointer to ResCap, const ptr
    uint8_t data_1[0x48];

        uint64_t padding_2[2]; //just pointers to self
        uint64_t data_2;
        uint64_t padding_3; //pointer to FileCap, const ptr
        void* unk1; //pointer to FrpgPhysSysIns. Do we need to save?

        uint64_t padding_4[2]; //just pointers to self
        uint64_t data_3;
        uint64_t padding_5; //pointer to FileCap, const ptr
        void* unk2; //pointer to FrpgPhysSysIns. Do we need to save?

    uint64_t data_4;
    uint64_t padding_6; //dbgnode
    uint64_t data_5;
};

static_assert(offsetof(HitIns, data_0) == 8);
static_assert(offsetof(HitIns, data_1) == 0x10+8);
static_assert(offsetof(HitIns, data_2) == 0x60+0x10);
static_assert(offsetof(HitIns, data_3) == 0x88+0x10);
static_assert(offsetof(HitIns, data_4) == 0xb0);
static_assert(offsetof(HitIns, data_5) == 0xc0);
static_assert(sizeof(HitIns) == 0xc8);

struct HavokChara
{
    uint8_t data_0[0x38];
    hkpCharacterProxy* char_proxy;
    void* padding_cap[2]; //neither of these seem to do anything
    void* padding_physShapePhantomIns[2]; //neither of these seem to do anything
    uint8_t data_1[0x98];
    uint64_t padding_1;
    uint8_t data_2[0xe8];
    HitIns* hitIns;
    uint8_t data_3[0x58];
    uint64_t padding_2;
    void* padding_3; //points to an offset into PlayerIns
    uint8_t data_4[0x38];
};

static_assert(offsetof(HavokChara, data_0) == 0);
static_assert(offsetof(HavokChara, char_proxy) == 0x38);
static_assert(offsetof(HavokChara, padding_cap) == 0x40);
static_assert(offsetof(HavokChara, padding_physShapePhantomIns) == 0x50);
static_assert(offsetof(HavokChara, data_1) == 0x60);
static_assert(offsetof(HavokChara, data_2) == 0x100);
static_assert(offsetof(HavokChara, hitIns) == 0x1e8);
static_assert(offsetof(HavokChara, data_3) == 0x1f0);
static_assert(offsetof(HavokChara, padding_3) == 0x250);
static_assert(offsetof(HavokChara, data_4) == 0x258);
static_assert(sizeof(HavokChara) == 0x290);

struct EzState_detail_EzStateMachineImpl
{
    uint8_t padding_0[24];
    uint8_t data_0[16]; //the cur_state pointer in this is just a pointer to const data
    uint64_t padding_1[1]; //i dunno what this pointer is, so hopefully its unneeded
    uint64_t padding_1a[5]; //these seem to be null
    uint8_t padding_1b[0x58]; //ezstate stuff, probably not needed
    uint8_t data_1[16];
    uint8_t padding_2[0x68];
    uint8_t data_2[8];
};

static_assert(offsetof(EzState_detail_EzStateMachineImpl, data_0) == 0x18);
static_assert(offsetof(EzState_detail_EzStateMachineImpl, data_1) == 0xb0);
static_assert(offsetof(EzState_detail_EzStateMachineImpl, data_2) == 0x128);
static_assert(sizeof(EzState_detail_EzStateMachineImpl) == 0x130);

struct ActionCtrl_0x30Substruct
{
    EzState_detail_EzStateMachineImpl* EzStateMachineImpl;
    uint8_t data_0[0x18];
};

static_assert(offsetof(ActionCtrl_0x30Substruct, EzStateMachineImpl) == 0);
static_assert(offsetof(ActionCtrl_0x30Substruct, data_0) == 8);
static_assert(sizeof(ActionCtrl_0x30Substruct) == 0x20);

struct ActionCtrl
{
    uint8_t padding_0[8];
    uint8_t data_0[8];
    uint8_t padding_1[32];
    ActionCtrl_0x30Substruct passive_state;
    ActionCtrl_0x30Substruct active_state;
    uint8_t data_1[0x4d0];
    uint8_t padding_2[8];
    uint8_t data_2[0x18];
};

static_assert(offsetof(ActionCtrl, data_0) == 0x8);
static_assert(offsetof(ActionCtrl, passive_state) == 0x30);
static_assert(offsetof(ActionCtrl, active_state) == 0x50);
static_assert(offsetof(ActionCtrl, data_1) == 0x70);
static_assert(offsetof(ActionCtrl, data_2) == 0x548);
static_assert(sizeof(ActionCtrl) == 0x560);

struct WalkAnim_Twist_Field0x228Elem_field0x10elem
{
    uint32_t data_0[4];
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
    uint8_t padding_0[8];
    uint8_t data_0[8];
    uint8_t padding_1[8];
    uint8_t data_1[0x1b8];
    uint8_t padding_2[8];
    uint8_t data_2[16];
    SpinJoint* Upper_Root_Joint;
    SpinJoint* master_joint;
    SpinJoint* neck_joint;
    uint8_t data_3[0x28];
    WalkAnim_Twist_Field0x228Elem walkAnim_Twist_Field0x228Elem;
    uint8_t data_4[0x48];
};

static_assert(offsetof(WalkAnim_Twist, data_0) == 8);
static_assert(offsetof(WalkAnim_Twist, data_1) == 0x18);
static_assert(offsetof(WalkAnim_Twist, data_2) == 0x1d8);
static_assert(offsetof(WalkAnim_Twist, data_3) == 0x200);
static_assert(offsetof(WalkAnim_Twist, data_4) == 0x258);
static_assert(sizeof(WalkAnim_Twist) == 0x2a0);

struct ChrCtrl
{
    uint8_t padding_0[8];
    uint8_t data_0[8];
    uint8_t padding_1[8];
    ChrCtrl_AnimationQueue* animationQueue;
    AnimationMediator* animationMediator;
    HavokChara* havokChara;
    uint8_t padding_2[24];
    ActionCtrl* actionctrl;
    uint8_t padding_3[0x30];
    uint8_t data_1[8];
    uint8_t padding_4[8]; //this is just another pointer to the padMan, can ignore
    uint8_t data_2[0x118];
    WalkAnim_Twist* walkAnim_Twist;
    uint8_t data_3[0xe0];
    uint8_t padding_5[16];
    uint8_t data_4[0x60];
};

static_assert(offsetof(ChrCtrl, data_0) == 8);
static_assert(offsetof(ChrCtrl, animationQueue) == 0x18);
static_assert(offsetof(ChrCtrl, animationMediator) == 0x20);
static_assert(offsetof(ChrCtrl, havokChara) == 0x28);
static_assert(offsetof(ChrCtrl, data_1) == 0x80);
static_assert(offsetof(ChrCtrl, data_2) == 0x90);
static_assert(offsetof(ChrCtrl, walkAnim_Twist) == 0x1a8);
static_assert(offsetof(ChrCtrl, data_3) == 0x1b0);
static_assert(offsetof(ChrCtrl, data_4) == 0x2a0);
static_assert(sizeof(ChrCtrl) == 0x300);

struct TurnAnim
{
    uint8_t padding_0[8];
    uint8_t data_0[8];
    uint8_t padding_1[8];
    uint8_t data_1[0x28];
    SpinJoint* joint_UpperRoot;
    SpinJoint* joint_LowerRoot;
    SpinJoint* joint_spine1_1;
    SpinJoint* joint_spine_1;
    SpinJoint* joint_master;
    uint8_t padding_2[8];
    uint8_t data_2[0x120];
    uint8_t padding_3[8];
    uint8_t data_3[8];
};

static_assert(offsetof(TurnAnim, data_0) == 0x8);
static_assert(offsetof(TurnAnim, data_1) == 0x18);
static_assert(offsetof(TurnAnim, data_2) == 0x70);
static_assert(offsetof(TurnAnim, data_3) == 0x198);
static_assert(sizeof(TurnAnim) == 0x1a0);

struct SpinJoint
{
    uint8_t padding_0[8];
    uint8_t data_0[8];
    uint8_t padding_1[8];
    uint8_t data_1[96];
    uint8_t padding_2[8];
    uint8_t data_2[8];
    uint8_t padding_3[8];
};

static_assert(offsetof(SpinJoint, data_0) == 8);
static_assert(offsetof(SpinJoint, padding_1) == 0x10);
static_assert(offsetof(SpinJoint, data_1) == 0x18);
static_assert(offsetof(SpinJoint, padding_2) == 0x78);
static_assert(offsetof(SpinJoint, data_2) == 0x80);
static_assert(offsetof(SpinJoint, padding_3) == 0x88);
static_assert(sizeof(SpinJoint) == 0x90);

struct ArrowTurnAnim
{
    TurnAnim turnAnim;
    uint8_t padding_0[8];
    SpinJoint* joint_spine_2;
    SpinJoint* joint_spine1_2;
    uint8_t data_0[8];
};

static_assert(offsetof(ArrowTurnAnim, padding_0) == 0x1a0);
static_assert(offsetof(ArrowTurnAnim, joint_spine_2) == 0x1a8);
static_assert(offsetof(ArrowTurnAnim, joint_spine1_2) == 0x1b0);
static_assert(offsetof(ArrowTurnAnim, data_0) == 0x1b8);
static_assert(sizeof(ArrowTurnAnim) == 0x1c0);

struct PlayerCtrl
{
    ChrCtrl chrCtrl;
    uint8_t data_0[8];
    TurnAnim* turnAnim;
    ArrowTurnAnim* arrowTurnAnim;
    uint8_t padding_0[24];
    uint8_t data_1[8];
    uint8_t padding_1[32];
    uint8_t data_2[24];
};

static_assert(offsetof(PlayerCtrl, data_0) == 0x300);
static_assert(offsetof(PlayerCtrl, turnAnim) == 0x308);
static_assert(offsetof(PlayerCtrl, arrowTurnAnim) == 0x310);
static_assert(offsetof(PlayerCtrl, padding_0) == 0x318);
static_assert(offsetof(PlayerCtrl, data_1) == 0x330);
static_assert(offsetof(PlayerCtrl, padding_1) == 0x338);
static_assert(offsetof(PlayerCtrl, data_2) == 0x358);
static_assert(sizeof(PlayerCtrl) == 0x370);

struct ChrManipulator
{
    uint8_t padding_0[0x8];
    uint8_t data_0[0x228];
};

static_assert(sizeof(ChrManipulator) == 0x230);

struct NetworkManipulator
{
    //this is probably used in place of PadManipulator in PlayerIns for remote PCs. need to verify
};

struct PadManipulator
{
    ChrManipulator chrManipulator;
    uint8_t padding_0[0xc];
    uint8_t data_0[4];
    uint8_t padding_1[0x20];
    uint8_t data_1[0x100];
};

static_assert(offsetof(PadManipulator, chrManipulator) == 0x0);
static_assert(offsetof(PadManipulator, data_0) == 0x23c);
static_assert(offsetof(PadManipulator, data_1) == 0x260);
static_assert(sizeof(PadManipulator) == 0x360);

struct ChrIns_AnimationMediatorStateInfo
{
    int32_t animationId;
    uint32_t stateIndex;
};

static_assert(offsetof(ChrIns_AnimationMediatorStateInfo, animationId) == 0x0);
static_assert(offsetof(ChrIns_AnimationMediatorStateInfo, stateIndex) == 0x4);
static_assert(sizeof(ChrIns_AnimationMediatorStateInfo) == 0x8);

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
    uint8_t data_0[0x38];
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
    uint8_t padding_0[8];
    SpecialEffect_Info* specialEffect_Info;
    uint8_t data_0[8];
    uint8_t padding_2[8];
    uint8_t data_1[8];
    uint8_t padding_3[8];
    uint8_t data_2[8];
};

static_assert(offsetof(SpecialEffect, specialEffect_Info) == 0x0 + 8);
static_assert(offsetof(SpecialEffect, data_0) == 0x8 + 8);
static_assert(offsetof(SpecialEffect, data_1) == 0x18 + 8);
static_assert(offsetof(SpecialEffect, data_2) == 0x28 + 8);
static_assert(sizeof(SpecialEffect) == 0x38);

struct QwcSpEffectEquipCtrl
{
    uint8_t padding_0[0x18];
    uint32_t* arry;
    uint32_t arry_len;
    uint32_t data_0[3];
};

static_assert(offsetof(QwcSpEffectEquipCtrl, arry) == 0x18);
static_assert(offsetof(QwcSpEffectEquipCtrl, arry_len) == 0x20);
static_assert(offsetof(QwcSpEffectEquipCtrl, data_0) == 0x24);
static_assert(sizeof(QwcSpEffectEquipCtrl) == 0x30);

struct ChrAttachSys
{
    //this is sfx stuff, unneeded for rollback
    uint8_t padding_0[0x18];
};

static_assert(sizeof(ChrAttachSys) == 0x18);

struct EntityThrowAnimationStatus
{
    uint8_t padding_0[0x10];
    uint64_t throw_paramdef; //this is a pointer to a const struct, so just read/write the pointer itself
    uint8_t data_0[0x40];
    uint8_t padding_1[0x8];
    uint8_t data_1[0x50];
};

static_assert(offsetof(EntityThrowAnimationStatus, throw_paramdef) == 0x10);
static_assert(offsetof(EntityThrowAnimationStatus, data_0) == 0x18);
static_assert(offsetof(EntityThrowAnimationStatus, padding_1) == 0x58);
static_assert(offsetof(EntityThrowAnimationStatus, data_1) == 0x60);
static_assert(sizeof(EntityThrowAnimationStatus) == 0xb0);

struct ChrIns_field0x2c8
{
    uint64_t padding_0;
    uint8_t data_0[0x18];
    uint64_t padding_1;
    uint64_t data_1;
    uint64_t padding_2;
    uint8_t data_2[0x10];
    uint64_t padding_3;
    uint64_t data_3;
};

static_assert(offsetof(ChrIns_field0x2c8, data_0) == 8);
static_assert(offsetof(ChrIns_field0x2c8, data_1) == 0x28);
static_assert(offsetof(ChrIns_field0x2c8, data_2) == 0x38);
static_assert(offsetof(ChrIns_field0x2c8, data_3) == 0x50);
static_assert(sizeof(ChrIns_field0x2c8) == 0x58);

struct ChrIns
{
    uint8_t padding_0[8 + 0x60];
    PlayerCtrl* playerCtrl;
    PadManipulator* padManipulator;
    uint8_t padding_1[0x54];
    int32_t CharaInitParamID;
    uint8_t padding_3[0xac];
    ChrIns_AnimationMediatorStateInfo lowerThrowAnim;
    ChrIns_AnimationMediatorStateInfo upperThrowAnim;
    uint8_t padding_4[0xd8];
    int32_t curSelectedMagicId;
    ItemUsed curUsedItem;
    uint8_t padding_5[8];
    SpecialEffect* specialEffects;
    QwcSpEffectEquipCtrl* qwcSpEffectEquipCtrl;
    uint8_t data_0[0x48];
    ChrIns_field0x2c8* field0x2c8;
    uint8_t data_0a[0x98];
    HitIns* hitins_1;
    HitIns* hitins_2;
    uint64_t data_0b;
    ChrAttachSys chrattachsys;
    uint8_t padding_7[0x48];
    uint32_t curHp;
    uint32_t maxHp;
    uint8_t padding_8[8];
    uint32_t curSp;
    uint32_t maxSp;
    float damage_taken_scalar;
    uint8_t padding_9[20];
    float PoisonResist;
    float ToxicResist;
    float BleedResist;
    float CurseResist;
    float resistPoisonTotal;
    float resistPlagueTotal;
    float resistBleedingTotal;
    float resistCurseTotal;
    uint8_t padding_10[0x10];
    EntityThrowAnimationStatus* throw_animation_info;
    uint8_t data_1[0x18];
    uint64_t padding_self;
    uint8_t data_2[0x50];
    uint64_t padding_dbgmenu[3];
    uint64_t data_3[3];
    uint8_t padding_str[0x28];
    uint8_t data_4[0x58];
};

static_assert(offsetof(ChrIns, playerCtrl) == 0x60+8);
static_assert(offsetof(ChrIns, padManipulator) == 0x68+8);
static_assert(offsetof(ChrIns, CharaInitParamID) == 0xc4+8);
static_assert(offsetof(ChrIns, lowerThrowAnim) == 0x174+8);
static_assert(offsetof(ChrIns, upperThrowAnim) == 0x17c+8);
static_assert(offsetof(ChrIns, curSelectedMagicId) == 0x25c+8);
static_assert(offsetof(ChrIns, curUsedItem) == 0x260+8);
static_assert(offsetof(ChrIns, specialEffects) == 0x270+8);
static_assert(offsetof(ChrIns, qwcSpEffectEquipCtrl) == 0x278+8);
static_assert(offsetof(ChrIns, data_0) == 0x280+8);
static_assert(offsetof(ChrIns, field0x2c8) == 0x2c8+8);
static_assert(offsetof(ChrIns, data_0a) == 0x2d0+8);
static_assert(offsetof(ChrIns, hitins_1) == 0x368+8);
static_assert(offsetof(ChrIns, hitins_2) == 0x370+8);
static_assert(offsetof(ChrIns, data_0b) == 0x378+8);
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
static_assert(offsetof(ChrIns, data_1) == 0x448+8);
static_assert(offsetof(ChrIns, data_2) == 0x468+8);
static_assert(offsetof(ChrIns, data_3) == 0x4d0+8);
static_assert(offsetof(ChrIns, data_4) == 0x510+8);
static_assert(sizeof(ChrIns) == 0x570);

struct PlayerGameData_AttributeInfo
{
    uint8_t data_0[0x1a4];
};

static_assert(sizeof(PlayerGameData_AttributeInfo) == 0x1a4);

struct EquipInventoryData
{
    //this only deals with what stuff is in your inventory, not equipped. Probably will never matter for rollback
    uint8_t padding_0[0x78];
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
    uint32_t quickbar[6];
};

static_assert(offsetof(EquipItemData, quickbar) == 0x18);
static_assert(sizeof(EquipItemData) == 0x30);

struct EquipGameData
{
    uint8_t padding_0[0x120];
    EquipInventoryData equippedInventory;
    EquipMagicData* equipMagicData;
    EquipItemData equippedItemsInQuickbar;
    uint8_t padding_1[0x48];
};

static_assert(offsetof(EquipGameData, equippedInventory) == 0x120);
static_assert(offsetof(EquipGameData, equipMagicData) == 0x198);
static_assert(offsetof(EquipGameData, equippedItemsInQuickbar) == 0x1a0);
static_assert(sizeof(EquipGameData) == 0x218);

struct PlayerGameData_ChrProperties
{
    uint8_t data_0[0x90];
};

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

struct ChrAsm
{
    uint8_t padding_0[8];
    uint32_t equipped_weapon_style;
    uint32_t l_hand_equipped_index;
    uint32_t r_hand_equipped_index;
    uint8_t padding_1[0x10];
    uint32_t equip_items[20];
    uint8_t padding_2[0xc];
};

static_assert(offsetof(ChrAsm, equipped_weapon_style) == 0x8);
static_assert(offsetof(ChrAsm, l_hand_equipped_index) == 0xc);
static_assert(offsetof(ChrAsm, r_hand_equipped_index) == 0x10);
static_assert(offsetof(ChrAsm, equip_items) == 0x24);
static_assert(sizeof(ChrAsm) == 0x80);

struct PlayerIns
{
    ChrIns chrins;
    uint8_t padding_0[8];
    PlayerGameData* playergamedata;
    uint8_t padding_1[0x268];
    RingEquipCtrl* ringequipctrl;
    WeaponEquipCtrl* weaponequipctrl;
    ProEquipCtrl* proequipctrl;
    uint8_t padding_2[8];
    int32_t curSelectedMagicId;
    ItemUsed curUsedItem;
    uint32_t itemId;
    uint32_t override_equipped_magicId;
    uint8_t padding_3[0x24];
    ChrAsm* chrasm;
    uint8_t padding_4[0x188];
};

static_assert(offsetof(PlayerIns, chrins) == 0x0);
static_assert(offsetof(PlayerIns, playergamedata) == 0x578);
static_assert(offsetof(PlayerIns, ringequipctrl) == 0x7e8);
static_assert(offsetof(PlayerIns, weaponequipctrl) == 0x7f0);
static_assert(offsetof(PlayerIns, proequipctrl) == 0x7f8);
static_assert(offsetof(PlayerIns, curSelectedMagicId) == 0x808);
static_assert(offsetof(PlayerIns, curUsedItem) == 0x80c);
static_assert(offsetof(PlayerIns, itemId) == 0x814);
static_assert(offsetof(PlayerIns, override_equipped_magicId) == 0x818);
static_assert(offsetof(PlayerIns, chrasm) == 0x840);
static_assert(sizeof(PlayerIns) == 0x9d0);

#endif
