/*
    DARK SOULS GAME FILE C++ LIBRARY

    Contributors to this file:
        Sean Pesce    -    C++


    Param/Object.h

    Object Parameters from ObjectParam.paramdef


    References:
        - Burton Radons' ParamDefs Sheet
          https://docs.google.com/spreadsheets/d/1KukblWL61We64-gNIyaAShga9h8RTXYmyFs98eQhY4E/

        - Kushisushi's ParamDefs Sheet
          https://docs.google.com/spreadsheets/d/1QX6i93H4_jGZ0So9mArDECnuv1lfkaJQhXOzkDuxdkE/edit#gid=2133439974

*/

#pragma once

#ifndef _DS1_OVERHAUL_OBJECT_PARAM_DEF_FILE_H_
    #define _DS1_OVERHAUL_OBJECT_PARAM_DEF_FILE_H_


#include "Param.h"


// Object parameter
typedef struct ObjectParameter : public Param {

    int16_t
        hp = -1; // Durability up to destruction (-1: not destructible)

    uint16_t
        defense = 0; // Attack power less than this value is no damage

    int16_t
        extRefTexId = -1, // MAA / mAA _ ????. Tpf (-1: none) (AA: area number)
        materialId = -1; // Material ID. The same treatment as floor material. When -1, the same behavior as before

    uint8_t
        animBreakIdMax = 0; // Anime destruction ID number from 0 to what number

    uint8_t
        isCamHit : 1, // Whether the camera is hit (0: not hit, 1: hit)
        isBreakByPlayerCollide : 1, // When the player touches it breaks (0: not, 1: it is)
        isAnimBreak : 1, // Animation destruction (0: physical destruction, 1: animation destruction)
        isPenetrationBulletHit : 1, // Whether a penetrating bullet hits (0: not hit, 1: hit)
        isChrHit : 1, // Whether the character hits it (0: not hit, 1: hit)
        isAttackBacklash : 1, // Do you play the attack (0: do not play, 1: play)
        isDisableBreakForFirstAppear : 1, // It is broken at the initial appearance of the player (0: 1, not 1)
        isLadder : 1, // Ladder is (0: different, 1: it is so)
        isAnimPauseOnRemoPlay : 1, // Whether to stop animating in poly play (0: do not, 1: do)
        isDamageNoHit : 1, // Damage is not hit (0: hit, 1: hit)
        isMoveObj : 1; // Is it a moving object (0: different, 1: so)

    uint8_t
        pad_1 : 5; // Padding

    int8_t
        defaultLodParamId = -1; // Default LOD param ID (-1: none)

    int32_t
        breakSfxId = -1; // SFXID at object destruction (-1: default (80))

} ObjectParam;



// Object parameter definitions file
class ObjectParamDef : public ParamDef {


public:
    static ObjectParamDef& get_instance()
    {
        static ObjectParamDef instance;
        return instance;
    }

    ObjectParam *data()
    {
        return (ObjectParam*)ParamDef::data();
    }

    ObjectParam *get(int index)
    {
        return (ObjectParam*)ParamDef::get(index);
    }


    // @TODO: Member data specific to this class

private:
    ObjectParamDef()
        : ParamDef(NULL, sizeof(ObjectParam),
                   //"D0 67 00 00 ?? ?? 01 00 01 00 ?? ?? 4F 42 4A 45",
                   "?? ?? ?? ?? ?? ?? 01 00 01 00 ?? ?? 4F 42 4A 45 43 54 5F 50 41 52 41 4D 5F 53 54 00",
                   "ObjectParam", "Object")
    {
    }

public:
    ObjectParamDef(ObjectParamDef const&) = delete;
    void operator=(ObjectParamDef const&) = delete;
};







#endif // _DS1_OVERHAUL_OBJECT_PARAM_DEF_FILE_H_
