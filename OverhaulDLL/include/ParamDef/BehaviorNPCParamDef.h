/*
	DARK SOULS OVERHAUL

	Contributors to this file:
		Sean Pesce	-	C++


	ParamDef/BehaviorNPCParamDef.h

	NPC Behavior Parameters from BehaviorParam.paramdef


	References:
		- (Author?) ParamDefs Sheet
		  https://docs.google.com/spreadsheets/d/1KukblWL61We64-gNIyaAShga9h8RTXYmyFs98eQhY4E/

		- Kushisushi's ParamDefs Sheet
		  https://docs.google.com/spreadsheets/d/1QX6i93H4_jGZ0So9mArDECnuv1lfkaJQhXOzkDuxdkE/edit#gid=2133439974

*/

#pragma once

#ifndef _DS1_OVERHAUL_NPC_BEHAVIOR_PARAM_DEF_FILE_H_
	#define _DS1_OVERHAUL_NPC_BEHAVIOR_PARAM_DEF_FILE_H_


#include "GameParamDef.h"


// NPC Behavior parameter
typedef struct BehaviorNpcParameter : public Param {

	int32_t
		variationId = 0, // It is used to calculate the ID for attack parameters. It is not used directly on the actual machine.
		behaviorJudgeId = 0; // It is used to calculate the ID for attack parameters. This ID matches the action judgment ID entered in TimeActEditor. It is not used directly on the actual machine.
	
	uint8_t
		ezStateBehaviorType_old = 0; // For ID calculation rule

	typedef uint8_t BEHAVIOR_REF_TYPE;
	BEHAVIOR_REF_TYPE
		refType = 0; // Specify not to mistake the reference ID.

	uint8_t
		pad0[2] = { 0, 0 }; // Padding

	int32_t
		refId = -1, // Attack power, jump tool, ID of special effect parameter, refType is used properly.
		sfxVariationId = 0, // Specify variation of SFX (Used to specify SFX in combination with ID of TimeActEditor)
		stamina = 0, // Set consumption amount of consumption stamina during action.
		mp = 0; // Set consumption MP amount during action.

	typedef uint8_t BEHAVIOR_CATEGORY;
	BEHAVIOR_CATEGORY
		category = 0; // Since there are effects (such as enchant weapons) whose parameters fluctuate with skills, magic, items, etc., set the action for each action so that the specified effect can correspond to the effect of "power up only weapon attacks" For varistors, etc. that do not require setting, set "none"
	
	uint8_t
		heroPoint = 0; // Set consumption amount of humanity consumption during behavior

	uint8_t
		pad1[2] = { 0, 0 }; // Padding

} BehaviorNpcParam;



// BehaviorNpc parameter definitions file
class BehaviorNpcParamDef : public GameParamDef {


public:
	static BehaviorNpcParamDef& get_instance()
	{
		static BehaviorNpcParamDef instance;
		return instance;
	}
	
	BehaviorNpcParam *data()
	{
		return (BehaviorNpcParam*)GameParamDef::data();
	}

	BehaviorNpcParam *get(int index)
	{
		return (BehaviorNpcParam*)GameParamDef::get(index);
	}


	// @TODO: Member data specific to this class
	
private:
	BehaviorNpcParamDef()
		: GameParamDef(NULL, 0x70B0, 2400, sizeof(BehaviorNpcParam),
			"B0 9C 01 00 B0 70 02 00 02 00 60 09 42 45 48 41",
			"BehaviorNpcParam", "Behavior (NPC)")
	{
	}
	
public:
	BehaviorNpcParamDef(BehaviorNpcParamDef const&) = delete;
	void operator=(BehaviorNpcParamDef const&) = delete;
};







#endif // _DS1_OVERHAUL_NPC_BEHAVIOR_PARAM_DEF_FILE_H_
