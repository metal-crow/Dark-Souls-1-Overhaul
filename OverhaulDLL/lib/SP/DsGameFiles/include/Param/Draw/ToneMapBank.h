/*
    DARK SOULS GAME FILE C++ LIBRARY

    Contributors to this file:
        Sean Pesce    -    C++


    Param/Draw/ToneMapBank.h

    Tone Map Bank Parameters from ToneMapBankParam.paramdef


    References:
        - Burton Radons' ParamDefs Sheet
          https://docs.google.com/spreadsheets/d/1KukblWL61We64-gNIyaAShga9h8RTXYmyFs98eQhY4E/

        - Kushisushi's ParamDefs Sheet
          https://docs.google.com/spreadsheets/d/1QX6i93H4_jGZ0So9mArDECnuv1lfkaJQhXOzkDuxdkE/edit#gid=2133439974

*/

#pragma once

#ifndef _DS1_OVERHAUL_TONE_MAP_BANK_PARAM_DEF_FILE_H_
    #define _DS1_OVERHAUL_TONE_MAP_BANK_PARAM_DEF_FILE_H_


#include "Param/Param.h"


// ToneMapBank parameter
typedef struct ToneMapBankParameter : public Param {
    
    int8_t
        bloomBegin = 50, // Bleeding begins when the brightness exceeds the threshold (near)
        bloomMul = 50, // It is a value to multiply the value above the threshold value (it will not be smeared with 0) (vicinity)
        bloomBeginFar = 50, // Bleeding begins when the luminance exceeds the threshold (far)
        bloomMulFar = 50; // It is a value to multiply the value above the threshold value (it will not be smeared with 0) (far)

    float
        bloomNearDist = 20, // Neighborhood parameters are applied up to this distance. (M)
        bloomFarDist = 60, // Parameters far from this distance will be applied. (M)
        grayKeyValue = 0.18f, // If this value is large the screen brightens and if it is small the screen gets darker
        minAdaptedLum = 0.1f, // Decreasing the value makes it adaptable to a dark place
        maxAdapredLum = 0.2f, // Increasing the value makes it adaptable to a bright place
        adaptSpeed = 1; // Increasing the value The speed of light adaptation and dark adaptation will be faster

    int8_t
        lightShaftBegin = 50; // When the luminance exceeds the threshold value, light streaks appear

    uint8_t
        pad_0[3] = { 0, 0, 0 }; // Padding

    float
        lightShaftPower = 0, // It is the strength of the light shaft. (It disappears with 0)
        lightShaftAttenRate = 0.95f; // It is the decay rate of the light shaft. Shrinking it becomes shorter

} ToneMapBankParam;



// ToneMapBank parameter definitions file
class ToneMapBankParamDef : public ParamDef {


public:
    static ToneMapBankParamDef& get_instance()
    {
        static ToneMapBankParamDef instance;
        return instance;
    }
    
    ToneMapBankParam *data()
    {
        return (ToneMapBankParam*)ParamDef::data();
    }

    ToneMapBankParam *get(int index)
    {
        return (ToneMapBankParam*)ParamDef::get(index);
    }


    // @TODO: Member data specific to this class
    
private:
    ToneMapBankParamDef()
        : ParamDef(NULL, sizeof(ToneMapBankParam),
                   "@TODO",
                   "ToneMapBank", "Tone Map")
    {
    }
    
public:
    ToneMapBankParamDef(ToneMapBankParamDef const&) = delete;
    void operator=(ToneMapBankParamDef const&) = delete;
};







#endif // _DS1_OVERHAUL_TONE_MAP_BANK_PARAM_DEF_FILE_H_
