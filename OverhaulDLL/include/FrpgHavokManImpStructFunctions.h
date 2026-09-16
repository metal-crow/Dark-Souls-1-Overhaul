#pragma once
#ifndef FRPGHAVOKMAN_FUNCTIONS_H
#define FRPGHAVOKMAN_FUNCTIONS_H

#include <stdint.h>
#include <string>
#include "FrpgHavokManImpStruct.h"
#include "Rollback.h"

class StateVisitor;
struct DamageEntry;
struct FrpgPhysShapePhantomIns;

/* ---------------- FrpgHavokManImp ------------------ */

// The world's own state (its clock and dynamics step info) by value, plus the world-side work a load needs: damage phantoms are put in
// or taken out of the world to match the restored DamageMan, restored character phantoms get their broadphase entry moved, the character
// proxy's restored contacts are checked against its restored phantom, and restored ragdoll bodies get their AABB recomputed.
// Everything else havok holds for the players is saved by its owner (HavokChara, FrpgRagdollIns, the DamageMan entry records), so copy
// this LAST in the load, after copy_PlayerIns and copy_DamageMan.
void copy_FrpgHavokManImp(FrpgHavokManImp* to, const FrpgHavokManImp* from, StateTarget target);
FrpgHavokManImp* init_FrpgHavokManImp();
void free_FrpgHavokManImp(FrpgHavokManImp* to);

void copy_FrpgPhysWorld(FrpgPhysWorld* to, const FrpgPhysWorld* from, StateTarget target);
FrpgPhysWorld* init_FrpgPhysWorld();
void free_FrpgPhysWorld(FrpgPhysWorld* to);

void serialize_FrpgHavokManImp(StateVisitor& v, const FrpgHavokManImp* h, const uint32_t* order, uint32_t nplayers);
std::string print_FrpgHavokManImp(const FrpgHavokManImp* h, const uint32_t* order, uint32_t nplayers);
uint64_t hash_FrpgHavokManImp(const FrpgHavokManImp* h, const uint32_t* order, uint32_t nplayers);

/* ---------------- havok values, saved by their owner ------------------ */

// The parts of a sphere or capsule shape the game rewrites: radius and vertices. A game shape is only written if it is of the saved kind.
void copy_hkpShape(SavedHavokShape* to, const hkpShape* from);
void copy_hkpShape(hkpShape* to, const SavedHavokShape* from);

// A character phantom's motion state and shape. copy_FrpgHavokManImp moves its broadphase entry afterwards.
void copy_SavedHavokPhantom(SavedHavokPhantom* to, const FrpgPhysShapePhantomIns* from);
void copy_SavedHavokPhantom(FrpgPhysShapePhantomIns* to, const SavedHavokPhantom* from);

// Everything in hkpMotion but its object header, the motion type and m_savedMotion, which is an allocation the motion owns
void copy_hkpMotion(hkpMotion* to, const hkpMotion* from);
// ...and into a live body, switching its motion type back first with hkpRigidBody::setMotionType if it changed (a ragdoll going limp)
void copy_hkpEntity_motion(hkpEntity* to, const hkpMotion* from);

// Take a DamageEntry's two phantoms out of the world, ahead of destroying the entry
void free_DamageEntry_phantoms(DamageEntry* entry);

void serialize_hkMotionState(StateVisitor& v, const hkMotionState* m);
void serialize_hkpMotion(StateVisitor& v, const hkpMotion* m);
void serialize_SavedHavokShape(StateVisitor& v, const char* name, const SavedHavokShape* s);
void serialize_SavedHavokPhantom(StateVisitor& v, const char* name, const SavedHavokPhantom* p);

#endif
