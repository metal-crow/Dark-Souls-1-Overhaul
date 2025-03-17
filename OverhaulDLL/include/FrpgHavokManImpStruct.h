#pragma once
#ifndef FRPGHAVOKMAN_STRUCT_H
#define FRPGHAVOKMAN_STRUCT_H

//NOTE: Anything marked "data_x" is a bunch of non-pointer data that can be saved/loaded without really knowing what it is.
// See the ghidra repo for more specific info about what is in that blob, if it's even known

#include <stddef.h>
#include <stdint.h>

typedef struct FrpgHavokManImp FrpgHavokManImp;
typedef struct FrpgPhysWorld FrpgPhysWorld;
typedef struct hkpWorld hkpWorld;
typedef struct hkpSimulationIsland hkpSimulationIsland;
typedef struct hkpEntity hkpEntity;
typedef struct hkpSphereShape hkpSphereShape;
typedef struct hkpCapsuleShape hkpCapsuleShape;
typedef struct hkpSimpleShapePhantom_field0x30 hkpSimpleShapePhantom_field0x30;
typedef struct hkpSimpleShapePhantom hkpSimpleShapePhantom;
typedef struct hkpCharacterProxy_field0x20elem hkpCharacterProxy_field0x20elem;
typedef struct hkpRigidBody hkpRigidBody;
typedef struct hkpCharacterProxy hkpCharacterProxy;
typedef struct ChrInsProxyListener ChrInsProxyListener;

struct FrpgHavokManImp
{
    void* vtable;
    uint8_t padding_0[0x20];
    FrpgPhysWorld* physWorld;
};
static_assert(offsetof(FrpgHavokManImp, physWorld) == 0x28);
static_assert(sizeof(FrpgHavokManImp) == 0x30);

struct FrpgPhysWorld
{
    void* vtable;
    hkpWorld* _hkpWorld;
    uint64_t heap;
};
static_assert(offsetof(FrpgPhysWorld, _hkpWorld) == 0x8);
//static_assert(sizeof(FrpgPhysWorld) == 0x30);

struct hkpWorld
{
    void* vtable;
    uint8_t _[0x38];
    hkpSimulationIsland**   m_activeSimulationIslands;
    uint32_t m_activeSimulationIslands_size;
    uint32_t m_activeSimulationIslands_cap;
    hkpSimulationIsland**   m_inactiveSimulationIslands;
    uint32_t m_inactiveSimulationIslands_size;
    uint32_t m_inactiveSimulationIslands_cap;
};
static_assert(offsetof(hkpWorld, m_activeSimulationIslands) == 0x40);
static_assert(offsetof(hkpWorld, m_inactiveSimulationIslands) == 0x50);
//static_assert(sizeof(hkpWorld) == 0x30);

struct hkpSimulationIsland
{
    void* vtable;
    uint8_t _[0x40];
    void** m_actions;
    uint32_t m_actions_size;
    uint32_t m_actions_cap;
    uint8_t _2[8];
    hkpEntity** m_entities;
    uint32_t m_entities_size;
    uint32_t m_entities_cap;
};
static_assert(offsetof(hkpSimulationIsland, m_actions) == 0x48);
static_assert(offsetof(hkpSimulationIsland, m_entities) == 0x60);
//static_assert(sizeof(hkpSimulationIsland) == 0x30);

struct hkpEntity
{
    void* vtable;
    
};

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

struct hkpSimpleShapePhantom_field0x30
{
    uint8_t padding_0[0x30];
    float position[4];
};

struct hkpSimpleShapePhantom
{
    uint8_t padding_0[0x30];
    hkpSimpleShapePhantom_field0x30* field0x30;
    uint8_t padding_1[0x188];
};
static_assert(sizeof(hkpSimpleShapePhantom) == 0x1c0);

struct hkpCharacterProxy
{
    uint64_t padding_0;
    uint64_t data_0;
    uint64_t padding_1[2];
    hkpCharacterProxy_field0x20elem* field0x20; //Ignore this, seems to not be important.
    uint32_t field0x20_len;
    uint32_t field0x20_cap;
    hkpRigidBody** field0x30; //Ignore this, seems to not be important.
    uint32_t field0x30_len;
    uint32_t field0x30_cap;
    void* field0x40; //each elem is ?? size. This appears to be unused
    uint32_t field0x40_len;
    uint32_t field0x40_cap;
    void* field0x50; //each elem is ?? size. This appears to be unused
    uint32_t field0x50_len;
    uint32_t field0x50_cap;
    uint8_t data_1[0x20];
    hkpSimpleShapePhantom* HkpSimpleShapePhantom;
    uint8_t data_2[0x40];
    ChrInsProxyListener** field0xc8; //Ignore this, seems to not be important.
    uint32_t field0xc8_len;
    uint32_t field0xc8_cap;
    uint8_t data_3[24];
};

static_assert(offsetof(hkpCharacterProxy, data_0) == 0x8);
static_assert(offsetof(hkpCharacterProxy, field0x20) == 0x20);
static_assert(offsetof(hkpCharacterProxy, field0x30) == 0x30);
static_assert(offsetof(hkpCharacterProxy, field0x40) == 0x40);
static_assert(offsetof(hkpCharacterProxy, field0x50) == 0x50);
static_assert(offsetof(hkpCharacterProxy, data_1) == 0x60);
static_assert(offsetof(hkpCharacterProxy, HkpSimpleShapePhantom) == 0x80);
static_assert(offsetof(hkpCharacterProxy, data_2) == 0x88);
static_assert(offsetof(hkpCharacterProxy, field0xc8) == 0xc8);
static_assert(offsetof(hkpCharacterProxy, data_3) == 0xd8);
static_assert(sizeof(hkpCharacterProxy) == 0xf0);

struct ChrInsProxyListener
{
    uint8_t padding_0[0x30];
};

static_assert(sizeof(ChrInsProxyListener) == 0x30);

struct hkpCharacterProxy_field0x20elem
{
    uint8_t data_0[0x20];
    void* pointer_0; //this seems to be a static pointer
    uint64_t data_1;
    void* pointer_1; //this seems to be a static pointer
    uint64_t data_2;
};

static_assert(sizeof(hkpCharacterProxy_field0x20elem) == 0x40);

struct hkpRigidBody
{
    uint8_t padding_0[0x2c0];
};

static_assert(sizeof(hkpRigidBody) == 0x2c0);

#endif
