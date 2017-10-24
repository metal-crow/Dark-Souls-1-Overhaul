/*
	DARK SOULS OVERHAUL

	Contributors to this file:
		Sean Pesce	-	C++


	ParamDef/MagicParamDef.h

	Magic Parameters from MagicParam.paramdef


	References:
		- (Author?) ParamDefs Sheet
		  https://docs.google.com/spreadsheets/d/1KukblWL61We64-gNIyaAShga9h8RTXYmyFs98eQhY4E/

		- Kushisushi's ParamDefs Sheet
		  https://docs.google.com/spreadsheets/d/1QX6i93H4_jGZ0So9mArDECnuv1lfkaJQhXOzkDuxdkE/edit#gid=2133439974

*/

#pragma once

#ifndef _DS1_OVERHAUL_MAGIC_PARAM_DEF_FILE_H_
	#define _DS1_OVERHAUL_MAGIC_PARAM_DEF_FILE_H_


#include "BaseParamDef.h"


// Magic parameter
typedef struct MagicParameter : public Param {

	int32_t
		yesNoDialogMessageId  =  0, // Issue when using magic Yes / No Message ID of dialog
		limitCancelSpEffectId = -1; // You can ignore usage restrictions when the specified special effect ID is activated

	int16_t
		sortId = 0, // Sort ID (-1: not collected)
		refId = -1, // ID called from magic
		mp = 0, // Consumption MP
		stamina = 0, // Consumption stamina
		iconId = 0, // Specify icon (For menu)
		behaviorId = 0, // Set action ID
		mtrlItemId = -1, // Item ID required for purchase
		replaceMagicId = -1, // ID to be replaced when state change matches (-1: invalid)
		maxQuantity = 0; // Number per piece (-1: infinite)

	uint8_t
		heroPoint     = 0, // Consumer humanity
		overDexterity = 0; // Skill over start value

	int8_t
		sfxVariationId = -1; // Specify variation of SFX (Used to specify SFX in combination with ID of TimeActEditor)

	uint8_t
		slotLength = 0, // Number of slots required for equipment (For menu)
		requirementIntellect = 0, // I can not equip it unless the PC's intelligence is no more
		requirementFaith = 0, // I can not equip it unless the PC's capacity is any more
		analogDexiterityMin = 0, // Motion cancellation analogization: minimum workmanship value
		analogDexiterityMax = 0, // Motion cancellation analogization: Highest skill value
		ezStateBehaviorType = 0, // Used for sort (For menu)
		refCategory = 0, // Category of ID of ? [attack, jumping tool, special]
		spEffectCategory = 0, // Since there are effects (such as enchant weapons) whose parameters fluctuate with skills, magic, items, etc., set the action for each action so that the specified effect can respond to the effect of "power up only weapon attacks" For varistors, etc. that do not require setting, set "none"
		refType = 0, // Specify motion (for EzState)
		opmeMenuType = 0, // Menu type to be put out when using magic
		hasSpEffectType = 0, // Designate state change that needs to replace magic ID
		replaceCategory = 0, // Additional conditions for replacing magic ID
		useLimitCategory = 0; // Specified to control whether it can be used by special effects

	uint8_t
		vowType0: 1, // Commitment 0
		vowType1: 1, // Commitment 1
		vowType2: 1, // Pledge 2
		vowType3: 1, // Pledge 3
		vowType4: 1, // Pledge 4
		vowType5: 1, // Pledge 5
		vowType6: 1, // Pledge 6
		vowType7: 1, // Pledge 7
		enable_multi: 1, // Can it be used in multi? It can be used for both single and multi
		enable_multi_only: 1, // Is it for multi only? I can not use it when I am single. It can be used when it is multi.
		isEnchant: 1, // Magic to enchant
		isShieldEnchant: 1, // Guard / shield Enchantment magic?
		enable_live: 1, // Can you use surviving characters
		enable_gray: 1, // Can gray characters be used?
		enable_white: 1, // Can white ghost characters be used?
		enable_black: 1, // Can black ghost characters be used?
		disableOffline: 1, // Not available offline
		castResonanceMagic: 1, // Do you distribute resonance magic
		pad_1: 6, // Padding
		vowType8: 1, // Commitment 8
		vowType9: 1, // Pledge 9
		vowType10: 1, // Pledge 10
		vowType11: 1, // Pledge 11
		vowType12: 1, // Pledge 12
		vowType13: 1, // Pledge 13
		vowType14: 1, // Pledge 14
		vowType15: 1; // Pledge 15

	uint8_t pad[2] = { 0, 0 }; // Padding

} MagicParam;



// Magic parameter definitions file
class MagicParamDef : public BaseParamDef {


public:
	static MagicParamDef& get_instance()
	{
		static MagicParamDef instance;
		return instance;
	}
	
	MagicParam *data()
	{
		return (MagicParam*)BaseParamDef::data();
	}

	MagicParam *get(int index)
	{
		return (MagicParam*)BaseParamDef::get(index);
	}


	// @TODO: Member data specific to this class
	
private:
	MagicParamDef()
		: BaseParamDef(NULL, 0x69C, 141, sizeof(MagicParam), "B8 0B 00 00 CC 06 00 00 3C 21 00 00 C2 0B 00 00", "MagicParam", "Magic")
	{
	}
	
public:
	MagicParamDef(MagicParamDef const&) = delete;
	void operator=(MagicParamDef const&) = delete;
};







#endif // _DS1_OVERHAUL_MAGIC_PARAM_DEF_FILE_H_
