/*
	DARK SOULS OVERHAUL

	Contributors to this file:
		Sean Pesce	-	C++


	ParamDef/BulletParamDef.h

	Bullet Parameters from BulletParam.paramdef


	References:
		- (Author?) ParamDefs Sheet
		  https://docs.google.com/spreadsheets/d/1KukblWL61We64-gNIyaAShga9h8RTXYmyFs98eQhY4E/

		- Kushisushi's ParamDefs Sheet
		  https://docs.google.com/spreadsheets/d/1QX6i93H4_jGZ0So9mArDECnuv1lfkaJQhXOzkDuxdkE/edit#gid=2133439974

*/

#pragma once

#ifndef _DS1_OVERHAUL_BULLET_PARAM_DEF_FILE_H_
	#define _DS1_OVERHAUL_BULLET_PARAM_DEF_FILE_H_


#include "BaseParamDef.h"


// Bullet parameter
typedef struct BulletParameter : public Param {

	int32_t
		atkId_Bullet = -1, // Attack Type / Attack Material / Physical Attack Power / Magic Attack Power / Stamina Attack Power / Knockback Distance.
		sfxId_Bullet = -1, // Insert SFX ID.
		sfxId_Hit = -1, // Impact SFXID. -1 does not occur.
		sfxId_Flick = -1; // When it hurts SFXID. -1 does not occur.

	float
		life = -1, // Time when jumps can continue to exist (infinity if -1).
		dist = 0, // Distance at which attenuation begins (not actual travel distance).
		shootInterval = 0, // Specify how many seconds to fire the jump tool.
		gravityInRange = 0, // Gravitational force applied downward within the range.
		gravityOutRange = 0, // Gravitational force to be applied downward when attenuation starts (Expressing feeling of falling with Poton.
		hormingStopRange = 0, // Distance to target to stop induction. Parameter to avoid overworking with guided bullets.
		initVellocity = 0, // Initial speed of SFX.
		accelInRange = 0, // Acceleration within range of SFX.
		accelOutRange = 0, // Acceleration when SFX goes out of range.
		maxVellocity = 0, // maximum speed.
		minVellocity = 0, // Minimum guaranteed speed.
		accelTime = 0, // Until this time, do not accelerate (make it possible to shoot magic like rocket).
		homingBeginDist = 0, // How much to start guiding from a point advanced by some m.
		hitRadius = -1, // Set the radius of the ball.
		hitRadiusMax = -1, // Maximum radius of the perimeter ball (if -1, make it the same as the initial radius / default)
		spreadTime = 0, // The time when the range radius widens to a very large extent.
		expDelay = 0, // Time to lapse, time to explosion (0 in case of explosion soon).
		hormingOffsetRange = 0, // When it is 0, it is accurate. Shoot XYZ components at the time of shooting by shifting by this amount.
		dmgHitRecordLifeTime = 0, // Survival time of damage hit history [sec] (&lt;= 0.0 f: indefinite)
		externalForce = 0; // External force applied in the direction of shooting (Y axis is pulled out)

	int32_t
		spEffectIDForShooter = -1, // Special effects on people who shoot
		autoSearchNPCThinkID = 0, // Parameters used by funnel search target
		HitBulletID = -1, // From bullet parameters, specify ID when new bullet parameter is generated
		spEffectId0 = -1, // Register the ID of the special effect parameter respectively ? General special effects.
		spEffectId1 = -1, // Register the ID of the special effect parameter respectively ? General special effects.
		spEffectId2 = -1, // Register the ID of the special effect parameter respectively ? General special effects.
		spEffectId3 = -1, // Register the ID of the special effect parameter respectively ? General special effects.
		spEffectId4 = -1; // Register the ID of the special effect parameter respectively ? General special effects.

	uint16_t
		numShoot = 0; // Number of jumping tools to fire at once.

	int16_t
		homingAngle = 0, // How many times should I compensate per second? .
		shootAngle = 0, // Specify how many times to fire the jumping tool forward.
		shootAngleInterval = 0, // When launching multiple jumps, specify how many times to shoot (Y axis)
		shootAngleXInterval = 0; // When launching multiple jumps, specify how many times to shoot (X axis)

	int8_t
		damageDamp = 0, // Correction value decreasing in 1 second after the decay distance.
		spelDamageDamp = 0,
		fireDamageDamp = 0,
		thunderDamageDamp = 0,
		staminaDamp = 0, // Correction value decreasing in 1 second after the decay distance.
		knockbackDamp = 0, // Correction value decreasing in 1 second after the decay distance.
		shootAngleXZ = 0; // Additional elevation from the horizontal direction.

	uint8_t
		lockShootLimitAng = 0, // Restriction angle for locking direction
		isPenetrate = 0, // Whether it will not disappear until the end of life if it hits PC, NPC, OBJ? Set up
		prevVelocityDirRate = 0, // The ratio at which the sliding bullet adds the previous moving direction to the current direction when it hits the wall
		atkAttribute = 0, // Set physical attributes to set bullets
		spAttribute = 0, // Set special attributes to set bullets
		Material_AttackType = 0, // Specify what the attack attribute is
		Material_AttackMaterial = 0, // Used for SFX / SE when attacking
		Material_Size = 0, // Used for SFX / SE when attacking (preliminary / default 0)
		launchConditionType = 0; // Designate conditions for determining whether to generate bullets when landing / life expires

	uint8_t
		FollowType : 3, // Following type. "Do not follow" is the default.
		EmittePosType : 3, // Origin type. It is usually from Damipoli. (Introduced to judge meteo)
		isAttackSFX : 1, // Sets whether bullets such as arrows will remain stuck in the character
		isEndlessHit : 1, // Do you keep going around?
		isPenetrateMap : 1, // Do you penetrate the map?
		isHitBothTeam : 1, // Are you an enemy friend or a friend? (It is not a wandering ghost)
		isUseSharedHitList : 1, // Whether to share hit list
		isUseMultiDmyPolyIfPlace : 1, // Do you use the same Damipori ID multiple times when arranging bullets?
		attachEffectType : 2, // Effect type to attach
		isHitForceMagic : 1, // Do you hit the Force magic?
		isIgnoreSfxIfHitWater : 1, // Whether you ignore effects when you hit the water surface
		isIgnoreMoveStateIfHitWater : 1, // Do you ignore the state transition even if you hit the water?
		isHitDarkForceMagic : 1; // Will it hit the dark force magic

	uint8_t
		pad[3] = { 0, 0, 0 }; // Padding

} BulletParam;



// Bullet parameter definitions file
class BulletParamDef : public BaseParamDef {


public:
	static BulletParamDef& get_instance()
	{
		static BulletParamDef instance;
		return instance;
	}
	
	BulletParam *data()
	{
		return (BulletParam*)BaseParamDef::data();
	}

	BulletParam *get(int index)
	{
		return (BulletParam*)BaseParamDef::get(index);
	}


	// @TODO: Member data specific to this class
	
private:
	BulletParamDef()
		: BaseParamDef(NULL, 0x1D84, 631, sizeof(BulletParam), "64 1E 00 00 2B A8 01 00 02 00 00 00 04 1F", "BulletParam", "Bullet")
	{
	}
	
public:
	BulletParamDef(BulletParamDef const&) = delete;
	void operator=(BulletParamDef const&) = delete;
};







#endif // _DS1_OVERHAUL_BULLET_PARAM_DEF_FILE_H_
