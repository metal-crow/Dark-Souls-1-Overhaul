/*
	DARK SOULS OVERHAUL

	Contributors to this file:
		Sean Pesce	-	C++


	ParamDef/MaterialSetParamDef.h

	Material Set Parameters from EquipMtrlSetParam.paramdef


	References:
		- (Author?) ParamDefs Sheet
		  https://docs.google.com/spreadsheets/d/1KukblWL61We64-gNIyaAShga9h8RTXYmyFs98eQhY4E/

		- Kushisushi's ParamDefs Sheet
		  https://docs.google.com/spreadsheets/d/1QX6i93H4_jGZ0So9mArDECnuv1lfkaJQhXOzkDuxdkE/edit#gid=2133439974

*/

#pragma once

#ifndef _DS1_OVERHAUL_EQUP_MATERIAL_SET_PARAM_DEF_FILE_H_
	#define _DS1_OVERHAUL_EQUP_MATERIAL_SET_PARAM_DEF_FILE_H_


#include "GameParamDef.h"


// MaterialSet parameter
typedef struct MaterialSetParameter : public Param {

	int32_t
		materialId01 = -1,
		materialId02 = -1,
		materialId03 = -1,
		materialId04 = -1,
		materialId05 = -1;

	int8_t
		itemNum01 = -1,
		itemNum02 = -1,
		itemNum03 = -1,
		itemNum04 = -1,
		itemNum05 = -1;

	uint8_t
		isDisableDispNum01: 1,
		isDisableDispNum02: 1,
		isDisableDispNum03: 1,
		isDisableDispNum04: 1,
		isDisableDispNum05: 1,
		pad0: 3;

	uint8_t
		pad[6] = { 0, 0, 0, 0, 0, 0 };

} MaterialSetParam;



// MaterialSet parameter definitions file
class MaterialSetParamDef : public GameParamDef {


public:
	static MaterialSetParamDef& get_instance()
	{
		static MaterialSetParamDef instance;
		return instance;
	}
	
	MaterialSetParam *data()
	{
		return (MaterialSetParam*)GameParamDef::data();
	}

	MaterialSetParam *get(int index)
	{
		return (MaterialSetParam*)GameParamDef::get(index);
	}


	// @TODO: Member data specific to this class
	
private:
	MaterialSetParamDef()
		: GameParamDef(NULL, 0x10, 276, sizeof(MaterialSetParam), "AA 39 00 00 2A 1C 00 00 80 2F 00 00 B3 39 00 00", "EquipMtrlSetParam", "Material Set")
	{
	}
	
public:
	MaterialSetParamDef(MaterialSetParamDef const&) = delete;
	void operator=(MaterialSetParamDef const&) = delete;
};







#endif // _DS1_OVERHAUL_EQUP_MATERIAL_SET_PARAM_DEF_FILE_H_
