#pragma once
#ifndef DAMAGEMANSTRUCTFUNCTIONS_H
#define DAMAGEMANSTRUCTFUNCTIONS_H

#include <stdint.h>
#include "DamageManStruct.h"
#include "Rollback.h"

void copy_DamageMan(DamageMan* to, DamageMan* from, const hkpWorld* to_world, const hkpWorld* from_world, StateTarget target);
DamageMan* init_DamageMan();
void free_DamageMan(DamageMan* to);

void copy_DamageEntry(DamageEntry* to, DamageEntry* from, const hkpWorld* to_world, const hkpWorld* from_world, StateTarget target);
DamageEntry* init_DamageEntry();
void free_DamageEntry(DamageEntry* to, bool freeself);

void copy_FrpgPhysShapePhantomIns(FrpgPhysShapePhantomIns** to, FrpgPhysShapePhantomIns** from, bool is_sphere, const hkpWorld* to_world, const hkpWorld* from_world, StateTarget target);
FrpgPhysShapePhantomIns* init_FrpgPhysShapePhantomIns(bool is_sphere, StateTarget target);
void free_FrpgPhysShapePhantomIns(FrpgPhysShapePhantomIns* to, bool is_sphere);

void copy_DamageEntryField0x118(DamageEntryField0x118** to, DamageEntryField0x118** from, StateTarget target);
DamageEntryField0x118* init_DamageEntryField0x118(StateTarget target);
void free_DamageEntryField0x118(DamageEntryField0x118* to, StateTarget target);

#endif
