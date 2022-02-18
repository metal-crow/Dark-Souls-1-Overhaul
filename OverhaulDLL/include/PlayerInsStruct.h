#pragma once
#ifndef PLAYERINS_STRUCT_H
#define PLAYERINS_STRUCT_H

#include <stddef.h>


typedef struct
{

} PlayerCtrl;

typedef struct
{

} PadManipulator;

typedef struct
{

} ChrTaeAnimEvent;

typedef struct
{
    int32_t animationId;
    uint32_t stateIndex;
} ChrIns_AnimationMediatorStateInfo;

typedef struct
{

} SpecialEffect;

typedef struct
{

} QwcSpEffectEquipCtrl;

typedef struct
{
    uint8_t padding_0[24];
} EntityThrowAnimationStatus;

typedef struct
{
    uint8_t padding_0[0x18];
} ChrAttachSys;

typedef struct
{
    uint8_t padding_0[8];
} ItemUsed;

typedef struct
{
    uint8_t padding_0[8 + 0x60];
    PlayerCtrl* playerCtrl;
    PadManipulator* padManipulator;
    uint8_t padding_1[0x48];
    ChrTaeAnimEvent* chrTaeAnimEvent;
    uint8_t padding_2[4];
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
    uint8_t padding_6[0x100];
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
    uint8_t padding_11[0x120];
} ChrIns;

static_assert(offsetof(ChrIns, playerCtrl) == 0x60+8);
static_assert(offsetof(ChrIns, padManipulator) == 0x68+8);
static_assert(offsetof(ChrIns, chrTaeAnimEvent) == 0xb8+8);
static_assert(offsetof(ChrIns, CharaInitParamID) == 0xc4+8);
static_assert(offsetof(ChrIns, lowerThrowAnim) == 0x174+8);
static_assert(offsetof(ChrIns, upperThrowAnim) == 0x17c+8);
static_assert(offsetof(ChrIns, curSelectedMagicId) == 0x25c+8);
static_assert(offsetof(ChrIns, curUsedItem) == 0x260+8);
static_assert(offsetof(ChrIns, specialEffects) == 0x270+8);
static_assert(offsetof(ChrIns, qwcSpEffectEquipCtrl) == 0x278+8);
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
static_assert(sizeof(ChrIns) == 0x570);

typedef struct
{

} PlayerGameData;

typedef struct
{

} RingEquipCtrl;

typedef struct
{

} WeaponEquipCtrl;

typedef struct
{

} ProEquipCtrl;

typedef struct
{
    uint8_t padding_0[8];
} ChrAsm;

typedef struct
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
} PlayerIns;

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
