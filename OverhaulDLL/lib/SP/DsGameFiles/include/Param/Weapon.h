/*
    DARK SOULS GAME FILE C++ LIBRARY

    Contributors to this file:
        Sean Pesce	-	C++


    Param/Weapon.h

    Weapon Parameters from WeaponParam.paramdef


    References:
        - Burton Radons' ParamDefs Sheet
          https://docs.google.com/spreadsheets/d/1KukblWL61We64-gNIyaAShga9h8RTXYmyFs98eQhY4E/

        - Kushisushi's ParamDefs Sheet
          https://docs.google.com/spreadsheets/d/1QX6i93H4_jGZ0So9mArDECnuv1lfkaJQhXOzkDuxdkE/edit#gid=2133439974

*/

#pragma once

#ifndef DS1_FILE_LIB_WEAPON_PARAM_DEF_FILE_H_
    #define DS1_FILE_LIB_WEAPON_PARAM_DEF_FILE_H_


#include "Param.h"


// Weapon parameter
typedef struct WeaponParameter : public Param {

    int32_t
        behaviorVariationId = 0, // Used to determine the action parameter ID to be referenced at the time of attack
        sortId = 0; // Sort ID (-1: not collected)

    uint32_t
        wanderingEquipId = 0; // The replacement equipment ID for the wandering ghost.

    float
        weight = 1, // Weight [kg].
        weaponWeightRate = 0; // Equipment weight ratio

    int32_t
        fixPrice = 0, // Repair base price
        basicPrice = 0, // Basic price
        sellValue = 0; // Selling price

    float
        correctStrength = 0, // Chara Para correction value.
        correctAgility = 0, // Chara Para correction value.
        correctMagic = 0, // Chara Para correction value.
        correctFaith = 0, // Chara Para correction value.
        physGuardCutRate = 0, // Damage cut rate at guard setting for each attack
        magGuardCutRate = 0, // If it is not a guard attack, put 0
        fireGuardCutRate = 0, // How much to cut a flame attack?
        thunGuardCutRate = 0; // How much to cut electric shock attacks?

    int32_t
        spEffectBehaviorId0 = -1, // Register to add special effects to weapons
        spEffectBehaviorId1 = -1, // Register to add special effects to weapons
        spEffectBehaviorId2 = -1, // Register to add special effects to weapons
        residentSpEffectId = -1, // Resident Special Effect ID 0
        residentSpEffectId1 = -1, // Resident Special Effect ID 1
        residentSpEffectId2 = -1, // Resident Special Effect ID 2
        materialSetId = -1, // Material parameter ID required to strengthen weapons
        originEquipWep = -1, // Enhancement weapon ID of this weapon
        originEquipWep1 = -1, // This weapon's reinforcement weapon ID 1
        originEquipWep2 = -1, // Enhancement weapon of this weapon ID 2
        originEquipWep3 = -1, // Enhancement weapon of this weapon ID 3
        originEquipWep4 = -1, // Enhancement weapon of this weapon ID 4
        originEquipWep5 = -1, // Enhancement weapon of this weapon ID 5
        originEquipWep6 = -1, // Enhancement weapon of this weapon ID 6
        originEquipWep7 = -1, // Enhancement weapon of this weapon ID 7
        originEquipWep8 = -1, // This weapon's reinforcement weapon ID 8
        originEquipWep9 = -1, // Enhancement weapon of this weapon ID 9
        originEquipWep10 = -1, // Enhancement weapon of this weapon ID 10
        originEquipWep11 = -1, // This Weapon Enhancement Weapon ID 11
        originEquipWep12 = -1, // Enhancement weapon of this weapon ID 12
        originEquipWep13 = -1, // Enhancement weapon of this weapon ID 13
        originEquipWep14 = -1, // Enhancement weapon of this weapon ID 14
        originEquipWep15 = -1; // Enhancement weapon of this weapon ID15

    float
        antiDemonDamageRate = 1, // Damage magnification for daemon against
        antSaintDamageRate = 1, // Damage magnification against sacred weakness
        antWeakA_DamageRate = 1, // Damage magnification for weak point A
        antWeakB_DamageRate = 1; // Damage magnification for weak point B

    int32_t
        vagrantItemLotId = 0, // -1: no beigrant 0: no drawing 1: ~ lottery
        vagrantBonusEneDropItemLotId = 0, // -1: No drop 0: no drawing 1: to be selected
        vagrantItemEneDropItemLotId = 0; // -1: No drop 0: no drawing 1: to be selected

    uint16_t
        equipModelId = 0, // Equipment model number.
        iconId = 0, // Menu icon ID.
        durability = 100, // Initial durability.
        durabilityMax = 100, // New article durability.
        attackThrowEscape = 0; // Basic value of throwing-off attack power

    int16_t
        parryDamageLife = -1; // Limit the life of parry damage. It does not last longer than it is set with TimeAct.

    uint16_t
        attackBasePhysics = 100, // Basic value of physical attribute attack that deals damage to enemy's HP
        attackBaseMagic = 100, // Basic value of magic attribute attack that deals damage to enemy's HP
        attackBaseFire = 100, // Basic value of fire attribute attack that gives damage to enemy's HP
        attackBaseThunder = 100, // Basic value of electric shock attribute attack that deals damage to enemy's HP
        attackBaseStamina = 100, // Stamina attack power to enemies
        saWeaponDamage = 0; // Super armor base attack power

    int16_t
        saDurability = 0, // Additional SA endurance value used during attack motion
        guardAngle = 0, // Defense occurrence range angle when weapons are guarded Angle
        staminaGuardDef = 0, // Defense against enemy's stamina attack when guard succeeds
        reinforceTypeId = 0, // Enhanced Type ID
        trophySGradeId = -1, // Is it related to the trophy system?
        trophySeqId = -1, // Trophy's SEQ number (13-29)
        throwAtkRate = 0, // Attack power magnification of throwing
        bowDistRate = 0; // Increase flying distance up%

    uint8_t
        equipModelCategory = 7, // Type of equipment model.
        equipModelGender = 0, // Gender of the equipment model.
        weaponCategory = 0, // Category of weapons.
        wepmotionCategory = 0, // Category of weapon motion.
        guardmotionCategory = 0, // Guard motion category
        atkMaterial = 0, // Attack material used from attack parapet
        defMaterial = 0, // Defense material used from attack parapet
        defSfxMaterial = 0, // Defense SFX material used from attack parapet
        correctType = 0, // Determine the type of correction graph by primary parameters
        spAttribute = 0, // Weapon special attribute value
        spAtkcategory = 0, // Special attack category (possible from 50 to 199)
        wepmotionOneHandId = 0, // Basic motion ID when equipped with one hand.
        wepmotionBothHandId = 0, // Basic motion ID when equipped with both hands.
        properStrength = 0, // Equipment proper value.
        properAgility = 0, // Equipment proper value.
        properMagic = 0, // Equipment proper value.
        properFaith = 0, // Equipment proper value.
        overStrength = 0, // Strength over start value
        attackBaseParry = 0, // Basic value for hitting enemy paris
        defenseBaseParry = 0, // Used to decide whether to become a parry or a guard at the time of parity determination
        guardBaseRepel = 0, // Guard Used to judge whether it will be repelled when attacking an enemy
        attackBaseRepel = 0; // Used to judge whether to start when guarding an enemy's attack

    int8_t
        guardCutCancelRate = 0, // The magnification to invalidate the guard cut of the opponent. Completely invalid at -100. Doubling the defense effect of the opponent at 100.
        guardLevel = 0, // When guarding, what guard motion will you take an enemy's attack? Decide
        slashGuardCutRate = 0, // By looking at the attack type, what percentage of cut-hammered damage is cut? Specify
        blowGuardCutRate = 0, // By looking at the attack type, what percentage of batting attribute damage is cut? Specify
        thrustGuardCutRate = 0, // By looking at the attack type, what percentage of threshing damage is cut? Specify
        poisonGuardResist = 0, // How much to cut poison attack power (set as special effect parameter)
        diseaseGuardResist = 0, // How much to cut attack power (set as special effect parameter) to make plague
        bloodGuardResist = 0, // How much to cut off the attack power (set as special effect parameter) to make bleeding
        curseGuardResist = 0; // How much to cut the attack power (set as a special effect parameter) to make it a curse

    uint8_t
        isDurabilityDivergence = 0; // Magical use weapon correspondence: Motion branching by durability

    uint8_t
        rightHandEquipable : 1, // Is it possible to equip the right hand?
        leftHandEquipable : 1, // Is it possible to equip the left hand?
        bothHandEquipable : 1, // Is it possible to equip both hands?
        arrowSlotEquipable : 1, // Can bow arrow bullets be equipped?
        boltSlotEquipable : 1, // Can you install armor bullets for.?
        enableGuard : 1, // Guard with L1 on left hand equipment
        enableParry : 1, // When equipped with left hand parry
        enableMagic : 1, // Magic invocation when attacking
        enableSorcery : 1, // Magical trigger at attack
        enableMiracle : 1, // Miraculous invocation on attack
        enableVowMagic : 1, // Pledge magic at attack
        isNormalAttackType : 1, // Attack type for menu display. Normal
        isBlowAttackType : 1, // Attack type for menu display. Is it a blow?
        isSlashAttackType : 1, // Attack type for menu display. Is it a slash?
        isThrustAttackType : 1, // Attack type for menu display. Stabbing
        isEnhance : 1, // Is it possible to strengthen it with rosin etc.?
        isLuckCorrect : 1, // Is there an attack power correction by luck
        isCustom : 1, // Line up in the reinforcement target list at reinforced shops (May be deleted by specification change?)
        disableBaseChangeReset : 1, // Is job change reset prohibited?
        disableRepair : 1, // Is repair prohibited?
        isDarkHand : 1, // Is it a dark hand?
        simpleModelForDlc : 1, // Does a simple model for DLC exist?
        lanternWep : 1, // Is it a lantern weapon?
        isVersusGhostWep : 1, // Spiritual weapon
        baseChangeCategory : 6, // Weapon change career category
        isDragonSlayer : 1, // Dragon hunting weapon?
        isDeposit : 1, // Will it be deposited in the warehouse?
        disableMultiDropShare : 1; // Does multi-drop sharing prohibited?

    uint8_t
        pad_0 = 0;

    int16_t
        oldSortId = 0; // Old sort ID (-1: not collected)

    uint8_t
        pad_1[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

} WeaponParam;



// Weapon parameter definitions file
class WeaponParamDef : public ParamDef {


public:
    static WeaponParamDef& get_instance()
    {
        static WeaponParamDef instance;
        return instance;
    }

    WeaponParam *data()
    {
        return (WeaponParam*)ParamDef::data();
    }

    WeaponParam *get(int index)
    {
        return (WeaponParam*)ParamDef::get(index);
    }


    // @TODO: Member data specific to this class

private:
    WeaponParamDef()
        : ParamDef(NULL, /*0x3A8C, 1245,*/ sizeof(WeaponParam),
                   //"60 65 05 00 ?? ?? 01 00 01 00 ?? ?? 45 51 55 49",
                   "?? ?? ?? ?? ?? ?? 01 00 01 00 ?? ?? 45 51 55 49 50 5F 50 41 52 41 4D 5F 57 45 41 50 4F 4E 5F 53 54 00",
                   "EquipParamWeapon", "Weapon")
    {
    }

public:
    WeaponParamDef(WeaponParamDef const&) = delete;
    void operator=(WeaponParamDef const&) = delete;
};







#endif // DS1_FILE_LIB_WEAPON_PARAM_DEF_FILE_H_
