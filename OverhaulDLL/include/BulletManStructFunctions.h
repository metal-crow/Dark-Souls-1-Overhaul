#pragma once
#ifndef BULLETMAN_STRUCTFUNCTIONS_H
#define BULLETMAN_STRUCTFUNCTIONS_H

#include <stdint.h>
#include <string>
#include "BulletManStruct.h"
#include "Rollback.h"

void copy_BulletMan(BulletMan* to, BulletMan* from, StateTarget target);
BulletMan* init_BulletMan();
void free_BulletMan(BulletMan* to);

void copy_ChrCam(ChrCam* to, ChrCam* from, StateTarget target);
ChrCam* init_ChrCam();
void free_ChrCam(ChrCam* to);

void copy_ChrExFollowCam(ChrExFollowCam* to, ChrExFollowCam* from, StateTarget target);
ChrExFollowCam* init_ChrExFollowCam();
void free_ChrExFollowCam(ChrExFollowCam* to);

void copy_BulletIns(BulletIns* to, BulletIns* from, StateTarget target);
BulletIns* init_BulletIns();
void free_BulletIns(BulletIns* to, bool freeself);

void copy_BulletIns_FollowupBullet_Data(BulletIns_FollowupBullet* to, BulletIns_FollowupBullet* from, StateTarget target);
void copy_BulletIns_FollowupBullet(BulletIns_FollowupBullet* to, BulletIns_FollowupBullet* from, StateTarget target);
void copy_BulletIns_FollowupBullet_List(
    BulletIns_FollowupBullet** to_followup_bullet_list_ptr, int16_t* to_followup_bullet_list_len_ptr,
    BulletIns_FollowupBullet** from_followup_bullet_list_ptr, int16_t* from_followup_bullet_list_len_ptr,
    StateTarget target);
void free_BulletIns_FollowupBullet(BulletIns_FollowupBullet* to, bool freeself, bool freenext);

void copy_BulletIns_Field0x90_Field0x1a0(BulletIns_Field0x90_Field0x1a0* to, BulletIns_Field0x90_Field0x1a0* from, StateTarget target);

void copy_BulletTargetingSystemOwner(BulletTargetingSystemOwner* to, BulletTargetingSystemOwner* from, StateTarget target);

void copy_TargetingSystemBase(TargetingSystemBase* to, TargetingSystemBase* from, StateTarget target);

void copy_BulletState(BulletState* to, BulletState* from, StateTarget target);

void copy_BulletFlyState(BulletFlyState* to, BulletFlyState* from, StateTarget target);

void copy_BulletParamInfo(BulletParamInfo* to, BulletParamInfo* from, StateTarget target);

void copy_BulletMan_Field0x20(BulletMan_Field0x20* to, BulletMan_Field0x20* from, StateTarget target);
BulletMan_Field0x20* init_BulletMan_Field0x20();
void free_BulletMan_Field0x20(BulletMan_Field0x20* to, bool freeself);

void copy_BulletMan_Field0x40(BulletMan_Field0x40* to, BulletMan_Field0x40* from, StateTarget target);
BulletMan_Field0x40* init_BulletMan_Field0x40();
void free_BulletMan_Field0x40(BulletMan_Field0x40* to, bool freeself);

void copy_BulletMan_field0x78Elem(BulletMan_field0x78Elem* to, BulletMan_field0x78Elem* from, StateTarget target);
BulletMan_field0x78Elem* init_BulletMan_field0x78Elem();
void free_BulletMan_field0x78Elem(BulletMan_field0x78Elem* to);

#endif
