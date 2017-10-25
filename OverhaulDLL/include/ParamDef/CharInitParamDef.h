/*
	DARK SOULS OVERHAUL

	Contributors to this file:
		Sean Pesce	-	C++


	ParamDef/CharInitParamDef.h

	CharInit Parameters from CharInitParam.paramdef


	References:
		- (Author?) ParamDefs Sheet
		  https://docs.google.com/spreadsheets/d/1KukblWL61We64-gNIyaAShga9h8RTXYmyFs98eQhY4E/

		- Kushisushi's ParamDefs Sheet
		  https://docs.google.com/spreadsheets/d/1QX6i93H4_jGZ0So9mArDECnuv1lfkaJQhXOzkDuxdkE/edit#gid=2133439974

*/

#pragma once

#ifndef _DS1_OVERHAUL_CHAR_INIT_PARAM_DEF_FILE_H_
	#define _DS1_OVERHAUL_CHAR_INIT_PARAM_DEF_FILE_H_


#include "GameParamDef.h"


// CharInit parameter
typedef struct CharInitParameter : public Param {
	
	float
		baseRec_mp = 0, // MP is the time to recover 1 point (the first decimal place)
		baseRec_sp = 0, // Time for stamina to recover 1 point (first decimal place)
		red_Falldam = 0; // Amount of damage reduction (%) (first decimal place) that can be cushion when the other characters are taken from above only

	int32_t
		soul = 0, // Soul quantity possessed early
		equip_Wep_Right = -1, // Equipment item weapon ID (right hand)
		equip_Subwep_Right = -1, // Weapon ID of equipment parameter (right reserve)
		equip_Wep_Left = -1, // Equipment item weapon ID (left hand)
		equip_Subwep_Left = -1, // Weapon ID of Equipment Parameter (left hand reserve)
		equip_Helm = -1, // Equipment item armor ID (head armor)
		equip_Armer = -1, // Equipment item armor ID (Fuselage armor)
		equip_Gaunt = -1, // Equipment item armor ID (Arm armor)
		equip_Leg = -1, // Equipment item armor ID (leg protector)
		equip_Arrow = -1, // Weapon ID of accessory parameter (arrow)
		equip_Bolt = -1, // Weapon ID (Volt) of Equipment Parameter
		equip_SubArrow = -1, // Weapon ID of Equipment Parameter (arrow spare)
		equip_SubBolt = -1, // Weapon ID of equipment parameter (Volt reserve)
		equip_Accessory01 = -1, // Equipment item decorations ID 01
		equip_Accessory02 = -1, // Equipment item decorative item ID 02
		equip_Accessory03 = -1, // Equipment item decorations ID 03
		equip_Accessory04 = -1, // Equipment item decorations ID 04
		equip_Accessory05 = -1, // Accessories Parameter Ornament ID 05
		equip_Skill_01 = -1, // Initial equipped skill ID 01
		equip_Skill_02 = -1, // Initial equipment skill ID 02
		equip_Skill_03 = -1, // Initial equipped skills ID 03
		equip_Spell_01 = -1, // Magic · miracle of initial arrangement ID 01
		equip_Spell_02 = -1, // Magic / miracle of initial arrangement ID 02
		equip_Spell_03 = -1, // Magic · miracle of initial arrangement ID 03
		equip_Spell_04 = -1, // Magic · miracle of initial arrangement ID 04
		equip_Spell_05 = -1, // Magic · miracle of initial arrangement ID 05
		equip_Spell_06 = -1, // Magic / miracle of initial arrangement ID 06
		equip_Spell_07 = -1, // Magic · miracle of initial arrangement ID 07
		item_01 = -1, // Initial possession item ID 01
		item_02 = -1, // Initial possession item ID 02
		item_03 = -1, // Initial possession item ID 03
		item_04 = -1, // Initial possession item ID 04
		item_05 = -1, // Initial possession item ID 05
		item_06 = -1, // Initial possession item ID 06
		item_07 = -1, // Initial possession item ID 07
		item_08 = -1, // Initial possession item ID 08
		item_09 = -1, // Initial possession item ID 09
		item_10 = -1, // Initial possession item ID 10
		npcPlayerFaceGenId = 0, // Facegen parameter ID used in NPC player. It is not used by regular players.
		npcPlayerThinkId = 0; // NPC thought parameter ID used by NPC player. It is not used by regular players.

	uint16_t 
		baseHp = 0, // Basic value of HP (Actually, it is corrected by calculation formula)
		baseMp = 0, // The basic value of MP (actually, it is corrected by calculation formula)
		baseSp = 0, // Basic value of stamina (Actually, it is corrected by calculation formula)
		arrowNum = 0, // Initial possession number of arrow
		boltNum = 0, // Initial possession number of bolt
		subArrowNum = 0, // Initial possession number of arrow
		subBoltNum = 0; // Initial possession number of bolt

	int16_t
		QWC_sb = 0, // Basic QWC value (Sword-Bow)
		QWC_mw = 0, // Basic QWC value (Mace-Wand)
		QWC_cd = 0, // Basic QWC value (understanding - diffusion)
		soulLv = 0; // Initial Lv

	uint8_t
		baseVit = 0, // Basic value of physical fitness
		baseWil = 0, // Basic value of spirit
		baseEnd = 0, // Basic value of robustness
		baseStr = 0, // Basic value of muscle strength
		baseDex = 0, // Basic value of agility
		baseMag = 0, // Basic value of magical power
		baseFai = 0, // Basic value of faith
		baseLuc = 0, // Basic value of luck
		baseHeroPoint = 0, // Basic value of human nature
		baseDurability = 0, // Basic value of durability
		itemNum_01 = 0, // Initial possession item number 01
		itemNum_02 = 0, // Item number of initial possession 02
		itemNum_03 = 0, // Initial possession item number 03
		itemNum_04 = 0, // Initial possession item number 04
		itemNum_05 = 0, // Item number of initial possession 05
		itemNum_06 = 0, // Initial possession item number 06
		itemNum_07 = 0, // Initial possession item number 07
		itemNum_08 = 0, // Item number of initial possession 08
		itemNum_09 = 0, // Initial possession item number 09
		itemNum_10 = 0; // Initial possessed item quantity 10

	int8_t
		bodyScaleHead = 0, // Change scale scale
		bodyScaleBreast = 0, // Change scale scale
		bodyScaleAbdomen = 0, // Change scale scale
		bodyScaleArm = 0, // Change scale scale
		bodyScaleLeg = 0, // Change scale scale
		gestureId0 = -1, // Gesture 0th (0 if you want to play gesture 0 of EzState)
		gestureId1 = -1, // Gesture 1st (0 if you want to play gesture 0 of EzState)
		gestureId2 = -1, // Second gesture (0 if you want to play EzState gesture 0)
		gestureId3 = -1, // Gesture third (0 if you want to play gesture 0 of EzState)
		gestureId4 = -1, // Gesture fourth (0 if you want to play gesture 0 of EzState)
		gestureId5 = -1, // Gesture fifth (0 if you want to play gesture 0 of EzState)
		gestureId6 = -1; // Gesture sixth (0 if you want to play gesture 0 of EzState)

	uint8_t
		npcPlayerType = 0, // NPC type used by NPC player. It is not used by regular players.
		npcPlayerDrawType = 0, // Drawing type used by NPC player. It is not used by regular players.
		npcPlayerSex = 0, // It is the sex used by NPC player. It does not reflect to normal players.
		vowType: 4, // Pledge type (None: 0)
		pad: 4; // Padding

	int8_t pad0[10] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }; // Padding

} CharInitParam;



// CharInit parameter definitions file
class CharInitParamDef : public GameParamDef {


public:
	static CharInitParamDef& get_instance()
	{
		static CharInitParamDef instance;
		return instance;
	}
	
	CharInitParam *data()
	{
		return (CharInitParam*)GameParamDef::data();
	}

	CharInitParam *get(int index)
	{
		return (CharInitParam*)GameParamDef::get(index);
	}


	// @TODO: Member data specific to this class
	
private:
	CharInitParamDef()
		: GameParamDef(NULL, 0xED0, 312, sizeof(CharInitParam),
			"50 33 01 00 D0 0E 00 00 01 00 38 01 43 48 41 52",
			"CharaInitParam", "Character Initialization")
	{
	}
	
public:
	CharInitParamDef(CharInitParamDef const&) = delete;
	void operator=(CharInitParamDef const&) = delete;
};







#endif // _DS1_OVERHAUL_CHAR_INIT_PARAM_DEF_FILE_H_
