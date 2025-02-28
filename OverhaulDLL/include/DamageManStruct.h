#pragma once
#ifndef DAMAGEMAN_STRUCT_H
#define DAMAGEMAN_STRUCT_H

//NOTE: Anything marked "data_x" is a bunch of non-pointer data that can be saved/loaded without really knowing what it is.
// See the ghidra repo for more specific info about what is in that blob, if it's even known

#include <stddef.h>
#include <stdint.h>
#include "PlayerInsStruct.h"

typedef struct DamageMan DamageMan;
typedef struct DamageEntry DamageEntry;
typedef struct FrpgPhysShapePhantomIns FrpgPhysShapePhantomIns;
typedef struct hkpSphereShape hkpSphereShape;
typedef struct hkpCapsuleShape hkpCapsuleShape;
typedef struct DamageEntryField0x118 DamageEntryField0x118;
typedef struct FrpgPhysWorld FrpgPhysWorld;

struct FrpgPhysWorld
{
    uint64_t vtable;
};
//static_assert(sizeof(FrpgPhysWorld) == 0x30);

struct hkpCapsuleShape
{
    uint64_t vtable;
    uint8_t data_0[0x48];
};
static_assert(sizeof(hkpCapsuleShape) == 0x50);

struct hkpSphereShape
{
    uint64_t vtable;
    uint8_t data_0[0x30];
};
static_assert(sizeof(hkpSphereShape) == 0x38);

struct FrpgPhysShapePhantomIns
{
    uint64_t vtable;
    uint64_t data_0;
    uint64_t damageEntry; //this points to an existing DamageEntry in the list, so treat as const
    FrpgPhysWorld* physWorld;
    hkpSimpleShapePhantom* _hkpSimpleShapePhantom;
    void* self;
    uint64_t data_1;
    union
    {
        hkpCapsuleShape* _hkpCapsuleShape;
        hkpSphereShape* _hkpSphereShape;
    };
};
static_assert(offsetof(FrpgPhysShapePhantomIns, damageEntry) == 0x10);
static_assert(offsetof(FrpgPhysShapePhantomIns, physWorld) == 0x18);
static_assert(offsetof(FrpgPhysShapePhantomIns, _hkpSimpleShapePhantom) == 0x20);
static_assert(offsetof(FrpgPhysShapePhantomIns, self) == 0x28);
static_assert(offsetof(FrpgPhysShapePhantomIns, _hkpCapsuleShape) == 0x38);
static_assert(sizeof(FrpgPhysShapePhantomIns) == 0x40);

struct DamageEntryField0x118
{
    uint8_t data_0[0x20];
};
static_assert(sizeof(DamageEntryField0x118) == 0x20);

struct DamageEntry
{
    uint64_t data_0;
    void* PhysShapePhantomIns1; //this always points to 1 of the 2 FrpgPhysShapePhantomIns and can be treated as data
    FrpgPhysShapePhantomIns* FrpgPhysShapePhantomIns_Sphere;
    FrpgPhysShapePhantomIns* FrpgPhysShapePhantomIns_Capsule;
    void* hkpSphereShape1; //these 2 are already handled by the FrpgPhysShapePhantomIns an can be treated as data
    void* hkpCapsuleShape1;
    void* PhysShapePhantomIns1_altPtr_A; //these 2 are just copies of PhysShapePhantomIns1 and can be treated as data
    void* PhysShapePhantomIns1_altPtr_B;
    uint8_t data_2[216];
    DamageEntryField0x118* field0x118;
    uint8_t data_4[184];
    void* DmgHitRecordManImp_field0x10Elem; //ptr to an entry in DmgHitRecordManImp
    uint64_t padding_2;
    FrpgPhysWorld* physWorld; //this is just a const ptr to FrpgHavokManImp->FrpgPhysWorld
    //these point to other entries in the DamageMan list. So treat as offsets, not objs
    DamageEntry* followup_a;
    DamageEntry* followup_b;
    DamageEntry* followup_c;
    void* dbgNode;
    uint8_t data_6[16];
    DamageEntry* next;
    uint64_t data_7;
};

static_assert(offsetof(DamageEntry, data_0) == 0);
static_assert(offsetof(DamageEntry, PhysShapePhantomIns1) == 8);
static_assert(offsetof(DamageEntry, FrpgPhysShapePhantomIns_Sphere) == 0x10);
static_assert(offsetof(DamageEntry, FrpgPhysShapePhantomIns_Capsule) == 0x18);
static_assert(offsetof(DamageEntry, hkpSphereShape1) == 0x20);
static_assert(offsetof(DamageEntry, hkpCapsuleShape1) == 0x28);
static_assert(offsetof(DamageEntry, PhysShapePhantomIns1_altPtr_A) == 0x30);
static_assert(offsetof(DamageEntry, data_2) == 0x40);
static_assert(offsetof(DamageEntry, field0x118) == 0x118);
static_assert(offsetof(DamageEntry, data_4) == 0x120);
static_assert(offsetof(DamageEntry, DmgHitRecordManImp_field0x10Elem) == 0x1d8);
static_assert(offsetof(DamageEntry, physWorld) == 0x1e8);
static_assert(offsetof(DamageEntry, dbgNode) == 0x208);
static_assert(offsetof(DamageEntry, data_6) == 0x210);
static_assert(offsetof(DamageEntry, data_7) == 0x228);
static_assert(sizeof(DamageEntry) == 0x230);

struct DamageMan
{
    DamageEntry* active_damage_entries_list;
    DamageEntry* all_damage_entries_list_cur;
    DamageEntry* all_damage_entries_list_start; //128 elements long
    uint64_t data_0;
    uint8_t padding_0[18];
    uint8_t data_1[6];
};

static_assert(offsetof(DamageMan, active_damage_entries_list) == 0);
static_assert(offsetof(DamageMan, all_damage_entries_list_cur) == 0x8);
static_assert(offsetof(DamageMan, all_damage_entries_list_start) == 0x10);
static_assert(offsetof(DamageMan, data_0) == 0x18);
static_assert(offsetof(DamageMan, data_1) == 0x32);
static_assert(sizeof(DamageMan) == 0x38);

#endif
