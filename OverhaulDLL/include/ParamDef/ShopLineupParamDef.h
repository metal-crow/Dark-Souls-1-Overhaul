/*
	DARK SOULS OVERHAUL

	Contributors to this file:
		Sean Pesce	-	C++


	ParamDef/ShopLineupParamDef.h

	ShopLineup Parameters from ShopLineupParam.paramdef


	References:
		- (Author?) ParamDefs Sheet
		  https://docs.google.com/spreadsheets/d/1KukblWL61We64-gNIyaAShga9h8RTXYmyFs98eQhY4E/

		- Kushisushi's ParamDefs Sheet
		  https://docs.google.com/spreadsheets/d/1QX6i93H4_jGZ0So9mArDECnuv1lfkaJQhXOzkDuxdkE/edit#gid=2133439974

*/

#pragma once

#ifndef _DS1_OVERHAUL_SHOP_LINEUP_PARAM_DEF_FILE_H_
	#define _DS1_OVERHAUL_SHOP_LINEUP_PARAM_DEF_FILE_H_


#include "GameParamDef.h"


// ShopLineup parameter
typedef struct ShopLineupParameter : public Param {

	int32_t
		equipId   =  0, // ID of the equipment being sold
		value     = -1, // Selling price
		mtrlId    = -1, // Material ID required for purchase
		eventFlag = -1, // The event flag value holding the number
		qwcId     = -1; // ID of QWC parameter

	int16_t
		sellQuantity = -1; // Number of items sold

	uint8_t
		shopType  = 0, // Shop type
		equipType = 0; // Types of equipment being sold

	uint8_t pad_0[8] = { 0, 0, 0, 0, 0, 0, 0, 0 }; // Padding

} ShopLineupParam;



// ShopLineup parameter definitions file
class ShopLineupParamDef : public GameParamDef {


public:
	static ShopLineupParamDef& get_instance()
	{
		static ShopLineupParamDef instance;
		return instance;
	}
	
	ShopLineupParam *data()
	{
		return (ShopLineupParam*)GameParamDef::data();
	}

	ShopLineupParam *get(int index)
	{
		return (ShopLineupParam*)GameParamDef::get(index);
	}


	// @TODO: Member data specific to this class
	
private:
	ShopLineupParamDef()
		: GameParamDef(NULL, 0x1284, 391, sizeof(ShopLineupParam),
			"70 43 00 00 84 12 01 00 01 00 87 01 53 48 4F 50",
			"ShopLineupParam", "Shop Lineup")
	{
	}
	
public:
	ShopLineupParamDef(ShopLineupParamDef const&) = delete;
	void operator=(ShopLineupParamDef const&) = delete;
};







#endif // _DS1_OVERHAUL_SHOP_LINEUP_PARAM_DEF_FILE_H_
