#pragma once
#ifndef FRPGHAVOKMAN_FUNCTIONS_H
#define FRPGHAVOKMAN_FUNCTIONS_H

#include <stdint.h>
#include <string>
#include "FrpgHavokManImpStruct.h"

/* ---------------- HAVOK MAN ------------------ */

void copy_FrpgHavokManImp(FrpgHavokManImp* to, const FrpgHavokManImp* from, bool to_game);
FrpgHavokManImp* init_FrpgHavokManImp();
void free_FrpgHavokManImp(FrpgHavokManImp* to);

void copy_FrpgPhysWorld(FrpgPhysWorld* to, const FrpgPhysWorld* from, bool to_game);
FrpgPhysWorld* init_FrpgPhysWorld();
void free_FrpgPhysWorld(FrpgPhysWorld* to);

void copy_hkpWorld(hkpWorld* to, const hkpWorld* from, bool to_game);
hkpWorld* init_hkpWorld();
void free_hkpWorld(hkpWorld* to);

void copy_hkp3AxisSweep(hkp3AxisSweep* to, const hkp3AxisSweep* from, bool to_game);
hkp3AxisSweep* init_hkp3AxisSweep();
void free_hkp3AxisSweep(hkp3AxisSweep* to);

void copy_hkpBpNode(hkpBpNode* to, const hkpBpNode* from, bool to_game);

void copy_hkpBpAxis(hkpBpAxis* to, const hkpBpAxis* from, bool to_game);

void copy_hkpBpEndPoint(hkpBpEndPoint* to, const hkpBpEndPoint* from, bool to_game);

/* ---------------- CHRCTRL + DAMAGE MAN ------------------ */

std::string print_hkpSimpleShapePhantom(hkpSimpleShapePhantom* to);
void copy_hkpSimpleShapePhantom(hkpSimpleShapePhantom* to, const hkpSimpleShapePhantom* from, bool to_game);
hkpSimpleShapePhantom* init_hkpSimpleShapePhantom();
void free_hkpSimpleShapePhantom(hkpSimpleShapePhantom* to);

void copy_hkpLinkedCollidable(hkpLinkedCollidable* to, const hkpLinkedCollidable* from, bool to_game);

void copy_hkpProperty(hkpProperty* to, hkpProperty* from, bool to_game);
void init_hkpProperty(hkpProperty* to, bool to_game);
void free_hkpProperty(hkpProperty* to);

void copy_hkMotionState(hkMotionState* to, const hkMotionState* from);

void copy_hkpCollidable(hkpCollidable* to, const hkpCollidable* from, bool to_game);
void init_hkpCollidable(hkpCollidable** to, bool to_game);
void free_hkpCollidable(hkpCollidable* to);

//void copy_CollisionEntry(CollisionEntry* to, CollisionEntry* from, hkpLinkedCollidable* parent);

void copy_hkpTypedBroadPhaseHandle(hkpTypedBroadPhaseHandle* to, const hkpTypedBroadPhaseHandle* from);

void copy_BoundingVolumeData(BoundingVolumeData* to, const BoundingVolumeData* from, bool to_game);
void init_BoundingVolumeData(BoundingVolumeData* to, bool to_game);
void free_BoundingVolumeData(BoundingVolumeData* to, bool to_game);


//void copy_hkpAgentNnEntry(hkpAgentNnEntry* to, hkpAgentNnEntry* from);
enum ShapeType { Sphere, Capsule, MoppBvTree, InvalidShape };
ShapeType hkpShape_getType(void* to);

void copy_hkpSphereShape(hkpSphereShape** to, hkpSphereShape** from, bool to_game);
hkpSphereShape* init_hkpSphereShape(bool to_game);
void free_hkpSphereShape(hkpSphereShape* to, bool to_game);

void copy_hkpCapsuleShape(hkpCapsuleShape** to, hkpCapsuleShape** from, bool to_game);
hkpCapsuleShape* init_hkpCapsuleShape(bool to_game);
void free_hkpCapsuleShape(hkpCapsuleShape* to, bool to_game);

void copy_hkpMoppBvTreeShape(hkpMoppBvTreeShape** to, hkpMoppBvTreeShape** from, bool to_game);
hkpMoppBvTreeShape* init_hkpMoppBvTreeShape(bool to_game);
void free_hkpMoppBvTreeShape(hkpMoppBvTreeShape* to, bool to_game);

/* ---------------- CHRCTRL ------------------ */

std::string print_hkpCharacterProxy(hkpCharacterProxy* to);
void copy_hkpCharacterProxy(hkpCharacterProxy* to, const hkpCharacterProxy* from, bool to_game);
hkpCharacterProxy* init_hkpCharacterProxy(bool to_game);
void free_hkpCharacterProxy(hkpCharacterProxy* to);


#endif
