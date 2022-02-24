#pragma once
#ifndef PLAYERINS_STRUCT_H
#define PLAYERINS_STRUCT_H

//NOTE: Anything marked "data_x" is a bunch of non-pointer data that can be saved/loaded without really knowing what it is.
// See the ghidra repo for more specific info about what is in that blob, if it's even known

#include <stddef.h>


typedef struct
{
    ChrCtrl chrCtrl;

} PlayerCtrl;

static_assert(sizeof(PlayerCtrl) == 0x36c);

typedef struct
{
    uint8_t padding_0[0x8];
    uint8_t data_0[0x228];
} ChrManipulator;

static_assert(sizeof(ChrManipulator) == 0x230);

typedef struct
{
    //this is probably used in place of PadManipulator in PlayerIns for remote PCs. need to verify
} NetworkManipulator;

typedef struct
{
    ChrManipulator chrManipulator;
    uint8_t padding_0[0xc];
    uint8_t data_0[4];
    uint8_t padding_1[0x20];
    uint8_t data_1[0x100];
} PadManipulator;

static_assert(offsetof(PadManipulator, chrManipulator) == 0x0);
static_assert(offsetof(PadManipulator, data_0) == 0x23c);
static_assert(offsetof(PadManipulator, data_1) == 0x260);
static_assert(sizeof(PadManipulator) == 0x360);

typedef struct
{
    int32_t animationId;
    uint32_t stateIndex;
} ChrIns_AnimationMediatorStateInfo;

static_assert(offsetof(ChrIns_AnimationMediatorStateInfo, animationId) == 0x0);
static_assert(offsetof(ChrIns_AnimationMediatorStateInfo, stateIndex) == 0x4);
static_assert(sizeof(ChrIns_AnimationMediatorStateInfo) == 0x8);

typedef struct
{
    int32_t itemId;
    int32_t amountUsed;
} ItemUsed;

static_assert(offsetof(ItemUsed, itemId) == 0x0);
static_assert(offsetof(ItemUsed, amountUsed) == 0x4);
static_assert(sizeof(ItemUsed) == 0x8);

typedef struct
{
    uint8_t data_0[0x38];
    uint64_t paramRowBytes; //this is a pointer to a const struct, so just read/write the pointer itself
    SpecialEffect_Info* next;
    SpecialEffect_Info* prev;
} SpecialEffect_Info;

static_assert(offsetof(SpecialEffect_Info, data_0) == 0x0);
static_assert(offsetof(SpecialEffect_Info, paramRowBytes) == 0x38);
static_assert(offsetof(SpecialEffect_Info, next) == 0x40);
static_assert(offsetof(SpecialEffect_Info, prev) == 0x48);
static_assert(sizeof(SpecialEffect_Info) == 0x50);

typedef struct
{
    uint8_t padding_0[8];
    SpecialEffect_Info* specialEffect_Info;
    bool speffectIsBeingRun;
    uint8_t data_0[3];
    float unk1;
    uint8_t padding_2[8];
    uint32_t flags;
    uint32_t unk2;
    uint8_t padding_3[8];
    int32_t debugActivateSpEffect;
    uint32_t unk3;
} SpecialEffect;

static_assert(offsetof(SpecialEffect, specialEffect_Info) == 0x0 + 8);
static_assert(offsetof(SpecialEffect, speffectIsBeingRun) == 0x8 + 8);
static_assert(offsetof(SpecialEffect, unk1) == 0xc + 8);
static_assert(offsetof(SpecialEffect, flags) == 0x18 + 8);
static_assert(offsetof(SpecialEffect, unk2) == 0x1c + 8);
static_assert(offsetof(SpecialEffect, debugActivateSpEffect) == 0x28 + 8);
static_assert(offsetof(SpecialEffect, unk3) == 0x2c + 8);
static_assert(sizeof(SpecialEffect) == 0x38);

typedef struct
{
    uint8_t padding_0[0x18];
    uint32_t* arry;
    uint32_t arry_len;
    uint32_t unk1;
    uint32_t unk2[2];
} QwcSpEffectEquipCtrl;

static_assert(offsetof(QwcSpEffectEquipCtrl, arry) == 0x18);
static_assert(offsetof(QwcSpEffectEquipCtrl, arry_len) == 0x20);
static_assert(offsetof(QwcSpEffectEquipCtrl, unk1) == 0x24);
static_assert(offsetof(QwcSpEffectEquipCtrl, unk2) == 0x28);
static_assert(sizeof(QwcSpEffectEquipCtrl) == 0x30);

typedef struct
{
    //this is sfx stuff, unneeded for rollback
    uint8_t padding_0[0x18];
} ChrAttachSys;

static_assert(sizeof(ChrAttachSys) == 0x18);

typedef struct
{
    uint8_t padding_0[0x10];
    uint64_t throw_paramdef; //this is a pointer to a const struct, so just read/write the pointer itself
    uint8_t data_0[0x40];
    uint8_t padding_1[0x8];
    uint8_t data_1[0x50];
} EntityThrowAnimationStatus;

static_assert(offsetof(EntityThrowAnimationStatus, throw_paramdef) == 0x10);
static_assert(offsetof(EntityThrowAnimationStatus, data_0) == 0x18);
static_assert(offsetof(EntityThrowAnimationStatus, padding_1) == 0x58);
static_assert(offsetof(EntityThrowAnimationStatus, data_1) == 0x60);
static_assert(sizeof(EntityThrowAnimationStatus) == 0xb0);

typedef struct
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
    uint8_t data_0[0x1a4];
} PlayerGameData_AttributeInfo;

static_assert(sizeof(PlayerGameData_AttributeInfo) == 0x1a4);

typedef struct
{
    //this only deals with what stuff is in your inventory, not equipped. Probably will never matter for rollback
    uint8_t padding_0[0x78];
} EquipInventoryData;

static_assert(sizeof(EquipInventoryData) == 0x78);

typedef struct
{
    int32_t magic_id;
    uint32_t count;
} MagicSlot;

static_assert(offsetof(MagicSlot, magic_id) == 0x0);
static_assert(offsetof(MagicSlot, count) == 0x4);
static_assert(sizeof(MagicSlot) == 0x8);

typedef struct
{
    uint8_t padding_0[0x18];
    MagicSlot equippedMagicList[12];
    int32_t curSelectedMagicSlot;
} EquipMagicData;

static_assert(offsetof(EquipMagicData, equippedMagicList) == 0x18);
static_assert(offsetof(EquipMagicData, curSelectedMagicSlot) == 0x78);
static_assert(sizeof(EquipMagicData) == 0x7c);

typedef struct
{
    uint8_t padding_0[0x18];
    uint32_t quickbar[6];
} EquipItemData;

static_assert(offsetof(EquipItemData, quickbar) == 0x18);
static_assert(sizeof(EquipItemData) == 0x30);

typedef struct
{
    uint8_t padding_0[0x120];
    EquipInventoryData equippedInventory;
    EquipMagicData* equipMagicData;
    EquipItemData equippedItemsInQuickbar;
    uint8_t padding_1[0x48];
} EquipGameData;

static_assert(offsetof(EquipGameData, equippedInventory) == 0x120);
static_assert(offsetof(EquipGameData, equipMagicData) == 0x198);
static_assert(offsetof(EquipGameData, equippedItemsInQuickbar) == 0x1a0);
static_assert(sizeof(EquipGameData) == 0x218);

typedef struct
{
    uint8_t data_0[0x90];
} PlayerGameData_ChrProperties;

static_assert(sizeof(PlayerGameData_ChrProperties) == 0x90);

typedef struct
{
    uint8_t padding_0[0x10];
    PlayerGameData_AttributeInfo attribs;
    uint8_t padding_1[0xcc];
    EquipGameData equipGameData;
    uint8_t padding_2[0xd8];
    PlayerGameData_ChrProperties ChrProperties;
    uint8_t padding_3[0x60];
} PlayerGameData;

static_assert(offsetof(PlayerGameData, attribs) == 0x10);
static_assert(offsetof(PlayerGameData, equipGameData) == 0x280);
static_assert(offsetof(PlayerGameData, ChrProperties) == 0x570);
static_assert(sizeof(PlayerGameData) == 0x660);

typedef struct
{
    uint8_t padding_0[0x10];
    SpecialEffect* spEffectList;
    uint32_t* equipped_rings_ids; //index is slot num, elem is id
    uint32_t array_len;
    uint8_t padding_1[0xc];
} RingEquipCtrl;

static_assert(offsetof(RingEquipCtrl, spEffectList) == 0x10);
static_assert(offsetof(RingEquipCtrl, equipped_rings_ids) == 0x18);
static_assert(offsetof(RingEquipCtrl, array_len) == 0x20);
static_assert(sizeof(RingEquipCtrl) == 0x30);

typedef struct
{
    uint8_t padding_0[0x10];
    SpecialEffect* spEffectList;
    uint32_t* equipped_weapons_ids; //index is slot num, elem is id
    uint32_t array_len;
    uint8_t padding_1[0xc];
} WeaponEquipCtrl;

static_assert(offsetof(WeaponEquipCtrl, spEffectList) == 0x10);
static_assert(offsetof(WeaponEquipCtrl, equipped_weapons_ids) == 0x18);
static_assert(offsetof(WeaponEquipCtrl, array_len) == 0x20);
static_assert(sizeof(WeaponEquipCtrl) == 0x30);

typedef struct
{
    uint8_t padding_0[0x10];
    SpecialEffect* spEffectList;
    uint32_t* equipped_armors_ids; //index is slot num, elem is id
    uint32_t array_len;
    uint8_t padding_1[0xc];
} ProEquipCtrl;

static_assert(offsetof(ProEquipCtrl, spEffectList) == 0x10);
static_assert(offsetof(ProEquipCtrl, equipped_armors_ids) == 0x18);
static_assert(offsetof(ProEquipCtrl, array_len) == 0x20);
static_assert(sizeof(ProEquipCtrl) == 0x30);

typedef struct
{
    uint8_t padding_0[8];
    uint32_t equipped_weapon_style;
    uint32_t l_hand_equipped_index;
    uint32_t r_hand_equipped_index;
    uint8_t padding_1[0x10];
    uint32_t equip_items[20];
    uint8_t padding_2[0xc];
} ChrAsm;

static_assert(offsetof(ChrAsm, equipped_weapon_style) == 0x8);
static_assert(offsetof(ChrAsm, l_hand_equipped_index) == 0xc);
static_assert(offsetof(ChrAsm, r_hand_equipped_index) == 0x10);
static_assert(offsetof(ChrAsm, equip_items) == 0x24);
static_assert(sizeof(ChrAsm) == 0x80);

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
