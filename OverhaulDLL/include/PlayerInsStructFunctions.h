#pragma once
#ifndef PLAYERINSSTRUCTFUNCTIONS_H
#define PLAYERINSSTRUCTFUNCTIONS_H

#include <stdint.h>
#include <string>
#include "PlayerInsStruct.h"

std::string print_PlayerIns(PlayerIns* to);
void copy_PlayerIns(PlayerIns* to, const PlayerIns* from, bool to_game);
PlayerIns* init_PlayerIns();
void free_PlayerIns(PlayerIns* to);

std::string print_ChrAsm(ChrAsm* to);
void copy_ChrAsm(ChrAsm* to, const ChrAsm* from);
ChrAsm* init_ChrAsm();
void free_ChrAsm(ChrAsm* to);

std::string print_ChrAsmModelRes(ChrAsmModelRes* to);
void copy_ChrAsmModelRes(ChrAsmModelRes* to, const ChrAsmModelRes* from, bool to_game);
ChrAsmModelRes* init_ChrAsmModelRes();
void free_ChrAsmModelRes(ChrAsmModelRes* to);

std::string print_ChrAsmModelRes_Elem(ChrAsmModelRes_Elem* to);
void copy_ChrAsmModelRes_Elem(ChrAsmModelRes_Elem* to, const ChrAsmModelRes_Elem* from, bool to_game);
ChrAsmModelRes_Elem* init_ChrAsmModelRes_Elem();
void free_ChrAsmModelRes_Elem(ChrAsmModelRes_Elem* to, bool freeself);

std::string print_ChrAsmModel(ChrAsmModel* to);
void copy_ChrAsmModel(ChrAsmModel* to, const ChrAsmModel* from, bool to_game);
ChrAsmModel* init_ChrAsmModel();
void free_ChrAsmModel(ChrAsmModel* to);

std::string print_ProEquipCtrl(ProEquipCtrl* to);
void copy_ProEquipCtrl(ProEquipCtrl* to, const ProEquipCtrl* from, bool to_game);
ProEquipCtrl* init_ProEquipCtrl();
void free_ProEquipCtrl(ProEquipCtrl* to);

std::string print_WeaponEquipCtrl(WeaponEquipCtrl* to);
void copy_WeaponEquipCtrl(WeaponEquipCtrl* to, const WeaponEquipCtrl* from, bool to_game);
WeaponEquipCtrl* init_WeaponEquipCtrl();
void free_WeaponEquipCtrl(WeaponEquipCtrl* to);

std::string print_RingEquipCtrl(RingEquipCtrl* to);
void copy_RingEquipCtrl(RingEquipCtrl* to, const RingEquipCtrl* from, bool to_game);
RingEquipCtrl* init_RingEquipCtrl();
void free_RingEquipCtrl(RingEquipCtrl* to);

std::string print_PlayerGameData(PlayerGameData* to);
void copy_PlayerGameData(PlayerGameData* to, const PlayerGameData* from);
PlayerGameData* init_PlayerGameData();
void free_PlayerGameData(PlayerGameData* to);

std::string print_PlayerGameData_ChrProperties(PlayerGameData_ChrProperties* to);
void copy_PlayerGameData_ChrProperties(PlayerGameData_ChrProperties* to, const PlayerGameData_ChrProperties* from);

std::string print_EquipGameData(EquipGameData* to);
void copy_EquipGameData(EquipGameData* to, const EquipGameData* from);
EquipGameData* init_EquipGameData();
void free_EquipGameData(EquipGameData* to, bool freeself);

std::string print_EquipItemData(EquipItemData* to);
void copy_EquipItemData(EquipItemData* to, const EquipItemData* from);

std::string print_EquipMagicData(EquipMagicData* to);
void copy_EquipMagicData(EquipMagicData* to, const EquipMagicData* from);
EquipMagicData* init_EquipMagicData();
void free_EquipMagicData(EquipMagicData* to);

std::string print_PlayerGameData_AttributeInfo(PlayerGameData_AttributeInfo* to);
void copy_PlayerGameData_AttributeInfo(PlayerGameData_AttributeInfo* to, const PlayerGameData_AttributeInfo* from);

std::string print_ChrIns(ChrIns* to);
void copy_ChrIns(ChrIns* to, const ChrIns* from, bool to_game);
ChrIns* init_ChrIns();
void free_ChrIns(ChrIns* to, bool freeself);

std::string print_ChrAttachSys(ChrAttachSys* to);
void copy_ChrAttachSys(ChrAttachSys* to, const ChrAttachSys* from, bool to_game);
ChrAttachSys* init_ChrAttachSys();
void free_ChrAttachSys(ChrAttachSys* to, bool freeself);

std::string print_ChrIns_field0x18(ChrIns_field0x18* to);
void copy_ChrIns_field0x18(ChrIns_field0x18* to, const ChrIns_field0x18* from);
ChrIns_field0x18* init_ChrIns_field0x18();
void free_ChrIns_field0x18(ChrIns_field0x18* to);

std::string print_ChrIns_field0x2c8(ChrIns_field0x2c8* to);
void copy_ChrIns_field0x2c8(ChrIns_field0x2c8* to, const ChrIns_field0x2c8* from);
ChrIns_field0x2c8* init_ChrIns_field0x2c8();
void free_ChrIns_field0x2c8(ChrIns_field0x2c8* to);

std::string print_EntityThrowAnimationStatus(EntityThrowAnimationStatus* to);
void copy_EntityThrowAnimationStatus(EntityThrowAnimationStatus* to, const EntityThrowAnimationStatus* from, bool to_game);
EntityThrowAnimationStatus* init_EntityThrowAnimationStatus();
void free_EntityThrowAnimationStatus(EntityThrowAnimationStatus* to);

std::string print_ThrowSelfEsc(ThrowSelfEsc* to);
void copy_ThrowSelfEsc(ThrowSelfEsc* to, const ThrowSelfEsc* from);
ThrowSelfEsc* init_ThrowSelfEsc();
void free_ThrowSelfEsc(ThrowSelfEsc* to);

std::string print_QwcSpEffectEquipCtrl(QwcSpEffectEquipCtrl* to);
void copy_QwcSpEffectEquipCtrl(QwcSpEffectEquipCtrl* to, const QwcSpEffectEquipCtrl* from);
QwcSpEffectEquipCtrl* init_QwcSpEffectEquipCtrl();
void free_QwcSpEffectEquipCtrl(QwcSpEffectEquipCtrl* to);

std::string print_SpecialEffect(SpecialEffect* to);
void copy_SpecialEffect(SpecialEffect* to, const SpecialEffect* from, bool to_game);
SpecialEffect* init_SpecialEffect();
void free_SpecialEffect(SpecialEffect* to);

std::string print_SpecialEffect_Info(SpecialEffect_Info* to);
void copy_SpecialEffect_Info(SpecialEffect_Info* to, const SpecialEffect_Info* from, bool to_game);
SpecialEffect_Info* init_SpecialEffect_Info();
void free_SpecialEffect_Info(SpecialEffect_Info* to);

std::string print_PlayerCtrl(PlayerCtrl* to);
void copy_PlayerCtrl(PlayerCtrl* to, const PlayerCtrl* from, bool to_game);
PlayerCtrl* init_PlayerCtrl();
void free_PlayerCtrl(PlayerCtrl* to);

std::string print_ArrowTurnAnim(ArrowTurnAnim* to);
void copy_ArrowTurnAnim(ArrowTurnAnim* to, const ArrowTurnAnim* from);
ArrowTurnAnim* init_ArrowTurnAnim();
void free_ArrowTurnAnim(ArrowTurnAnim* to);

std::string print_SpinJoint(SpinJoint* to);
void copy_SpinJoint(SpinJoint* to, const SpinJoint* from);
SpinJoint* init_SpinJoint();
void free_SpinJoint(SpinJoint* to);

std::string print_TurnAnim(TurnAnim* to);
void copy_TurnAnim(TurnAnim* to, const TurnAnim* from);
TurnAnim* init_TurnAnim();
void free_TurnAnim(TurnAnim* to, bool freeself);

std::string print_ChrCtrl(ChrCtrl* to);
void copy_ChrCtrl(ChrCtrl* to, const ChrCtrl* from, bool to_game);
ChrCtrl* init_ChrCtrl();
void free_ChrCtrl(ChrCtrl* to, bool freeself);

std::string print_WalkAnim_Twist(WalkAnim_Twist* to);
void copy_WalkAnim_Twist(WalkAnim_Twist* to, const WalkAnim_Twist* from, bool to_game);
WalkAnim_Twist* init_WalkAnim_Twist();
void free_WalkAnim_Twist(WalkAnim_Twist* to);

std::string print_WalkAnim_Twist_Field0x228Elem(WalkAnim_Twist_Field0x228Elem* to);
void copy_WalkAnim_Twist_Field0x228Elem(WalkAnim_Twist_Field0x228Elem* to, const WalkAnim_Twist_Field0x228Elem* from, bool to_game);
WalkAnim_Twist_Field0x228Elem* init_WalkAnim_Twist_Field0x228Elem();
void free_WalkAnim_Twist_Field0x228Elem(WalkAnim_Twist_Field0x228Elem* to, bool freeself);

std::string print_WalkAnim_Twist_Field0x228Elem_field0x10elem(WalkAnim_Twist_Field0x228Elem_field0x10elem* to);
void copy_WalkAnim_Twist_Field0x228Elem_field0x10elem(WalkAnim_Twist_Field0x228Elem_field0x10elem* to, const WalkAnim_Twist_Field0x228Elem_field0x10elem* from);

std::string print_ActionCtrl(ActionCtrl* to);
void copy_ActionCtrl(ActionCtrl* to, const ActionCtrl* from, bool to_game);
ActionCtrl* init_ActionCtrl();
void free_ActionCtrl(ActionCtrl* to);

std::string print_ActionCtrl_0x30Substruct(ActionCtrl_0x30Substruct* to);
void copy_ActionCtrl_0x30Substruct(ActionCtrl_0x30Substruct* to, const ActionCtrl_0x30Substruct* from, bool to_game);

std::string print_EzState_detail_EzStateMachineImpl(EzState_detail_EzStateMachineImpl* to);
void copy_EzState_detail_EzStateMachineImpl(EzState_detail_EzStateMachineImpl* to, const EzState_detail_EzStateMachineImpl* from, bool to_game);
EzState_detail_EzStateMachineImpl* init_EzState_detail_EzStateMachineImpl();
void free_EzState_detail_EzStateMachineImpl(EzState_detail_EzStateMachineImpl* to);

std::string print_EzStateRegisterSet(EzStateRegisterSet* to);
void copy_EzStateRegisterSet(EzStateRegisterSet* to, const EzStateRegisterSet* from, bool to_game);
EzStateRegisterSet* init_EzStateRegisterSet();
void free_EzStateRegisterSet(EzStateRegisterSet* to, bool freeself);

std::string print_HavokChara(HavokChara * to);
void copy_HavokChara(HavokChara* to, const HavokChara* from, bool to_game);
HavokChara* init_HavokChara();
void free_HavokChara(HavokChara* to);

std::string print_HitIns(HitIns* to);
void copy_HitIns(HitIns* to, const HitIns* from);
HitIns* init_HitIns();
void free_HitIns(HitIns* to);

std::string print_AnimationMediator(AnimationMediator* to);
void copy_AnimationMediator(AnimationMediator* to, const AnimationMediator* from);
AnimationMediator* init_AnimationMediator();
void free_AnimationMediator(AnimationMediator* to);

std::string print_AnimationMediatorStateEntry(AnimationMediatorStateEntry* to);
void copy_AnimationMediatorStateEntry(AnimationMediatorStateEntry* to, const AnimationMediatorStateEntry* from);
AnimationMediatorStateEntry* init_AnimationMediatorStateEntry();
void free_AnimationMediatorStateEntry(AnimationMediatorStateEntry* to, bool freeself);

std::string print_ChrCtrl_AnimationQueue(ChrCtrl_AnimationQueue* to);
void copy_ChrCtrl_AnimationQueue(ChrCtrl_AnimationQueue* to, const ChrCtrl_AnimationQueue* from, bool to_game);
ChrCtrl_AnimationQueue* init_ChrCtrl_AnimationQueue();
void free_ChrCtrl_AnimationQueue(ChrCtrl_AnimationQueue* to);

std::string print_ChrCtrl_AnimationQueue_field0x20(ChrCtrl_AnimationQueue_field0x20* to);
void copy_ChrCtrl_AnimationQueue_field0x20(ChrCtrl_AnimationQueue_field0x20* to, const ChrCtrl_AnimationQueue_field0x20* from, bool to_game);
ChrCtrl_AnimationQueue_field0x20* init_ChrCtrl_AnimationQueue_field0x20();
void free_ChrCtrl_AnimationQueue_field0x20(ChrCtrl_AnimationQueue_field0x20* to);

std::string print_hkaAnimatedSkeleton(hkaAnimatedSkeleton* to);
void copy_hkaAnimatedSkeleton(hkaAnimatedSkeleton* to, const hkaAnimatedSkeleton* from);
hkaAnimatedSkeleton* init_hkaAnimatedSkeleton();
void free_hkaAnimatedSkeleton(hkaAnimatedSkeleton* to);

std::string print_hkaDefaultAnimationControl(hkaDefaultAnimationControl* to);
void copy_hkaDefaultAnimationControl(hkaDefaultAnimationControl* to, const hkaDefaultAnimationControl* from);
hkaDefaultAnimationControl* init_hkaDefaultAnimationControl();
void free_hkaDefaultAnimationControl(hkaDefaultAnimationControl* to);

std::string print_hkaAnimationControl(hkaAnimationControl* to);
void copy_hkaAnimationControl(hkaAnimationControl* to, const hkaAnimationControl* from);
hkaAnimationControl* init_hkaAnimationControl();
void free_hkaAnimationControl(hkaAnimationControl* to, bool freeself);

std::string print_ChrCtrl_AnimationQueue_field0x10(ChrCtrl_AnimationQueue_field0x10* to);
void copy_ChrCtrl_AnimationQueue_field0x10(ChrCtrl_AnimationQueue_field0x10* to, const ChrCtrl_AnimationQueue_field0x10* from);
ChrCtrl_AnimationQueue_field0x10* init_ChrCtrl_AnimationQueue_field0x10();
void free_ChrCtrl_AnimationQueue_field0x10(ChrCtrl_AnimationQueue_field0x10* to);

std::string print_ChrCtrl_AnimationQueueEntry(ChrCtrl_AnimationQueueEntry* to);
void copy_ChrCtrl_AnimationQueueEntry(ChrCtrl_AnimationQueueEntry* to, const ChrCtrl_AnimationQueueEntry* from, bool to_game);
ChrCtrl_AnimationQueueEntry* init_ChrCtrl_AnimationQueueEntry();
void free_ChrCtrl_AnimationQueueEntry(ChrCtrl_AnimationQueueEntry* to, bool freeself);

std::string print_AnimationQueue(AnimationQueue* to);
void copy_AnimationQueue(AnimationQueue* to, const AnimationQueue* from);
AnimationQueue* init_AnimationQueue();
void free_AnimationQueue(AnimationQueue* to);

std::string print_AnimationQueue_Entry(AnimationQueue_Entry* to);
void copy_AnimationQueue_Entry(AnimationQueue_Entry* to, const AnimationQueue_Entry* from);
AnimationQueue_Entry* init_AnimationQueue_Entry();
void free_AnimationQueue_Entry(AnimationQueue_Entry* to);

std::string print_AnimationQueue_Entry_sub1(AnimationQueue_Entry_sub1* to);
void copy_AnimationQueue_Entry_sub1(AnimationQueue_Entry_sub1* to, const AnimationQueue_Entry_sub1* from);
AnimationQueue_Entry_sub1* init_AnimationQueue_Entry_sub1();
void free_AnimationQueue_Entry_sub1(AnimationQueue_Entry_sub1* to, bool freeself);

std::string print_AnimationQueue_Entry_sub1_field0x10(AnimationQueue_Entry_sub1_field0x10* to);
void copy_AnimationQueue_Entry_sub1_field0x10(AnimationQueue_Entry_sub1_field0x10* to, const AnimationQueue_Entry_sub1_field0x10* from);

#endif
