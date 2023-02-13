#pragma once
#ifndef BULLETMAN_STRUCTFUNCTIONS_H
#define BULLETMAN_STRUCTFUNCTIONS_H

#include <stdint.h>
#include "BulletManStruct.h"

void copy_BulletMan(BulletMan* to, BulletMan* from, bool to_game);
BulletMan* init_BulletMan();
void free_BulletMan(BulletMan* to);

void copy_ChrCam(ChrCam* to, ChrCam* from, bool to_game);
ChrCam* init_ChrCam();
void free_ChrCam(ChrCam* to);

void copy_ChrExFollowCam(ChrExFollowCam* to, ChrExFollowCam* from, bool to_game);
ChrExFollowCam* init_ChrExFollowCam();
void free_ChrExFollowCam(ChrExFollowCam* to);

void copy_BulletIns(BulletIns* to, BulletIns* from, bool to_game);
BulletIns* init_BulletIns();
void free_BulletIns(BulletIns* to, bool freeself);

void copy_BulletIns_FollowupBullet(BulletIns_FollowupBullet* to, BulletIns_FollowupBullet* from, bool to_game);

void copy_BulletIns_Field0x90_Field0x1a0(BulletIns_Field0x90_Field0x1a0* to, BulletIns_Field0x90_Field0x1a0* from, bool to_game);

void copy_BulletTargetingSystemOwner(BulletTargetingSystemOwner* to, BulletTargetingSystemOwner* from, bool to_game);

void copy_TargetingSystemBase(TargetingSystemBase* to, TargetingSystemBase* from, bool to_game);

void copy_BulletState(BulletState* to, BulletState* from, bool to_game);

void copy_BulletFlyState(BulletFlyState* to, BulletFlyState* from, bool to_game);

void copy_BulletParamInfo(BulletParamInfo* to, BulletParamInfo* from, bool to_game);

void copy_BulletMan_Field0x20(BulletMan_Field0x20* to, BulletMan_Field0x20* from, bool to_game);
BulletMan_Field0x20* init_BulletMan_Field0x20();
void free_BulletMan_Field0x20(BulletMan_Field0x20* to, bool freeself);

void copy_BulletMan_Field0x40(BulletMan_Field0x40* to, BulletMan_Field0x40* from, bool to_game);
BulletMan_Field0x40* init_BulletMan_Field0x40();
void free_BulletMan_Field0x40(BulletMan_Field0x40* to, bool freeself);

void copy_BulletMan_field0x78Elem(BulletMan_field0x78Elem* to, BulletMan_field0x78Elem* from, bool to_game);
BulletMan_field0x78Elem* init_BulletMan_field0x78Elem();
void free_BulletMan_field0x78Elem(BulletMan_field0x78Elem* to);

#endif
