/*
	DARK SOULS OVERHAUL

	Contributors to this file:
		Sean Pesce	-	C++


	ParamDef/LockCamParamDef.h

	LockCam Parameters from LockCamParam.paramdef


	References:
		- (Author?) ParamDefs Sheet
		  https://docs.google.com/spreadsheets/d/1KukblWL61We64-gNIyaAShga9h8RTXYmyFs98eQhY4E/

		- Kushisushi's ParamDefs Sheet
		  https://docs.google.com/spreadsheets/d/1QX6i93H4_jGZ0So9mArDECnuv1lfkaJQhXOzkDuxdkE/edit#gid=2133439974

*/

#pragma once

#ifndef _DS1_OVERHAUL_LOCK_CAM_PARAM_DEF_EQUIP_PROTECTOR_H_
	#define _DS1_OVERHAUL_LOCK_CAM_PARAM_DEF_EQUIP_PROTECTOR_H_


#include "BaseParamDef.h"


// LockCam parameter
typedef struct LockCamParameter : public Param {
	float camDistTarget = 4.0f;
	float rotRangeMinX = -40.0f;
	float lockRotXShiftRatio = 0.6f;
	float chrOrgOffset_Y = 1.42f;
	float chrLockRangeMaxRadius = 15.0f;
	float camFovY = 43.0f;
	uint8_t pad[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
} LockCamParam;



// LockCam parameter definitions file
class LockCamParamDef : public BaseParamDef {


public:
	static LockCamParamDef& get_instance()
	{
		static LockCamParamDef instance;
		return instance;
	}
	
	LockCamParam *data()
	{
		return (LockCamParam*)BaseParamDef::data();
	}

	LockCamParam *get(int index)
	{
		return (LockCamParam*)BaseParamDef::get(index);
	}


	// @TODO: Member data specific to this class
	
private:
	LockCamParamDef()
		: BaseParamDef(NULL, 0x20, 38, sizeof(LockCamParam),
			"58 06 00 00 25 09 00 00 14 27 00 00 78 06 00 00 "
			"30 09 00 00 15 27 00 00 98 06 00 00 45 09 00 00 "
			"00 00 80 40 00 00 20 C2 9A 99 19 3F 8F C2 B5 3F "
			"00 00 70 41 00 00 2C 42 00 00 00 00 00 00 00 00",
			"LockCam", "Camera lock")
	{
	}
	
public:
	LockCamParamDef(LockCamParamDef const&) = delete;
	void operator=(LockCamParamDef const&) = delete;
};







#endif // _DS1_OVERHAUL_LOCK_CAM_PARAM_DEF_EQUIP_PROTECTOR_H_
