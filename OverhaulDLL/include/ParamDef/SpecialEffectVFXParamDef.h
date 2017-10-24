/*
	DARK SOULS OVERHAUL

	Contributors to this file:
		Sean Pesce	-	C++


	ParamDef/SpEffectVfxParamDef.h

	Special Effect Visual Effects Parameters from SpEffectVfxParam.paramdef


	References:
		- (Author?) ParamDefs Sheet
		  https://docs.google.com/spreadsheets/d/1KukblWL61We64-gNIyaAShga9h8RTXYmyFs98eQhY4E/

		- Kushisushi's ParamDefs Sheet
		  https://docs.google.com/spreadsheets/d/1QX6i93H4_jGZ0So9mArDECnuv1lfkaJQhXOzkDuxdkE/edit#gid=2133439974

*/

#pragma once

#ifndef _DS1_OVERHAUL_SPECIAL_EFFECT_VFX_PARAM_DEF_FILE_H_
	#define _DS1_OVERHAUL_SPECIAL_EFFECT_VFX_PARAM_DEF_FILE_H_


#include "GameParamDef.h"


// SpEffectVfx parameter
typedef struct SpEffectVfxParameter : public Param {

	
	int32_t
		midstSfxId  = -1, // In effect SfxID (-1: invalid)
		midstSeId   = -1, // Effect SeID (-1: invalid)
		initSfxId   = -1, // SfxID at activation (-1: invalid)
		initSeId    = -1, // Activation SeID (-1: invalid)
		finishSfxId = -1, // Release time SfxID (-1: invalid)
		finishSeId  = -1; // When released SeID (-1: invalid)
	
	float
		camouflageBeginDist = -1, // Camouflage starting distance
		camouflageEndDist   = -1; // It is the camouflage end distance
	
	int32_t
		transformProtectorId = -1; // Transformation armor ID (-1: none)
	
	int16_t
		midstDmyId  = -1, // Effectiveness Damipoly ID (-1: root)
		initDmyId   = -1, // Damipoly ID at activation (-1: root)
		finishDmyId = -1; // When releasing Damipol ID (-1: root)
	
	uint8_t
		effectType = 0, // Effect type
		soulParamIdForWepEnchant = 0, // Weapon enchantment soulpalam ID (-1: none)
		playCategory = 0, // Controls effect playback due to overlap effect
		playPriority = 0; // Set playback priority when category matches (lower priority first)
	
	uint8_t
		existEffectForLarge: 1, // Is there a large-sized effect?
		existEffectForSoul: 1, // Is there an effect for the soul body
		effectInvisibleAtCamouflage: 1, // Whether to hide the effect when disguising
		useCamouflage: 1, // Do you hide it?
		invisibleAtFriendCamouflage: 1, // Does not hide ally when disguising
		addMapAreaBlockOffset: 1, // Add map number (AAB 0) to SfxID
		halfCamouflage: 1, // Semitransparent figure disguising?
		isFullBodyTransformProtectorId: 1, // Transformation armor ID for whole body
		isInvisibleWeapon: 1, // Invisible weapon for weapon enchantment (0: weapon display, 1: no weapon)
		isSilence: 1; // Silence? (0: different, 1: so)
	
	uint8_t
		pad_1: 6; // Padding
	
	uint8_t
		pad[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Padding

} SpEffectVfxParam;



// SpEffectVfx parameter definitions file
class SpEffectVfxParamDef : public GameParamDef {


public:
	static SpEffectVfxParamDef& get_instance()
	{
		static SpEffectVfxParamDef instance;
		return instance;
	}
	
	SpEffectVfxParam *data()
	{
		return (SpEffectVfxParam*)GameParamDef::data();
	}

	SpEffectVfxParam *get(int index)
	{
		return (SpEffectVfxParam*)GameParamDef::get(index);
	}


	// @TODO: Member data specific to this class
	
private:
	SpEffectVfxParamDef()
		: GameParamDef(NULL, 0x18, 232, sizeof(SpEffectVfxParam), "F1 00 00 00 90 44 00 00 D2 51 00 00 F2 00 00 00 D0 44 00 00 E2 51 00 00", "SpEffectVfxParam", "Special Effect Visual Effects")
	{
	}
	
public:
	SpEffectVfxParamDef(SpEffectVfxParamDef const&) = delete;
	void operator=(SpEffectVfxParamDef const&) = delete;
};







#endif // _DS1_OVERHAUL_SPECIAL_EFFECT_VFX_PARAM_DEF_FILE_H_
