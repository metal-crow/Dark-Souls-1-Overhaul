/*
	DARK SOULS GAME FILE C++ LIBRARY

	Contributors to this file:
		Sean Pesce	-	C++


	Param/Throw.h

	Throw Parameters from ThrowParam.paramdef


	References:
		- Burton Radons' ParamDefs Sheet
		  https://docs.google.com/spreadsheets/d/1KukblWL61We64-gNIyaAShga9h8RTXYmyFs98eQhY4E/

		- Kushisushi's ParamDefs Sheet
		  https://docs.google.com/spreadsheets/d/1QX6i93H4_jGZ0So9mArDECnuv1lfkaJQhXOzkDuxdkE/edit#gid=2133439974

*/

#pragma once

#ifndef _DS1_FILE_LIB_THROW_PARAM_DEF_FILE_H_
	#define _DS1_FILE_LIB_THROW_PARAM_DEF_FILE_H_


#include "Param.h"


// Throw parameter
typedef struct ThrowParameter : public Param {

	int32_t
		AtkChrId = 0, // Throwing character character ID
		DefChrId = 0; // Receiver character ID
	
	float
		Dist = 0, // I can not throw it unless it is closer than this value [m]
		DiffAngMin = 0, // You can not throw it unless the angle difference (Y axis) between the throwing side and the receiving side is larger than this angle
		DiffAngMax = 0, // You can not throw it unless the angle difference (Y axis) between the throwing side and the receiving side is smaller than this angle
		upperYRange = 0.2f, // You can not throw it unless the relative distance of the Y axis on the receiving side to the throwing side is smaller than this value
		lowerYRange = 0.2f, // You can not throw it unless the relative distance of the Y axis on the receiving side to the throwing side is smaller than this value
		diffAngMyToDef = 60; // Angle difference between the vector in front of you and the vector in the direction from yourself to your opponent. If it is larger than this value you can not throw
	
	int32_t
		throwTypeId = 0, // ID identifying the type of throw (associated with attack parameters)
		atkAnimId = 0, // Set Attack Animation ID (Linked with EzState)
		defAnimId = 0; // Set damage animation ID (linked with EzState)

	uint16_t
		escHp = 0, // Value to withstand throwing
		selfEscCycleTime = 0, // Cycle time of self-throwing missing [ms]
		sphereCastRadiusRateTop = 80, // The ratio of the upper radius of the sphere cast [80 -> 0.8]
		sphereCastRadiusRateLow = 80; // The ratio of the lower radius of the sphere cast [80 -> 0.8]

	uint8_t
		PadType = 1, // Operation type
		AtkEnableState = 0, // Please set the state type that thrower can throw
		atkSorbDmyId = 0, // Where on the throw side should the receiving side be adsorbed?
		defSorbDmyId = 0, // Where on the receiving side will you pick up the throw side?
		throwType = 0, // Type of throw
		selfEscCycleCnt = 0, // Number of cycles of self-throwing missing
		dmyHasChrDirType = 0; // Orientation of the character possessed by Damipoli at the time of throwing

	uint8_t
		isTurnAtker: 1, // Will the throwing side turn?
		isSkipWepCate: 1, // Do you want to skip attack side weapon category check?
		isSkipSphereCast: 1, // Do you want to skip Sphere Cast?
		pad0: 5; // Padding

	uint8_t
		pad1[4] = { 0, 0, 0, 0 }; // Padding

} ThrowParam;



// Throw parameter definitions file
class ThrowParamDef : public ParamDef {


public:
	static ThrowParamDef& get_instance()
	{
		static ThrowParamDef instance;
		return instance;
	}
	
	ThrowParam *data()
	{
		return (ThrowParam*)ParamDef::data();
	}

	ThrowParam *get(int index)
	{
		return (ThrowParam*)ParamDef::get(index);
	}


	// @TODO: Member data specific to this class
	
private:
	ThrowParamDef()
		: ParamDef(NULL, /*0x954, 195,*/ sizeof(ThrowParam),
			"20 3A 00 00 ?? ?? 00 00 01 00 ?? ?? 54 48 52 4F",
			"ThrowParam", "Throw")
	{
	}
	
public:
	ThrowParamDef(ThrowParamDef const&) = delete;
	void operator=(ThrowParamDef const&) = delete;
};







#endif // _DS1_FILE_LIB_THROW_PARAM_DEF_FILE_H_
