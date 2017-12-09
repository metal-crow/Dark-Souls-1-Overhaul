/*
    DARK SOULS GAME FILE C++ LIBRARY

    Contributors to this file:
        Sean Pesce    -    C++


    Param/Draw/PointLightBank.h

    Point Light Bank Parameters as defined in PointLightBank.paramdef


    References:
        - Burton Radons' ParamDefs Sheet
          https://docs.google.com/spreadsheets/d/1KukblWL61We64-gNIyaAShga9h8RTXYmyFs98eQhY4E/

        - Kushisushi's ParamDefs Sheet
          https://docs.google.com/spreadsheets/d/1QX6i93H4_jGZ0So9mArDECnuv1lfkaJQhXOzkDuxdkE/edit#gid=2133439974

*/

#pragma once

#ifndef _DS1_OVERHAUL_POINT_LIGHT_BANK_PARAM_DEF_FILE_H_
    #define _DS1_OVERHAUL_POINT_LIGHT_BANK_PARAM_DEF_FILE_H_


#include "Param/Param.h"


// PointLightBank parameter
typedef struct PointLightBankParameter : public Param {

    float
        dwindleBegin = 0.5, // Point light source
        dwindleEnd   = 2;   // Point light source

    int16_t
        colR = 255, // Point light source
        colG = 255, // Point light source
        colB = 255, // Point light source
        colA = 100; // Point light source

} PointLightBankParam;



// PointLightBank parameter definitions file
class PointLightBankParamDef : public ParamDef {


public:
    static PointLightBankParamDef& get_instance()
    {
        static PointLightBankParamDef instance;
        return instance;
    }
    
    PointLightBankParam *data()
    {
        return (PointLightBankParam*)ParamDef::data();
    }

    PointLightBankParam *get(int index)
    {
        return (PointLightBankParam*)ParamDef::get(index);
    }


    // @TODO: Member data specific to this class
    
private:
    PointLightBankParamDef()
        : ParamDef(NULL, sizeof(PointLightBankParam),
                   "@TODO",
                   "_PointLightBank", "Point Light Bank")
    {
    }
    
public:
    PointLightBankParamDef(PointLightBankParamDef const&) = delete;
    void operator=(PointLightBankParamDef const&) = delete;
};







#endif // _DS1_OVERHAUL_POINT_LIGHT_BANK_PARAM_DEF_FILE_H_
