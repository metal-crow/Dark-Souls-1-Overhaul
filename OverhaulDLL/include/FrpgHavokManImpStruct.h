#pragma once
#ifndef FRPGHAVOKMAN_STRUCT_H
#define FRPGHAVOKMAN_STRUCT_H

//NOTE: Anything marked "data_x" is a bunch of non-pointer data that can be saved/loaded without really knowing what it is.
// See the ghidra repo for more specific info about what is in that blob, if it's even known

#include <stddef.h>
#include <stdint.h>
#include <vector>

typedef struct FrpgHavokManImp FrpgHavokManImp;
typedef struct FrpgPhysWorld FrpgPhysWorld;
typedef struct hkpWorld hkpWorld;
typedef struct hkpSimulationIsland hkpSimulationIsland;
typedef struct hkpAgentNnSector hkpAgentNnSector;
typedef struct hkpEntity hkpEntity;
typedef struct hkpMotion hkpMotion;
typedef struct hkpSphereShape hkpSphereShape;
typedef struct hkpCapsuleShape hkpCapsuleShape;
typedef struct hkpPhantom hkpPhantom;
typedef struct hkpSimpleShapePhantom hkpSimpleShapePhantom;
typedef struct hkpAabbPhantom hkpAabbPhantom;
typedef struct hkpCollidable hkpCollidable;
typedef struct hkMotionState hkMotionState;
typedef struct hkpProperty hkpProperty;
typedef struct hkpLinkedCollidable hkpLinkedCollidable;
typedef struct BoundingVolumeData BoundingVolumeData;
typedef struct hkAabbUint32 hkAabbUint32;
typedef struct CollisionEntry CollisionEntry;
typedef struct hkConstraintInternal hkConstraintInternal;
typedef struct hkpConstraintInstance hkpConstraintInstance;
typedef struct hkpBroadPhaseBorder hkpBroadPhaseBorder;
typedef struct hkpTypedBroadPhaseHandle hkpTypedBroadPhaseHandle;
typedef struct SavedEntityState SavedEntityState;
typedef struct SavedPhantomState SavedPhantomState;
typedef struct HkpWorldSnapshot HkpWorldSnapshot;


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
    // Local-only fields (not part of the game struct, only used in our snapshot copies)
    HkpWorldSnapshot* snapshot;
};
static_assert(offsetof(FrpgPhysWorld, _hkpWorld) == 0x8);

struct hkpWorld
{
    void* vtable;
    uint8_t _0[0x28];
    hkpSimulationIsland* m_fixedIsland;
    void* m_fixedRigidBody;
    hkpSimulationIsland** m_activeSimulationIslands;
    uint32_t m_activeSimulationIslands_size;
    uint32_t m_activeSimulationIslands_cap;
    hkpSimulationIsland** m_inactiveSimulationIslands;
    uint32_t m_inactiveSimulationIslands_size;
    uint32_t m_inactiveSimulationIslands_cap;
    hkpSimulationIsland** m_dirtySimulationIslands;
    uint32_t m_dirtySimulationIslands_size;
    uint32_t m_dirtySimulationIslands_cap;
    uint8_t _1[0x18];
    void* m_broadPhase; //DSR always uses a hkp3AxisSweep broadPhase
    uint8_t _2a[0x58];
    int m_criticalOperationsLockCount;
    int m_criticalOperationsLockCountForPhantoms;
    uint8_t _2b[0x98];
    hkpPhantom** m_phantoms;
    uint32_t m_phantoms_size;
    uint32_t m_phantoms_cap;
    uint8_t _3[0xE8];
    hkpBroadPhaseBorder* m_broadPhaseBorder;
};
static_assert(offsetof(hkpWorld, m_activeSimulationIslands) == 0x40);
static_assert(offsetof(hkpWorld, m_inactiveSimulationIslands) == 0x50);
static_assert(offsetof(hkpWorld, m_dirtySimulationIslands) == 0x60);
static_assert(offsetof(hkpWorld, m_broadPhase) == 0x88);
static_assert(offsetof(hkpWorld, m_criticalOperationsLockCount) == 0xE8);
static_assert(offsetof(hkpWorld, m_phantoms) == 0x188);
static_assert(offsetof(hkpWorld, m_broadPhaseBorder) == 0x280);

struct hkpSimulationIsland
{
    void* vtable;
    uint8_t data_0[0x18];
    hkpWorld* m_world;
    uint32_t m_numConstraints;
    uint16_t m_storageIndex;
    uint16_t m_dirtyListIndex;
    uint8_t data_1[0x10];
    float m_timeSinceLastHighFrequencyCheck;
    float m_timeSinceLastLowFrequencyCheck;
    void** m_actions; //this seems to always be empty, keep as null
    uint32_t m_actions_size;
    uint32_t m_actions_cap;
    uint64_t data_2;
    hkpEntity** m_entities;
    uint32_t m_entities_size;
    uint32_t m_entities_cap;
    hkpEntity* m_entities_inline; //just set to null i think, and clear the cap flags
    uint64_t data_3;
    hkpAgentNnSector** m_sectorsmidphase;
    uint32_t m_sectorsmidphase_size;
    uint32_t m_sectorsmidphase_cap;
    hkpAgentNnSector* m_sectorsmidphase_inline; //just set to null i think, and clear the cap flags
    uint64_t data_4;
    hkpAgentNnSector** m_sectorsnarrowphase;
    uint32_t m_sectorsnarrowphase_size;
    uint32_t m_sectorsnarrowphase_cap;
    hkpAgentNnSector* m_sectorsnarrowphase_inline; //just set to null i think, and clear the cap flags
};
static_assert(offsetof(hkpSimulationIsland, m_world) == 0x20);
static_assert(offsetof(hkpSimulationIsland, m_storageIndex) == 0x2c);
static_assert(offsetof(hkpSimulationIsland, data_1) == 0x30);
static_assert(offsetof(hkpSimulationIsland, m_timeSinceLastHighFrequencyCheck) == 0x40);
static_assert(offsetof(hkpSimulationIsland, m_actions) == 0x48);
static_assert(offsetof(hkpSimulationIsland, data_2) == 0x58);
static_assert(offsetof(hkpSimulationIsland, m_entities) == 0x60);
static_assert(offsetof(hkpSimulationIsland, m_sectorsmidphase) == 0x80);
static_assert(offsetof(hkpSimulationIsland, m_sectorsnarrowphase) == 0xa0);
static_assert(sizeof(hkpSimulationIsland) == 0xb8);

struct hkAabbUint32
{
    uint32_t m_min[3];
    uint8_t m_expansionMin[3];
    uint8_t m_expansionShift;
    uint32_t m_max[3];
    uint8_t m_expansionMax[3];
    uint8_t m_shapeKeyByte;
};
static_assert(offsetof(hkAabbUint32, m_expansionMin) == 0xc);
static_assert(offsetof(hkAabbUint32, m_expansionShift) == 0xf);
static_assert(offsetof(hkAabbUint32, m_max) == 0x10);
static_assert(offsetof(hkAabbUint32, m_expansionMax) == 0x1c);
static_assert(sizeof(hkAabbUint32) == 0x20);

#pragma pack(push, 1)
struct BoundingVolumeData
{
    hkAabbUint32 base;
    uint16_t m_numChildShapeAabbs;
    uint16_t m_capacityChildShapeAabbs;
    hkAabbUint32* m_childShapeAabbs;
    uint32_t* m_childShapeKeys;
    uint32_t padding2;
};
#pragma pack(pop)
static_assert(offsetof(BoundingVolumeData, m_numChildShapeAabbs) == 0x20);
static_assert(offsetof(BoundingVolumeData, m_capacityChildShapeAabbs) == 0x22);
static_assert(offsetof(BoundingVolumeData, m_childShapeAabbs) == 0x24);
static_assert(offsetof(BoundingVolumeData, m_childShapeKeys) == 0x2c);
static_assert(sizeof(BoundingVolumeData) == 0x38);

struct hkpTypedBroadPhaseHandle
{
    uint32_t m_id;
    uint8_t m_type;
    uint8_t m_ownerOffset;
    uint8_t m_objectQualityType;
    uint8_t padding;
    uint32_t m_collisionFilterInfo;
};
static_assert(offsetof(hkpTypedBroadPhaseHandle, m_objectQualityType) == 0x6);
static_assert(sizeof(hkpTypedBroadPhaseHandle) == 0xc);

struct hkpCollidable
{
    void* shape;
    uint32_t m_shapeKey;
    uint32_t padding1;
    void* m_motion; //either a hkTransform, or a hkMotionState. For our purposes i think this is always just an offset to the parent hkpShapePhantom's inline hkMotionState struct
    void* m_parent;
    uint8_t m_ownerOffset;
    uint8_t m_forceCollideOntoPpu;
    uint16_t m_shapeSizeOnSpu;
    hkpTypedBroadPhaseHandle m_broadPhaseHandle;
    BoundingVolumeData m_boundingVolumeData;
    float m_allowedPenetrationDepth;
    uint32_t padding2;
};
static_assert(offsetof(hkpCollidable, shape) == 0);
static_assert(offsetof(hkpCollidable, m_shapeKey) == 8);
static_assert(offsetof(hkpCollidable, m_motion) == 0x10);
static_assert(offsetof(hkpCollidable, m_ownerOffset) == 0x20);
static_assert(offsetof(hkpCollidable, m_forceCollideOntoPpu) == 0x21);
static_assert(offsetof(hkpCollidable, m_shapeSizeOnSpu) == 0x22);
static_assert(offsetof(hkpCollidable, m_broadPhaseHandle) == 0x24);
static_assert(offsetof(hkpCollidable, m_boundingVolumeData) == 0x30);
static_assert(offsetof(hkpCollidable, m_allowedPenetrationDepth) == 0x68);
static_assert(sizeof(hkpCollidable) == 0x70);

struct hkpLinkedCollidable
{
    hkpCollidable base;
    CollisionEntry* m_collisionEntries;
    uint32_t m_collisionEntries_len;
    uint32_t m_collisionEntries_cap;
};
static_assert(offsetof(hkpLinkedCollidable, m_collisionEntries) == 0x70);
static_assert(sizeof(hkpLinkedCollidable) == 0x80);

struct hkMotionState
{
    //hkTransform
    float m_transform_rotation0[4];
    float m_transform_rotation1[4];
    float m_transform_rotation2[4];
    float m_transform_translation[4];
    //hkSweptTRansform
    float m_sweptTransform_centerOfMass0[4];
    float m_sweptTransform_centerOfMass1[4];
    float m_sweptTransform_rotation0[4];
    float m_sweptTransform_rotation1[4];
    float m_sweptTransform_centerOfMassLocal[4];
    //
    float m_deltaAngle[4];
    float m_objectRadius;
    uint16_t m_linearDamping;
    uint16_t m_angularDamping;
    uint16_t m_timeFactor;
    uint8_t m_maxLinearVelocity;
    uint8_t m_maxAngularVelocity;
    uint8_t m_deactivationClass;
    uint8_t padding[3];
};
static_assert(offsetof(hkMotionState, m_sweptTransform_centerOfMass0) == 0x40);
static_assert(offsetof(hkMotionState, m_deltaAngle) == 0x90);
static_assert(offsetof(hkMotionState, m_linearDamping) == 0xa4);
static_assert(offsetof(hkMotionState, m_maxAngularVelocity) == 0xab);
static_assert(sizeof(hkMotionState) == 0xb0);

struct hkpPhantom
{
    void* vtable;
    uint64_t data_0;
    void* hkpWorldPtr; //static pointer to the havok world
    //this is actually a FrpgPhysShapePhantomIns**, but we are saving these already as part of DamageMan, so treat as raw ptrs.
    // This is only used when we write to the game, and the target isn't realloc'd when we write to the game so it's stable.
    void** m_userData;
    hkpLinkedCollidable m_collidable;
};
static_assert(offsetof(hkpPhantom, data_0) == 0x8);
static_assert(offsetof(hkpPhantom, hkpWorldPtr) == 0x10);
static_assert(offsetof(hkpPhantom, m_userData) == 0x18);
static_assert(offsetof(hkpPhantom, m_collidable) == 0x20);

struct hkpSimpleShapePhantom
{
    hkpPhantom base;
    uint8_t data_1[16];
    void* unk_0;
    hkpProperty* m_properties;
    uint32_t m_properties_len;
    uint32_t m_properties_cap;
    void** m_overlapListeners;
    uint32_t m_overlapListeners_len;
    uint32_t m_overlapListeners_cap;
    void** m_phantomListeners;
    uint32_t m_phantomListeners_len;
    uint32_t m_phantomListeners_cap;
    uint64_t padding_0;
    hkMotionState m_motionState;
    hkpCollidable** m_collisionDetails;
    uint32_t m_collisionDetails_len;
    uint32_t m_collisionDetails_cap;
    uint8_t data_2[16];
};
static_assert(offsetof(hkpSimpleShapePhantom, data_1) == 0xa0);
static_assert(offsetof(hkpSimpleShapePhantom, unk_0) == 0xb0);
static_assert(offsetof(hkpSimpleShapePhantom, m_properties) == 0xb8);
static_assert(offsetof(hkpSimpleShapePhantom, m_phantomListeners) == 0xd8);
static_assert(offsetof(hkpSimpleShapePhantom, m_motionState) == 0xf0);
static_assert(offsetof(hkpSimpleShapePhantom, m_collisionDetails) == 0x1a0);
static_assert(offsetof(hkpSimpleShapePhantom, data_2) == 0x1b0);
static_assert(sizeof(hkpSimpleShapePhantom) == 0x1c0);

struct hkpMotion
{
    uint64_t vtable;
    uint8_t data_0[0x120];
    hkpMotion* m_savedMotion; //hkpSphereMotion/hkpBoxMotion, but the data itself is just hkpMotion
    uint8_t data_1[0x10];
};
static_assert(sizeof(hkpMotion) == 0x140);
static_assert(offsetof(hkpMotion, data_0) == 0x8);
static_assert(offsetof(hkpMotion, m_savedMotion) == 0x128);
static_assert(offsetof(hkpMotion, data_1) == 0x130);

struct hkpEntity
{
    uint64_t vtable;
    uint64_t data_0;
    hkpWorld* _hkpWorld;
    void* m_userData; //FrpgPhysSysIns_Entity*, points to the entry in FrpgPhysSysIns's arrays
    hkpLinkedCollidable m_collidable;
    uint8_t data_1[16];
    void* m_name;
    hkpProperty* m_properties; //always null
    uint32_t m_properties_len;
    uint32_t m_properties_cap;
    uint8_t m_material[0x10];
    void* m_limitContactImpulseUtilAndFlag; //always null
    uint64_t data_2;
    void* m_breakableBody; //always null
    uint8_t data_3[8];
    hkConstraintInternal* m_constraintsMaster;
    uint16_t m_constraintsMaster_size;
    uint16_t m_constraintsMaster_cap;
    uint32_t data_4;
    hkpConstraintInstance** m_constraintsSlave;
    uint32_t m_constraintsSlave_size;
    uint32_t m_constraintsSlave_cap;
    uint8_t* m_constraintRuntime;
    uint32_t m_constraintRuntime_size;
    uint32_t m_constraintRuntime_cap;
    hkpSimulationIsland* m_simulationIsland;
    uint8_t data_5[8];
    void* spuCollisionCallback_m_util; //always null
    uint16_t spuCollisionCallback_m_capacity;
    uint8_t data_6[14];
    hkpMotion m_motion;
    void** m_contactListeners; //always null
    uint16_t m_contactListeners_size;
    uint16_t m_contactListeners_cap;
    uint32_t data_8;
    void** m_actions; //always null
    uint16_t m_actions_size;
    uint16_t m_actions_cap;
    uint32_t data_9;
    void* m_localFrame;
    void* m_extendedListeners;
};
static_assert(sizeof(hkpEntity) == 0x2c0);
static_assert(offsetof(hkpEntity, data_0) == 0x8);
static_assert(offsetof(hkpEntity, _hkpWorld) == 0x10);
static_assert(offsetof(hkpEntity, m_userData) == 0x18);
static_assert(offsetof(hkpEntity, m_collidable) == 0x20);
static_assert(offsetof(hkpEntity, data_1) == 0xa0);
static_assert(offsetof(hkpEntity, m_name) == 0xb0);
static_assert(offsetof(hkpEntity, m_properties) == 0xb8);
static_assert(offsetof(hkpEntity, m_limitContactImpulseUtilAndFlag) == 0xd8);
static_assert(offsetof(hkpEntity, m_breakableBody) == 0xe8);
static_assert(offsetof(hkpEntity, m_constraintsMaster) == 0xf8);
static_assert(offsetof(hkpEntity, m_constraintsSlave) == 0x108);
static_assert(offsetof(hkpEntity, m_constraintRuntime) == 0x118);
static_assert(offsetof(hkpEntity, m_simulationIsland) == 0x128);
static_assert(offsetof(hkpEntity, spuCollisionCallback_m_util) == 0x138);
static_assert(offsetof(hkpEntity, m_motion) == 0x150);
static_assert(offsetof(hkpEntity, m_contactListeners) == 0x290);
static_assert(offsetof(hkpEntity, m_actions) == 0x2a0);
static_assert(offsetof(hkpEntity, m_localFrame) == 0x2b0);

struct hkpBroadPhaseBorder
{
    void* vtable;
    uint32_t m_memSizeAndFlags_and_refCount;
    uint32_t _pad0;
    void* listener_vtable1;
    void* listener_vtable2;
    void* listener_vtable3;
    void* m_world;
    void* phantom1;
    void* phantom2;
    void* phantom3;
    void* phantom4;
    void* phantom5;
    void* phantom6;
};
static_assert(offsetof(hkpBroadPhaseBorder, listener_vtable1) == 0x10);
static_assert(offsetof(hkpBroadPhaseBorder, phantom1) == 0x30);
static_assert(offsetof(hkpBroadPhaseBorder, phantom6) == 0x58);

struct hkpCapsuleShape
{
    uint64_t vtable;
    uint8_t _0[0x10];
    void* m_userData;
    uint8_t data_1[0x30];
};
static_assert(sizeof(hkpCapsuleShape) == 0x50);
static_assert(offsetof(hkpCapsuleShape, m_userData) == 0x18);
static_assert(offsetof(hkpCapsuleShape, data_1) == 0x20);

struct hkpSphereShape
{
    uint64_t vtable;
    uint8_t _0[0x10];
    void* m_userData;
    uint8_t data_1[0x18];
};
static_assert(sizeof(hkpSphereShape) == 0x38);
static_assert(offsetof(hkpSphereShape, m_userData) == 0x18);
static_assert(offsetof(hkpSphereShape, data_1) == 0x20);

/* ============================================================
 * Per-frame physics snapshot for rollback
 *  unsure if the shape ptr can get messed with during game operations. Testing seems to show yes so need to save it
 */
struct SavedEntityState
{
    hkpEntity* ptr;
    void* shapePtr; 
    alignas(16) uint8_t motionData[sizeof(hkpMotion)]; // hkpMotion in hkpEntity
    union
    {
        hkpCapsuleShape capsuleData;
        hkpSphereShape sphereData;
    } shapeData;
};

struct SavedPhantomState
{
    hkpSimpleShapePhantom* ptr;
    void* shapePtr;
    alignas(16) uint8_t motionStateData[sizeof(hkMotionState)]; // hkMotionState in hkpSimpleShapePhantom
    union
    {
        hkpCapsuleShape capsuleData;
        hkpSphereShape sphereData;
    } shapeData;
};

struct HkpWorldSnapshot
{
    std::vector<SavedEntityState> entities;
    std::vector<SavedPhantomState> phantoms;
};
/* ============================================================ */

#endif
