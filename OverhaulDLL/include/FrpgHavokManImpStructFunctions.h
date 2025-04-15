#pragma once
#ifndef FRPGHAVOKMAN_FUNCTIONS_H
#define FRPGHAVOKMAN_FUNCTIONS_H

#include <stdint.h>
#include <string>
#include "FrpgHavokManImpStruct.h"

/* ---------------- GENERAL + DAMAGE ENTRY ------------------ */

std::string print_hkpSimpleShapePhantom(hkpSimpleShapePhantom* to);
void copy_hkpSimpleShapePhantom(hkpSimpleShapePhantom* to, const hkpSimpleShapePhantom* from);
hkpSimpleShapePhantom* init_hkpSimpleShapePhantom();
void free_hkpSimpleShapePhantom(hkpSimpleShapePhantom* to);

std::string print_hkpSimpleShapePhantom_field0x30(hkpSimpleShapePhantom_field0x30* to);
void copy_hkpSimpleShapePhantom_field0x30(hkpSimpleShapePhantom_field0x30* to, const hkpSimpleShapePhantom_field0x30* from);
hkpSimpleShapePhantom_field0x30* init_hkpSimpleShapePhantom_field0x30();
void free_hkpSimpleShapePhantom_field0x30(hkpSimpleShapePhantom_field0x30* to);

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
