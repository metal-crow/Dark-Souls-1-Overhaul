/*
    DARK SOULS GAME FILE C++ LIBRARY

    Contributors to this file:
        Sean Pesce    -    C++


    Param/Attack.h

    Attack Parameters from AtkParam.paramdef


    References:
        - Burton Radons' ParamDefs Sheet
          https://docs.google.com/spreadsheets/d/1KukblWL61We64-gNIyaAShga9h8RTXYmyFs98eQhY4E/

        - Kushisushi's ParamDefs Sheet
          https://docs.google.com/spreadsheets/d/1QX6i93H4_jGZ0So9mArDECnuv1lfkaJQhXOzkDuxdkE/edit#gid=2133439974

*/

#pragma once

#ifndef _DS1_OVERHAUL_ATTACK_PARAM_DEF_FILE_H_
    #define _DS1_OVERHAUL_ATTACK_PARAM_DEF_FILE_H_


#include "Param.h"


// Attack parameter
typedef struct AttackParameter : public Param {

    float
        hit0_Radius = 0, // Sphere, radius of the capsule
        hit1_Radius = 0, // Sphere, radius of the capsule
        hit2_Radius = 0, // Sphere, radius of the capsule
        hit3_Radius = 0, // Sphere, radius of the capsule
        knockbackDist = 0, // Knockback distance [m]
        hitStopTime = 0; // Hit stop stop time [s]

    int32_t
        spEffectId0 = -1, // Insert the ID created with the special effect parameter
        spEffectId1 = -1, // Insert the ID created with the special effect parameter
        spEffectId2 = -1, // Insert the ID created with the special effect parameter
        spEffectId3 = -1, // Insert the ID created with the special effect parameter
        spEffectId4 = -1; // Insert the ID created with the special effect parameter

    int16_t
        hit0_DmyPoly1 = 0, // Damipoly at the ball, capsule position
        hit1_DmyPoly1 = 0, // Damipoly at the ball, capsule position
        hit2_DmyPoly1 = 0, // Damipoly at the ball, capsule position
        hit3_DmyPoly1 = 0, // Damipoly at the ball, capsule position
        hit0_DmyPoly2 = 0, // The position of another point of the capsule Damipol. -1 turns into a sphere
        hit1_DmyPoly2 = 0, // The position of another point of the capsule Damipol. -1 turns into a sphere
        hit2_DmyPoly2 = 0, // The position of another point of the capsule Damipol. -1 turns into a sphere
        hit3_DmyPoly2 = 0; // The position of another point of the capsule Damipol. -1 turns into a sphere

    uint16_t
        blowingCorrection = 0, // Correction value when blowing off
        atkPhysCorrection = 0, // PC only. Physical attack power multiplied by basic value
        atkMagCorrection = 0, // PC only. Magnification multiplied by magic attack power (in case of a bow, correct jump tool)
        atkFireCorrection = 0, // PC only. Magnification applied to flame attack power (In case of bow, correct jump tool)
        atkThunCorrection = 0, // PC only. Magnification multiplied by the electric shock offensive power (in the case of a bow, correct jump tool)
        atkStamCorrection = 0, // PC only. Stamina multiplication factor on attack power
        guardAtkRateCorrection = 0, // PC only. 1 only
        guardBreakCorrection = 0, // PC only. Magnification multiplied by the base value of the attack
        atkThrowEscapeCorrection = 0, // Weapon correction value for throw-off attack
        atkSuperArmorCorrection = 0, // PC only. Correction value applied to  [Basic value] set as weapon
        atkPhys = 0, // NPC only. Basic damage of physical attack
        atkMag = 0, // NPC only. Additional magic attack damage
        atkFire = 0, // NPC only. Additional damage of fire attack
        atkThun = 0, // NPC only. Additional Damage Attack Damage
        atkStam = 0, // NPC only. Damage to enemy (player) stamina
        guardAtkRate = 0, // NPC only. Higgling value
        guardBreakRate = 0, // NPC only. Value used to determine whether attacks are repulsed
        atkSuperArmor = 0, // NPC only. Value to be used for SA break calculation formula
        atkThrowEscape = 0, // Throwing off attack power
        atkObj = 0; // Attack power against OBJ

    int16_t
        guardStaminaCutRate = 0, // Corrects [Guard time stamina cut rate] set for weapon parameters and NPC parameters
        guardRate = 0; // Guard magnification to make the parameter increase / decrease to 0 at 2 times / -100 with 1/100, with guard performance set uniformly by NPC, weapon parameter uniformly 0 = guard magnification / 100 1)

    uint16_t
        throwTypeId = 0; // ID associated with throw parameter

    uint8_t
        hit0_hitType = 0, // Site
        hit1_hitType = 0, // Site
        hit2_hitType = 0, // Site
        hit3_hitType = 0, // Site
        hti0_Priority = 0, // priority. If two or more people hit at the same time, adopt the one with the highest priority.
        hti1_Priority = 0, // priority. If two or more people hit at the same time, adopt the one with the highest priority.
        hti2_Priority = 0, // priority. If two or more people hit at the same time, adopt the one with the highest priority.
        hti3_Priority = 0, // priority. If two or more people hit at the same time, adopt the one with the highest priority.
        dmgLevel = 0, // When attacking, which damage motions will be played against the enemy? .
        mapHitType = 0; // Per attack, which map do you see? Set up

    int8_t
        guardCutCancelRate = 0; // If the guard cut ratio invalidation ratio (-100 to 100) -> 0, normal / -100 completely invalidated / 100 with opponent's defense effect doubling -> -50, the 100% cut shield will be cut to 50% Become

    uint8_t
        atkAttribute = 0, // Physical attribute to set for attack
        spAttribute = 0, // Special attribute to set for attack
        atkType = 0, // Specify SFX / SE at attack time (1 set by attribute, material, size)
        atkMaterial = 0, // Specify SFX / SE at attack time (1 set by attribute, material, size)
        atkSize = 0, // Specify SFX / SE at attack time (Be sure to set blank or small because it is spare but it is used)
        defMaterial = 0, // Used for guarding SE
        defSfxMaterial = 0, // Used for SFX at guard.
        hitSourceType = 0, // Where do you get Damipol ID per attack? Specify
        throwFlag = 0; // Flag used for throw information

    uint8_t
        disableGuard : 1, // 1, ignore the guard on the guard side and enter the damage level
        disableStaminaAttack : 1, // "Stone breakdown judgment" by stamina attack power is done, but actually stamina is not reduced
        disableHitSpEffect : 1, // Invalidates the special effect when an attack hits. SCE bug countermeasure
        IgnoreNotifyMissSwingForAI : 1, // Do not report to the AI
        repeatHitSfx : 1, // Enemy exclusive: SFX at wall Hit occurs continuously
        isArrowAtk : 1, // It is used for part damage judgment.
        isGhostAtk : 1, // It is used for spiritual body damage judgment.
        isDisableNoDamage : 1; // Ignoring the invincible effects such as steps, TAE's complete invincibility can not be ignored.

    uint8_t
        pad = 0; // Pad

} AttackParam;



// Attack parameter definitions file
class AttackParamDef : public ParamDef {


public:
    static AttackParamDef& get_instance()
    {
        static AttackParamDef instance;
        return instance;
    }

    AttackParam *data()
    {
        return (AttackParam*)ParamDef::data();
    }

    AttackParam *get(int index)
    {
        return (AttackParam*)ParamDef::get(index);
    }


    // @TODO: Member data specific to this class

private:
    AttackParamDef()
        : ParamDef(NULL, sizeof(AttackParam),
                   "A0 FB 02 00 ?? ?? 00 00 01 00 ?? ?? 41 54 4B 5F",
                   "AtkParam", "Attack")
    {
    }

public:
    AttackParamDef(AttackParamDef const&) = delete;
    void operator=(AttackParamDef const&) = delete;
};







#endif // _DS1_OVERHAUL_ATTACK_PARAM_DEF_FILE_H_
