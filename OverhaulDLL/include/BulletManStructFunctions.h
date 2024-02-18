#pragma once
#ifndef BULLETMAN_STRUCTFUNCTIONS_H
#define BULLETMAN_STRUCTFUNCTIONS_H

#include <stdint.h>
#include <string>
#include "BulletManStruct.h"

std::string print_BulletMan(BulletMan* to);
void copy_BulletMan(BulletMan* to, BulletMan* from, bool to_game);
BulletMan* init_BulletMan();
void free_BulletMan(BulletMan* to);

std::string print_ChrCam(ChrCam* to);
void copy_ChrCam(ChrCam* to, ChrCam* from, bool to_game);
ChrCam* init_ChrCam();
void free_ChrCam(ChrCam* to);

std::string print_ChrExFollowCam(ChrExFollowCam* to);
void copy_ChrExFollowCam(ChrExFollowCam* to, ChrExFollowCam* from, bool to_game);
ChrExFollowCam* init_ChrExFollowCam();
void free_ChrExFollowCam(ChrExFollowCam* to);

std::string print_BulletIns(BulletIns* to);
void copy_BulletIns(BulletIns* to, BulletIns* from, bool to_game);
BulletIns* init_BulletIns();
void free_BulletIns(BulletIns* to, bool freeself);

std::string print_BulletIns_FollowupBullet(BulletIns_FollowupBullet* to);
void copy_BulletIns_FollowupBullet_Data(BulletIns_FollowupBullet* to, BulletIns_FollowupBullet* from, bool to_game);
void copy_BulletIns_FollowupBullet(BulletIns_FollowupBullet* to, BulletIns_FollowupBullet* from, bool to_game);
void copy_BulletIns_FollowupBullet_List(
    BulletIns_FollowupBullet** to_followup_bullet_list_ptr, int16_t* to_followup_bullet_list_len_ptr,
    BulletIns_FollowupBullet** from_followup_bullet_list_ptr, int16_t* from_followup_bullet_list_len_ptr,
    bool to_game);
void free_BulletIns_FollowupBullet(BulletIns_FollowupBullet* to, bool freeself, bool freenext);

std::string print_BulletIns_Field0x90_Field0x1a0(BulletIns_Field0x90_Field0x1a0* to);
void copy_BulletIns_Field0x90_Field0x1a0(BulletIns_Field0x90_Field0x1a0* to, BulletIns_Field0x90_Field0x1a0* from, bool to_game);

std::string print_BulletTargetingSystemOwner(BulletTargetingSystemOwner* to);
void copy_BulletTargetingSystemOwner(BulletTargetingSystemOwner* to, BulletTargetingSystemOwner* from, bool to_game);

std::string print_TargetingSystemBase(TargetingSystemBase* to);
void copy_TargetingSystemBase(TargetingSystemBase* to, TargetingSystemBase* from, bool to_game);

std::string print_BulletState(BulletState* to);
void copy_BulletState(BulletState* to, BulletState* from, bool to_game);

std::string print_BulletFlyState(BulletFlyState* to);
void copy_BulletFlyState(BulletFlyState* to, BulletFlyState* from, bool to_game);

std::string print_BulletParamInfo(BulletParamInfo* to);
void copy_BulletParamInfo(BulletParamInfo* to, BulletParamInfo* from, bool to_game);

std::string print_BulletMan_Field0x20(BulletMan_Field0x20* to);
void copy_BulletMan_Field0x20(BulletMan_Field0x20* to, BulletMan_Field0x20* from, bool to_game);
BulletMan_Field0x20* init_BulletMan_Field0x20();
void free_BulletMan_Field0x20(BulletMan_Field0x20* to, bool freeself);

std::string print_BulletMan_Field0x40(BulletMan_Field0x40* to);
void copy_BulletMan_Field0x40(BulletMan_Field0x40* to, BulletMan_Field0x40* from, bool to_game);
BulletMan_Field0x40* init_BulletMan_Field0x40();
void free_BulletMan_Field0x40(BulletMan_Field0x40* to, bool freeself);

std::string print_BulletMan_field0x78Elem(BulletMan_field0x78Elem* to);
void copy_BulletMan_field0x78Elem(BulletMan_field0x78Elem* to, BulletMan_field0x78Elem* from, bool to_game);
BulletMan_field0x78Elem* init_BulletMan_field0x78Elem();
void free_BulletMan_field0x78Elem(BulletMan_field0x78Elem* to);

#endif
