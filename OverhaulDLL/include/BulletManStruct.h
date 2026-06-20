#pragma once
#ifndef BULLETMAN_STRUCT_H
#define BULLETMAN_STRUCT_H

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
struct FxBehaviorNode;

struct BulletIns_FollowupBullet
{
    uint64_t vtable;
    FXManager* FXManager;
    FxBehaviorNode* fxentry_a;
    BulletIns_FollowupBullet* next;
    BulletIns_FollowupBullet* prev;
    FxBehaviorNode* fxentry_b;
};
static_assert(sizeof(BulletIns_FollowupBullet) == 0x30);

struct BulletIns_Field0x90_Field0x1a0
{
    uint32_t unk_0;            // 0x0
    uint32_t bullet_id;        // 0x4
    uint32_t unk_8;            // 0x8
    uint32_t owner_entityId;   // 0xc
    uint32_t unk_10;           // 0x10
    uint32_t unk_14;           // 0x14
    uint32_t goodsId;          // 0x18
    uint32_t unk_1c;           // 0x1c (alignment)
    uint8_t AttackData[104];   // 0x20 (ghidra: AttackData)
    uint8_t unk_88[8];         // 0x88 (gap)
    uint8_t target0x90[49];    // 0x90 (ghidra: NS_FRPG_BulletIns_Field0x90_Field0x90)
    uint8_t unk_c1[7];         // 0xc1 (gap)
    uint64_t unk_c8;           // 0xc8
    uint32_t unk_d0;           // 0xd0
    float bowDist;             // 0xd4
    uint8_t unk_d8[8];         // 0xd8 (gap)
    uint8_t fieldE0_0[48];     // 0xe0 (ghidra: NS_FRPG_BulletIns_Field0x90_Field0xE0)
    uint8_t fieldE0_1[48];     // 0x110 (ghidra: NS_FRPG_BulletIns_Field0x90_Field0xE0)
    uint32_t unk_140;          // 0x140
    uint8_t unk_144[4];        // 0x144 (gap)
    void* unk_148;             // 0x148 (ghidra: pointer)
    float unk_150;             // 0x150
    float unk_154;             // 0x154
    float unk_158;             // 0x158
    float unk_15c;             // 0x15c
};
static_assert(offsetof(BulletIns_Field0x90_Field0x1a0, AttackData) == 0x20);
static_assert(offsetof(BulletIns_Field0x90_Field0x1a0, target0x90) == 0x90);
static_assert(offsetof(BulletIns_Field0x90_Field0x1a0, fieldE0_0) == 0xe0);
static_assert(offsetof(BulletIns_Field0x90_Field0x1a0, unk_148) == 0x148);
static_assert(sizeof(BulletIns_Field0x90_Field0x1a0) == 0x160);

struct BulletTargetingSystemOwner
{
    uint64_t vtable;                       // 0x0
    BulletIns* parent;                     // 0x8
    uint32_t ownerEntityId;                // 0x10
    uint32_t unk_14;                       // 0x14 (alignment)
    uint8_t autoSearchNPCThinkParam[24];   // 0x18 (ghidra: NS_FRPG::NpcThinkParamInfo)
};
static_assert(offsetof(BulletTargetingSystemOwner, parent) == 0x8);
static_assert(offsetof(BulletTargetingSystemOwner, autoSearchNPCThinkParam) == 0x18);
static_assert(sizeof(BulletTargetingSystemOwner) == 0x30);

struct TargetingSystemBase
{
    uint64_t vtable;                       // 0x0
    BulletTargetingSystemOwner* owner_1;   // 0x8 (nulled out by copy_BulletIns; see note there)
    uint8_t sub0x10[32];                   // 0x10 (ghidra: TargetingSystemBase_sub0x10)
    float unk_30;                          // 0x30
    uint8_t unk_34;                        // 0x34
    uint8_t unk_35;                        // 0x35
    uint8_t unk_36[2];                     // 0x36 (alignment)
    uint32_t unk_38;                       // 0x38
    uint32_t unk_3c;                       // 0x3c (alignment)
    uint64_t unk_40;                       // 0x40
    uint16_t unk_48;                       // 0x48
    uint8_t unk_4a[6];                     // 0x4a (alignment)
};
static_assert(offsetof(TargetingSystemBase, owner_1) == 0x8);
static_assert(offsetof(TargetingSystemBase, sub0x10) == 0x10);
static_assert(offsetof(TargetingSystemBase, unk_30) == 0x30);
static_assert(sizeof(TargetingSystemBase) == 0x50);

struct BulletState
{
    uint64_t vtable;            // 0x0
    uint8_t param_info[16];     // 0x8 (ghidra: NS_FRPG::BulletParamInfo - bullet_id + BulletParam*)
    float frametime;            // 0x18
    uint32_t unk_1c;            // 0x1c
};
static_assert(offsetof(BulletState, param_info) == 0x8);
static_assert(offsetof(BulletState, frametime) == 0x18);
static_assert(sizeof(BulletState) == 0x20);

struct BulletFlyState
{
    BulletState base;           // 0x0 (ghidra: BulletFlyState inherits BulletState)
    uint16_t unk_20;            // 0x20
    uint8_t unk_22[6];          // 0x22 (alignment)
};
static_assert(offsetof(BulletFlyState, unk_20) == 0x20);
static_assert(sizeof(BulletFlyState) == 0x28);

struct BulletIns
{
    uint64_t vtable;            // 0x0
    uint8_t bullet_number;      // 0x8
    uint8_t unk_9;              // 0x9 (padding in ghidra)
    uint16_t unk_a;             // 0xa
    uint8_t unk_c[4];           // 0xc (gap)
    float player_position[4];   // 0x10 (ghidra: Vector4)
    float orbit_radius_X;       // 0x20
    float orbit_radius_Y;       // 0x24
    float orbit_radius_Z;       // 0x28
    float orbit_radius_W;       // 0x2c
    float unk_30;               // 0x30
    float unk_34;               // 0x34
    float unk_38;               // 0x38
    float unk_3c;               // 0x3c
    float unk_40;               // 0x40
    float unk_44;               // 0x44
    float unk_48;               // 0x48
    float unk_4c;               // 0x4c
    uint32_t unk_50;            // 0x50
    uint32_t unk_54;            // 0x54
    BulletIns_FollowupBullet FollowupBullet;   // 0x58
    int32_t bulletClassType;    // 0x88
    int32_t unk_8c;             // 0x8c
    BulletIns_Field0x90_Field0x1a0 owner;      // 0x90
    float bullet_distance;      // 0x1f0
    float bullet_life;          // 0x1f4
    float unk_1f8;              // 0x1f8
    float unk_1fc;              // 0x1fc
    float unk_200;              // 0x200
    float unk_204;              // 0x204
    BulletTargetingSystemOwner bulletTargetingSystemOwner;   // 0x208
    TargetingSystemBase targetingSystemBase;                 // 0x238
    uint32_t unk_288;           // 0x288
    uint32_t unk_28c;           // 0x28c
    uint8_t target[49];         // 0x290 (ghidra: NS_FRPG_BulletIns_Field0x90_Field0x90)
    uint8_t unk_2c1[3];         // 0x2c1 (gap)
    uint32_t unk_2c4;           // 0x2c4
    uint32_t unk_2c8;           // 0x2c8
    uint32_t unk_2cc;           // 0x2cc
    uint8_t unk_2d0;            // 0x2d0
    uint8_t unk_2d1[7];         // 0x2d1 (gap)
    BulletState bulletState;    // 0x2d8
    uint32_t unk_2f8;           // 0x2f8 (gap)
    float unk_2fc;              // 0x2fc
    BulletFlyState bulletFlyState;          // 0x300
    BulletState bulletExplosionState;       // 0x328 (ghidra type: BulletExplosionState, structurally a BulletState)
    BulletIns* previous_bullet_in_use;      // 0x348
    uint64_t padding_1;         // 0x350 (dbgMenuItem ptr; not copied)
    uint32_t unk_358;           // 0x358
    uint32_t unk_35c;           // 0x35c
};
static_assert(offsetof(BulletIns, bullet_number) == 0x8);
static_assert(offsetof(BulletIns, player_position) == 0x10);
static_assert(offsetof(BulletIns, FollowupBullet) == 0x58);
static_assert(offsetof(BulletIns, bulletClassType) == 0x88);
static_assert(offsetof(BulletIns, owner) == 0x90);
static_assert(offsetof(BulletIns, bullet_distance) == 0x1f0);
static_assert(offsetof(BulletIns, bulletTargetingSystemOwner) == 0x208);
static_assert(offsetof(BulletIns, targetingSystemBase) == 0x238);
static_assert(offsetof(BulletIns, unk_288) == 0x288);
static_assert(offsetof(BulletIns, bulletState) == 0x2d8);
static_assert(offsetof(BulletIns, unk_2f8) == 0x2f8);
static_assert(offsetof(BulletIns, bulletFlyState) == 0x300);
static_assert(offsetof(BulletIns, bulletExplosionState) == 0x328);
static_assert(offsetof(BulletIns, previous_bullet_in_use) == 0x348);
static_assert(offsetof(BulletIns, unk_358) == 0x358);
static_assert(sizeof(BulletIns) == 0x360);

struct BulletMan_Field0x20
{
    uint32_t unk_0;             // 0x0
    uint8_t field0x4Elem[6*64]; // 0x4 (ghidra: BulletMan_Field0x4Elem[6], each 8x uint64_t)
    uint8_t unk_184[8];         // 0x184 (ghidra: uint64_t; byte array - lands on a non-8-aligned offset)
    uint8_t unk_18c[8];         // 0x18c (ghidra: uint64_t; byte array - lands on a non-8-aligned offset)
    //former data_1 (0x194)
    uint8_t unk_194;            // 0x194 (ghidra: bool)
    uint8_t unk_195;            // 0x195
    uint8_t unk_196[2];         // 0x196 (alignment)
    void* BulletParamInfo;      // 0x198 - NS_FRPG__BulletParamInfo*, game-allocated. Treat as raw ptr for rollback
    BulletIns_Field0x90_Field0x1a0 field0x1a0; // 0x1a0 - reuses same struct as BulletIns owner
    float unk_300;              // 0x300
    uint32_t unk_304;           // 0x304
    BulletMan_Field0x20* next_in_use; // 0x308 - linked list pointer within field0x20 array
    void* chrCam;               // 0x310
    void* dbgNode;              // 0x318
};
static_assert(offsetof(BulletMan_Field0x20, field0x4Elem) == 0x4);
static_assert(offsetof(BulletMan_Field0x20, unk_194) == 0x194);
static_assert(offsetof(BulletMan_Field0x20, BulletParamInfo) == 0x198);
static_assert(offsetof(BulletMan_Field0x20, field0x1a0) == 0x1a0);
static_assert(offsetof(BulletMan_Field0x20, unk_300) == 0x300);
static_assert(offsetof(BulletMan_Field0x20, next_in_use) == 0x308);
static_assert(offsetof(BulletMan_Field0x20, chrCam) == 0x310);
static_assert(offsetof(BulletMan_Field0x20, dbgNode) == 0x318);
static_assert(sizeof(BulletMan_Field0x20) == 0x320);

struct BulletMan_Field0x40
{
    int32_t bulletOwnerEntityId;            // 0x0
    uint32_t padding_0;                     // 0x4
    BulletIns_FollowupBullet arry[16];      // 0x8
    //former data_0 (0x308)
    uint32_t unk_308;                       // 0x308
    uint32_t unk_30c;                       // 0x30c (gap)
    uint64_t unk_310;                       // 0x310 (pointer to static data; raw)
};
static_assert(offsetof(BulletMan_Field0x40, arry) == 0x8);
static_assert(offsetof(BulletMan_Field0x40, unk_308) == 0x308);
static_assert(offsetof(BulletMan_Field0x40, unk_310) == 0x310);
static_assert(sizeof(BulletMan_Field0x40) == 0x318);


struct ChrCam
{
    uint8_t FrpgPersCam_base[0x60]; // 0x0 (ghidra: FrpgPersCam base)
    void* ChrExFollowCam; //0x60 bytes 0x340, game-allocated sub-camera
    void* ChrAimCam; //0x68 bytes 0x1d0, game-allocated sub-camera
    void* FallDieCam; //0x70 bytes 0xb0, game-allocated sub-camera
    void* BallistaAimCam; //0x78 bytes 0xe0, game-allocated sub-camera
    uint8_t unk_80;             // 0x80 (ghidra: byte)
    uint8_t unk_81[3];          // 0x81 (gap)
    uint32_t unk_84;            // 0x84 (ghidra: uint)
    float unk_88;               // 0x88
    float unk_8c;               // 0x8c
    uint8_t unk_90[12];         // 0x90 (gap)
    float unk_9c;               // 0x9c
    uint8_t unk_a0;             // 0xa0 (ghidra: byte)
    uint8_t unk_a1;             // 0xa1 (gap)
    uint8_t unk_a2;             // 0xa2 (ghidra: byte)
    uint8_t UseBallistaAimCam;  // 0xa3
    uint8_t unk_a4[28];         // 0xa4 (gap)
    uint8_t unk_c0;             // 0xc0 (ghidra: byte)
    uint8_t unk_c1[7];          // 0xc1 (gap)
    uint8_t DbgNode;            // 0xc8 (ghidra: undefined1, 1 byte - not a pointer)
    uint8_t unk_c9[7];          // 0xc9 (gap)
};
static_assert(offsetof(ChrCam, ChrExFollowCam) == 0x60);
static_assert(offsetof(ChrCam, ChrAimCam) == 0x68);
static_assert(offsetof(ChrCam, FallDieCam) == 0x70);
static_assert(offsetof(ChrCam, BallistaAimCam) == 0x78);
static_assert(offsetof(ChrCam, unk_80) == 0x80);
static_assert(offsetof(ChrCam, UseBallistaAimCam) == 0xa3);
static_assert(offsetof(ChrCam, DbgNode) == 0xc8);
static_assert(sizeof(ChrCam) == 0xd0);

struct BulletMan
{
    BulletIns* bulletins_arry; //length == 128
    //cursor pointers into bulletins_arry + counts
    BulletIns* end_of_bullets_in_use;       // 0x8
    BulletIns* start_of_unused_bullets;     // 0x10
    uint32_t bullet_count_1;                // 0x18
    uint32_t bullet_count_2;                // 0x1c
    BulletMan_Field0x20* field0x20; //length == 64
    //cursor pointers into field0x20 + count
    BulletMan_Field0x20* end_of_bullets_in_use_field0x20;     // 0x28
    BulletMan_Field0x20* start_of_unused_bullets_field0x20;   // 0x30
    uint32_t unk_38;                        // 0x38
    uint32_t unk_3c;                        // 0x3c (gap)
    BulletMan_Field0x40* field0x40; //length == 4
    //cursor pointers into field0x40 + count
    BulletMan_Field0x40* end_of_bullets_in_use_field0x40;     // 0x48
    BulletMan_Field0x40* start_of_unused_bullets_field0x40;   // 0x50
    uint32_t unk_58;                        // 0x58
    uint32_t unk_5c;                        // 0x5c (gap)
    ChrCam* chrCam;                         // 0x60
    uint8_t attach_system_disabled;         // 0x68
    uint8_t unk_69[7];                      // 0x69 (gap)
    uint64_t padding_1;                     // 0x70 (heap ptr; not copied)
    void** field0x78; //should be fully created and destroyed during a frame, no inter-frame data
    uint64_t field0x78_next;                // 0x80
    uint64_t field0x78_end;                 // 0x88
    uint64_t padding_2[3];                  // 0x90 (dbgMenu ptrs; not copied)
    uint8_t bullet_direction_drawing;       // 0xa8 (ghidra: bool)
    uint8_t unk_a9[3];                      // 0xa9 (gap)
    float speed_attenuation_rate;           // 0xac
    uint32_t last_addition_rate;            // 0xb0
    uint32_t external_force;                // 0xb4
};
static_assert(offsetof(BulletMan, bulletins_arry) == 0);
static_assert(offsetof(BulletMan, end_of_bullets_in_use) == 8);
static_assert(offsetof(BulletMan, field0x20) == 0x20);
static_assert(offsetof(BulletMan, end_of_bullets_in_use_field0x20) == 0x28);
static_assert(offsetof(BulletMan, field0x40) == 0x40);
static_assert(offsetof(BulletMan, end_of_bullets_in_use_field0x40) == 0x48);
static_assert(offsetof(BulletMan, chrCam) == 0x60);
static_assert(offsetof(BulletMan, attach_system_disabled) == 0x68);
static_assert(offsetof(BulletMan, padding_1) == 0x70);
static_assert(offsetof(BulletMan, field0x78) == 0x78);
static_assert(offsetof(BulletMan, field0x78_next) == 0x80);
static_assert(offsetof(BulletMan, field0x78_end) == 0x88);
static_assert(offsetof(BulletMan, padding_2) == 0x90);
static_assert(offsetof(BulletMan, bullet_direction_drawing) == 0xa8);
static_assert(sizeof(BulletMan) == 0xb8);

#endif
