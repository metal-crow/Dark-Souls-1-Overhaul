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

void copy_BulletIns(BulletIns* to, BulletIns* from, StateTarget target);
BulletIns* init_BulletIns();
void free_BulletIns(BulletIns* to, bool freeself);

void copy_BulletIns_FollowupBullet(BulletIns_FollowupBullet* to, BulletIns_FollowupBullet* from, StateTarget target);
void copy_BulletIns_FollowupBullet_Data(BulletIns_FollowupBullet* to, BulletIns_FollowupBullet* from, StateTarget target);
void copy_BulletIns_FollowupBullet_List(BulletIns_FollowupBullet** to_list, int16_t* to_len, BulletIns_FollowupBullet** from_list, int16_t* from_len, StateTarget target);

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
