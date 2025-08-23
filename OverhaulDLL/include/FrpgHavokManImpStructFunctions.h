#pragma once
#ifndef FRPGHAVOKMAN_FUNCTIONS_H
#define FRPGHAVOKMAN_FUNCTIONS_H

#include <stdint.h>
#include <string>
#include "FrpgHavokManImpStruct.h"

/* ---------------- CHRCTRL + DAMAGE MAN ------------------ */

std::string print_hkpSimpleShapePhantom(hkpSimpleShapePhantom* to);
void copy_hkpSimpleShapePhantom(hkpSimpleShapePhantom* to, const hkpSimpleShapePhantom* from, bool to_game);
hkpSimpleShapePhantom* init_hkpSimpleShapePhantom();
void free_hkpSimpleShapePhantom(hkpSimpleShapePhantom* to);

void copy_hkpLinkedCollidable(hkpLinkedCollidable* to, const hkpLinkedCollidable* from, bool to_game);

void copy_hkpProperty(hkpProperty* to, hkpProperty* from);

void copy_hkMotionState(hkMotionState* to, const hkMotionState* from);

void copy_hkpCollidable(hkpCollidable* to, const hkpCollidable* from, bool to_game);

bool hkpShape_isSphere(hkpSphereShape* to);
void copy_hkpSphereShape(hkpSphereShape** to, hkpSphereShape** from, bool to_game);
hkpSphereShape* init_hkpSphereShape(bool to_game);
void free_hkpSphereShape(hkpSphereShape* to, bool to_game);

void copy_hkpCapsuleShape(hkpCapsuleShape** to, hkpCapsuleShape** from, bool to_game);
hkpCapsuleShape* init_hkpCapsuleShape(bool to_game);
void free_hkpCapsuleShape(hkpCapsuleShape* to, bool to_game);

/* ---------------- CHRCTRL ------------------ */

std::string print_hkpCharacterProxy(hkpCharacterProxy* to);
void copy_hkpCharacterProxy(hkpCharacterProxy* to, const hkpCharacterProxy* from);
hkpCharacterProxy* init_hkpCharacterProxy();
void free_hkpCharacterProxy(hkpCharacterProxy* to);


#endif
