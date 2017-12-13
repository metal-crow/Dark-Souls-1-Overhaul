/*
    DARK SOULS GAME FILE C++ LIBRARY

    Contributors to this file:
        Sean Pesce    -    C++


    Param/Ragdoll.h

    Ragdoll Parameters from RagdollParam.paramdef


    References:
        - Burton Radons' ParamDefs Sheet
          https://docs.google.com/spreadsheets/d/1KukblWL61We64-gNIyaAShga9h8RTXYmyFs98eQhY4E/

        - Kushisushi's ParamDefs Sheet
          https://docs.google.com/spreadsheets/d/1QX6i93H4_jGZ0So9mArDECnuv1lfkaJQhXOzkDuxdkE/edit#gid=2133439974

*/

#pragma once

#ifndef _DS1_OVERHAUL_RAGDOLL_PARAM_DEF_FILE_H_
    #define _DS1_OVERHAUL_RAGDOLL_PARAM_DEF_FILE_H_


#include "Param.h"


// Ragdoll parameter
typedef struct RagdollParameter : public Param {

    float
        hierarchyGain = 0.17f, // When it is lowered, it approaches the original pose, and when it gets larger, it gets on the edge.
        velocityDamping = 0, // Deceleration rate of ragdoll movement speed. Go slowly as you approach 0, get back at 1 as you approach 1
        accelGain = 1, // Adjustment parameter of rigid acceleration. Movement will soften when it is lower, and harder when it gets higher. It seems to be lower than the acceleration gain.
        velocityGain = 0.6f, // Rigid speed adjustment parameter. Movement will soften when it is lower, and harder when it gets higher. It seems to be lower than speed gain.
        positionGain = 0.05f, // Adjustment parameter of rigid movement position. Movement will soften when it is lower, and harder when it gets higher.
        maxLinerVelocity = 1.4f, // Maximum moving speed of rigid
        maxAngularVelocity = 1.8f, // Maximum angular velocity of rigid
        snapGain = 0.1f; // Adjustment value to approximate the original pose. Effect similar to position gain

    uint8_t
        enable = 0; // Do you move during damage ragdoll

    int8_t
        partsHitMaskNo = -1; // Mask number per site. -1: Mask invalid

    uint8_t
        pad[14] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Padding

} RagdollParam;



// Ragdoll parameter definitions file
class RagdollParamDef : public ParamDef {


public:
    static RagdollParamDef& get_instance()
    {
        static RagdollParamDef instance;
        return instance;
    }

    RagdollParam *data()
    {
        return (RagdollParam*)ParamDef::data();
    }

    RagdollParam *get(int index)
    {
        return (RagdollParam*)ParamDef::get(index);
    }


    // @TODO: Member data specific to this class

private:
    RagdollParamDef()
        : ParamDef(NULL, sizeof(RagdollParam),
                   "A0 08 00 00 ?? ?? 01 00 01 00 ?? ?? 52 41 47 44",
                   "RagdollParam", "Ragdoll")
    {
    }

public:
    RagdollParamDef(RagdollParamDef const&) = delete;
    void operator=(RagdollParamDef const&) = delete;
};







#endif // _DS1_OVERHAUL_RAGDOLL_PARAM_DEF_FILE_H_
