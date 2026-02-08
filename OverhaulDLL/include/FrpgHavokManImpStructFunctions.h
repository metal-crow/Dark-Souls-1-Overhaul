#pragma once
#ifndef FRPGHAVOKMAN_FUNCTIONS_H
#define FRPGHAVOKMAN_FUNCTIONS_H

#include <stdint.h>
#include <string>
#include "FrpgHavokManImpStruct.h"
#include "Rollback.h"

/* ---------------- hkpWorld ------------------ */

void copy_FrpgHavokManImp(FrpgHavokManImp* to, const FrpgHavokManImp* from, StateTarget target);
FrpgHavokManImp* init_FrpgHavokManImp();
void free_FrpgHavokManImp(FrpgHavokManImp* to);

void copy_FrpgPhysWorld(FrpgPhysWorld* to, const FrpgPhysWorld* from, StateTarget target);
FrpgPhysWorld* init_FrpgPhysWorld();
void free_FrpgPhysWorld(FrpgPhysWorld* to);

void copy_hkpWorld(hkpWorld* to, const hkpWorld* from, StateTarget target);
hkpWorld* init_hkpWorld();
void free_hkpWorld(hkpWorld* to);

void copy_hkpSimulationIsland(hkpSimulationIsland* to, uint32_t to_size, hkpSimulationIsland* from, uint32_t from_size, StateTarget target);
void free_hkpSimulationIsland(hkpSimulationIsland* to);

void copy_hkp3AxisSweep(hkp3AxisSweep* to, const hkp3AxisSweep* from, StateTarget target);
hkp3AxisSweep* init_hkp3AxisSweep();
void free_hkp3AxisSweep(hkp3AxisSweep* to);

void copy_hkpBpNode(hkpBpNode* to, const hkpBpNode* from, StateTarget target);

void copy_hkpBpAxis(hkpBpAxis* to, const hkpBpAxis* from, StateTarget target);

void copy_hkpBpEndPoint(hkpBpEndPoint* to, const hkpBpEndPoint* from, StateTarget target);

enum PhantomType { SimpleShape, Aabb, PhantomNull, InvalidPhantom };
PhantomType hkpPhantom_getType(void* to);
void copy_hkpPhantom(void** to, void* from, StateTarget target);
void free_hkpPhantom(void* to, StateTarget target);

/* ---------------- hkpWorld, DamagEntry ------------------ */

std::string print_hkpSimpleShapePhantom(hkpSimpleShapePhantom* to);
void copy_hkpSimpleShapePhantom(hkpSimpleShapePhantom* to, const hkpSimpleShapePhantom* from, StateTarget target);
hkpSimpleShapePhantom* init_hkpSimpleShapePhantom(StateTarget target);
void free_hkpSimpleShapePhantom(hkpSimpleShapePhantom* to, StateTarget target);
hkpSimpleShapePhantom* get_hkpSimpleShapePhantom(hkpWorld* world, FrpgPhysShapePhantomIns* parent);
hkpSimpleShapePhantom* find_hkpSimpleShapePhantom(hkpWorld* world, FrpgPhysShapePhantomIns* parent);

void copy_hkpLinkedCollidable(hkpLinkedCollidable* to, const hkpLinkedCollidable* from, StateTarget target);

void copy_hkpProperty(hkpProperty* to, hkpProperty* from, StateTarget target);
void init_hkpProperty(hkpProperty* to, StateTarget target);
void free_hkpProperty(hkpProperty* to);

void copy_hkMotionState(hkMotionState* to, const hkMotionState* from);

void copy_hkpCollidable(hkpCollidable* to, const hkpCollidable* from, StateTarget target);
void init_hkpCollidable(hkpCollidable** to, StateTarget target);
void free_hkpCollidable(hkpCollidable* to);

//void copy_CollisionEntry(CollisionEntry* to, CollisionEntry* from, hkpLinkedCollidable* parent);

void copy_hkpTypedBroadPhaseHandle(hkpTypedBroadPhaseHandle* to, const hkpTypedBroadPhaseHandle* from);

void copy_BoundingVolumeData(BoundingVolumeData* to, const BoundingVolumeData* from, StateTarget target);
void init_BoundingVolumeData(BoundingVolumeData* to, StateTarget target);
void free_BoundingVolumeData(BoundingVolumeData* to, StateTarget target);


//void copy_hkpAgentNnEntry(hkpAgentNnEntry* to, hkpAgentNnEntry* from);
enum ShapeType { Sphere, Capsule, MoppBvTree, ConvexVertices, ShapeNull, InvalidShape };
ShapeType hkpShape_getType(void* to);
void copy_hkpShape(void** to, void* from, StateTarget target);
void free_hkpShape(void* to, StateTarget target);

void copy_hkpSphereShape(hkpSphereShape** to, hkpSphereShape* from, StateTarget target);
hkpSphereShape* init_hkpSphereShape(StateTarget target);
void free_hkpSphereShape(hkpSphereShape* to, StateTarget target);

void copy_hkpCapsuleShape(hkpCapsuleShape** to, hkpCapsuleShape* from, StateTarget target);
hkpCapsuleShape* init_hkpCapsuleShape(StateTarget target);
void free_hkpCapsuleShape(hkpCapsuleShape* to, StateTarget target);

void copy_hkpMoppBvTreeShape(hkpMoppBvTreeShape** to, hkpMoppBvTreeShape* from, StateTarget target);
hkpMoppBvTreeShape* init_hkpMoppBvTreeShape(StateTarget target);
void free_hkpMoppBvTreeShape(hkpMoppBvTreeShape* to, StateTarget target);

void copy_hkpConvexVerticesShape(hkpConvexVerticesShape** to, hkpConvexVerticesShape* from, StateTarget target);
hkpConvexVerticesShape* init_hkpConvexVerticesShape(StateTarget target);
void free_hkpConvexVerticesShape(hkpConvexVerticesShape* to, StateTarget target);

void copy_hkpConvexVerticesConnectivity(hkpConvexVerticesConnectivity** to, hkpConvexVerticesConnectivity* from, StateTarget target);
hkpConvexVerticesConnectivity* init_hkpConvexVerticesConnectivity(StateTarget target);
void free_hkpConvexVerticesConnectivity(hkpConvexVerticesConnectivity* to, StateTarget target);

/* ---------------- CHRCTRL ------------------ */

std::string print_hkpCharacterProxy(hkpCharacterProxy* to);
void copy_hkpCharacterProxy(hkpCharacterProxy* to, const hkpCharacterProxy* from, hkpWorld* world, StateTarget target);
hkpCharacterProxy* init_hkpCharacterProxy(StateTarget target);
void free_hkpCharacterProxy(hkpCharacterProxy* to);


#endif
