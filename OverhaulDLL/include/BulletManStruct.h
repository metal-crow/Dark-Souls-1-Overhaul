#pragma once
#ifndef BULLETMAN_STRUCT_H
#define BULLETMAN_STRUCT_H

//NOTE: Anything marked "data_x" is a bunch of non-pointer data that can be saved/loaded without really knowing what it is.
// See the ghidra repo for more specific info about what is in that blob, if it's even known

#include <stddef.h>
#include <stdint.h>

typedef struct BulletMan BulletMan;
typedef struct BulletIns BulletIns;
typedef struct BulletIns_FollowupBullet BulletIns_FollowupBullet;
typedef struct BulletIns_Field0x90_Field0x1a0 BulletIns_Field0x90_Field0x1a0;
typedef struct BulletTargetingSystemOwner BulletTargetingSystemOwner;
typedef struct TargetingSystemBase TargetingSystemBase;
typedef struct BulletState BulletState;
typedef struct BulletFlyState BulletFlyState;
typedef struct BulletMan_Field0x20 BulletMan_Field0x20;
typedef struct BulletMan_Field0x40 BulletMan_Field0x40;
typedef struct ChrCam ChrCam;

struct FXManager;
struct FXEntry_Substruct;

struct BulletIns_FollowupBullet
{
    uint64_t vtable;
    FXManager* FXManager;
    FXEntry_Substruct* fxentry_a;
    BulletIns_FollowupBullet* next;
    BulletIns_FollowupBullet* prev;
    FXEntry_Substruct* fxentry_b;
};
static_assert(sizeof(BulletIns_FollowupBullet) == 0x30);

struct BulletIns_Field0x90_Field0x1a0
{
    uint8_t data_0[0x160]; //contains entity IDs, attack data, position vectors, etc. All treatable as a data blob for rollback
};
static_assert(sizeof(BulletIns_Field0x90_Field0x1a0) == 0x160);

struct BulletTargetingSystemOwner
{
    uint8_t data_0[0x30]; //contains parent ptr, vtable, entityId, NpcThinkParam info. All raw pointers or data
};
static_assert(sizeof(BulletTargetingSystemOwner) == 0x30);

struct TargetingSystemBase
{
    uint8_t data_0[0x50]; //contains vtable, owner ptr, targeting sub-struct, floats, flags
};
static_assert(sizeof(TargetingSystemBase) == 0x50);

struct BulletState
{
    uint8_t data_0[0x20]; //contains vtable, BulletParamInfo (bullet_id + BulletParam*), frametime, flags
};
static_assert(sizeof(BulletState) == 0x20);

struct BulletFlyState
{
    uint8_t data_0[0x28]; //BulletState base (0x20) + fly-specific fields
};
static_assert(sizeof(BulletFlyState) == 0x28);

struct BulletIns
{
    uint64_t vtable;
    uint8_t data_0[0x50];
    BulletIns_FollowupBullet FollowupBullet;
    uint64_t data_1;
    BulletIns_Field0x90_Field0x1a0 owner;
    uint8_t data_2[24];
    BulletTargetingSystemOwner bulletTargetingSystemOwner;
    TargetingSystemBase targetingSystemBase;
    uint8_t data_3[0x50];
    BulletState bulletState;
    uint64_t data_4;
    BulletFlyState bulletFlyState;
    BulletState bulletExplosionState;
    BulletIns* previous_bullet_in_use;
    uint64_t padding_1;
    uint64_t data_5;
};
static_assert(offsetof(BulletIns, data_0) == 0x8);
static_assert(offsetof(BulletIns, FollowupBullet) == 0x58);
static_assert(offsetof(BulletIns, data_1) == 0x88);
static_assert(offsetof(BulletIns, owner) == 0x90);
static_assert(offsetof(BulletIns, data_2) == 0x1f0);
static_assert(offsetof(BulletIns, bulletTargetingSystemOwner) == 0x208);
static_assert(offsetof(BulletIns, targetingSystemBase) == 0x238);
static_assert(offsetof(BulletIns, data_3) == 0x288);
static_assert(offsetof(BulletIns, bulletState) == 0x2d8);
static_assert(offsetof(BulletIns, data_4) == 0x2f8);
static_assert(offsetof(BulletIns, bulletFlyState) == 0x300);
static_assert(offsetof(BulletIns, bulletExplosionState) == 0x328);
static_assert(offsetof(BulletIns, previous_bullet_in_use) == 0x348);
static_assert(offsetof(BulletIns, data_5) == 0x358);
static_assert(sizeof(BulletIns) == 0x360);

struct BulletMan_Field0x20
{
    uint8_t data_0[0x194];
    uint8_t data_1[4];
    void* BulletParamInfo; //NS_FRPG__BulletParamInfo*, game-allocated. Treat as raw ptr for rollback
    BulletIns_Field0x90_Field0x1a0 field0x1a0; //reuses same struct as BulletIns owner
    uint8_t data_2[8];
    BulletMan_Field0x20* next_in_use; //linked list pointer within field0x20 array
    void* chrCam;
    void* dbgNode;
};
static_assert(offsetof(BulletMan_Field0x20, data_1) == 0x194);
static_assert(offsetof(BulletMan_Field0x20, BulletParamInfo) == 0x198);
static_assert(offsetof(BulletMan_Field0x20, field0x1a0) == 0x1a0);
static_assert(offsetof(BulletMan_Field0x20, data_2) == 0x300);
static_assert(offsetof(BulletMan_Field0x20, next_in_use) == 0x308);
static_assert(offsetof(BulletMan_Field0x20, chrCam) == 0x310);
static_assert(offsetof(BulletMan_Field0x20, dbgNode) == 0x318);
static_assert(sizeof(BulletMan_Field0x20) == 0x320);

struct BulletMan_Field0x40
{
    int32_t bulletOwnerEntityId;
    uint32_t padding_0;
    BulletIns_FollowupBullet arry[16];
    uint64_t data_0;
    uint64_t data_1; //pointer to static data
};
static_assert(offsetof(BulletMan_Field0x40, arry) == 0x8);
static_assert(offsetof(BulletMan_Field0x40, data_0) == 0x308);
static_assert(offsetof(BulletMan_Field0x40, data_1) == 0x310);
static_assert(sizeof(BulletMan_Field0x40) == 0x318);


struct ChrCam
{
    uint8_t data_0[0x60];
    void* ChrExFollowCam; //0x340 bytes, game-allocated sub-camera
    void* ChrAimCam; //0x1d0 bytes, game-allocated sub-camera
    void* FallDieCam; //0xb0 bytes, game-allocated sub-camera
    void* BallistaAimCam; //0xe0 bytes, game-allocated sub-camera
    uint8_t data_1[0x50];
};
static_assert(offsetof(ChrCam, ChrExFollowCam) == 0x60);
static_assert(offsetof(ChrCam, ChrAimCam) == 0x68);
static_assert(offsetof(ChrCam, FallDieCam) == 0x70);
static_assert(offsetof(ChrCam, BallistaAimCam) == 0x78);
static_assert(offsetof(ChrCam, data_1) == 0x80);
static_assert(sizeof(ChrCam) == 0xd0);

struct BulletMan
{
    BulletIns* bulletins_arry; //length == 128
    uint64_t data_0[3]; //includes pointers to offsets in above (end_of_bullets_in_use, start_of_unused_bullets, counts)
    BulletMan_Field0x20* field0x20; //length == 64
    uint64_t data_1[3]; //includes pointers to offsets in above
    BulletMan_Field0x40* field0x40; //length == 4
    uint64_t data_2[3]; //includes pointers to offsets in above
    ChrCam* chrCam;
    uint64_t data_3;
    uint64_t padding_1;
    void** field0x78; //should be fully created and destroyed during a frame, no inter-frame data
    uint64_t field0x78_next;
    uint64_t field0x78_end;
    uint64_t padding_2[3];
    uint64_t data_5[2];
};
static_assert(offsetof(BulletMan, bulletins_arry) == 0);
static_assert(offsetof(BulletMan, data_0) == 8);
static_assert(offsetof(BulletMan, field0x20) == 0x20);
static_assert(offsetof(BulletMan, data_1) == 0x28);
static_assert(offsetof(BulletMan, field0x40) == 0x40);
static_assert(offsetof(BulletMan, data_2) == 0x48);
static_assert(offsetof(BulletMan, chrCam) == 0x60);
static_assert(offsetof(BulletMan, data_3) == 0x68);
static_assert(offsetof(BulletMan, padding_1) == 0x70);
static_assert(offsetof(BulletMan, field0x78) == 0x78);
static_assert(offsetof(BulletMan, field0x78_next) == 0x80);
static_assert(offsetof(BulletMan, field0x78_end) == 0x88);
static_assert(offsetof(BulletMan, padding_2) == 0x90);
static_assert(offsetof(BulletMan, data_5) == 0xa8);
static_assert(sizeof(BulletMan) == 0xb8);

#endif
