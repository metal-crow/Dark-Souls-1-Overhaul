/*
    DARK SOULS GAME FILE C++ LIBRARY

    Contributors to this file:
        Sean Pesce  -  C++


    Param/KnockBack.h

    Knock Back Parameters from KnockBackParam.paramdef


    References:
        - Burton Radons' ParamDefs Sheet
          https://docs.google.com/spreadsheets/d/1KukblWL61We64-gNIyaAShga9h8RTXYmyFs98eQhY4E/

        - Kushisushi's ParamDefs Sheet
          https://docs.google.com/spreadsheets/d/1QX6i93H4_jGZ0So9mArDECnuv1lfkaJQhXOzkDuxdkE/edit#gid=2133439974

*/

#pragma once

#ifndef DS1_FILE_LIB_KNOCK_BACK_PARAM_DEF_FILE_H_
    #define DS1_FILE_LIB_KNOCK_BACK_PARAM_DEF_FILE_H_


#include "Param.h"


// KnockBack parameter
typedef struct KnockBackParameter : public Param {
    
    float
        damage_Min_ContTime,
        damage_S_ContTime,
        damage_M_ContTime,
        damage_L_ContTime,
        damage_BlowS_ContTime,
        damage_BlowM_ContTime,
        damage_Strike_ContTime,
        damage_Uppercut_ContTime,
        damage_Push_ContTime,
        damage_Breath_ContTime,
        damage_HeadShot_ContTime,
        guard_S_ContTime,
        guard_L_ContTime,
        guard_LL_ContTime,
        guardBrake_ContTime, // "Guard break"* ?
        damage_Min_DecTime,
        damage_S_DecTime,
        damage_M_DecTime,
        damage_L_DecTime,
        damage_BlowS_DecTime,
        damage_BlowM_DecTime,
        damage_Strike_DecTime,
        damage_Uppercut_DecTime,
        damage_Push_DecTime,
        damage_Breath_DecTime,
        damage_HeadShot_DecTime,
        guard_S_DecTime,
        guard_L_DecTime,
        guard_LL_DecTime,
        guardBrake_DecTime; // "Guard break"* ?

    uint8_t
        padding[8];

} KnockBackParam;



// KnockBack parameter definitions file
class KnockBackParamDef : public ParamDef {


public:
    static KnockBackParamDef& get_instance()
    {
        static KnockBackParamDef instance;
        return instance;
    }
    
    KnockBackParam *data()
    {
        return (KnockBackParam*)ParamDef::data();
    }

    KnockBackParam *get(int index)
    {
        return (KnockBackParam*)ParamDef::get(index);
    }


    // @TODO: Member data specific to this class
    
private:
    KnockBackParamDef()
        : ParamDef(NULL, sizeof(KnockBackParam),
                   "?? ?? ?? ?? ?? ?? 00 00 01 00 ?? ?? 4B 4E 4F 43 4B 42 41 43 4B 5F 50 41 52 41 4D 5F 53 54 00",
                   "KnockBackParam", "Knock-Back")
    {
    }
    
public:
    KnockBackParamDef(KnockBackParamDef const&) = delete;
    void operator=(KnockBackParamDef const&) = delete;
};







#endif // DS1_FILE_LIB_KNOCK_BACK_PARAM_DEF_FILE_H_
