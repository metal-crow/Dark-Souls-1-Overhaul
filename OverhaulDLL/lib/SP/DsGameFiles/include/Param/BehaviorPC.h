/*
	DARK SOULS GAME FILE C++ LIBRARY

	Contributors to this file:
		Sean Pesce	-	C++


	Param/BehaviorPC.h

	Player Character Behavior Parameters


	References:
		- Burton Radons' ParamDefs Sheet
		  https://docs.google.com/spreadsheets/d/1KukblWL61We64-gNIyaAShga9h8RTXYmyFs98eQhY4E/

		- Kushisushi's ParamDefs Sheet
		  https://docs.google.com/spreadsheets/d/1QX6i93H4_jGZ0So9mArDECnuv1lfkaJQhXOzkDuxdkE/edit#gid=2133439974

*/

#pragma once

#ifndef _DS1_FILE_LIB_PC_BEHAVIOR_PARAM_DEF_FILE_H_
	#define _DS1_FILE_LIB_PC_BEHAVIOR_PARAM_DEF_FILE_H_


#include "Param.h"


// PC Behavior parameter
typedef struct BehaviorPcParameter : public Param {

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

} BehaviorPcParam;



// BehaviorPc parameter definitions file
class BehaviorPcParamDef : public ParamDef {


public:
	static BehaviorPcParamDef& get_instance()
	{
		static BehaviorPcParamDef instance;
		return instance;
	}
	
	BehaviorPcParam *data()
	{
		return (BehaviorPcParam*)ParamDef::data();
	}

	BehaviorPcParam *get(int index)
	{
		return (BehaviorPcParam*)ParamDef::get(index);
	}


	// @TODO: Member data specific to this class
	
private:
	BehaviorPcParamDef()
		: ParamDef(NULL, /*0x9468, 3162,*/ sizeof(BehaviorPcParam),
			"B0 1F 02 00 ?? ?? 02 00 02 00 ?? ?? 42 45 48 41",
			"BehaviorPcParam", "Behavior (Player character)")
	{
	}
	
public:
	BehaviorPcParamDef(BehaviorPcParamDef const&) = delete;
	void operator=(BehaviorPcParamDef const&) = delete;
};







#endif // _DS1_FILE_LIB_PC_BEHAVIOR_PARAM_DEF_FILE_H_
