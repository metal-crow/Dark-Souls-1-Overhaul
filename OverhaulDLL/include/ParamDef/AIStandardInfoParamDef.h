/*
	DARK SOULS OVERHAUL

	Contributors to this file:
		Sean Pesce	-	C++


	ParamDef/AiStandardInfoParamDef.h

	Default AI Parameters from default_AiStandardInfoParam.paramdef


	References:
		- (Author?) ParamDefs Sheet
		  https://docs.google.com/spreadsheets/d/1KukblWL61We64-gNIyaAShga9h8RTXYmyFs98eQhY4E/

		- Kushisushi's ParamDefs Sheet
		  https://docs.google.com/spreadsheets/d/1QX6i93H4_jGZ0So9mArDECnuv1lfkaJQhXOzkDuxdkE/edit#gid=2133439974

*/

#pragma once

#ifndef _DS1_OVERHAUL_AI_STANDARD_INFO_PARAM_DEF_FILE_H_
	#define _DS1_OVERHAUL_AI_STANDARD_INFO_PARAM_DEF_FILE_H_


#include "GameParamDef.h"


// AI Standard Info parameter
typedef struct AiStandardInfoParameter : public Param {

	uint16_t
		RadarRange = 20; // Distance to recognize enemy character
	
	uint8_t
		RadarAngleX = 30, // X angle to recognize the enemy character with the current line of sight direction set to 0 degree, the top is.
		RadarAngleY = 60; // Y angle to recognize the enemy character with the current line of sight direction set to 0 degree, the right side is is.
	
	uint16_t
		TerritorySize = 20; // The distance of my territory. When the recognized player is out of this distance, it returns to the initial position.
	
	uint8_t
		ThreatBeforeAttackRate = 50, // Probability to threaten before attack
		ForceThreatOnFirstLocked = 0; // Whether to intimidate when first player recognition
	
	uint8_t
		reserve0[24] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	
	uint16_t
		Attack1_Distance = 0, // Interruption when attacking [m]
		Attack1_Margin = 0; // Play of attacking intercession. In the vicinity of distance, so as not to vibrate
	
	uint8_t
		Attack1_Rate = 50, // Frequency of attack
		Attack1_ActionID = 0, // Type of attack
		Attack1_DelayMin = 0, // The minimum delay time from the time it is possible to attack until attack.
		Attack1_DelayMax = 0, // The longest delay time from when it becomes available to attack until attack.
		Attack1_ConeAngle = 30; // Attack OK if the angle between the line of sight direction and the direction vector to the target is within this angle.
	
	uint8_t
		reserve10[7] = { 0, 0, 0, 0, 0, 0, 0 };
	
	uint16_t
		Attack2_Distance = 0, // Interruption when attacking [m]
		Attack2_Margin = 0; // Play of attacking intercession. In the vicinity of distance, so as not to vibrate
	
	uint8_t
		Attack2_Rate = 50, // Frequency of attack
		Attack2_ActionID = 0, // Type of attack
		Attack2_DelayMin = 0, // The minimum delay time from the time it is possible to attack until attack.
		Attack2_DelayMax = 0, // The longest delay time from when it becomes available to attack until attack.
		Attack2_ConeAngle = 30; // Attack OK if the angle between the line of sight direction and the direction vector to the target is within this angle.
	
	uint8_t
		reserve11[7] = { 0, 0, 0, 0, 0, 0, 0 };
	
	uint16_t
		Attack3_Distance = 0, // Interruption when attacking [m]
		Attack3_Margin = 0; // Play of attacking intercession. In the vicinity of distance, so as not to vibrate
	
	uint8_t
		Attack3_Rate = 50, // Frequency of attack
		Attack3_ActionID = 0, // Type of attack
		Attack3_DelayMin = 0, // The minimum delay time from the time it is possible to attack until attack.
		Attack3_DelayMax = 0, // The longest delay time from when it becomes available to attack until attack.
		Attack3_ConeAngle = 30; // Attack OK if the angle between the line of sight direction and the direction vector to the target is within this angle.
	
	uint8_t
		reserve12[7] = { 0, 0, 0, 0, 0, 0, 0 };
	
	uint16_t
		Attack4_Distance = 0, // Interruption when attacking [m]
		Attack4_Margin = 0; // Play of attacking intercession. In the vicinity of distance, so as not to vibrate
	
	uint8_t
		Attack4_Rate = 50, // Frequency of attack
		Attack4_ActionID = 0, // Type of attack
		Attack4_DelayMin = 0, // The minimum delay time from the time it is possible to attack until attack.
		Attack4_DelayMax = 0, // The longest delay time from when it becomes available to attack until attack.
		Attack4_ConeAngle = 30; // Attack OK if the angle between the line of sight direction and the direction vector to the target is within this angle.

	uint8_t
		reserve13[7] = { 0, 0, 0, 0, 0, 0, 0 };

	uint8_t
		reserve_last[32] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

} AiStandardInfoParam;



// AiStandardInfo parameter definitions file
class AiStandardInfoParamDef : public GameParamDef {


public:
	static AiStandardInfoParamDef& get_instance()
	{
		static AiStandardInfoParamDef instance;
		return instance;
	}
	
	AiStandardInfoParam *data()
	{
		return (AiStandardInfoParam*)GameParamDef::data();
	}

	AiStandardInfoParam *get(int index)
	{
		return (AiStandardInfoParam*)GameParamDef::get(index);
	}


	// @TODO: Member data specific to this class
	
private:
	AiStandardInfoParamDef()
		: GameParamDef(NULL, 0x230, 40, sizeof(AiStandardInfoParam),
			"30 16 00 00 30 02 01 00 01 00 28 00 41 49 5F 53",
			"default_AiStandardInfoParam", "Default AI")
	{
	}
	
public:
	AiStandardInfoParamDef(AiStandardInfoParamDef const&) = delete;
	void operator=(AiStandardInfoParamDef const&) = delete;
};







#endif // _DS1_OVERHAUL_AI_STANDARD_INFO_PARAM_DEF_FILE_H_
