#pragma once
#ifndef SYSSLOT_STRUCT_H
#define SYSSLOT_STRUCT_H

//NOTE: Anything marked "data_x" is a bunch of non-pointer data that can be saved/loaded without really knowing what it is.
// See the ghidra repo for more specific info about what is in that blob, if it's even known

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
    uint32_t data_0;
    AttachSysSlotBaseImpl* next;
    uint64_t padding_1;
};
static_assert(offsetof(AttachSysSlotBaseImpl, timerId) == 8);
static_assert(offsetof(AttachSysSlotBaseImpl, data_0) == 0xc);
static_assert(offsetof(AttachSysSlotBaseImpl, next) == 0x10);
static_assert(sizeof(AttachSysSlotBaseImpl) == 0x20);

struct ChrSingleSfxSlot
{
    AttachSysSlotBaseImpl base;
    uint8_t data_0[0x18];
    BulletIns_FollowupBullet bullet;
};
static_assert(sizeof(ChrSingleSfxSlot) == 0x68);

struct ChrResonanceMagicSlot
{
    AttachSysSlotBaseImpl base;
    uint8_t data_0[16];
};
static_assert(sizeof(ChrResonanceMagicSlot) == 0x30);

struct ChrPlayerResidentSlot
{
    AttachSysSlotBaseImpl base;
    uint64_t data_0; //ptr to data we already save
    uint64_t data_1;
    //These are pointers to PlayerGameData's from players in FrpgSessionManagerImp->connected_player_info.
    //Since we save Players already these can just be treated as raw pointers
    uint64_t data_2[4];
};
static_assert(sizeof(ChrPlayerResidentSlot) == 0x50);

struct ChrMultiSfxSlot
{
    AttachSysSlotBaseImpl base;
    uint8_t data_0[16];
    BulletIns_FollowupBullet* bullet;
};
static_assert(sizeof(ChrMultiSfxSlot) == 0x38);

struct ChrFootEffectSlot
{
    AttachSysSlotBaseImpl base;
    uint8_t data_0[16];
    BulletIns_FollowupBullet* bullet;
};
static_assert(sizeof(ChrFootEffectSlot) == 0x38);

struct ChrActPntSlot
{
    AttachSysSlotBaseImpl base;
    uint64_t data_0;
};
static_assert(sizeof(ChrActPntSlot) == 0x28);

struct ChrWepEnchantSlot
{
    AttachSysSlotBaseImpl base;
    uint8_t data_0[30];
    int16_t followup_bullet_list_len;
    BulletIns_FollowupBullet* followup_bullet_list;
    uint8_t data_1[8];
};
static_assert(sizeof(ChrWepEnchantSlot) == 0x50);

struct ChrStatueDeadSlot
{
    AttachSysSlotBaseImpl base;
    uint32_t data_0;
    int32_t followup_bullet_list_len;
    BulletIns_FollowupBullet* followup_bullet_list;
};
static_assert(sizeof(ChrStatueDeadSlot) == 0x30);

struct ChrSoulEatSlot
{
    AttachSysSlotBaseImpl base;
    uint8_t data_0[32];
    BulletIns_FollowupBullet bullet;
};
static_assert(sizeof(ChrSoulEatSlot) == 0x70);

struct ChrSoulDeadSlot
{
    AttachSysSlotBaseImpl base;
    uint64_t data_0;
};
static_assert(sizeof(ChrSoulDeadSlot) == 0x28);

struct ChrSorceryWepSlot
{
    AttachSysSlotBaseImpl base;
    uint8_t data_0[8];
    uint64_t data_1;
    BulletIns_FollowupBullet bullet1;
    uint64_t data_2;
    BulletIns_FollowupBullet bullet2;
};
static_assert(sizeof(ChrSorceryWepSlot) == 0x98);

struct ChrSingleTraceSfxSlot
{
    AttachSysSlotBaseImpl base;
    uint8_t data_0[16];
    BulletIns_FollowupBullet bullet;
};
static_assert(sizeof(ChrSingleTraceSfxSlot) == 0x60);

struct ChrSingleSeSlot
{
    AttachSysSlotBaseImpl base;
    uint8_t data_0[32];
};
static_assert(sizeof(ChrSingleSeSlot) == 0x40);

struct ChrSingleOneshotSfxSlot
{
    AttachSysSlotBaseImpl base;
    uint8_t data_0[8];
    BulletIns_FollowupBullet bullet;
};
static_assert(sizeof(ChrSingleOneshotSfxSlot) == 0x58);

struct ChrShinpanshaHaraSlot
{
    AttachSysSlotBaseImpl base;
    uint8_t data_0[8];
    BulletIns_FollowupBullet bullet;
};
static_assert(sizeof(ChrShinpanshaHaraSlot) == 0x58);

struct ChrShineTreasureSlot
{
    AttachSysSlotBaseImpl base;
    uint8_t data_0[8];
    BulletIns_FollowupBullet bullet;
};
static_assert(sizeof(ChrShineTreasureSlot) == 0x58);

struct ChrRigidOffsetSfxSlot
{
    AttachSysSlotBaseImpl base;
    uint8_t data_0[16+0x30];
    BulletIns_FollowupBullet bullet;
};
static_assert(sizeof(ChrRigidOffsetSfxSlot) == 0x90);

struct ChrRigidOffsetChrSlot
{
    AttachSysSlotBaseImpl base;
    uint8_t data_0[0x40];
};
static_assert(sizeof(ChrRigidOffsetChrSlot) == 0x60);

struct ChrRetributionMagicSlot
{
    AttachSysSlotBaseImpl base;
    uint8_t data_0[16];
};
static_assert(sizeof(ChrRetributionMagicSlot) == 0x30);

struct ChrPointLightSlot
{
    AttachSysSlotBaseImpl base;
    uint8_t data_0[16];
};
static_assert(sizeof(ChrPointLightSlot) == 0x30);

struct ChrMagicGoodsUseSfxSlot
{
    AttachSysSlotBaseImpl base;
    uint8_t data_0[16];
    BulletIns_FollowupBullet bullet;
};
static_assert(sizeof(ChrMagicGoodsUseSfxSlot) == 0x60);

struct ChrLimitInvincibleSlot
{
    AttachSysSlotBaseImpl base;
    uint64_t data_0;
    void* chr; //ChrIns*, can treat as raw ptr
};
static_assert(sizeof(ChrLimitInvincibleSlot) == 0x30);

struct ChrLanternSlot
{
    AttachSysSlotBaseImpl base;
    uint8_t data_0[8];
    BulletIns_FollowupBullet bullet1;
    BulletIns_FollowupBullet bullet2;
    BulletIns_FollowupBullet bullet3;
};
static_assert(sizeof(ChrLanternSlot) == 0xb8);

struct ChrGrassSlot
{
    AttachSysSlotBaseImpl base;
    
};
//static_assert(sizeof(ChrGrassSlot) == 0x50);

struct ChrGasmanSlot_ListElem
{
    uint8_t data_0[0x20];
    BulletIns_FollowupBullet bullet_a;
    BulletIns_FollowupBullet bullet_b;
};
static_assert(sizeof(ChrGasmanSlot_ListElem) == 0x80);

struct ChrGasmanSlot
{
    AttachSysSlotBaseImpl base;
    uint8_t data_0[6];
    int16_t list_len;
    ChrGasmanSlot_ListElem* list;
};
static_assert(sizeof(ChrGasmanSlot) == 0x30);

struct ChrConditionSfxSeSlot
{
    AttachSysSlotBaseImpl base;
    uint8_t data_0[22];
    int16_t followupbullet_list_len;
    BulletIns_FollowupBullet* followupbullet_list;
    uint64_t data_1;
};
static_assert(sizeof(ChrConditionSfxSeSlot) == 0x48);

struct ChrCamouflageSlot
{
    AttachSysSlotBaseImpl base;

};
//static_assert(sizeof(ChrCamouflageSlot) == 0x40);

struct ChrBurnSlot_ListElem
{
    uint64_t data_0;
    BulletIns_FollowupBullet bullet;
};
static_assert(sizeof(ChrBurnSlot_ListElem) == 0x38);

struct ChrBurnSlot
{
    AttachSysSlotBaseImpl base;
    uint8_t data_0[10];
    int16_t floatlist_len;
    uint32_t data_1;
    float* floatlist;
    int32_t list_len;
    uint32_t data_2;
    ChrBurnSlot_ListElem* list;
    uint8_t data_3[16];
};
static_assert(offsetof(ChrBurnSlot, floatlist_len) == 0x2a);
static_assert(offsetof(ChrBurnSlot, floatlist) == 0x30);
static_assert(offsetof(ChrBurnSlot, list_len) == 0x38);
static_assert(offsetof(ChrBurnSlot, list) == 0x40);
static_assert(sizeof(ChrBurnSlot) == 0x58);

struct ObjRigidOffsetSfxSlot
{
    AttachSysSlotBaseImpl base;
    uint8_t data_0[16+0x30];
    BulletIns_FollowupBullet bullet;
};
static_assert(sizeof(ObjRigidOffsetSfxSlot) == 0x90);

struct ChrFollowSfxSlot
{
    AttachSysSlotBaseImpl base;
    uint64_t data_0;
    BulletIns_FollowupBullet bullet;
    uint8_t data_1[16];
};
static_assert(sizeof(ChrFollowSfxSlot) == 0x68);

struct ChrTravelItemSlot
{
    AttachSysSlotBaseImpl base;
    uint64_t data_0;
};
static_assert(sizeof(ChrTravelItemSlot) == 0x28);

struct ChrFallingControlSlot
{
    AttachSysSlotBaseImpl base;
    uint64_t data_0;
};
static_assert(sizeof(ChrFallingControlSlot) == 0x28);

#endif
