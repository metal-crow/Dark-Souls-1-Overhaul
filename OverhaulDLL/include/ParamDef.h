/*
	DARK SOULS OVERHAUL

	Contributors to this file:
		Sean Pesce	-	C++


	ParamDef.h

	ParamDef namespace header, used to reference ParamDef classes
	more intuitively.

*/

#pragma once

#ifndef _DS1_OVERHAUL_PARAM_DEF_NAMESPACE_H_
	#define _DS1_OVERHAUL_PARAM_DEF_NAMESPACE_H_



#include "ParamDef/ArmorParamDef.h"
#include "ParamDef/LockCamParamDef.h"



namespace ParamDef {


	// Armor parameters
	static ArmorParamDef& Armor() { return ArmorParamDef::get_instance(); }
	//static ArmorParamDef& Protector() { return ArmorParamDef::get_instance(); } // Just another name for armor params
	//static ArmorParamDef& EquipProtector() { return ArmorParamDef::get_instance(); } // Just another name for Armor params

	// Camera lock paramters
	static LockCamParamDef& CamLock() { return LockCamParamDef::get_instance(); }


}



#endif _DS1_OVERHAUL_PARAM_DEF_NAMESPACE_H_