#pragma once
#ifndef SYSSLOT_STRUCT_H
#define SYSSLOT_STRUCT_H


#include <stddef.h>
#include <stdint.h>
#include "BulletManStruct.h"

enum AttachSysSlotType
{
    TypeChrShineTreasureSlot=0,
    TypeChrSingleSeSlot=1,
    TypeChrSingleSfxSlot=2,
    TypeChrMultiSfxSlot=3,
    TypeChrBurnSlot=4,
    TypeChrGasmanSlot=5,
    TypeChrGrassSlot=6,
    TypeChrFootEffectSlot=7,
    TypeChrRigidOffsetSfxSlot=8,
    TypeChrRigidOffsetChrSlot=9,
    TypeChrSoulEatSlot=10,
    TypeChrSorceryWepSlot=11,
    TypeChrLanternSlot=12,
    TypeChrSingleOneshotSfxSlot=13,
    TypeChrSingleTraceSfxSlot=14,
    TypeChrMagicGoodsUseSfxSlot=15,
    TypeChrActPntSlot=16,
    TypeChrPointLightSlot=17,
    TypeChrWepEnchantSlot=18,
    TypeChrFallingControlSlot=19,
    TypeChrConditionSfxSeSlot=20,
    TypeChrCamouflageSlot=21,
    TypeChrSoulDeadSlot=22,
    TypeChrShinpanshaHaraSlot=23,
    TypeChrLimitInvincibleSlot=25,
    TypeChrDetectSignSlot=26,
    TypeChrRescueSignSlot=27,
    TypeChrTravelItemSlot=28,
    TypeChrStatueDeadSlot=29,
    TypeChrResonanceMagicSlot=30,
    TypeChrRetributionMagicSlot=31,
    TypeChrHellkiteBreathSlot=32,
    TypeChrColiseumEntrantSlot=33,
    TypeChrPlayerResidentSlot=34,
    TypeChrFollowSfxSlot=35,
};

typedef struct AttachSysSlotBaseImpl AttachSysSlotBaseImpl;
typedef struct ChrSingleSfxSlot ChrSingleSfxSlot;
typedef struct ChrResonanceMagicSlot ChrResonanceMagicSlot;
typedef struct ChrPlayerResidentSlot ChrPlayerResidentSlot;
typedef struct ChrMultiSfxSlot ChrMultiSfxSlot;
typedef struct ChrFootEffectSlot ChrFootEffectSlot;
typedef struct ChrActPntSlot_ArrayElem_Field0x8 ChrActPntSlot_ArrayElem_Field0x8;
typedef struct ChrActPntSlot_ArrayElem ChrActPntSlot_ArrayElem;
typedef struct ChrActPntSlot ChrActPntSlot;
typedef struct ChrWepEnchantSlot ChrWepEnchantSlot;
typedef struct ChrStatueDeadSlot ChrStatueDeadSlot;
typedef struct ChrSoulEatSlot ChrSoulEatSlot;
typedef struct ChrSoulDeadSlot ChrSoulDeadSlot;
typedef struct ChrSorceryWepSlot ChrSorceryWepSlot;
typedef struct ChrSingleTraceSfxSlot ChrSingleTraceSfxSlot;
typedef struct ChrSingleSeSlot ChrSingleSeSlot;
typedef struct ChrSingleOneshotSfxSlot ChrSingleOneshotSfxSlot;
typedef struct ChrShinpanshaHaraSlot ChrShinpanshaHaraSlot;
typedef struct ChrShineTreasureSlot ChrShineTreasureSlot;
typedef struct ChrRigidOffsetSfxSlot ChrRigidOffsetSfxSlot;
typedef struct ChrRigidOffsetChrSlot ChrRigidOffsetChrSlot;
typedef struct ChrRetributionMagicSlot ChrRetributionMagicSlot;
typedef struct ChrRescueSignSlot ChrRescueSignSlot;
typedef struct ChrPointLightSlot ChrPointLightSlot;
typedef struct ChrLimitInvincibleSlot ChrLimitInvincibleSlot;
typedef struct ChrLanternSlot ChrLanternSlot;
typedef struct ChrGrassSlot ChrGrassSlot;
typedef struct ChrGasmanSlot_ListElem ChrGasmanSlot_ListElem;
typedef struct ChrGasmanSlot ChrGasmanSlot;
typedef struct ChrConditionSfxSeSlot ChrConditionSfxSeSlot;
typedef struct ChrCamouflageSlot ChrCamouflageSlot;
typedef struct ChrBurnSlot_ListElem ChrBurnSlot_ListElem;
typedef struct ChrBurnSlot ChrBurnSlot;
typedef struct ObjRigidOffsetSfxSlot ObjRigidOffsetSfxSlot;
typedef struct ChrFollowSfxSlot ChrFollowSfxSlot;
typedef struct ChrTravelItemSlot ChrTravelItemSlot;
typedef struct ChrFallingControlSlot ChrFallingControlSlot;

struct AttachSysSlotBaseImpl
{
    uint64_t vtable;
    uint16_t timerId;
    uint16_t slotType: 14;
    uint16_t slotIsUsable: 1;
    uint32_t unk_c;
    AttachSysSlotBaseImpl* next;
    uint64_t padding_1;
};
static_assert(offsetof(AttachSysSlotBaseImpl, timerId) == 8);
static_assert(offsetof(AttachSysSlotBaseImpl, unk_c) == 0xc);
static_assert(offsetof(AttachSysSlotBaseImpl, next) == 0x10);
static_assert(sizeof(AttachSysSlotBaseImpl) == 0x20);

struct ChrSingleSfxSlot
{
    AttachSysSlotBaseImpl base;
    uint32_t unk_20;       // 0x20
    int32_t ffxid;         // 0x24
    uint16_t dummyPolyID_B;// 0x28
    uint16_t dummyPolyID_A;// 0x2a
    uint16_t unk_2c;       // 0x2c
    uint16_t unk_2e;       // 0x2e
    uint16_t unk_30;       // 0x30
    uint8_t unk_32[6];     // 0x32 (gap)
    BulletIns_FollowupBullet bullet; // 0x38
};
static_assert(offsetof(ChrSingleSfxSlot, ffxid) == 0x24);
static_assert(offsetof(ChrSingleSfxSlot, bullet) == 0x38);
static_assert(sizeof(ChrSingleSfxSlot) == 0x68);

struct ChrResonanceMagicSlot
{
    AttachSysSlotBaseImpl base;
    float unk_20;          // 0x20
    float unk_24;          // 0x24
    uint8_t unk_28;        // 0x28 (ghidra: byte)
    uint8_t unk_29[7];     // 0x29 (gap)
};
static_assert(sizeof(ChrResonanceMagicSlot) == 0x30);

struct ChrPlayerResidentSlot
{
    AttachSysSlotBaseImpl base;
    void* playerIns; //PlayerIns*. Points to a player we already save, so a raw ptr is fine (0x20)
    uint8_t unk_28;  //bool (0x28)
    uint8_t unk_29[7]; //gap (0x29)
    //PlayerGameData vector. These are pointers to PlayerGameData's from players in
    //FrpgSessionManagerImp->connected_player_info; since we save Players already these are raw pointers
    uint64_t PlayerGameData_Vec[4]; // 0x30
};
static_assert(offsetof(ChrPlayerResidentSlot, playerIns) == 0x20);
static_assert(offsetof(ChrPlayerResidentSlot, PlayerGameData_Vec) == 0x30);
static_assert(sizeof(ChrPlayerResidentSlot) == 0x50);

struct ChrMultiSfxSlot
{
    AttachSysSlotBaseImpl base;
    uint32_t unk_20;       // 0x20
    int32_t ffxid;         // 0x24
    uint16_t dummyPolyID_B;// 0x28
    uint16_t dummyPolyID_A;// 0x2a
    uint16_t unk_2c;       // 0x2c
    int16_t bullet_list_len; // 0x2e
    BulletIns_FollowupBullet* bullet_list; // 0x30
};
static_assert(offsetof(ChrMultiSfxSlot, bullet_list_len) == 0x2e);
static_assert(sizeof(ChrMultiSfxSlot) == 0x38);

struct ChrFootEffectSlot
{
    AttachSysSlotBaseImpl base;
    uint32_t unk_20;       // 0x20
    uint16_t unk_24;       // 0x24
    uint16_t unk_26;       // 0x26
    uint32_t unk_28;       // 0x28
    uint32_t unk_2c;       // 0x2c
    BulletIns_FollowupBullet bullet; // 0x30
};
static_assert(offsetof(ChrFootEffectSlot, bullet) == 0x30);
static_assert(sizeof(ChrFootEffectSlot) == 0x60);

struct ChrActPntSlot_ArrayElem
{
    float unk_0;           // 0x0 (ghidra: float)
    uint32_t unk_4;        // 0x4 (gap)
    void* ActPntManImp_Entry; //always non-null. Points to a ActPntManImp_Entry in the ActPntManImp global list, so if we save that this can just be a raw pointer (0x8)
};
static_assert(offsetof(ChrActPntSlot_ArrayElem, ActPntManImp_Entry) == 0x8);
static_assert(sizeof(ChrActPntSlot_ArrayElem) == 0x10);

struct ChrActPntSlot
{
    AttachSysSlotBaseImpl base;
    uint16_t unk_20;       // 0x20 (ghidra: ushort)
    int16_t array_len;     // 0x22
    uint32_t unk_24;       // 0x24 (gap)
    ChrActPntSlot_ArrayElem arry[10]; //variable length array, we just have to preallocate a safe max (0x28)
};
static_assert(offsetof(ChrActPntSlot, array_len) == 0x22);
static_assert(offsetof(ChrActPntSlot, arry) == 0x28);
static_assert(sizeof(ChrActPntSlot) == 0x28 + sizeof(ChrActPntSlot_ArrayElem)*10);

struct ChrWepEnchantSlot
{
    AttachSysSlotBaseImpl base;
    uint32_t unk_20;       // 0x20
    int32_t sfxId;         // 0x24
    uint32_t unk_28;       // 0x28
    int32_t seId;          // 0x2c
    float time_a;          // 0x30
    float time_b;          // 0x34
    uint8_t unk_38;        // 0x38
    uint8_t effectType;    // 0x39 (ghidra: SP_EFFECT_VFX_EFFECT_TYPE)
    uint8_t unk_3a;        // 0x3a
    uint8_t invisible;     // 0x3b (ghidra: bool)
    uint8_t unk_3c;        // 0x3c
    uint8_t unk_3d;        // 0x3d (gap)
    int16_t followup_bullet_list_len; // 0x3e
    BulletIns_FollowupBullet* followup_bullet_list; // 0x40
    uint32_t unk_48;       // 0x48
    uint32_t unk_4c;       // 0x4c
};
static_assert(offsetof(ChrWepEnchantSlot, followup_bullet_list_len) == 0x3e);
static_assert(offsetof(ChrWepEnchantSlot, followup_bullet_list) == 0x40);
static_assert(sizeof(ChrWepEnchantSlot) == 0x50);

struct ChrStatueDeadSlot
{
    AttachSysSlotBaseImpl base;
    uint8_t unk_20;        // 0x20 (ghidra: bool)
    uint8_t unk_21[3];     // 0x21 (gap)
    int32_t followup_bullet_list_len; // 0x24
    BulletIns_FollowupBullet* followup_bullet_list; // 0x28
};
static_assert(offsetof(ChrStatueDeadSlot, followup_bullet_list_len) == 0x24);
static_assert(offsetof(ChrStatueDeadSlot, followup_bullet_list) == 0x28);
static_assert(sizeof(ChrStatueDeadSlot) == 0x30);

struct ChrSoulEatSlot
{
    AttachSysSlotBaseImpl base;
    uint16_t unk_20;       // 0x20
    uint8_t unk_22[14];    // 0x22 (gap)
    float unk_30;          // 0x30
    float unk_34;          // 0x34
    float unk_38;          // 0x38
    float unk_3c;          // 0x3c
    BulletIns_FollowupBullet bullet; // 0x40
};
static_assert(offsetof(ChrSoulEatSlot, bullet) == 0x40);
static_assert(sizeof(ChrSoulEatSlot) == 0x70);

struct ChrSoulDeadSlot
{
    AttachSysSlotBaseImpl base;
    uint32_t unk_20;       // 0x20
    uint32_t unk_24;       // 0x24 (gap)
};
static_assert(sizeof(ChrSoulDeadSlot) == 0x28);

struct ChrSorceryWepSlot
{
    AttachSysSlotBaseImpl base;
    //ghidra models 0x28..0x98 as ChrSorceryWepSlot_field0x28Elem[2] = { uint; undefined4; FollowupBullet }
    uint8_t unk_20;        // 0x20
    uint8_t unk_21;        // 0x21
    uint8_t unk_22[6];     // 0x22 (gap)
    uint32_t unk_28;       // 0x28 (elem[0] header)
    uint32_t unk_2c;       // 0x2c (elem[0] header)
    BulletIns_FollowupBullet bullet1; // 0x30 (elem[0] followupBullet)
    uint32_t unk_60;       // 0x60 (elem[1] header)
    uint32_t unk_64;       // 0x64 (elem[1] header)
    BulletIns_FollowupBullet bullet2; // 0x68 (elem[1] followupBullet)
};
static_assert(offsetof(ChrSorceryWepSlot, bullet1) == 0x30);
static_assert(offsetof(ChrSorceryWepSlot, bullet2) == 0x68);
static_assert(sizeof(ChrSorceryWepSlot) == 0x98);

struct ChrSingleTraceSfxSlot
{
    AttachSysSlotBaseImpl base;
    uint32_t ffxid;        // 0x20
    uint16_t dummyPolyA;   // 0x24
    uint16_t dummyPolyB;   // 0x26
    uint8_t dummyPolySourceInternal; // 0x28
    uint8_t unk_29;        // 0x29 (gap)
    uint16_t unk_2a;       // 0x2a
    uint8_t unk_2c[4];     // 0x2c (gap)
    BulletIns_FollowupBullet bullet; // 0x30
};
static_assert(offsetof(ChrSingleTraceSfxSlot, bullet) == 0x30);
static_assert(sizeof(ChrSingleTraceSfxSlot) == 0x60);

struct ChrSingleSeSlot
{
    AttachSysSlotBaseImpl base;
    uint32_t unk_20;       // 0x20
    uint32_t unk_24;       // 0x24
    uint32_t unk_28;       // 0x28
    uint32_t unk_2c;       // 0x2c
    uint16_t unk_30;       // 0x30
    uint16_t unk_32;       // 0x32
    uint32_t unk_34;       // 0x34
    uint32_t unk_38;       // 0x38
    uint32_t unk_3c;       // 0x3c
};
static_assert(sizeof(ChrSingleSeSlot) == 0x40);

struct ChrSingleOneshotSfxSlot
{
    AttachSysSlotBaseImpl base;
    uint32_t unk_20;       // 0x20
    uint16_t unk_24;       // 0x24
    uint16_t unk_26;       // 0x26
    BulletIns_FollowupBullet bullet; // 0x28
};
static_assert(offsetof(ChrSingleOneshotSfxSlot, bullet) == 0x28);
static_assert(sizeof(ChrSingleOneshotSfxSlot) == 0x58);

struct ChrShinpanshaHaraSlot
{
    AttachSysSlotBaseImpl base;
    uint8_t unk_20;        // 0x20
    uint8_t unk_21[7];     // 0x21 (gap)
    BulletIns_FollowupBullet bullet; // 0x28
};
static_assert(offsetof(ChrShinpanshaHaraSlot, bullet) == 0x28);
static_assert(sizeof(ChrShinpanshaHaraSlot) == 0x58);

struct ChrShineTreasureSlot
{
    AttachSysSlotBaseImpl base;
    uint16_t unk_20;       // 0x20
    uint16_t unk_22;       // 0x22 (gap)
    uint32_t unk_24;       // 0x24
    BulletIns_FollowupBullet bullet; // 0x28
};
static_assert(offsetof(ChrShineTreasureSlot, bullet) == 0x28);
static_assert(sizeof(ChrShineTreasureSlot) == 0x58);

struct ChrRigidOffsetSfxSlot
{
    AttachSysSlotBaseImpl base;
    uint32_t unk_20;       // 0x20
    uint16_t unk_24;       // 0x24
    uint16_t unk_26;       // 0x26
    uint8_t unk_28[8];     // 0x28 (gap)
    uint8_t fieldE0[48];   // 0x30 (ghidra: NS_FRPG_BulletIns_Field0x90_Field0xE0)
    BulletIns_FollowupBullet bullet; // 0x60
};
static_assert(offsetof(ChrRigidOffsetSfxSlot, fieldE0) == 0x30);
static_assert(offsetof(ChrRigidOffsetSfxSlot, bullet) == 0x60);
static_assert(sizeof(ChrRigidOffsetSfxSlot) == 0x90);

struct ChrRigidOffsetChrSlot
{
    AttachSysSlotBaseImpl base;
    uint32_t unk_20;       // 0x20
    uint16_t unk_24;       // 0x24
    uint16_t unk_26;       // 0x26
    uint8_t unk_28[8];     // 0x28 (gap)
    uint8_t fieldE0[48];   // 0x30 (ghidra: NS_FRPG_BulletIns_Field0x90_Field0xE0)
};
static_assert(offsetof(ChrRigidOffsetChrSlot, fieldE0) == 0x30);
static_assert(sizeof(ChrRigidOffsetChrSlot) == 0x60);

struct ChrRetributionMagicSlot
{
    AttachSysSlotBaseImpl base;
    uint32_t unk_20;       // 0x20
    uint32_t unk_24;       // 0x24
    uint32_t unk_28;       // 0x28
    uint32_t unk_2c;       // 0x2c (gap)
};
static_assert(sizeof(ChrRetributionMagicSlot) == 0x30);

struct ChrPointLightSlot
{
    AttachSysSlotBaseImpl base;
    uint16_t unk_20;       // 0x20
    uint16_t unk_22;       // 0x22
    uint16_t unk_24;       // 0x24
    uint16_t unk_26;       // 0x26 (gap)
    uint32_t unk_28;       // 0x28
    uint32_t unk_2c;       // 0x2c (gap)
};
static_assert(sizeof(ChrPointLightSlot) == 0x30);

struct ChrMagicGoodsUseSfxSlot
{
    AttachSysSlotBaseImpl base;
    int32_t ffxid;         // 0x20
    int16_t dummyPolyID;   // 0x24
    int8_t dummyPolySourceInternal; // 0x26
    uint8_t unk_27;        // 0x27 (gap)
    uint16_t unk_28;       // 0x28
    uint8_t unk_2a[6];     // 0x2a (gap)
    BulletIns_FollowupBullet bullet; // 0x30
};
static_assert(offsetof(ChrMagicGoodsUseSfxSlot, bullet) == 0x30);
static_assert(sizeof(ChrMagicGoodsUseSfxSlot) == 0x60);

struct ChrLimitInvincibleSlot
{
    AttachSysSlotBaseImpl base;
    uint32_t unk_20;       // 0x20
    uint32_t unk_24;       // 0x24 (gap)
    void* chr; //ChrIns*, can treat as raw ptr (0x28)
};
static_assert(offsetof(ChrLimitInvincibleSlot, chr) == 0x28);
static_assert(sizeof(ChrLimitInvincibleSlot) == 0x30);

struct ChrLanternSlot
{
    AttachSysSlotBaseImpl base;
    uint16_t unk_20;       // 0x20
    uint16_t unk_22;       // 0x22 (gap)
    uint32_t unk_24;       // 0x24
    BulletIns_FollowupBullet bullet1; // 0x28
    BulletIns_FollowupBullet bullet2; // 0x58
    BulletIns_FollowupBullet bullet3; // 0x88
};
static_assert(offsetof(ChrLanternSlot, bullet1) == 0x28);
static_assert(offsetof(ChrLanternSlot, bullet2) == 0x58);
static_assert(offsetof(ChrLanternSlot, bullet3) == 0x88);
static_assert(sizeof(ChrLanternSlot) == 0xb8);

struct ChrGrassSlot
{
    AttachSysSlotBaseImpl base;

};
//static_assert(sizeof(ChrGrassSlot) == 0x50);

struct ChrGasmanSlot_ListElem
{
    uint8_t unk_0;         // 0x0
    uint8_t unk_1[11];     // 0x1 (gap)
    float vector[4];       // 0xc (ghidra: float[4])
    uint8_t unk_1c[4];     // 0x1c (gap)
    BulletIns_FollowupBullet bullet_a; // 0x20
    BulletIns_FollowupBullet bullet_b; // 0x50
};
static_assert(offsetof(ChrGasmanSlot_ListElem, vector) == 0xc);
static_assert(offsetof(ChrGasmanSlot_ListElem, bullet_a) == 0x20);
static_assert(sizeof(ChrGasmanSlot_ListElem) == 0x80);

struct ChrGasmanSlot
{
    AttachSysSlotBaseImpl base;
    uint32_t unk_20;       // 0x20
    uint16_t unk_24;       // 0x24
    int16_t list_len;      // 0x26
    ChrGasmanSlot_ListElem* list; // 0x28
};
static_assert(offsetof(ChrGasmanSlot, list_len) == 0x26);
static_assert(offsetof(ChrGasmanSlot, list) == 0x28);
static_assert(sizeof(ChrGasmanSlot) == 0x30);

struct ChrConditionSfxSeSlot
{
    AttachSysSlotBaseImpl base;
    int32_t sfxId1;        // 0x20
    int32_t seId1;         // 0x24
    int16_t dummyId1;      // 0x28
    int16_t unkBitfield;   // 0x2a
    int32_t sfxId2;        // 0x2c
    int32_t seId2;         // 0x30
    int16_t dummyId2;      // 0x34
    int16_t followupbullet_list_len; // 0x36
    BulletIns_FollowupBullet* followupbullet_list; // 0x38
    uint32_t unk_40;       // 0x40
    uint32_t unk_44;       // 0x44
};
static_assert(offsetof(ChrConditionSfxSeSlot, followupbullet_list_len) == 0x36);
static_assert(offsetof(ChrConditionSfxSeSlot, followupbullet_list) == 0x38);
static_assert(sizeof(ChrConditionSfxSeSlot) == 0x48);

struct ChrCamouflageSlot
{
    AttachSysSlotBaseImpl base;

};
//static_assert(sizeof(ChrCamouflageSlot) == 0x40);

struct ChrBurnSlot_ListElem
{
    uint64_t unk_0;        // 0x0 (undefined in ghidra)
    BulletIns_FollowupBullet bullet; // 0x8
};
static_assert(offsetof(ChrBurnSlot_ListElem, bullet) == 0x8);
static_assert(sizeof(ChrBurnSlot_ListElem) == 0x38);

struct ChrBurnSlot
{
    AttachSysSlotBaseImpl base;
    uint32_t unk_20;       // 0x20
    uint32_t unk_24;       // 0x24
    uint16_t unk_28;       // 0x28
    int16_t floatlist_len; // 0x2a
    uint32_t unk_2c;       // 0x2c (gap)
    float* floatlist;      // 0x30
    int32_t list_len;      // 0x38
    uint32_t unk_3c;       // 0x3c (gap)
    ChrBurnSlot_ListElem* list; // 0x40
    uint8_t unk_48;        // 0x48 (ghidra: bool)
    uint8_t unk_49[3];     // 0x49 (gap)
    uint32_t unk_4c;       // 0x4c
    uint32_t unk_50;       // 0x50
    float unk_54;          // 0x54
};
static_assert(offsetof(ChrBurnSlot, floatlist_len) == 0x2a);
static_assert(offsetof(ChrBurnSlot, floatlist) == 0x30);
static_assert(offsetof(ChrBurnSlot, list_len) == 0x38);
static_assert(offsetof(ChrBurnSlot, list) == 0x40);
static_assert(sizeof(ChrBurnSlot) == 0x58);

struct ObjRigidOffsetSfxSlot
{
    AttachSysSlotBaseImpl base;
    uint32_t unk_20;       // 0x20
    uint16_t unk_24;       // 0x24
    uint16_t unk_26;       // 0x26
    uint8_t unk_28[8];     // 0x28 (gap)
    uint8_t fieldE0[48];   // 0x30 (ghidra: NS_FRPG_BulletIns_Field0x90_Field0xE0)
    BulletIns_FollowupBullet bullet; // 0x60
};
static_assert(offsetof(ObjRigidOffsetSfxSlot, fieldE0) == 0x30);
static_assert(offsetof(ObjRigidOffsetSfxSlot, bullet) == 0x60);
static_assert(sizeof(ObjRigidOffsetSfxSlot) == 0x90);

struct ChrFollowSfxSlot
{
    AttachSysSlotBaseImpl base;
    uint32_t unk_20;       // 0x20
    uint32_t unk_24;       // 0x24 (gap)
    BulletIns_FollowupBullet bullet; // 0x28
    uint32_t unk_58;       // 0x58
    uint32_t unk_5c;       // 0x5c
    uint32_t unk_60;       // 0x60
    uint32_t unk_64;       // 0x64 (gap)
};
static_assert(offsetof(ChrFollowSfxSlot, bullet) == 0x28);
static_assert(offsetof(ChrFollowSfxSlot, unk_58) == 0x58);
static_assert(sizeof(ChrFollowSfxSlot) == 0x68);

struct ChrTravelItemSlot
{
    AttachSysSlotBaseImpl base;
    uint8_t unk_20;        // 0x20
    uint8_t unk_21[7];     // 0x21 (gap)
};
static_assert(sizeof(ChrTravelItemSlot) == 0x28);

struct ChrFallingControlSlot
{
    AttachSysSlotBaseImpl base;
    uint8_t unk_20;        // 0x20
    uint8_t unk_21[7];     // 0x21 (gap)
};
static_assert(sizeof(ChrFallingControlSlot) == 0x28);

#endif
