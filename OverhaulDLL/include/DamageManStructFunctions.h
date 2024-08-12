#pragma once
#ifndef DAMAGEMANSTRUCTFUNCTIONS_H
#define DAMAGEMANSTRUCTFUNCTIONS_H

#include <stdint.h>
#include "DamageManStruct.h"

void copy_DamageMan(DamageMan* to, DamageMan* from, bool to_game);
DamageMan* init_DamageMan();
void free_DamageMan(DamageMan* to);

void copy_DamageEntry(DamageEntry* to, DamageEntry* from, bool to_game);
DamageEntry* init_DamageEntry();
void free_DamageEntry(DamageEntry* to, bool freeself);

void copy_FrpgPhysShapePhantomIns(FrpgPhysShapePhantomIns** to, FrpgPhysShapePhantomIns** from, bool to_game);
FrpgPhysShapePhantomIns* init_FrpgPhysShapePhantomIns(bool to_game);
void free_FrpgPhysShapePhantomIns(FrpgPhysShapePhantomIns* to, bool to_game);

void copy_hkpSphereShape(hkpSphereShape** to, hkpSphereShape** from, bool to_game);
hkpSphereShape* init_hkpSphereShape(bool to_game);
void free_hkpSphereShape(hkpSphereShape* to, bool to_game);

void copy_hkpCapsuleShape(hkpCapsuleShape** to, hkpCapsuleShape** from, bool to_game);
hkpCapsuleShape* init_hkpCapsuleShape(bool to_game);
void free_hkpCapsuleShape(hkpCapsuleShape* to, bool to_game);

void copy_DamageEntryField0x118(DamageEntryField0x118** to, DamageEntryField0x118** from, bool to_game);
DamageEntryField0x118* init_DamageEntryField0x118(bool to_game);
void free_DamageEntryField0x118(DamageEntryField0x118* to, bool to_game);

#endif
