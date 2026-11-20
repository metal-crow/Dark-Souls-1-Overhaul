#pragma once
#ifndef BULLETMAN_STRUCTFUNCTIONS_H
#define BULLETMAN_STRUCTFUNCTIONS_H

#include <stdint.h>
#include <string>
#include "BulletManStruct.h"
#include "Rollback.h"

class StateVisitor;

void copy_BulletMan(BulletMan* to, BulletMan* from, StateTarget target);
BulletMan* init_BulletMan();
void free_BulletMan(BulletMan* to);
void serialize_BulletMan(StateVisitor& v, BulletMan* m);
std::string print_BulletMan(BulletMan* m);
uint64_t hash_BulletMan(BulletMan* m);

//session start: make every free pool slot the same on every machine (see the definition)
void canonicalize_BulletMan_free_slots(BulletMan* m);

void copy_BulletIns(BulletIns* to, BulletIns* from, StateTarget target);
BulletIns* init_BulletIns();
void free_BulletIns(BulletIns* to, bool freeself);

void copy_SfxHandle_List(SfxHandle** to_list, int16_t* to_len, SfxHandle** from_list, int16_t* from_len, StateTarget target);
SfxHandle* alloc_SfxHandle_List(int16_t count, StateTarget target);
void free_SfxHandle_List(SfxHandle** list, int16_t* len, StateTarget target);

void copy_BulletMan_Field0x20(BulletMan_Field0x20* to, BulletMan_Field0x20* from, StateTarget target);
BulletMan_Field0x20* init_BulletMan_Field0x20();
void free_BulletMan_Field0x20(BulletMan_Field0x20* to, bool freeself);

void copy_BulletMan_Field0x40(BulletMan_Field0x40* to, BulletMan_Field0x40* from, StateTarget target);
BulletMan_Field0x40* init_BulletMan_Field0x40();
void free_BulletMan_Field0x40(BulletMan_Field0x40* to, bool freeself);

void copy_ChrCam(ChrCam* to, ChrCam* from, StateTarget target);
ChrCam* init_ChrCam();
void free_ChrCam(ChrCam* to);

#endif
