/*
    DARK SOULS GAME FILE C++ LIBRARY

    Contributors to this file:
        Sean Pesce    -    C++


    Param/EnemyStandardInfo.h

    Default Enemy Info Parameters from default_EnemyBehaviorBank.paramdef


    References:
        - Burton Radons' ParamDefs Sheet
          https://docs.google.com/spreadsheets/d/1KukblWL61We64-gNIyaAShga9h8RTXYmyFs98eQhY4E/

        - Kushisushi's ParamDefs Sheet
          https://docs.google.com/spreadsheets/d/1QX6i93H4_jGZ0So9mArDECnuv1lfkaJQhXOzkDuxdkE/edit#gid=2133439974

*/

#pragma once

#ifndef _DS1_OVERHAUL_ENEMY_STANDARD_INFO_PARAM_DEF_FILE_H_
    #define _DS1_OVERHAUL_ENEMY_STANDARD_INFO_PARAM_DEF_FILE_H_


#include "Param.h"


// EnemyStandardInfo parameter
typedef struct EnemyStandardInfoParameter : public Param {

    int32_t
        EnemyBehaviorID = 0; // Enemy behavior ID

    uint16_t
        HP = 1, // Hit points
        AttackPower = 1; // Attack power (protot only)

    int32_t
        ChrType = 5; // Character type

    float
        HitHeight = 2, // Height (Please specify a size larger than the diameter)
        HitRadius = 0.4f, // Radius around
        Weight = 60, // The weight of the character
        DynamicFriction = 0, // Dynamic friction force
        StaticFriction = 0; // Static frictional force

    int32_t
        UpperDefState = 0, // Waist upper body state (PG input)
        ActionDefState = 0; // Action initial state (PG input)

    float
        RotY_per_Second = 10; // Y axis rotation angle per unit time [deg / s] (In the sequel make this 10 million)

    uint8_t
        reserve0[20] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Padding

    uint8_t
        RotY_per_Second_old = 0, // unused
        EnableSideStep = 0, // Can you move left and right?
        UseRagdollHit = 0; // Do you use ragdolls per character

    uint8_t
        reserve_last[5] = { 0, 0, 0, 0, 0 }; // Padding

    uint16_t
        stamina = 0, // Total stamina
        staminaRecover = 0, // Stamina recovery amount per second
        staminaConsumption = 0, // Basic value of stamina consumption used at attack, guard
        deffenct_Phys = 0; // Basic damage reduction against physical attack

    uint8_t
        reserve_last2[48] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                              0, 0, 0, 0, 0, 0, 0, 0 }; // Padding

} EnemyStandardInfoParam;



// EnemyStandardInfo parameter definitions file
class EnemyStandardInfoParamDef : public ParamDef {


public:
    static EnemyStandardInfoParamDef& get_instance()
    {
        static EnemyStandardInfoParamDef instance;
        return instance;
    }

    EnemyStandardInfoParam *data()
    {
        return (EnemyStandardInfoParam*)ParamDef::data();
    }

    EnemyStandardInfoParam *get(int index)
    {
        return (EnemyStandardInfoParam*)ParamDef::get(index);
    }


    // @TODO: Member data specific to this class

private:
    EnemyStandardInfoParamDef()
        : ParamDef(NULL, sizeof(EnemyStandardInfoParam),
                   "60 0C 00 00 ?? ?? 01 00 01 00 ?? ?? 45 4E 45 4D",
                   "default_EnemyBehaviorBank", "Default Enemy Behavior")
    {
    }

public:
    EnemyStandardInfoParamDef(EnemyStandardInfoParamDef const&) = delete;
    void operator=(EnemyStandardInfoParamDef const&) = delete;
};







#endif // _DS1_OVERHAUL_ENEMY_STANDARD_INFO_PARAM_DEF_FILE_H_
