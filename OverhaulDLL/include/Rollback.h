#pragma once
#ifndef ROLLBACK_H
#define ROLLBACK_H

#include <stdint.h>
#include "PlayerInsStruct.h"
#include <unordered_map>

class Rollback
{
public:
    static void start();
    static void save();
    static void load();

    static bool bsave;
    static bool bload;

private:
    static PlayerIns* saved_playerins;
    static const uint64_t disableType18PacketEnforcement = 0x35b13b;
    static const uint64_t disableType18PacketSending = 0x5067f1;
};

void copy_PlayerIns(PlayerIns* to, PlayerIns* from, bool to_game);
PlayerIns* init_PlayerIns();
void copy_ChrAsm(ChrAsm* to, ChrAsm* from);
ChrAsm* init_ChrAsm();
void copy_ProEquipCtrl(ProEquipCtrl* to, ProEquipCtrl* from, bool to_game);
ProEquipCtrl* init_ProEquipCtrl();
void copy_WeaponEquipCtrl(WeaponEquipCtrl* to, WeaponEquipCtrl* from, bool to_game);
WeaponEquipCtrl* init_WeaponEquipCtrl();
void copy_RingEquipCtrl(RingEquipCtrl* to, RingEquipCtrl* from, bool to_game);
RingEquipCtrl* init_RingEquipCtrl();
void copy_PlayerGameData(PlayerGameData* to, PlayerGameData* from);
PlayerGameData* init_PlayerGameData();
void copy_PlayerGameData_ChrProperties(PlayerGameData_ChrProperties* to, PlayerGameData_ChrProperties* from);
void copy_EquipGameData(EquipGameData* to, EquipGameData* from);
EquipGameData* init_EquipGameData();
void copy_EquipItemData(EquipItemData* to, EquipItemData* from);
void copy_EquipMagicData(EquipMagicData* to, EquipMagicData* from);
EquipMagicData* init_EquipMagicData();
void copy_PlayerGameData_AttributeInfo(PlayerGameData_AttributeInfo* to, PlayerGameData_AttributeInfo* from);
void copy_ChrIns(ChrIns* to, ChrIns* from, bool to_game);
ChrIns* init_ChrIns();
void copy_EntityThrowAnimationStatus(EntityThrowAnimationStatus* to, EntityThrowAnimationStatus* from);
EntityThrowAnimationStatus* init_EntityThrowAnimationStatus();
void copy_QwcSpEffectEquipCtrl(QwcSpEffectEquipCtrl* to, QwcSpEffectEquipCtrl* from);
QwcSpEffectEquipCtrl* init_QwcSpEffectEquipCtrl();
void copy_SpecialEffect(SpecialEffect* to, SpecialEffect* from, bool to_game);
SpecialEffect* init_SpecialEffect();
void copy_SpecialEffect_Info(SpecialEffect_Info* to, SpecialEffect_Info* from, bool to_game);
SpecialEffect_Info* init_SpecialEffect_Info();
void copy_PadManipulator(PadManipulator* to, PadManipulator* from);
PadManipulator* init_PadManipulator();
void copy_NetworkManipulator(NetworkManipulator* to, NetworkManipulator* from);
void copy_ChrManipulator(ChrManipulator* to, ChrManipulator* from);
void copy_PlayerCtrl(PlayerCtrl* to, PlayerCtrl* from, bool to_game);
PlayerCtrl* init_PlayerCtrl();
void copy_ArrowTurnAnim(ArrowTurnAnim* to, ArrowTurnAnim* from);
ArrowTurnAnim* init_ArrowTurnAnim();
void copy_SpinJoint(SpinJoint* to, SpinJoint* from);
SpinJoint* init_SpinJoint();
void copy_TurnAnim(TurnAnim* to, TurnAnim* from);
TurnAnim* init_TurnAnim();
void copy_ChrCtrl(ChrCtrl* to, ChrCtrl* from, bool to_game);
ChrCtrl* init_ChrCtrl();
void copy_WalkAnim_Twist(WalkAnim_Twist* to, WalkAnim_Twist* from);
WalkAnim_Twist* init_WalkAnim_Twist();
void copy_WalkAnim_Twist_Field0x228Elem(WalkAnim_Twist_Field0x228Elem* to, WalkAnim_Twist_Field0x228Elem* from);
WalkAnim_Twist_Field0x228Elem* init_WalkAnim_Twist_Field0x228Elem();
void copy_ActionCtrl(ActionCtrl* to, ActionCtrl* from);
ActionCtrl* init_ActionCtrl();
void copy_ActionCtrl_0x30Substruct(ActionCtrl_0x30Substruct* to, ActionCtrl_0x30Substruct* from);
void copy_EzState_detail_EzStateMachineImpl(EzState_detail_EzStateMachineImpl* to, EzState_detail_EzStateMachineImpl* from);
EzState_detail_EzStateMachineImpl* init_EzState_detail_EzStateMachineImpl();
void copy_HavokChara(HavokChara* to, HavokChara* from);
HavokChara* init_HavokChara();
void copy_hkpCharacterProxy(hkpCharacterProxy* to, hkpCharacterProxy* from);
hkpCharacterProxy* init_hkpCharacterProxy();
void copy_AnimationMediator(AnimationMediator* to, AnimationMediator* from);
AnimationMediator* init_AnimationMediator();
void copy_AnimationMediatorStateEntry(AnimationMediatorStateEntry* to, AnimationMediatorStateEntry* from);
AnimationMediatorStateEntry* init_AnimationMediatorStateEntry();
void copy_ChrCtrl_AnimationQueue(ChrCtrl_AnimationQueue* to, ChrCtrl_AnimationQueue* from, bool to_game);
ChrCtrl_AnimationQueue* init_ChrCtrl_AnimationQueue();
void copy_ChrCtrl_AnimationQueue_field0x20(ChrCtrl_AnimationQueue_field0x20* to, ChrCtrl_AnimationQueue_field0x20* from, bool to_game);
ChrCtrl_AnimationQueue_field0x20* init_ChrCtrl_AnimationQueue_field0x20();
void copy_hkaAnimatedSkeleton(hkaAnimatedSkeleton* to, hkaAnimatedSkeleton* from);
hkaAnimatedSkeleton* init_hkaAnimatedSkeleton();
void copy_hkaDefaultAnimationControl(hkaDefaultAnimationControl* to, hkaDefaultAnimationControl* from);
hkaDefaultAnimationControl* init_hkaDefaultAnimationControl();
void copy_hkaAnimationControl(hkaAnimationControl* to, hkaAnimationControl* from);
hkaAnimationControl* init_hkaAnimationControl();
void copy_hkaAnimationBinding(hkaAnimationBinding* to, hkaAnimationBinding* from);
hkaAnimationBinding* init_hkaAnimationBinding();
ChrCtrl_AnimationQueue_field0x10* init_ChrCtrl_AnimationQueue_field0x10();
void copy_ChrCtrl_AnimationQueue_field0x10(ChrCtrl_AnimationQueue_field0x10* to, ChrCtrl_AnimationQueue_field0x10* from);
void copy_AnimationQueueEntry_AnimationInfo(AnimationQueueEntry_AnimationInfo* to, AnimationQueueEntry_AnimationInfo* from);
AnimationQueueEntry_AnimationInfo* init_AnimationQueueEntry_AnimationInfo();
void copy_ChrCtrl_AnimationQueueEntry(ChrCtrl_AnimationQueueEntry* to, ChrCtrl_AnimationQueueEntry* from, bool to_game);
ChrCtrl_AnimationQueueEntry* init_ChrCtrl_AnimationQueueEntry();
void copy_AnimationQueue(AnimationQueue* to, AnimationQueue* from);
AnimationQueue* init_AnimationQueue();
void copy_AnimationQueue_Entry(AnimationQueue_Entry* to, AnimationQueue_Entry* from);
AnimationQueue_Entry* init_AnimationQueue_Entry();

#endif
