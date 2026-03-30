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
typedef struct hkp3AxisSweep hkp3AxisSweep;
typedef struct hkpSimulationIsland hkpSimulationIsland;
typedef struct hkpAgentNnSector hkpAgentNnSector;
typedef struct hkpEntity hkpEntity;
typedef struct hkpMotion hkpMotion;
typedef struct hkpBpNode hkpBpNode;
typedef struct hkpBpAxis hkpBpAxis;
typedef struct hkpBpEndPoint hkpBpEndPoint;
typedef struct hkpSphereShape hkpSphereShape;
typedef struct hkpCapsuleShape hkpCapsuleShape;
typedef struct hkpMoppBvTreeShape hkpMoppBvTreeShape;
typedef struct hkpMoppCode hkpMoppCode;
typedef struct hkpConvexVerticesShape hkpConvexVerticesShape;
typedef struct hkpConvexVerticesConnectivity hkpConvexVerticesConnectivity;
typedef struct hkpConvexTranslateShape hkpConvexTranslateShape;
typedef struct hkpBoxShape hkpBoxShape;
typedef struct hkpSimpleShapePhantom hkpSimpleShapePhantom;
typedef struct hkpAabbPhantom hkpAabbPhantom;
typedef struct hkpCollidable hkpCollidable;
typedef struct hkMotionState hkMotionState;
typedef struct hkpProperty hkpProperty;
typedef struct hkpLinkedCollidable hkpLinkedCollidable;
typedef struct hkpTypedBroadPhaseHandle hkpTypedBroadPhaseHandle;
typedef struct BoundingVolumeData BoundingVolumeData;
typedef struct hkAabbUint32 hkAabbUint32;
typedef struct hkpAgentNnEntry hkpAgentNnEntry;
typedef struct hkpContactMgr hkpContactMgr;
typedef struct CollisionEntry CollisionEntry;
typedef struct hkpCharacterProxy_field0x20elem hkpCharacterProxy_field0x20elem;
typedef struct hkpCharacterProxy hkpCharacterProxy;
typedef struct ChrInsProxyListener ChrInsProxyListener;
typedef struct hkConstraintInternal hkConstraintInternal;
typedef struct hkpConstraintInstance hkpConstraintInstance;


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
    hkp3AxisSweep* m_broadPhase; //DSR always uses a hkp3AxisSweep broadPhase
    uint8_t _2[0xF8];
    void** m_phantoms;
    uint32_t m_phantoms_size;
    uint32_t m_phantoms_cap;
};
static_assert(offsetof(hkpWorld, m_activeSimulationIslands) == 0x40);
static_assert(offsetof(hkpWorld, m_inactiveSimulationIslands) == 0x50);
static_assert(offsetof(hkpWorld, m_dirtySimulationIslands) == 0x60);
static_assert(offsetof(hkpWorld, m_broadPhase) == 0x88);
static_assert(offsetof(hkpWorld, m_phantoms) == 0x188);
//static_assert(sizeof(hkpWorld) == 0x30);

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

struct hkpAgentNnSector
{
    uint8_t m_data[960];
};

struct hkpBpNode
{
    uint8_t data_1[0x10];
    uint32_t* index_in_array;
};
static_assert(sizeof(hkpBpNode) == 0x18);

struct hkpBpAxis
{
    hkpBpEndPoint* arry;
    uint32_t len;
    uint32_t capacity;
};
static_assert(sizeof(hkpBpAxis) == 0x10);

struct hkpBpEndPoint
{
    uint16_t m_value;
    uint16_t m_nodeIndex;
};
static_assert(sizeof(hkpBpEndPoint) == 0x4);

struct hkp3AxisSweep
{
    void* vtable;
    uint8_t data_1[0x10];
    uint8_t m_multiThreadCheck[0x10]; //nosave
    void* m_criticalSection; //nosave
    uint8_t data_2[0x80];
    hkpBpNode* m_nodes;
    uint32_t m_nodes_len;
    uint32_t m_nodes_cap;
    hkpBpAxis m_axis[3];
    //game does not use markers, always 0/null
    uint32_t m_numMarkers;
    uint32_t m_ld2NumMarkers;
    void* m_markers;
    uint8_t data_3[0x10];
};
static_assert(offsetof(hkp3AxisSweep, data_1) == 0x8);
static_assert(offsetof(hkp3AxisSweep, m_multiThreadCheck) == 0x18);
static_assert(offsetof(hkp3AxisSweep, m_criticalSection) == 0x28);
static_assert(offsetof(hkp3AxisSweep, data_2) == 0x30);
static_assert(offsetof(hkp3AxisSweep, m_nodes) == 0xb0);
static_assert(offsetof(hkp3AxisSweep, m_axis) == 0xc0);
static_assert(offsetof(hkp3AxisSweep, m_numMarkers) == 0xf0);
static_assert(offsetof(hkp3AxisSweep, m_markers) == 0xf8);
static_assert(offsetof(hkp3AxisSweep, data_3) == 0x100);
static_assert(sizeof(hkp3AxisSweep) == 0x110);

struct hkpCapsuleShape
{
    uint64_t vtable;
    uint8_t data_0[0x10];
    void* m_userData;
    uint8_t data_1[0x30];
};
static_assert(sizeof(hkpCapsuleShape) == 0x50);
static_assert(offsetof(hkpCapsuleShape, m_userData) == 0x18);

struct hkpSphereShape
{
    uint64_t vtable;
    uint8_t data_0[0x10];
    void* m_userData;
    uint8_t data_1[0x18];
};
static_assert(sizeof(hkpSphereShape) == 0x38);
static_assert(offsetof(hkpSphereShape, m_userData) == 0x18);

struct hkpMoppCode
{
    uint64_t vtable;
    uint8_t data_0[0x18];
    uint8_t* m_data;
    uint32_t m_data_len;
    uint32_t m_data_cap;
    uint8_t data_1[0x10];
};
static_assert(sizeof(hkpMoppCode) == 0x40);
static_assert(offsetof(hkpMoppCode, m_data) == 0x20);

//this shape is usually only used for large static geometry like level mesh.
//it should be save to just save and restore it by pointer instead of a full copy (which is complex)
struct hkpMoppBvTreeShape
{
    uint64_t vtable1;
    uint8_t data_0[0x10];
    void* m_userData;
    uint64_t data_1;
    hkpMoppCode* m_code;
    uint8_t* m_moppData;
    uint32_t m_moppData_size;
    uint8_t data_2[0x14];
    uint64_t vtable2;
    void* m_childShape; //this is a shape type. usually something very complex like a hkpStorageExtendedMeshShape
    uint8_t data_3[0x10];
};
static_assert(sizeof(hkpMoppBvTreeShape) == 0x70);
static_assert(offsetof(hkpMoppBvTreeShape, m_userData) == 0x18);
static_assert(offsetof(hkpMoppBvTreeShape, m_code) == 0x28);
static_assert(offsetof(hkpMoppBvTreeShape, m_moppData) == 0x30);
static_assert(offsetof(hkpMoppBvTreeShape, vtable2) == 0x50);
static_assert(offsetof(hkpMoppBvTreeShape, m_childShape) == 0x50+0x8);

struct hkpConvexVerticesShape
{
    uint64_t vtable;
    uint8_t data_0[0x10];
    void* m_userData;
    uint8_t data_1[0x30];
    void* m_rotatedVertices;
    uint32_t m_rotatedVertices_size;
    uint32_t m_rotatedVertices_cap;
    uint64_t data_2;
    void* m_planeEquations;
    uint32_t m_planeEquations_size;
    uint32_t m_planeEquations_cap;
    hkpConvexVerticesConnectivity* m_connectivity;
};
static_assert(sizeof(hkpConvexVerticesShape) == 0x80);
static_assert(offsetof(hkpConvexVerticesShape, m_userData) == 0x18);
static_assert(offsetof(hkpConvexVerticesShape, m_rotatedVertices) == 0x50);
static_assert(offsetof(hkpConvexVerticesShape, m_planeEquations) == 0x68);
static_assert(offsetof(hkpConvexVerticesShape, m_connectivity) == 0x78);

struct hkpConvexVerticesConnectivity
{
    uint64_t vtable;
    uint8_t data_0[0x8];
    uint16_t* m_vertexIndices;
    uint32_t m_vertexIndices_size;
    uint32_t m_vertexIndices_cap;
    uint8_t* m_numVerticesPerFace;
    uint32_t m_numVerticesPerFace_size;
    uint32_t m_numVerticesPerFace_cap;
};
static_assert(sizeof(hkpConvexVerticesConnectivity) == 0x30);
static_assert(offsetof(hkpConvexVerticesConnectivity, m_vertexIndices) == 0x10);
static_assert(offsetof(hkpConvexVerticesConnectivity, m_numVerticesPerFace) == 0x20);

struct hkpConvexTranslateShape
{
    uint64_t vtable;
    uint8_t data_0[0x10];
    void* m_userData;
    uint64_t data_1;
    uint64_t vtable2;
    void* m_childShape;
    uint8_t data_2[0x18];
};
static_assert(sizeof(hkpConvexTranslateShape) == 0x50);
static_assert(offsetof(hkpConvexTranslateShape, m_userData) == 0x18);
static_assert(offsetof(hkpConvexTranslateShape, m_childShape) == 0x30);

struct hkpBoxShape
{
    uint64_t vtable;
    uint8_t data_0[0x10];
    void* m_userData;
    uint8_t data_1[0x20];
};
static_assert(sizeof(hkpBoxShape) == 0x40);
static_assert(offsetof(hkpBoxShape, m_userData) == 0x18);

struct hkpContactMgr
{
    uint64_t vtable;
    uint64_t data;
    uint8_t m_type;
};

struct hkpAgentNnEntry
{
    uint8_t m_streamCommand;
    uint8_t m_agentType;
    uint8_t m_numContactPoints;
    uint8_t m_size;
    uint16_t m_agentIndexOnCollidable[2];
    hkpContactMgr* m_contactMgr;
    uint8_t m_collisionQualityIndex;
    uint8_t m_forceCollideOntoPpu;
    uint8_t m_nnTrackType;
    uint8_t m_padding;
    // Additional 4 bytes padding on 64-bit platforms
    hkpLinkedCollidable* m_collidable[2];
};

struct CollisionEntry
{
    hkpAgentNnEntry* m_agentEntry;
    hkpLinkedCollidable* m_partner;
};

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

struct hkpProperty
{
    uint32_t key;
    uint32_t padding;
    void* data; //this is just a pointer back to the parent hkpCharacterProxy??
};

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

struct hkpSimpleShapePhantom
{
    void* vtable;
    uint64_t data_0;
    void* hkpWorldPtr; //this is just a pointer to the havok world, we should be able to just treat it as a const ptr. Anything important inside it we handle elsewhere
    //this is actually a FrpgPhysShapePhantomIns**, but we are saving these already as part of DamageMan, so treat as raw ptrs.
    // This is only used when we write to the game, and the target isn't realloc'd when we write to the game so it's stable.
    void** m_userData;
    hkpLinkedCollidable m_collidable;
    uint8_t data_1[16];
    void* unk_0;
    hkpProperty* m_properties;
    uint32_t m_properties_len;
    uint32_t m_properties_cap;
    //i guess we don't have to save the listeners?? maybe?
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
static_assert(offsetof(hkpSimpleShapePhantom, data_0) == 0x8);
static_assert(offsetof(hkpSimpleShapePhantom, hkpWorldPtr) == 0x10);
static_assert(offsetof(hkpSimpleShapePhantom, m_userData) == 0x18);
static_assert(offsetof(hkpSimpleShapePhantom, m_collidable) == 0x20);
static_assert(offsetof(hkpSimpleShapePhantom, data_1) == 0xa0);
static_assert(offsetof(hkpSimpleShapePhantom, unk_0) == 0xb0);
static_assert(offsetof(hkpSimpleShapePhantom, m_properties) == 0xb8);
static_assert(offsetof(hkpSimpleShapePhantom, m_phantomListeners) == 0xd8);
static_assert(offsetof(hkpSimpleShapePhantom, m_motionState) == 0xf0);
static_assert(offsetof(hkpSimpleShapePhantom, m_collisionDetails) == 0x1a0);
static_assert(offsetof(hkpSimpleShapePhantom, data_2) == 0x1b0);
static_assert(sizeof(hkpSimpleShapePhantom) == 0x1c0);

struct hkpAabbPhantom
{
    void* vtable;
    uint64_t data_0;
    void* hkpWorldPtr;
    void** m_userData; //same as hkpSimpleShapePhantom
    hkpLinkedCollidable m_collidable;
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
    void* vtable2;
    uint8_t data_2[0x20];
    hkpCollidable** m_overlappingCollidables;
    uint32_t m_overlappingCollidables_len;
    uint32_t m_overlappingCollidables_cap;
    uint8_t data_3[0x10];
};
static_assert(sizeof(hkpAabbPhantom) == 0x130);
static_assert(offsetof(hkpAabbPhantom, data_0) == 0x8);
static_assert(offsetof(hkpAabbPhantom, hkpWorldPtr) == 0x10);
static_assert(offsetof(hkpAabbPhantom, m_userData) == 0x18);
static_assert(offsetof(hkpAabbPhantom, m_collidable) == 0x20);
static_assert(offsetof(hkpAabbPhantom, data_1) == 0xa0);
static_assert(offsetof(hkpAabbPhantom, unk_0) == 0xb0);
static_assert(offsetof(hkpAabbPhantom, m_properties) == 0xb8);
static_assert(offsetof(hkpAabbPhantom, m_phantomListeners) == 0xd8);
static_assert(offsetof(hkpAabbPhantom, data_2) == 0xf0);
static_assert(offsetof(hkpAabbPhantom, m_overlappingCollidables) == 0x110);
static_assert(offsetof(hkpAabbPhantom, data_3) == 0x120);

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
    //TODO
    //FrpgPhysSysIns_Entity* m_userData; //points to the entry in FrpgPhysSysIns's arrays
    void* m_userData;
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

#endif
