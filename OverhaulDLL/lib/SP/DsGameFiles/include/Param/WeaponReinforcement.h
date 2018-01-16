/*
    DARK SOULS GAME FILE C++ LIBRARY

    Contributors to this file:
        Sean Pesce    -    C++


    Param/WeaponReinforcement.h

    Weapon Reinforcement Parameters from ReinforceParamWeapon.paramdef


    References:
        - Burton Radons' ParamDefs Sheet
          https://docs.google.com/spreadsheets/d/1KukblWL61We64-gNIyaAShga9h8RTXYmyFs98eQhY4E/

        - Kushisushi's ParamDefs Sheet
          https://docs.google.com/spreadsheets/d/1QX6i93H4_jGZ0So9mArDECnuv1lfkaJQhXOzkDuxdkE/edit#gid=2133439974

*/

#pragma once

#ifndef DS1_FILE_LIB_WEAPON_REINFORCEMENT_PARAM_DEF_FILE_H_
    #define DS1_FILE_LIB_WEAPON_REINFORCEMENT_PARAM_DEF_FILE_H_


#include "Param.h"


// WeaponReinforce parameter
typedef struct WeaponReinforceParameter : public Param {

    float
        physicsAtkRate = 1, // Physical Attack Power Correction Value
        magicAtkRate = 1, // Magic Attack Power Correction Value
        fireAtkRate = 1, // Correction value of fire attack power
        thunderAtkRate = 1, // Corrected value of lightning attack power
        staminaAtkRate = 1, // Stamina attack strength correction value
        saWeaponAtkRate = 1, // Super Armor weapon attack color correction value
        saDurabilityRate = 1, // Correction value of SA durability
        correctStrengthRate = 1, // Correction value of muscle force correction
        correctAgilityRate = 1, // Corrected value for agile correction
        correctMagicRate = 1, // Correction value for magical power correction
        correctFaithRate = 1, // Faith correction correction value
        physicsGuardCutRate = 1, // Correction value of guard time physical attack cut rate
        magicGuardCutRate = 1, // Correction value of magic attack cut ratio at guard
        fireGuardCutRate = 1, // Correction value of fire attack cut ratio at guard
        thunderGuardCutRate = 1, // Correction value of electric shock attack cut ratio at guard
        poisonGuardResistRate = 1, // Correction value of guarded poison attack cut ratio
        diseaseGuardResistRate = 1, // Correction value of attack rate of attack on epidemic attack at guard
        bloodGuardResistRate = 1, // Correction value of guarded bleeding attack cut rate
        curseGuardResistRate = 1, // Corruption attack guard time correction value correction
        staminaGuardDefRate = 1; // Correction value of guard stamina defense power

    uint8_t
        spEffectId1 = 0, // Addition correction value of special effect ID 1
        spEffectId2 = 0, // Addition correction value of special effect ID 2
        spEffectId3 = 0, // Addition correction value of special effect ID 3
        residentSpEffectId1 = 0, // Addition correction value of resident special effect ID 1
        residentSpEffectId2 = 0, // Additive correction value of resident special effect ID 2
        residentSpEffectId3 = 0, // Addition correction value of resident special effect ID 3
        materialSetId = 0; // Addition correction value of material parameter ID

    uint8_t
        pad[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Padding

} WeaponReinforceParam;



// WeaponReinforce parameter definitions file
class WeaponReinforceParamDef : public ParamDef {


public:
    static WeaponReinforceParamDef& get_instance()
    {
        static WeaponReinforceParamDef instance;
        return instance;
    }

    WeaponReinforceParam *data()
    {
        return (WeaponReinforceParam*)ParamDef::data();
    }

    WeaponReinforceParam *get(int index)
    {
        return (WeaponReinforceParam*)ParamDef::get(index);
    }


    // @TODO: Member data specific to this class

private:
    WeaponReinforceParamDef()
        : ParamDef(NULL, sizeof(WeaponReinforceParam),
                   //"10 C6 00 00 ?? ?? 00 00 01 00 ?? ?? 52 45 49 4E",
                   "?? ?? ?? ?? ?? ?? 00 00 01 00 ?? ?? 52 45 49 4E 46 4F 52 43 45 5F 50 41 52 41 4D 5F 57 45 41 50 4F 4E 5F 53 54 00",
                   "ReinforceParamWeapon", "Weapon Reinforcement")
    {
    }

public:
    WeaponReinforceParamDef(WeaponReinforceParamDef const&) = delete;
    void operator=(WeaponReinforceParamDef const&) = delete;
};







#endif // DS1_FILE_LIB_WEAPON_REINFORCEMENT_PARAM_DEF_FILE_H_
