/*
	DARK SOULS OVERHAUL

	Contributors to this file:
		Sean Pesce	-	C++


	ParamDef/ItemLotParamDef.h

	ItemLot Parameters from ItemLotParam.paramdef


	References:
		- (Author?) ParamDefs Sheet
		  https://docs.google.com/spreadsheets/d/1KukblWL61We64-gNIyaAShga9h8RTXYmyFs98eQhY4E/

		- Kushisushi's ParamDefs Sheet
		  https://docs.google.com/spreadsheets/d/1QX6i93H4_jGZ0So9mArDECnuv1lfkaJQhXOzkDuxdkE/edit#gid=2133439974

*/

#pragma once

#ifndef _DS1_OVERHAUL_ITEM_LOT_PARAM_DEF_FILE_H_
	#define _DS1_OVERHAUL_ITEM_LOT_PARAM_DEF_FILE_H_


#include "GameParamDef.h"
#include "GameEnum.h"

// ItemLot parameter
typedef struct ItemLotParameter : public Param {

	int32_t
		// ID of the item that can be retrieved
		lotItemId01 = 0,
		lotItemId02 = 0,
		lotItemId03 = 0,
		lotItemId04 = 0,
		lotItemId05 = 0,
		lotItemId06 = 0,
		lotItemId07 = 0,
		lotItemId08 = 0;

	ITEMLOT_ITEMCATEGORY // Type: int32_t
		// Category of items that can be retrieved
		lotItemCategory01 = ITEMLOT_ITEMCATEGORY::Weapon, // ITEMLOT_ITEMCATEGORY::Weapon = 0
		lotItemCategory02 = ITEMLOT_ITEMCATEGORY::Weapon,
		lotItemCategory03 = ITEMLOT_ITEMCATEGORY::Weapon,
		lotItemCategory04 = ITEMLOT_ITEMCATEGORY::Weapon,
		lotItemCategory05 = ITEMLOT_ITEMCATEGORY::Weapon,
		lotItemCategory06 = ITEMLOT_ITEMCATEGORY::Weapon,
		lotItemCategory07 = ITEMLOT_ITEMCATEGORY::Weapon,
		lotItemCategory08 = ITEMLOT_ITEMCATEGORY::Weapon;

	uint16_t
		// Basic appearance point:
		// Emergence point at the normal time
		lotItemBasePoint01 = 0,
		lotItemBasePoint02 = 0,
		lotItemBasePoint03 = 0,
		lotItemBasePoint04 = 0,
		lotItemBasePoint05 = 0,
		lotItemBasePoint06 = 0,
		lotItemBasePoint07 = 0,
		lotItemBasePoint08 = 0,

		// The cumulative post- emergence point:
		// Appearance point of maximum cumulative time
		cumulateLotPoint01 = 0,
		cumulateLotPoint02 = 0,
		cumulateLotPoint03 = 0,
		cumulateLotPoint04 = 0,
		cumulateLotPoint05 = 0,
		cumulateLotPoint06 = 0,
		cumulateLotPoint07 = 0,
		cumulateLotPoint08 = 0;

	int32_t
		// Another crunch flag ID
		// Crunch frame shared with the acquired flag (0 : common usage)
		getItemFlagId01   = 0,
		getItemFlagId02   = 0,
		getItemFlagId03   = 0,
		getItemFlagId04   = 0,
		getItemFlagId05   = 0,
		getItemFlagId06   = 0,
		getItemFlagId07   = 0,
		getItemFlagId08   = 0,

		// Crunch flag ID
		// Crunch frame shared with the acquired flag ( < 0 : invalid flag )
		getItemFlagId     = 0,
		cumulateNumFlagId = 0; // Lottery cumulative save flag ID: Lottery number storage (8 flag sequence number used)

	uint8_t
		cumulateNumMax = 0, // Lottery cumulative maximum number: Lottery cumulative maximum number (0: non-accumulative )
		lotItem_Rarity = 0, // Rarity setting: I will be identified as such the treasure chest , valuable items whether on how much
		
		// Number of items that can be retrieved
		lotItemNum01   = 0,
		lotItemNum02   = 0,
		lotItemNum03   = 0,
		lotItemNum04   = 0,
		lotItemNum05   = 0,
		lotItemNum06   = 0,
		lotItemNum07   = 0,
		lotItemNum08   = 0;


	typedef uint16_t ITEMLOT_ENABLE_LUCK; // As defined in ItemLotParam.paramdef
	ITEMLOT_ENABLE_LUCK
		// Luck parameter validation: Or to reflect the luck of the player the probability of lottery
		enableLuck01 : 1,
		enableLuck02 : 1,
		enableLuck03 : 1,
		enableLuck04 : 1,
		enableLuck05 : 1,
		enableLuck06 : 1,
		enableLuck07 : 1,
		enableLuck08 : 1;


	typedef uint16_t ITEMLOT_CUMULATE_RESET; // As defined in ItemLotParam.paramdef
	ITEMLOT_CUMULATE_RESET
		// Cumulative reset: Or reset the cumulative
		cumulateReset01: 1,
		cumulateReset02: 1,
		cumulateReset03: 1,
		cumulateReset04: 1,
		cumulateReset05: 1,
		cumulateReset06: 1,
		cumulateReset07: 1,
		cumulateReset08: 1;

} ItemLotParam;



// ItemLot parameter definitions file
class ItemLotParamDef : public GameParamDef {


public:
	static ItemLotParamDef& get_instance()
	{
		static ItemLotParamDef instance;
		return instance;
	}
	
	ItemLotParam *data()
	{
		return (ItemLotParam*)GameParamDef::data();
	}

	ItemLotParam *get(int index)
	{
		return (ItemLotParam*)GameParamDef::get(index);
	}


	// @TODO: Member data specific to this class
	
private:
	ItemLotParamDef()
		: GameParamDef(NULL, 0x47A0, 1524, sizeof(ItemLotParam),
			"B0 B8 03 00 A0 47 00 00 02 00 F4 05 49 54 45 4D",
			"ItemLotParam", "Item Lot")
	{
	}
	
public:
	ItemLotParamDef(ItemLotParamDef const&) = delete;
	void operator=(ItemLotParamDef const&) = delete;
};







#endif // _DS1_OVERHAUL_ITEM_LOT_PARAM_DEF_FILE_H_
