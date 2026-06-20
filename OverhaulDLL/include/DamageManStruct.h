#pragma once
#ifndef DAMAGEMAN_STRUCT_H
#define DAMAGEMAN_STRUCT_H

//NOTE: Anything marked "data_x" is a bunch of non-pointer data that can be saved/loaded without really knowing what it is.
// See the ghidra repo for more specific info about what is in that blob, if it's even known

#include <stddef.h>
#include <stdint.h>
#include "FrpgHavokManImpStruct.h"

typedef struct DamageMan DamageMan;
typedef struct SavedDamageEntry SavedDamageEntry;
typedef struct DamageEntry DamageEntry;
typedef struct FrpgPhysIns FrpgPhysIns;
typedef struct FrpgPhysPhantomIns FrpgPhysPhantomIns;
typedef struct FrpgPhysShapePhantomIns FrpgPhysShapePhantomIns;
typedef struct DamageEntryField0x118 DamageEntryField0x118;

struct FrpgPhysIns
{
    uint64_t vtable;          // 0x0
    uint64_t data_0;          // 0x8 (ghidra: undefined2 + ushort + alignment)
    uint64_t damageEntry;     // 0x10 (ghidra: hitEntry, HitIns*) - points to an existing DamageEntry, treat as const
    FrpgPhysWorld* physWorld; // 0x18 - const ptr to FrpgHavokManImp->FrpgPhysWorld
};
static_assert(offsetof(FrpgPhysIns, damageEntry) == 0x10);
static_assert(offsetof(FrpgPhysIns, physWorld) == 0x18);
static_assert(sizeof(FrpgPhysIns) == 0x20);

struct FrpgPhysPhantomIns
{
    FrpgPhysIns base;         // 0x0
    hkpSimpleShapePhantom* _hkpSimpleShapePhantom;  // 0x20
    void* self;               // 0x28 (ghidra: this)
    uint64_t data_1;          // 0x30 (ghidra: undefined2 + ushort + alignment)
};
static_assert(offsetof(FrpgPhysPhantomIns, _hkpSimpleShapePhantom) == 0x20);
static_assert(offsetof(FrpgPhysPhantomIns, self) == 0x28);
static_assert(sizeof(FrpgPhysPhantomIns) == 0x38);

struct FrpgPhysShapePhantomIns
{
    FrpgPhysPhantomIns base;  // 0x0
    union
    {
        hkpCapsuleShape* _hkpCapsuleShape;
        hkpSphereShape* _hkpSphereShape;
        void* _shape;
    };                        // 0x38 (ghidra: hkpCapsuleOrSphereShape)
};
static_assert(offsetof(FrpgPhysShapePhantomIns, _hkpCapsuleShape) == 0x38);
static_assert(sizeof(FrpgPhysShapePhantomIns) == 0x40);

struct DamageEntryField0x118
{
    float unk_0[4];        // 0x0 (ghidra: float[4])
    int32_t PlayerHandle;  // 0x10
    uint8_t unk_14[12];    // 0x14 (undefined in ghidra; trailing)
};
static_assert(offsetof(DamageEntryField0x118, PlayerHandle) == 0x10);
static_assert(sizeof(DamageEntryField0x118) == 0x20);

struct DamageEntry
{
    uint32_t id;            // 0x0
    uint32_t unk_4;         // 0x4
    void* PhysShapePhantomIns1; //this always points to 1 of the 2 FrpgPhysShapePhantomIns and can be treated as data
    FrpgPhysShapePhantomIns* FrpgPhysShapePhantomIns_Sphere;
    FrpgPhysShapePhantomIns* FrpgPhysShapePhantomIns_Capsule;
    void* hkpSphereShape1; //these 2 are already handled by the FrpgPhysShapePhantomIns an can be treated as data
    void* hkpCapsuleShape1;
    void* PhysShapePhantomIns1_altPtr_A; //these 2 are just copies of PhysShapePhantomIns1 and can be treated as data
    void* PhysShapePhantomIns1_altPtr_B;
    //former data_2 (0x40)
    uint32_t attackerHandle;       // 0x40
    uint8_t attackInfo[208];       // 0x44 (ghidra: AttackInfoBase)
    uint32_t unk_114;              // 0x114
    DamageEntryField0x118* field0x118;  // 0x118
    //former data_4 (0x120)
    float life;                    // 0x120
    int32_t unk_124;               // 0x124
    uint8_t unk_128[8];            // 0x128 (gap)
    uint8_t field0x130[64];        // 0x130 (ghidra: DamageEntry_field0x130)
    float unk_170;                 // 0x170
    float unk_174;                 // 0x174
    float unk_178;                 // 0x178
    float unk_17c;                 // 0x17c
    float unk_180;                 // 0x180
    float unk_184;                 // 0x184
    float unk_188;                 // 0x188
    float unk_18c;                 // 0x18c
    uint32_t unk_190;              // 0x190
    uint16_t unk_194;              // 0x194
    uint16_t unk_196;              // 0x196
    uint32_t unk_198;              // 0x198
    uint32_t unk_19c;              // 0x19c
    float unk_1a0;                 // 0x1a0
    float unk_1a4;                 // 0x1a4
    float unk_1a8;                 // 0x1a8
    float unk_1ac;                 // 0x1ac
    float unk_1b0;                 // 0x1b0
    uint8_t isSweetSpot;           // 0x1b4 (ghidra: bool)
    uint8_t isWeakSpot;            // 0x1b5 (ghidra: bool)
    uint16_t unk_1b6;              // 0x1b6
    float unk_1b8;                 // 0x1b8
    float physical_magnification;  // 0x1bc
    float magic_magnification;     // 0x1c0
    float fire_magnification;      // 0x1c4
    float lightning_magnification; // 0x1c8
    float stamina_dmg_magnification; // 0x1cc
    float knockback_percent;       // 0x1d0
    uint32_t unk_1d4;              // 0x1d4
    void* DmgHitRecordManImp_field0x10Elem; //ptr to an entry in DmgHitRecordManImp (0x1d8)
    uint64_t padding_2;            // 0x1e0 (ghidra: pointer; not copied)
    void* physWorld; //this is just a const ptr to FrpgHavokManImp->FrpgPhysWorld (0x1e8)
    //able to treat these as static, since we're preseving the address of even dynamic DamageEntrys
    DamageEntry* followup_a;        // 0x1f0
    DamageEntry* followup_b;        // 0x1f8
    DamageEntry* followup_c;        // 0x200
    void* dbgNode;                  // 0x208
    uint32_t num_hits;              // 0x210
    uint32_t unk_214;               // 0x214
    uint8_t unk_218[8];             // 0x218 (gap)
    //able to treat this as static, since we're preseving the address of even dynamic DamageEntrys
    DamageEntry* next;              // 0x220
    uint32_t unk_228;               // 0x228
    uint32_t unk_22c;               // 0x22c
};

static_assert(offsetof(DamageEntry, id) == 0);
static_assert(offsetof(DamageEntry, PhysShapePhantomIns1) == 8);
static_assert(offsetof(DamageEntry, FrpgPhysShapePhantomIns_Sphere) == 0x10);
static_assert(offsetof(DamageEntry, FrpgPhysShapePhantomIns_Capsule) == 0x18);
static_assert(offsetof(DamageEntry, hkpSphereShape1) == 0x20);
static_assert(offsetof(DamageEntry, hkpCapsuleShape1) == 0x28);
static_assert(offsetof(DamageEntry, PhysShapePhantomIns1_altPtr_A) == 0x30);
static_assert(offsetof(DamageEntry, attackerHandle) == 0x40);
static_assert(offsetof(DamageEntry, attackInfo) == 0x44);
static_assert(offsetof(DamageEntry, field0x118) == 0x118);
static_assert(offsetof(DamageEntry, life) == 0x120);
static_assert(offsetof(DamageEntry, field0x130) == 0x130);
static_assert(offsetof(DamageEntry, isSweetSpot) == 0x1b4);
static_assert(offsetof(DamageEntry, knockback_percent) == 0x1d0);
static_assert(offsetof(DamageEntry, DmgHitRecordManImp_field0x10Elem) == 0x1d8);
static_assert(offsetof(DamageEntry, physWorld) == 0x1e8);
static_assert(offsetof(DamageEntry, dbgNode) == 0x208);
static_assert(offsetof(DamageEntry, num_hits) == 0x210);
static_assert(offsetof(DamageEntry, next) == 0x220);
static_assert(offsetof(DamageEntry, unk_228) == 0x228);
static_assert(sizeof(DamageEntry) == 0x230);

struct SavedDamageEntry
{
    DamageEntry* game_addr;
    DamageEntry* data;
    bool is_dynamic; //if this is false we can just use the game_addr raw, since it points to an entry in all_damage_entries_list_start we already handle
};

struct DamageMan
{
    DamageEntry* active_damage_entries_list;
    DamageEntry* all_damage_entries_list_cur;
    DamageEntry* all_damage_entries_list_start; //128 elements long
    uint32_t unk_18;            // 0x18 (undefined in ghidra)
    float put_out_sparks;       // 0x1c
    uint8_t padding_0[18];      // 0x20: dbg_node + dbg_node_damageList ptrs + DebugBallDisplay/DebugInformationDisplay bools (not copied)
    uint8_t damage_from_weapon; // 0x32
    uint8_t damage_to_occur;    // 0x33
    uint8_t unk_34[4];          // 0x34 (gap)
    // Local-only fields (not part of the game struct, only used in our local copies)
    std::vector<SavedDamageEntry> saved_active_damage_entries;
};

static_assert(offsetof(DamageMan, active_damage_entries_list) == 0);
static_assert(offsetof(DamageMan, all_damage_entries_list_cur) == 0x8);
static_assert(offsetof(DamageMan, all_damage_entries_list_start) == 0x10);
static_assert(offsetof(DamageMan, unk_18) == 0x18);
static_assert(offsetof(DamageMan, padding_0) == 0x20);
static_assert(offsetof(DamageMan, damage_from_weapon) == 0x32);
//static_assert(sizeof(DamageMan) == 0x38);

#endif
