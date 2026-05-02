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

// Snapshot helpers
void SaveHkpWorldSnapshot(HkpWorldSnapshot* snap, hkpWorld* world);
void RestoreHkpWorldSnapshot(const HkpWorldSnapshot* snap, hkpWorld* world);
void CopyHkpWorldSnapshot(HkpWorldSnapshot* dst, const HkpWorldSnapshot* src);
void FreeHkpWorldSnapshotRefs(HkpWorldSnapshot* snap);

void copy_hkpShape(void* to, void* from);

void copy_hkpSphereShape(hkpSphereShape* to, hkpSphereShape* from);
hkpSphereShape* init_hkpSphereShape(StateTarget target);
void free_hkpSphereShape(hkpSphereShape* to, StateTarget target);

void copy_hkpCapsuleShape(hkpCapsuleShape* to, hkpCapsuleShape* from);
hkpCapsuleShape* init_hkpCapsuleShape(StateTarget target);
void free_hkpCapsuleShape(hkpCapsuleShape* to, StateTarget target);

#endif
