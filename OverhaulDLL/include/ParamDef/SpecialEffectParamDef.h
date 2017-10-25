/*
	DARK SOULS OVERHAUL

	Contributors to this file:
		Sean Pesce	-	C++


	ParamDef/SpecialEffectParamDef.h

	Special Effect Parameters from SpEffectParam.paramdef


	References:
		- (Author?) ParamDefs Sheet
		  https://docs.google.com/spreadsheets/d/1KukblWL61We64-gNIyaAShga9h8RTXYmyFs98eQhY4E/

		- Kushisushi's ParamDefs Sheet
		  https://docs.google.com/spreadsheets/d/1QX6i93H4_jGZ0So9mArDECnuv1lfkaJQhXOzkDuxdkE/edit#gid=2133439974

*/

#pragma once

#ifndef _DS1_OVERHAUL_SPECIAL_EFFECT_PARAM_DEF_FILE_H_
	#define _DS1_OVERHAUL_SPECIAL_EFFECT_PARAM_DEF_FILE_H_


#include "GameParamDef.h"


// Special Effect parameter
typedef struct SpEffectParameter : public Param {

	int32_t
		iconId = -1; // Icon ID (When icon is -1, there is no need for icon)

	float
		conditionHp = -1, // Set what percentage of maxHP the rest HP will activate
		effectEndurance = 0, // Change duration / permanent at -1 / 1 and instantaneous for 1 time only
		motionInterval = 0, // Set how many seconds it will occur
		maxHpRate = 1, // Correct the maximum HP
		maxMpRate = 1, // Correct the maximum MP
		maxStaminaRate = 1, // Correct the maximum SP
		slashDamageCutRate = 1, // Slash damage magnification: Correct the calculated damage by * ? times. 1 is normal.
		blowDamageCutRate = 1, // Battle damage magnification: Correct the calculated damage by * ? times. 1 is normal.
		thrustDamageCutRate = 1, // Pencil damage magnification: Correct the calculated damage by * ? times. 1 is normal.
		neutralDamageCutRate = 1, // Non-attribute damage magnification: Correct the calculated damage by * ? times. 1 is normal.
		magicDamageCutRate = 1, // Magic damage magnification: Correct the calculated damage by * ? times. 1 is normal.
		fireDamageCutRate = 1, // Flame damage magnification: Correct the calculated damage by * ? times. 1 is normal.
		thunderDamageCutRate = 1, // Flame damage magnification: Correct the calculated damage by * ? times. 1 is normal.
		physicsAttackRate = 1, // Physical damage magnification: Correct the calculated damage by * ? times. 1 is normal.
		magicAttackRate = 1, // Magic damage magnification: Correct the calculated damage by * ? times. 1 is normal.
		fireAttackRate = 1, // Flame damage magnification: Correct the calculated damage by * ? times. 1 is normal.
		thunderAttackRate = 1, // Electric shock damage magnification: Correct the calculated damage by * ? times. 1 is normal.
		physicsAttackPowerRate = 1, // We multiply the physical attack power by the set number
		magicAttackPowerRate = 1, // Magical attack power multiplied by the number you set
		fireAttackPowerRate = 1, // It multiplies the number set in the flame attack power
		thunderAttackPowerRate = 1; // Apply the number set for the electric shock offensive power

	int32_t
		physicsAttackPower = 0, // Add / subtract the numerical value set for physical attack power
		magicAttackPower = 0, // Add / subtract the numerical value set for magic attack power
		fireAttackPower = 0, // Add or subtract the numerical value set for fire attack power
		thunderAttackPower = 0; // Adding or subtracting the numerical value set for electric shock offensive power

	float
		physicsDiffenceRate = 1, // We will multiply the physical defense strength by the number you set
		magicDiffenceRate = 1, // It multiplies the value set for magic defense strength
		fireDiffenceRate = 1, // Apply the numerical value set for fire defense strength
		thunderDiffenceRate = 1; // Apply the number set for electric shock defense strength

	int32_t
		physicsDiffence = 0, // Adding / subtracting the numerical value set for physical defense ability
		magicDiffence = 0, // Add or subtract values ??set for magic defense strength
		fireDiffence = 0, // Add or subtract numerical value set for fire defense strength
		thunderDiffence = 0; // Add or subtract values ??set for electric shock defense

	float
		NoGuardDamageRate = 1, // Replace the damage magnification in the gap with the set value (set on the damage side)
		vitalSpotChangeRate = -1, // Replace damage calculation of sweet spot with designated numerical value (mental damage correction) Invalid at -1.0
		normalSpotChangeRate = -1, // Replace normal damage hit damage calculation with specified numerical value -1 invalid
		maxHpChangeRate = 0; // Increase / decrease by the set magnification for maximum HP

	int32_t
		behaviorId = -1; // Specified -1 to invalidate damage by using action ID from special effect

	float
		changeHpRate = 0; // Set what percentage of maximum HP to add (or subtract) every second

	int32_t
		changeHpPoint = 0; // Set how many points are added (or subtracted) per second

	float
		changeMpRate = 0; // Set what percentage of the maximum MP to add (or subtract) every second

	int32_t
		changeMpPoint = 0, // Set how many points are added (or subtracted) per second
		mpRecoverChangeSpeed = 0; // Change the recovery speed. Add to or subtract from the standard recovery speed and initial recovery speed of recovery formula.

	float
		changeStaminaRate = 0; // Set what percentage of the maximum stamina to add (or subtract) every second

	int32_t
		changeStaminaPoint = 0, // Set how many points are added (or subtracted) per second
		staminaRecoverChangeSpeed = 0; // Change the recovery speed. Add to or subtract from the standard recovery speed and initial recovery speed of recovery formula.

	float
		magicEffectTimeChange = 0; // Added or subtracted the time set for the effect duration only for magic that is set to be more than 0.1 seconds in effect duration

	int32_t
		insideDurability = 0, // Add or subtract numerical value to internal wear degree
		maxDurability = 0; // The set numerical value is added to the maximum value of the internal wear degree of the durability

	float
		staminaAttackRate = 1; // Apply magnification to stamina attack strength (1.0 1 times 0.5 half)

	int32_t
		poizonAttackPower = 0, // When hit, the numerical value to be added to the target [poison resistance value]
		registIllness = 0, // When hit, the numerical value to be added to the target [pestilial tolerance value]
		registBlood = 0, // When hit, the numerical value to be added to the target [bleeding tolerance value]
		registCurse = 0; // When hit, the numerical value to be added to the target curse resistance value

	float
		fallDamageRate = 0, // Apply magnification to damage calculation when falling
		soulRate = 0, // The amount of acquired soul at the time of defeating the enemy is added by a specified multiple
		equipWeightChangeRate = 0, // Apply the set magnification to the maximum equipment weight
		allItemWeightChangeRate = 0; // Multiply the maximum possession weight by the set magnification

	int32_t
		soul = 0, // Add setting value to possession soul
		animIdOffset = 0; // Anime ID offset

	float
		haveSoulRate = 1, // For enemy landing effects. It is applied when Seoul goes out from the set character.
		targetPriority = 0; // At the time of multiplayer, it becomes targeted as a target preferentially from the enemy. Addition of priority. 0 is the default. It is often aimed at a plus value. Minus is up to -1.

	int32_t
		sightSearchEnemyCut = 0, // It becomes easier for AI to deviate from the visual enemy target. 0 is the default.
		hearingSearchEnemyCut = 0; // It becomes easy for AI to fall out of auditory enemy target. 0 is the default.

	float
		grabityRate = 1, // Gravity rate
		registPoizonChangeRate = 0, // Multiply the poison resistance value by the set magnification
		registIllnessChangeRate = 0, // Multiply by the magnification set for the pestilence resistance value
		registBloodChangeRate = 0, // Multiply the bleeding resistance value by the magnification set
		registCurseChangeRate = 0, // Multiply the curse resistance value by the set magnification
		soulStealRate = 0, // Defense against HP that NPC is robbed by Soul Steel
		lifeReductionRate = 0,
		hpRecoverRate = 0; // It will not work when HP decreases.

	int32_t
		replaceSpEffectId = -1, // Special effect ID added when lifetime runs out (-1 is ignored)
		cycleOccurrenceSpEffectId = -1, // Special effect ID generated every activation cycle (-1 is ignored)
		atkOccurrenceSpEffectId = -1; // Special effect ID generated at attack Hit (-1 is ignored)

	float
		guardDefFlickPowerRate = 1, // Hidden defense force correction value at guard
		guardStaminaCutRate = 1; // Stamina cut rate correction value at guard

	int16_t
		rayCastPassedTime = -1, // Gaze passing: activation time [ms] (for evil eye)
		changeSuperArmorPoint = 0, // Value to be added to super armor value
		bowDistRate = 0; // Correction value added to correction of distance of weapon

	uint16_t
		spCategory = 0; // Categories that determine behaviors such as overwriting of special effects

	uint8_t
		categoryPriority = 0; // Priority within the same category (lower priority first)

	int8_t
		saveCategory = -1; // Category for saving special effects

	uint8_t
		changeMagicSlot = 0, // You can increase the number of magic registration frames by a specified number
		changeMiracleSlot = 0; // You can increase the trajectory registration frame by the specified number

	int8_t
		heroPointDamage = 0; // Damage value given to humanity value

	uint8_t
		defFlickPower = 0, // Set a value to overwrite repelling power
		flickDamageCutRate = 0, // Set a value to overwrite the damage attenuation rate at repelling
		bloodDamageRate = 100;

	int8_t
		dmgLv_None = 0, // Specify the type to replace damage Lv 0
		dmgLv_S = 0, // Specify the type to replace damage Lv1
		dmgLv_M = 0, // Designate the type to replace damage Lv 2
		dmgLv_L = 0, // Specify the type to replace damage Lv3
		dmgLv_BlowM = 0, // Specify the type to replace damage Lv 4
		dmgLv_Push = 0, // Specify the type to replace damage Lv 5
		dmgLv_Strike = 0, // Specify the type to replace damage Lv6
		dmgLv_BlowS = 0, // Specify the type to replace damage Lv7
		dmgLv_Min = 0, // Specify the type to replace damage Lv8
		dmgLv_Uppercut = 0, // Designate the type to replace damage Lv 9
		dmgLv_BlowLL = 0, // Specify the type to replace damage Lv 10
		dmgLv_Breath = 0; // Specify the type to replace damage Lv 11

	uint8_t
		atkAttribute = 0, // Physical attribute to set for special effects
		spAttribute = 0, // Special attributes to set for special effects
		stateInfo = 0, // Flag for judging state change
		wepParamChange = 0, // Designate which weapon will exert its effect. In case of no limit, all attacks and defenses including enemies
		moveType = 0, // Movement type. Change the moving speed.
		lifeReductionType = 0,
		throwCondition = 0; // Throwing conditions. It affects the throwing mask.

	int8_t
		addBehaviorJudgeId_condition = -1; // A condition value (Def: -1) for adding a value to the behavior determination ID

	uint8_t
		addBehaviorJudgeId_add = 0; // If the added value of action judgment ID is 0, it will not act, not action.

	uint8_t
		effectTargetSelf : 1, // The effect is effective only for objects that are checked in this judgment, the default is x
		effectTargetFriend : 1, // The effect is effective only for objects that are checked in this judgment, the default is x
		effectTargetEnemy : 1, // The effect is effective only for objects that are checked in this judgment, the default is x
		effectTargetPlayer : 1, // The effect is effective only for objects that are checked in this judgment, the default is x
		effectTargetAI : 1, // The effect is effective only for objects that are checked in this judgment, the default is x
		effectTargetLive : 1, // The effect is effective only for objects that are checked in this judgment, the default is x
		effectTargetGhost : 1, // The effect is effective only for objects that are checked in this judgment, the default is x
		effectTargetWhiteGhost : 1, // The effect is effective only for objects that are checked in this judgment, the default is x
		effectTargetBlackGhost : 1, // The effect is effective only for objects that are checked in this judgment, the default is x
		effectTargetAttacker : 1, // Apply a special effect to the attacker after the damage (Do not put on the defender)
		dispIconNonactive : 1, // Icons are displayed even in the state of waiting for activation.
		useSpEffectEffect : 1, // Whether to use special effects
		bAdjustMagicAblity : 1, // Do you want to correct magical powers?
		bAdjustFaithAblity : 1, // Do you want to correct your faith?
		bGameClearBonus : 1, // Whether it is for game clear orbiting bonus.
		magParamChange : 1, // Set whether or not to exert its effect against magic
		miracleParamChange : 1, // Set whether or not to exert effects on miracles
		clearSoul : 1, // Possession Seoul will be 0.
		requestSOS : 1, // If checked, issue SOS sign request at activation
		requestBlackSOS : 1, // If checked, issue a black SOS sign request at activation
		requestForceJoinBlackSOS : 1, // If checked, issue a Black Forced Participation SOS Sign Request at invocation
		requestKickSession : 1, // If checked, issue a kick request at activation
		requestLeaveSession : 1, // If checked, issue an exit request at invocation
		requestNpcInveda : 1, // When checked, issue intrusion request to NPC at invocation
		noDead : 1, // Whether it can be in a corpse state. With this check, it will not result in death
		bCurrHPIndependeMaxHP : 1, // Even if the maximum HP increases or decreases, will HP have no effect?
		corrosionIgnore : 1, // [State change type] ignore [durability] decrease due to [corrosion]
		sightSearchCutIgnore : 1, // Ignore visual enemy invalidity
		hearingSearchCutIgnore : 1, // Ignore hearing enemy invalidity
		antiMagicIgnore : 1, // You can use magic in the anti-magic range
		fakeTargetIgnore : 1, // It will not catch on the fake target that occurred
		fakeTargetIgnoreUndead : 1, // It will not catch on the immortal fake target generated
		fakeTargetIgnoreAnimal : 1, // It will not catch on the fake target of the generated beast system
		grabityIgnore : 1, // Invalid gravity effect
		disablePoison : 1, // With this effect it will not be toxic
		disableDisease : 1, // When this effect is applied it will not be affected by plague
		disableBlood : 1, // If this effect is applied it will not be bleeding
		disableCurse : 1, // If this effect is applied it will not be cursed
		enableCharm : 1, // It will be fascinated if this effect is applied
		enableLifeTime : 1, // Does the lifetime extension be extended when setting the flag by TAE?
		hasTarget : 1, // Do you know the enemy? : [Invocation condition] (for evil eye users)
		isFireDamageCancel : 1, // Do you release special effects due to flame damage?
		isExtendSpEffectLife : 1, // Whether it will be subject to extension when the lifespan extension effect is applied
		requestLeaveColiseumSession : 1; // If checked, issue a request to leave the battlefield at activation

	uint8_t
		pad_2 : 4; // Pad

	uint8_t
		vowType0 : 1, // Commitment 0
		vowType1 : 1, // Commitment 1
		vowType2 : 1, // Pledge 2
		vowType3 : 1, // Pledge 3
		vowType4 : 1, // Pledge 4
		vowType5 : 1, // Pledge 5
		vowType6 : 1, // Pledge 6
		vowType7 : 1, // Pledge 7
		vowType8 : 1, // Commitment 8
		vowType9 : 1, // Pledge 9
		vowType10 : 1, // Pledge 10
		vowType11 : 1, // Pledge 11
		vowType12 : 1, // Pledge 12
		vowType13 : 1, // Pledge 13
		vowType14 : 1, // Pledge 14
		vowType15 : 1; // Pledge 15

	uint8_t
		pad1[11] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Padding

} SpEffectParam;



// SpEffect parameter definitions file
class SpEffectParamDef : public GameParamDef {


public:
	static SpEffectParamDef& get_instance()
	{
		static SpEffectParamDef instance;
		return instance;
	}
	
	SpEffectParam *data()
	{
		return (SpEffectParam*)GameParamDef::data();
	}

	SpEffectParam *get(int index)
	{
		return (SpEffectParam*)GameParamDef::get(index);
	}


	// @TODO: Member data specific to this class
	
private:
	SpEffectParamDef()
		: GameParamDef(NULL, 0x27E4, 847, sizeof(SpEffectParam),
			"80 E9 04 00 E4 27 01 00 01 00 4F 03 53 50 5F 45",
			"SpEffectParam", "Special Effect")
	{
	}
	
public:
	SpEffectParamDef(SpEffectParamDef const&) = delete;
	void operator=(SpEffectParamDef const&) = delete;
};







#endif // _DS1_OVERHAUL_SPECIAL_EFFECT_PARAM_DEF_FILE_H_
