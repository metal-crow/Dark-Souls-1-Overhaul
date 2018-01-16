/*
    DARK SOULS GAME FILE C++ LIBRARY

    Contributors to this file:
        Sean Pesce    -    C++


    Param/Draw/FogBank.h

    Fog Bank Parameters as defined in FogBank.paramdef


    References:
        - Burton Radons' ParamDefs Sheet
          https://docs.google.com/spreadsheets/d/1KukblWL61We64-gNIyaAShga9h8RTXYmyFs98eQhY4E/

        - Kushisushi's ParamDefs Sheet
          https://docs.google.com/spreadsheets/d/1QX6i93H4_jGZ0So9mArDECnuv1lfkaJQhXOzkDuxdkE/edit#gid=2133439974

*/

#pragma once

#ifndef DS1_FILE_LIB_FOG_BANK_PARAM_DEF_FILE_H_
    #define DS1_FILE_LIB_FOG_BANK_PARAM_DEF_FILE_H_


#include "Param/Param.h"


// FogBank parameter
typedef struct FogBankParameter : public Param {

    int16_t
        fogBeginZ = 0, // Starting distance of fog
        fogEndZ = 100, // End distance of fog
        degRotZ = 0, // Dummy (??)
        degRotW = 100, // Normally you should set it to 100 (0 means fog is not applied)
        colR = 255, // Fog color
        colG = 255, // Fog color
        colB = 255, // Fog color
        colA = 100; // Fog color magnification (100 is standard)

} FogBankParam;



// FogBank parameter definitions file
class FogBankParamDef : public ParamDef {


public:
    static FogBankParamDef& get_instance()
    {
        static FogBankParamDef instance;
        return instance;
    }

    FogBankParam *data()
    {
        return (FogBankParam*)ParamDef::data();
    }

    FogBankParam *get(int index)
    {
        return (FogBankParam*)ParamDef::get(index);
    }


    // @TODO: Member data specific to this class

private:
    FogBankParamDef()
        : ParamDef(NULL, sizeof(FogBankParam),
                   "@TODO",
                   "FogBank", "Fog Bank")
    {
    }

public:
    FogBankParamDef(FogBankParamDef const&) = delete;
    void operator=(FogBankParamDef const&) = delete;
};







#endif // DS1_FILE_LIB_FOG_BANK_PARAM_DEF_FILE_H_
