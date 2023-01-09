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
typedef struct FrpgPhysWorld FrpgPhysWorld;

struct DamageEntry
{
    uint64_t data_0;
    FrpgPhysShapePhantomIns* PhysShapePhantomIns;
    FrpgPhysShapePhantomIns* FrpgPhysShapePhantomIns_Sphere;
    FrpgPhysShapePhantomIns* FrpgPhysShapePhantomIns_Capsule;
    hkpSphereShape* hkpSphereShape1;
    hkpCapsuleShape* hkpCapsuleShape1;
    uint64_t data_1;
    uint64_t padding_0;
    uint8_t data_2[216];
    hkpCapsuleShape* hkpCapsuleShape2;
    uint64_t data_3;
    uint64_t padding_1;
    uint8_t data_4[176];
    uint64_t padding_2;
    FrpgPhysWorld* physWorld;
    hkpSimpleShapePhantom* simpleShapePhantom;
    uint64_t padding_3;
    uint64_t data_5;
    hkpSphereShape* hkpSphereShape2;
    uint64_t data_6;
    uint64_t padding_4;
    DamageEntry* next;
    uint64_t data_7;
};

static_assert(offsetof(DamageEntry, data_0) == 0);
static_assert(offsetof(DamageEntry, PhysShapePhantomIns) == 8);
static_assert(offsetof(DamageEntry, FrpgPhysShapePhantomIns_Sphere) == 0x10);
static_assert(offsetof(DamageEntry, FrpgPhysShapePhantomIns_Capsule) == 0x18);
static_assert(offsetof(DamageEntry, hkpSphereShape1) == 0x20);
static_assert(offsetof(DamageEntry, hkpCapsuleShape1) == 0x28);
static_assert(offsetof(DamageEntry, data_1) == 0x30);
static_assert(offsetof(DamageEntry, data_2) == 0x40);
static_assert(offsetof(DamageEntry, hkpCapsuleShape2) == 0x118);
static_assert(offsetof(DamageEntry, data_3) == 0x120);
static_assert(offsetof(DamageEntry, data_4) == 0x130);
static_assert(offsetof(DamageEntry, physWorld) == 0x1e8);
static_assert(offsetof(DamageEntry, simpleShapePhantom) == 0x1f0);
static_assert(offsetof(DamageEntry, data_5) == 0x200);
static_assert(offsetof(DamageEntry, hkpSphereShape2) == 0x208);
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
