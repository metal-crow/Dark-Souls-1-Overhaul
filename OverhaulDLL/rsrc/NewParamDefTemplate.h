/*
	DARK SOULS OVERHAUL

	Contributors to this file:
		Sean Pesce	-	C++


	ParamDef/NewParamDef.h

	New Parameters from NewParam.paramdef


	References:
		- (Author?) ParamDefs Sheet
		  https://docs.google.com/spreadsheets/d/1KukblWL61We64-gNIyaAShga9h8RTXYmyFs98eQhY4E/

		- Kushisushi's ParamDefs Sheet
		  https://docs.google.com/spreadsheets/d/1QX6i93H4_jGZ0So9mArDECnuv1lfkaJQhXOzkDuxdkE/edit#gid=2133439974

*/

#pragma once

#ifndef _DS1_OVERHAUL_NEW_PARAM_DEF_EQUIP_PROTECTOR_H_
	#define _DS1_OVERHAUL_NEW_PARAM_DEF_EQUIP_PROTECTOR_H_


#include "BaseParamDef.h"


// New parameter
typedef struct NewParameter : public Param {
	@TODO
} NewParam;



// New parameter definitions file
class NewParamDef : public BaseParamDef {


public:
	static NewParamDef& get_instance()
	{
		static NewParamDef instance;
		return instance;
	}
	
	NewParam *data()
	{
		return (NewParam*)BaseParamDef::data();
	}

	NewParam *get(int index)
	{
		return (NewParam*)BaseParamDef::get(index);
	}


	// @TODO: Member data specific to this class
	
private:
	NewParamDef()
		: BaseParamDef(/* @TODO: base, data_start_offset, param_count, sizeof(NewParam), scan_pattern, file, title */)
	{
	}
	
public:
	NewParamDef(NewParamDef const&) = delete;
	void operator=(NewParamDef const&) = delete;
};







#endif // _DS1_OVERHAUL_NEW_PARAM_DEF_EQUIP_PROTECTOR_H_
