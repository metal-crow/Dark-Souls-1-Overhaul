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
    uint8_t padding_0[8];
} ItemUsed;

typedef struct
{

} SpecialEffect;

typedef struct
{

} QwcSpEffectEquipCtrl;

typedef struct
{

} ChrPlayerResidentSlot;

typedef struct
{
    uint8_t padding_0[8];
    //this looks like sfx stuff? do i need it?
    ChrPlayerResidentSlot* firstChrSlot;
    uint8_t padding_1[8];
} ChrAttachSys;

static_assert(offsetof(ChrAttachSys, firstChrSlot) == 0x8);
static_assert(sizeof(ChrAttachSys) == 0x18);

typedef struct
{
    uint8_t padding_0[0x10];
    void* throw_paramdef; //this is a pointer to a const struct, so just read/write the pointer itself
    int32_t throw_id;
    uint8_t padding_1[4];
    uint8_t throwState;
    uint8_t padding_2[3];
    uint32_t ThrowPairHandle;
    uint8_t padding_3[0x48];
    float starting_position_self[4];
    float starting_position_self_[4];
    float starting_position_other[4];
    uint8_t padding_4[4];
    uint32_t throwMask;
    uint8_t padding_5[8];
} EntityThrowAnimationStatus;

static_assert(offsetof(EntityThrowAnimationStatus, throw_paramdef) == 0x10);
static_assert(offsetof(EntityThrowAnimationStatus, throw_id) == 0x18);
static_assert(offsetof(EntityThrowAnimationStatus, throwState) == 0x20);
static_assert(offsetof(EntityThrowAnimationStatus, ThrowPairHandle) == 0x24);
static_assert(offsetof(EntityThrowAnimationStatus, starting_position_self) == 0x70);
static_assert(offsetof(EntityThrowAnimationStatus, starting_position_self_) == 0x80);
static_assert(offsetof(EntityThrowAnimationStatus, starting_position_other) == 0x90);
static_assert(offsetof(EntityThrowAnimationStatus, throwMask) == 0xa4);
static_assert(sizeof(EntityThrowAnimationStatus) == 0xb0);

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
    uint8_t padding_0[0xc];
    int32_t BaseMaxHp;
    uint8_t padding_1[0x18];
    int32_t BaseMaxSp;
    uint8_t padding_2[0xc4];
    float PoisonResist;
    float BleedResist;
    float ToxicResist;
    float CurseResist;
    uint8_t padding_3[0xa4];
} PlayerGameData_AttributeInfo;

static_assert(offsetof(PlayerGameData_AttributeInfo, BaseMaxHp) == 0xc);
static_assert(offsetof(PlayerGameData_AttributeInfo, BaseMaxSp) == 0x28);
static_assert(offsetof(PlayerGameData_AttributeInfo, PoisonResist) == 0xf0);
static_assert(offsetof(PlayerGameData_AttributeInfo, BleedResist) == 0xf4);
static_assert(offsetof(PlayerGameData_AttributeInfo, ToxicResist) == 0xf8);
static_assert(offsetof(PlayerGameData_AttributeInfo, CurseResist) == 0xfc);
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
    //there's a bunch of named fields in here but we can just raw memcpy this entire struct
    uint8_t blob_of_important_properties[0x90];
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
