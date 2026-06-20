#pragma once
#ifndef FRPGHAVOKMAN_STRUCT_H
#define FRPGHAVOKMAN_STRUCT_H

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
    void* vtable;                          // 0x0 (hkReferencedObject::hkBaseObject)
    uint16_t m_memSizeAndFlags;            // 0x8
    uint16_t m_referenceCount;             // 0xa
    uint32_t unk_c;                        // 0xc (padding)
    uint32_t m_constraintInfo[4];          // 0x10
    hkpWorld* m_world;                     // 0x20
    uint32_t m_numConstraints;             // 0x28
    uint16_t m_storageIndex;               // 0x2c
    uint16_t m_dirtyListIndex;             // 0x2e
    uint8_t m_splitCheckFrameCounter;      // 0x30
    uint8_t flags1;                        // 0x31
    uint8_t flags2;                        // 0x32
    uint8_t unk_33;                        // 0x33 (gap)
    uint32_t unk_34;                       // 0x34
    uint8_t unk_38[4];                     // 0x38 (gap)
    uint16_t unk_3c;                       // 0x3c
    uint8_t unk_3e[2];                     // 0x3e (gap)
    float m_timeSinceLastHighFrequencyCheck;  // 0x40
    float m_timeSinceLastLowFrequencyCheck;   // 0x44
    void** m_actions; //this seems to always be empty, keep as null (0x48)
    uint32_t m_actions_size;               // 0x50
    uint32_t m_actions_cap;                // 0x54
    float m_timeOfDeactivation;            // 0x58
    uint32_t unk_5c;                       // 0x5c (gap)
    hkpEntity** m_entities;                // 0x60
    uint32_t m_entities_size;              // 0x68
    uint32_t m_entities_cap;               // 0x6c
    hkpEntity* m_entities_inline; //just set to null i think, and clear the cap flags (0x70)
    uint16_t m_midphase_bytesUsedInLastSector;  // 0x78
    uint8_t m_midphase_nnTrackType;        // 0x7a
    uint8_t unk_7b[5];                     // 0x7b (gap)
    hkpAgentNnSector** m_sectorsmidphase;  // 0x80 (hkpAgentNnTrack::m_sectors)
    uint32_t m_sectorsmidphase_size;       // 0x88
    uint32_t m_sectorsmidphase_cap;        // 0x8c
    hkpAgentNnSector* m_sectorsmidphase_inline; //just set to null i think, and clear the cap flags (0x90)
    uint16_t m_narrowphase_bytesUsedInLastSector;  // 0x98
    uint8_t m_narrowphase_nnTrackType;     // 0x9a
    uint8_t unk_9b[5];                     // 0x9b (gap)
    hkpAgentNnSector** m_sectorsnarrowphase;  // 0xa0 (hkpAgentNnTrack::m_sectors)
    uint32_t m_sectorsnarrowphase_size;    // 0xa8
    uint32_t m_sectorsnarrowphase_cap;     // 0xac
    hkpAgentNnSector* m_sectorsnarrowphase_inline; //just set to null i think, and clear the cap flags (0xb0)
};
static_assert(offsetof(hkpSimulationIsland, m_constraintInfo) == 0x10);
static_assert(offsetof(hkpSimulationIsland, m_world) == 0x20);
static_assert(offsetof(hkpSimulationIsland, m_storageIndex) == 0x2c);
static_assert(offsetof(hkpSimulationIsland, m_splitCheckFrameCounter) == 0x30);
static_assert(offsetof(hkpSimulationIsland, m_timeSinceLastHighFrequencyCheck) == 0x40);
static_assert(offsetof(hkpSimulationIsland, m_actions) == 0x48);
static_assert(offsetof(hkpSimulationIsland, m_timeOfDeactivation) == 0x58);
static_assert(offsetof(hkpSimulationIsland, m_entities) == 0x60);
static_assert(offsetof(hkpSimulationIsland, m_midphase_bytesUsedInLastSector) == 0x78);
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
    void* vtable;                // 0x0 (hkReferencedObject::hkBaseObject)
    uint16_t m_memSizeAndFlags;  // 0x8
    uint16_t m_referenceCount;   // 0xa
    uint32_t unk_c;              // 0xc (padding)
    void* hkpWorldPtr; //static pointer to the havok world (0x10)
    //this is actually a FrpgPhysShapePhantomIns**, but we are saving these already as part of DamageMan, so treat as raw ptrs.
    // This is only used when we write to the game, and the target isn't realloc'd when we write to the game so it's stable.
    void** m_userData;           // 0x18
    hkpLinkedCollidable m_collidable;  // 0x20
};
static_assert(offsetof(hkpPhantom, m_memSizeAndFlags) == 0x8);
static_assert(offsetof(hkpPhantom, hkpWorldPtr) == 0x10);
static_assert(offsetof(hkpPhantom, m_userData) == 0x18);
static_assert(offsetof(hkpPhantom, m_collidable) == 0x20);

struct hkpSimpleShapePhantom
{
    hkpPhantom base;           // 0x0
    uint32_t unk_a0;           // 0xa0 (ghidra: undefined4)
    uint8_t unk_a4[4];         // 0xa4 (gap)
    uint16_t unk_a8;           // 0xa8 (ghidra: undefined2)
    uint8_t unk_aa[6];         // 0xaa (gap)
    void* unk_0;               // 0xb0 (ghidra: hkpWorldObject::m_name)
    hkpProperty* m_properties; // 0xb8
    uint32_t m_properties_len; // 0xc0
    uint32_t m_properties_cap; // 0xc4
    void** m_overlapListeners; // 0xc8
    uint32_t m_overlapListeners_len;  // 0xd0
    uint32_t m_overlapListeners_cap;  // 0xd4
    void** m_phantomListeners; // 0xd8
    uint32_t m_phantomListeners_len;  // 0xe0
    uint32_t m_phantomListeners_cap;  // 0xe4
    uint64_t padding_0;        // 0xe8
    hkMotionState m_motionState;  // 0xf0
    hkpCollidable** m_collisionDetails;  // 0x1a0
    uint32_t m_collisionDetails_len;  // 0x1a8
    uint32_t m_collisionDetails_cap;  // 0x1ac
    uint8_t m_orderDirty;      // 0x1b0 (ghidra: bool)
    uint8_t unk_1b1[15];       // 0x1b1 (gap)
};
static_assert(offsetof(hkpSimpleShapePhantom, unk_a0) == 0xa0);
static_assert(offsetof(hkpSimpleShapePhantom, unk_0) == 0xb0);
static_assert(offsetof(hkpSimpleShapePhantom, m_properties) == 0xb8);
static_assert(offsetof(hkpSimpleShapePhantom, m_phantomListeners) == 0xd8);
static_assert(offsetof(hkpSimpleShapePhantom, m_motionState) == 0xf0);
static_assert(offsetof(hkpSimpleShapePhantom, m_collisionDetails) == 0x1a0);
static_assert(offsetof(hkpSimpleShapePhantom, m_orderDirty) == 0x1b0);
static_assert(sizeof(hkpSimpleShapePhantom) == 0x1c0);

struct hkpMotion
{
    uint64_t vtable;                          // 0x0 (ghidra: hkReferencedObject::hkBaseObject)
    uint16_t m_memSizeAndFlags;               // 0x8
    uint16_t m_referenceCount;                // 0xa
    uint32_t unk_c;                           // 0xc (padding)
    uint8_t m_type;                           // 0x10
    uint8_t m_deactivationIntegrateCounter;   // 0x11
    uint16_t m_deactivationNumInactiveFrames[2]; // 0x12
    uint8_t unk_16[10];                       // 0x16 (gap)
    hkMotionState m_motionState;              // 0x20 (the actual transform/swept transform)
    float m_inertiaAndMassInv[4];             // 0xd0 (ghidra: Vector4)
    float m_linearVelocity[4];                // 0xe0 (ghidra: Vector4)
    float m_angularVelocity[4];               // 0xf0 (ghidra: Vector4)
    float m_deactivationRefPosition[8];       // 0x100 (ghidra: Vector4[2])
    uint32_t m_deactivationRefOrientation[2]; // 0x120
    hkpMotion* m_savedMotion; //hkpSphereMotion/hkpBoxMotion, but the data itself is just hkpMotion (0x128)
    uint16_t m_savedQualityTypeIndex;         // 0x130
    uint16_t m_gravityFactor;                 // 0x132
    uint8_t unk_134[12];                      // 0x134 (gap)
};
static_assert(sizeof(hkpMotion) == 0x140);
static_assert(offsetof(hkpMotion, m_memSizeAndFlags) == 0x8);
static_assert(offsetof(hkpMotion, m_motionState) == 0x20);
static_assert(offsetof(hkpMotion, m_linearVelocity) == 0xe0);
static_assert(offsetof(hkpMotion, m_savedMotion) == 0x128);
static_assert(offsetof(hkpMotion, m_savedQualityTypeIndex) == 0x130);

struct hkpEntity
{
    uint64_t vtable;           // 0x0 (hkReferencedObject::hkBaseObject)
    uint16_t m_memSizeAndFlags;  // 0x8
    uint16_t m_referenceCount;   // 0xa
    uint32_t unk_c;             // 0xc (padding)
    hkpWorld* _hkpWorld;       // 0x10
    void* m_userData; //FrpgPhysSysIns_Entity*, points to the entry in FrpgPhysSysIns's arrays (0x18)
    hkpLinkedCollidable m_collidable;  // 0x20
    uint32_t unk_a0;           // 0xa0 (ghidra: undefined4)
    uint8_t unk_a4[4];         // 0xa4 (gap)
    uint16_t unk_a8;           // 0xa8 (ghidra: undefined2)
    uint8_t unk_aa[6];         // 0xaa (gap)
    void* m_name;              // 0xb0
    hkpProperty* m_properties; //always null (0xb8)
    uint32_t m_properties_len; // 0xc0
    uint32_t m_properties_cap; // 0xc4
    uint8_t m_material[0x10];  // 0xc8 (ghidra: hkpMaterial(12) + 4 gap)
    void* m_limitContactImpulseUtilAndFlag; //always null (0xd8)
    float m_damageMultiplier;  // 0xe0
    uint32_t unk_e4;           // 0xe4 (gap)
    void* m_breakableBody; //always null (0xe8)
    uint32_t m_solverData;     // 0xf0
    uint16_t m_storageIndex;   // 0xf4
    uint16_t m_contactPointCallbackDelay;  // 0xf6
    hkConstraintInternal* m_constraintsMaster;  // 0xf8
    uint16_t m_constraintsMaster_size;  // 0x100
    uint16_t m_constraintsMaster_cap;   // 0x102
    uint32_t unk_104;          // 0x104 (former data_4; gap)
    hkpConstraintInstance** m_constraintsSlave;  // 0x108
    uint32_t m_constraintsSlave_size;   // 0x110
    uint32_t m_constraintsSlave_cap;    // 0x114
    uint8_t* m_constraintRuntime;       // 0x118
    uint32_t m_constraintRuntime_size;  // 0x120
    uint32_t m_constraintRuntime_cap;   // 0x124
    hkpSimulationIsland* m_simulationIsland;  // 0x128
    uint8_t m_autoRemoveLevel;          // 0x130
    uint8_t m_numShapeKeysInContactPointProperties;  // 0x131
    uint8_t m_responseModifierFlags;    // 0x132
    uint8_t unk_133;                    // 0x133 (gap)
    uint32_t m_uid;                     // 0x134
    void* spuCollisionCallback_m_util; //always null (0x138)
    uint16_t spuCollisionCallback_m_capacity;  // 0x140
    uint8_t spuCollisionCallback_m_eventFilter;  // 0x142
    uint8_t spuCollisionCallback_m_userFilter;   // 0x143
    uint8_t unk_144[12];                // 0x144 (gap)
    hkpMotion m_motion;                 // 0x150
    void** m_contactListeners; //always null (0x290)
    uint16_t m_contactListeners_size;   // 0x298
    uint16_t m_contactListeners_cap;    // 0x29a
    uint32_t unk_29c;          // 0x29c (former data_8; gap)
    void** m_actions; //always null (0x2a0)
    uint16_t m_actions_size;            // 0x2a8
    uint16_t m_actions_cap;             // 0x2aa
    uint32_t unk_2ac;          // 0x2ac (former data_9; gap)
    void* m_localFrame;        // 0x2b0
    void* m_extendedListeners; // 0x2b8
};
static_assert(sizeof(hkpEntity) == 0x2c0);
static_assert(offsetof(hkpEntity, m_memSizeAndFlags) == 0x8);
static_assert(offsetof(hkpEntity, _hkpWorld) == 0x10);
static_assert(offsetof(hkpEntity, m_userData) == 0x18);
static_assert(offsetof(hkpEntity, m_collidable) == 0x20);
static_assert(offsetof(hkpEntity, unk_a0) == 0xa0);
static_assert(offsetof(hkpEntity, m_name) == 0xb0);
static_assert(offsetof(hkpEntity, m_properties) == 0xb8);
static_assert(offsetof(hkpEntity, m_limitContactImpulseUtilAndFlag) == 0xd8);
static_assert(offsetof(hkpEntity, m_damageMultiplier) == 0xe0);
static_assert(offsetof(hkpEntity, m_breakableBody) == 0xe8);
static_assert(offsetof(hkpEntity, m_solverData) == 0xf0);
static_assert(offsetof(hkpEntity, m_constraintsMaster) == 0xf8);
static_assert(offsetof(hkpEntity, m_constraintsSlave) == 0x108);
static_assert(offsetof(hkpEntity, m_constraintRuntime) == 0x118);
static_assert(offsetof(hkpEntity, m_simulationIsland) == 0x128);
static_assert(offsetof(hkpEntity, m_autoRemoveLevel) == 0x130);
static_assert(offsetof(hkpEntity, m_uid) == 0x134);
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
    uint64_t vtable;       // 0x0
    uint8_t _0[0x10];      // 0x8
    void* m_userData;      // 0x18
    float m_radius;        // 0x20 (ghidra: hkpConvexShape::m_radius)
    uint32_t unk_24;       // 0x24 (padding)
    uint8_t unk_28[8];     // 0x28 (gap before the vertices)
    float vertexA[4];      // 0x30 (ghidra: Vector4)
    float vertexB[4];      // 0x40 (ghidra: Vector4)
};
static_assert(sizeof(hkpCapsuleShape) == 0x50);
static_assert(offsetof(hkpCapsuleShape, m_userData) == 0x18);
static_assert(offsetof(hkpCapsuleShape, m_radius) == 0x20);
static_assert(offsetof(hkpCapsuleShape, vertexA) == 0x30);

struct hkpSphereShape
{
    uint64_t vtable;       // 0x0
    uint8_t _0[0x10];      // 0x8
    void* m_userData;      // 0x18
    float m_radius;        // 0x20 (ghidra: hkpConvexShape::m_radius)
    uint32_t unk_24;       // 0x24 (padding)
    uint32_t m_pad[3];     // 0x28 (ghidra: uint32_t[3])
    uint32_t unk_34;       // 0x34 (padding)
};
static_assert(sizeof(hkpSphereShape) == 0x38);
static_assert(offsetof(hkpSphereShape, m_userData) == 0x18);
static_assert(offsetof(hkpSphereShape, m_radius) == 0x20);
static_assert(offsetof(hkpSphereShape, m_pad) == 0x28);

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
