/*
	DARK SOULS GAME FILE C++ LIBRARY

	Contributors to this file:
		Sean Pesce	-	C++


	Param/NPC.h

	Npc Parameters from NpcParam.paramdef


	References:
		- Burton Radons' ParamDefs Sheet
		  https://docs.google.com/spreadsheets/d/1KukblWL61We64-gNIyaAShga9h8RTXYmyFs98eQhY4E/

		- Kushisushi's ParamDefs Sheet
		  https://docs.google.com/spreadsheets/d/1QX6i93H4_jGZ0So9mArDECnuv1lfkaJQhXOzkDuxdkE/edit#gid=2133439974

*/

#pragma once

#ifndef _DS1_FILE_LIB_NEW_PARAM_DEF_FILE_H_
	#define _DS1_FILE_LIB_NEW_PARAM_DEF_FILE_H_


#include "Param.h"


// Npc parameter
typedef struct NpcParameter : public Param {

	int32_t
		behaviorVariationId = 0, // Variation ID to use when calculating the action ID.
		aiThinkId = 0, // ID of AI thinking to use.
		nameId = -1; // ID for NPC name message parameter

	float
		turnVellocity = 0, // Rotational speed [degrees / sec] that can be rotated per second.
		hitHeight = 0, // The height of capsule per hit.
		hitRadius = 0; // Radius of hit capsule.

	uint32_t
		weight = 0; // Weight

	float
		hitYOffset = 0; // The offset in the Y (height) direction of the model display position. It can float above the perimeter position.

	uint32_t
		hp = 0, // Death grace
		mp = 0, // Magical usage
		getSoul = 0; // The amount of soul that a character can obtain at the time of death.

	int32_t
		itemLotId_1 = -1, // Specify lottery ID of item to be acquired at the time of death
		itemLotId_2 = -1, // Specify lottery ID of item to be acquired at the time of death
		itemLotId_3 = -1, // Specify lottery ID of item to be acquired at the time of death
		itemLotId_4 = -1, // Specify lottery ID of item to be acquired at the time of death
		itemLotId_5 = -1, // Specify lottery ID of item to be acquired at the time of death
		itemLotId_6 = -1, // Specify lottery ID of item to be acquired at the time of death
		humanityLotId = -1, // Specify lottery ID of human nature to be acquired at the time of death
		spEffectID0 = -1, // Resident Special Effect 0
		spEffectID1 = -1, // Resident Special Effect 1
		spEffectID2 = -1, // Resident special effect 2
		spEffectID3 = -1, // Resident Special Effect 3
		spEffectID4 = -1, // Resident Special Effect 4
		spEffectID5 = -1, // Resident special effect 5
		spEffectID6 = -1, // Resident special effect 6
		spEffectID7 = -1, // Resident Special Effect 7
		GameClearSpEffectID = -1; // Special effect ID for circling bonus

	float
		physGuardCutRate = 0, // Damage cut rate at guard setting for each attack
		magGuardCutRate  = 0, // If it is not a guard attack, put 0
		fireGuardCutRate = 0, // How much to cut a flame attack?
		thunGuardCutRate = 0; // How much to cut electric shock attacks?

	int32_t
		animIdOffset  = 0, // Play all animations with IDs shifted by this number. If not, it refers to the original animation ID.
		moveAnimId    = 0, // Moving animation parameter reference ID
		spMoveAnimId1 = 0, // Special movement animation parameter reference ID
		spMoveAnimId2 = 0; // Special movement animation parameter reference ID

	float
		networkWarpDist = 0; // With network synchronization, distance to warp instead of complementary movement. It is necessary to lengthen person (ex dragon) who is fast in speed.

	int32_t
		dbgBehaviorR1 = -1, // Register ID from Behavior Parameter Tool and specify action.
		dbgBehaviorL1 = -1, // Register ID from Behavior Parameter Tool and specify action.
		dbgBehaviorR2 = -1, // Register ID from Behavior Parameter Tool and specify action.
		dbgBehaviorL2 = -1, // Register ID from Behavior Parameter Tool and specify action.
		dbgBehaviorRL = -1, // Register ID from Behavior Parameter Tool and specify action.
		dbgBehaviorRR = -1, // Register ID from Behavior Parameter Tool and specify action.
		dbgBehaviorRD = -1, // Register ID from Behavior Parameter Tool and specify action.
		dbgBehaviorRU = -1, // Register ID from Behavior Parameter Tool and specify action.
		dbgBehaviorLL = -1, // Register ID from Behavior Parameter Tool and specify action.
		dbgBehaviorLR = -1, // Register ID from Behavior Parameter Tool and specify action.
		dbgBehaviorLD = -1, // Register ID from Behavior Parameter Tool and specify action.
		dbgBehaviorLU = -1, // Register ID from Behavior Parameter Tool and specify action.
		animIdOffset2 =  0; // Play all animations with IDs shifted by this number. If it is not, it refers to the animation ID of animation ID offset 1.

	float
		partsDamageRate1 = 1, // Magnification adapted to damage treatment for site 1
		partsDamageRate2 = 1, // Magnification adapted to damage treatment for site 2
		partsDamageRate3 = 1, // Magnification adapted to damage treatment on site 3
		partsDamageRate4 = 1, // Magnification adapted to damage treatment for part 4
		partsDamageRate5 = 1, // Magnification adapted to damage treatment for site 5
		partsDamageRate6 = 1, // Magnification adapted to damage treatment for the site 6
		partsDamageRate7 = 1, // Magnification adapted to damage treatment on the site 7
		partsDamageRate8 = 1, // Magnification adapted to damage treatment for site 8
		weakPartsDamageRate = 1, // Magnification to accommodate damage handling for weak spots
		superArmorRecoverCorrection = 0, // Correction value for super armor recovery time
		superArmorBrakeKnockbackDist = 0; // Knockback distance that can be used only when SA break

	uint16_t
		stamina = 0, // Total stamina.
		staminaRecoverBaseVel = 0, // Stamina recovery basic speed [point / s]
		def_phys = 0; // Basic damage reduction against physical attack.

	int16_t
		def_slash  = 0, // Look at the attack attribute, and when it is attacking ability, reduce defense power.
		def_blow   = 0, // Look at the attack attribute, and in the case of batting attribute, reduce defense power.
		def_thrust = 0; // Look at the attack attribute and decrease defense power when it is threshing.

	uint16_t
		def_mag = 0, // Damage reduction basic value against magic attack.
		def_fire = 0, // Basic damage reduction against fire attack.
		def_thunder = 0, // Basic value of damage reduction against electric shock attack.
		defFlickPower = 0, // It is used to judge the attack of enemy's attack. // It is to make it possible to start with a normal attack other than the guard. // The enemies of the hard epidermis may be repelled without doing anything ... Feeling like it is normal irrespective of the enemies.
		resist_poison = 0, // Susceptibility to toxic condition abnormalities
		resist_desease = 0, // Less susceptible to plague condition abnormalities
		resist_blood = 0, // It is less prone to bleeding condition abnormalities
		resist_curse = 0; // It is less prone to cursing condition abnormality

	int16_t
		ghostModelId = -1, // Replacement model when wandering ghosting, texture ID
		normalChangeResouceId = -1, // Resource ID replacement at the normal time (not to use it unnecessarily)
		guardAngle = 0, // Defense occurrence range angle when weapons are guarded Angle. Pending
		slashGuardCutRate = 0, // By looking at the attack type, what percentage of cut-hammered damage is cut? Specify
		blowGuardCutRate = 0, // By looking at the attack type, what percentage of batting attribute damage is cut? Specify
		thrustGuardCutRate = 0, // By looking at the attack type, what percentage of threshing damage is cut? Specify
		superArmorDurability = 0, // Super armor duration value
		normalChangeTexChrId = -1; // Normal replacement texture character ID (not to use it unnecessarily)

	uint16_t
		dropType = 0; // Display method when item is dropped (carcass firing or item display)

	uint8_t
		knockbackRate = 0, // Decrease value when knockback damage is received / Specifically, cut off knockback initial speed of attack side
		knockbackParamId = 0, // Set parameter ID to be used at knockback
		fallDamageDump = 0, // Fall damage reduction correction [%]
		staminaGuardDef = 0, // Defense against enemy's stamina attack when guard succeeds
		pcAttrB = 0, // QWC change amount PC attribute value Black
		pcAttrW = 0, // QWC change amount PC attribute value white
		pcAttrL = 0, // QWC change amount PC attribute value Left
		pcAttrR = 0, // QWC change amount PC attribute value right
		areaAttrB = 0, // QWC change amount area attribute value black
		areaAttrW = 0, // QWC change amount area attribute value white
		areaAttrL = 0, // QWC change amount area attribute value Left
		areaAttrR = 0, // QWC change amount area attribute value right
		mpRecoverBaseVel   = 0, // MP recovery basic speed [% / s]
		flickDamageCutRate = 0; // Set a value to attenuate damage when attack is repulsed

	int8_t
		defaultLodParamId = -1; // Default LOD param ID (-1: none)

	uint8_t
		drawType = 0, // Drawing type
		npcType  = 0, // Type of NPC. Zako enemy / boss enemies are distinguished OK
		teamType = 0, // NPC attacks hits / does not hit, aim / do not target
		moveType = 0, // How to move. This changes the control.
		lockDist = 0, // Lockable distance that can be turned on [m]
		material = 0, // Determine the SE to ring when receiving damage. Set it by appearance and OK.
		materialSfx = 0, // The SFX that occurs when receiving damage is judged. Set it by appearance and OK.
		material_Weak = 0, // Weak point Determines SE to ring when receiving damage.
		materialSfx_Weak = 0, // Weak point Determines SFX that occurs when damage is received.
		partsDamageType  = 0, // Set attack type to apply regional damage
		maxUndurationAng = 0; // Upper limit angle when angle is adjusted to undulation. It is better to set it lower if the total length is long.

	int8_t
		guardLevel = 0; // When guarding, what guard motion will you take an enemy's attack? Decide

	uint8_t
		burnSfxType = 0; // SFX type at burning

	int8_t
		poisonGuardResist  = 0, // How much to cut poison attack power (set as special effect parameter)
		diseaseGuardResist = 0, // How much to cut attack power (set as special effect parameter) to make plague
		bloodGuardResist   = 0, // How much to cut off the attack power (set as special effect parameter) to make bleeding
		curseGuardResist   = 0; // How much to cut the attack power (set as a special effect parameter) to be a curse

	uint8_t
		parryAttack = 0, // Parry attack power. Used by parry side
		parryDefence = 0, // PARIS Defense power. It is used by the parry side.
		sfxSize = 0, // SFX size
		pushOutCamRegionRadius = 12, // Camera extrusion area radius [m]
		hitStopType = 0, // Setting whether to perform hit stop processing
		ladderEndChkOffsetTop = 15, // Offset for ladder termination determination offset
		ladderEndChkOffsetLow = 8; // Offset lower side for ladder termination determination

	uint8_t
		useRagdollCamHit: 1, // Does the camera hit enemy ragdolls? (Valid only when hitting a player)
		disableClothRigidHit: 1, // If you want to prevent cross rigid from hitting you ?
		useRagdoll: 1, // Does the player hit enemy ragdolls? Only Deca character wants to let the player hit ragdoll, so it turns on and off with this setting.
		isDemon: 1, // Daemon? It is only whether or not the effect of the demon brand is present so far.
		isGhost: 1, // Spirit body? Damage calculation etc will be dedicated Please be careful not to confuse with rogue ghost
		isNoDamageMotion: 1, // Do you play damage motions when damage is 0?
		isUnduration: 1, // Does the character 's forward and backward rotation match the undulation of the ground? Can not be used for flying characters
		isChangeWanderGhost: 1, // Whether the player becomes a wandering ghost when it is a client
		modelDispMask0: 1, // Display the model corresponding to the display mask.
		modelDispMask1: 1, // Display the model corresponding to the display mask.
		modelDispMask2: 1, // Display the model corresponding to the display mask.
		modelDispMask3: 1, // Display the model corresponding to the display mask.
		modelDispMask4: 1, // Display the model corresponding to the display mask.
		modelDispMask5: 1, // Display the model corresponding to the display mask.
		modelDispMask6: 1, // Display the model corresponding to the display mask.
		modelDispMask7: 1, // Display the model corresponding to the display mask.
		modelDispMask8: 1, // Display the model corresponding to the display mask.
		modelDispMask9: 1, // Display the model corresponding to the display mask.
		modelDispMask10: 1, // Display the model corresponding to the display mask.
		modelDispMask11: 1, // Display the model corresponding to the display mask.
		modelDispMask12: 1, // Display the model corresponding to the display mask.
		modelDispMask13: 1, // Display the model corresponding to the display mask.
		modelDispMask14: 1, // Display the model corresponding to the display mask.
		modelDispMask15: 1, // Display the model corresponding to the display mask.
		isEnableNeckTurn: 1, // Would you like to make the head swing set by Paul Wieba effective?
		disableRespawn: 1, // Do you forbid responders?
		isMoveAnimWait: 1, // Do you want to play moving animation until the animation ends? (Like the dragonfly.
		isCrowd: 1, // Do you reduce the processing load at the crowd? For babies (preferably Phalanx as well)
		isWeakSaint: 1, // Is it sacred weakness? Sacred damage magnification will be included in the calculation
		isWeakA: 1, // Weak point A? Weak point A damage magnification will be included in the calculation
		isWeakB: 1, // Weak point B? Weak point B damage magnification will be included in the calculation
		pad1: 1, // Padding
		vowType: 3, // Pledge type (None: 0)
		disableInitializeDead: 1, // TRUE if you do not do the initial death, you will not reproduce the corpse if you save it by killing.
		pad3: 4; // Padding

	uint8_t pad2[6] = { 0, 0, 0, 0, 0, 0 }; // Padding

} NpcParam;



// Npc parameter definitions file
class NpcParamDef : public ParamDef {


public:
	static NpcParamDef& get_instance()
	{
		static NpcParamDef instance;
		return instance;
	}
	
	NpcParam *data()
	{
		return (NpcParam*)ParamDef::data();
	}

	NpcParam *get(int index)
	{
		return (NpcParam*)ParamDef::get(index);
	}


	// @TODO: Member data specific to this class
	
private:
	NpcParamDef()
		: ParamDef(NULL, /*0x1A40, 556,*/ sizeof(NpcParam),
			"00 F4 02 00 ?? ?? 01 00 03 00 ?? ?? 4E 50 43 5F",
			"NpcParam", "NPC")
	{
	}
	
public:
	NpcParamDef(NpcParamDef const&) = delete;
	void operator=(NpcParamDef const&) = delete;
};







#endif // _DS1_FILE_LIB_NEW_PARAM_DEF_FILE_H_
