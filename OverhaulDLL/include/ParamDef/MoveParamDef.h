/*
	DARK SOULS OVERHAUL

	Contributors to this file:
		Sean Pesce	-	C++


	ParamDef/MoveParamDef.h

	Movement Parameters from MoveParam.paramdef


	References:
		- (Author?) ParamDefs Sheet
		  https://docs.google.com/spreadsheets/d/1KukblWL61We64-gNIyaAShga9h8RTXYmyFs98eQhY4E/

		- Kushisushi's ParamDefs Sheet
		  https://docs.google.com/spreadsheets/d/1QX6i93H4_jGZ0So9mArDECnuv1lfkaJQhXOzkDuxdkE/edit#gid=2133439974

*/

#pragma once

#ifndef _DS1_OVERHAUL_MOVE_PARAM_DEF_FILE_H_
	#define _DS1_OVERHAUL_MOVE_PARAM_DEF_FILE_H_


#include "GameParamDef.h"


// Move parameter
typedef struct MoveParameter : public Param {

	int32_t
		stayId = -1, // Wait (Idle)
		walkF  = -1, // Walk forward
		walkB  = -1, // Walk backwards
		walkL  = -1, // Walk left
		walkR  = -1, // Walking right
		dashF  = -1, // Run forward
		dashB  = -1, // Run backwards
		dashL  = -1, // Move left
		dashR  = -1, // Move right
		superDash = -1, // Dash move
		escapeF   = -1, // Before emergency aversion
		escapeB = -1, // After emergency avoidance
		escapeL = -1, // Emergency evasion left
		escapeR = -1, // Emergency evasion right
		turnL = -1, // 90 degree turning left
		trunR = -1, // 90 degree turning right
		largeTurnL = -1, // 180 degree turning left
		largeTurnR = -1, // 180 degree turning right
		stepMove = -1, // 180 degree turning right
		flyStay  = -1, // Flight wait
		flyWalkF = -1, // Advance flight
		flyWalkFL = -1, // Fly left forward. Low rotation
		flyWalkFR = -1, // Fly right advance. Low rotation
		flyWalkFL2 = -1, // Flying left forward 2. High rotation
		flyWalkFR2 = -1, // Fly right forward 2. High rotation
		flyDashF = -1, // High speed flight forward
		flyDashFL = -1, // High Speed ??Flight Left Advance. Low rotation
		flyDashFR = -1, // High Speed ??Flight Right Forward. Low rotation
		flyDashFL2 = -1, // High speed flying left forward 2. High rotation
		flyDashFR2 = -1, // High Speed ??Flight Right Forward 2. High rotation
		dashEscapeF = -1, // Before Dash Emergency Avoidance
		dashEscapeB = -1, // Dash after emergency evasion
		dashEscapeL = -1, // Dash Emergency Avoidance Left
		dashEscapeR = -1, // Dash Emergency Avoidance Right
		analogMoveParamId = -1; // Moving animation parameter ID used in moving animation blend

	uint8_t
		pad[4] = { 0, 0, 0, 0 }; // Padding

} MoveParam;



// Move parameter definitions file
class MoveParamDef : public GameParamDef {


public:
	static MoveParamDef& get_instance()
	{
		static MoveParamDef instance;
		return instance;
	}
	
	MoveParam *data()
	{
		return (MoveParam*)GameParamDef::data();
	}

	MoveParam *get(int index)
	{
		return (MoveParam*)GameParamDef::get(index);
	}


	// @TODO: Member data specific to this class
	
private:
	MoveParamDef()
		: GameParamDef(NULL, 0x20, 65, sizeof(MoveParam), "1C 26 00 00 F8 2B 00 00 79 00 00 00 AC 26 00 00 03 2C 00 00 84 03 00 00", "MoveParam", "Movement")
	{
	}
	
public:
	MoveParamDef(MoveParamDef const&) = delete;
	void operator=(MoveParamDef const&) = delete;
};







#endif // _DS1_OVERHAUL_MOVE_PARAM_DEF_FILE_H_
