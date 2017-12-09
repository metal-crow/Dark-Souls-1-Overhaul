/*
	DARK SOULS GAME FILE C++ LIBRARY

	Contributors to this file:
		Sean Pesce	-	C++


	Param/NpcThink.h

	NPC Thought Parameters from NpcThinkParam.paramdef


	References:
		- Burton Radons' ParamDefs Sheet
		  https://docs.google.com/spreadsheets/d/1KukblWL61We64-gNIyaAShga9h8RTXYmyFs98eQhY4E/

		- Kushisushi's ParamDefs Sheet
		  https://docs.google.com/spreadsheets/d/1QX6i93H4_jGZ0So9mArDECnuv1lfkaJQhXOzkDuxdkE/edit#gid=2133439974

*/

#pragma once

#ifndef _DS1_FILE_LIB_NPC_THINK_PARAM_DEF_FILE_H_
	#define _DS1_FILE_LIB_NPC_THINK_PARAM_DEF_FILE_H_


#include "Param.h"


// NpcThink parameter
typedef struct NpcThinkParameter : public Param {

	int32_t
		logicId = -1, // Set the ID of the logic created by the script.
		battleGoalID = -1; // Battle goal ID
	
	float
		nearDist = 0, // It is a range regarded as a short distance. It is used to judge proximity fighting.
		midDist = 0, // It is a range regarded as medium distance. It is used for judging middle fight fighting.
		farDist = 0, // It is a range regarded as a long distance. It is used to judge the jump tool.
		outDist = 0, // It is the distance considered to be outside the battle range. We will not attempt to fight against enemies outside the range.
		BackHomeLife_OnHitEneWal = 5; // When touching an enemy wall blocking a block, the life of BackToHome goal
	
	int32_t
		goalID_ToCaution = 0, // Goal: Alert status
		idAttackCannotMove = 0, // If the movement is stopped by a destructible object, the action to be executed automatically.
		goalID_ToFind = 0, // Goal: discovery state
		callHelp_ActionAnimId = -1, // Anime ID when responding (EzStateAnimID)
		callHelp_CallActionId = -1; // Action ID when calling friends (EzStateAnimID)
	
	uint16_t
		eye_dist = 0, // Visual search range.
		ear_dist = 0, // Search range by hearing ..
		ear_soundcut_dist = 0, // Distance to reduce the size of the sound source. You will lose the sound below this distance.
		nose_dist = 0, // Search range due to olfaction.
		maxBackhomeDist = 0, // COMMON_SetBattleActLogic argument
		backhomeDist = 0, // COMMON_SetBattleActLogic argument
		backhomeBattleDist = 0, // COMMON_SetBattleActLogic argument
		nonBattleActLife = 0, // COMMON_SetBattleActLogic argument
		BackHome_LookTargetTime = 3, // When going home: time watching the target [sec]
		BackHome_LookTargetDist = 10, // When coming home: the distance watching the target [m]
		SightTargetForgetTime = 600, // Visual target The time to forget. It is input with a frame.
		SoundTargetForgetTime = 150, // Time to forget sound target. It is input with a frame.
		BattleStartDist = 0,
		callHelp_MyPeerId = 0, // My fellow group ID
		callHelp_CallPeerId = 0, // Fellow group ID to be called as a friend
		targetSys_DmgEffectRate = 100; // Damage impact rate acquisition (target system evaluation information)
	
	uint8_t
		TeamAttackEffectivity = 25, // Value to decide the number of attacks in the team simultaneously. Increasing the value reduces the number of attack participants at the same time.
		eye_angX = 0, // Visual search range.
		eye_angY = 0, // Visual search range.
		ear_angX = 90, // Searching range by hearing.
		ear_angY = 180, // Searching range by hearing.
		callHelp_CallValidMinDistTarget = 5, // If it is nearer than this value it can not be called by a friend.
		callHelp_CallValidRange = 15, // We do not call if this person is far from this value.
		callHelp_ForgetTimeByArrival = 0, // Time to respond
		callHelp_MinWaitTime = 0, // Minimum time on the first wait goal of the response goal [101 -> 10.1 sec]
		callHelp_MaxWaitTime = 0, // Maximum time on the first wait goal of the response goal [101 -> 10.1 sec]
		goalAction_ToCaution = 0, // Goal action: Target is in alert state
		goalAction_ToFind = 0, // Goal action: Target is in discovery state
		callHelp_ReplyBehaviorType = 0, // After response, the action type to the target position
		disablePathMove = 0, // Whether to move directly without following the path even if the path move command comes
		skipArrivalVisibleCheck = 0, // Do you want to skip the arrival determination by gaze? When it is set to On, the arrival judgment is made even if the line of sight does not pass.
		thinkAttr_doAdmirer = 0; // Thinking Attributes: If ON, play a winding role.
	
	uint8_t
		enableNaviFlg_Edge : 1, // Can you pass the node "cliff"? (Def: 1)
		enableNaviFlg_LargeSpace : 1, // Can we pass the node "wide"? (Def: 1)
		enableNaviFlg_Ladder : 1, // Can you pass the node "ladder"? (Def: 0)
		enableNaviFlg_Hole : 1, // Can you pass through the node "hole"? (Def: 0)
		enableNaviFlg_Door : 1, // Can you pass the node "door"? (Def: 0)
		enableNaviFlg_InSideWall : 1; // Can you pass through the node "In the Wall"? (Def: 0)
	
	uint8_t
		enableNaviFlg_reserve0 : 2; // If you need a new flag you can put it here (Not padding)
	
	uint8_t
		enableNaviFlg_reserve1[3] = { 0, 0, 0 }; // If you need a new flag you can put it here (Not padding)
	
	uint8_t
		pad0[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Padding

} NpcThinkParam;



// NpcThink parameter definitions file
class NpcThinkParamDef : public ParamDef {


public:
	static NpcThinkParamDef& get_instance()
	{
		static NpcThinkParamDef instance;
		return instance;
	}
	
	NpcThinkParam *data()
	{
		return (NpcThinkParam*)ParamDef::data();
	}

	NpcThinkParam *get(int index)
	{
		return (NpcThinkParam*)ParamDef::get(index);
	}


	// @TODO: Member data specific to this class
	
private:
	NpcThinkParamDef()
		: ParamDef(NULL, /*0x1680, 476,*/ sizeof(NpcThinkParam),
			"C0 E6 00 00 ?? ?? 01 00 01 00 ?? ?? 4E 50 43 5F",
			"NpcThinkParam", "NPC Thought")
	{
	}
	
public:
	NpcThinkParamDef(NpcThinkParamDef const&) = delete;
	void operator=(NpcThinkParamDef const&) = delete;
};







#endif // _DS1_FILE_LIB_NPC_THINK_PARAM_DEF_FILE_H_
