/*
    DARK SOULS GAME FILE C++ LIBRARY

    Contributors to this file:
        Sean Pesce    -    C++


    Param/Draw/LightBank.h

    Light Bank Parameters as defined in LightBankParam.paramdef


    References:
        - Burton Radons' ParamDefs Sheet
          https://docs.google.com/spreadsheets/d/1KukblWL61We64-gNIyaAShga9h8RTXYmyFs98eQhY4E/

        - Kushisushi's ParamDefs Sheet
          https://docs.google.com/spreadsheets/d/1QX6i93H4_jGZ0So9mArDECnuv1lfkaJQhXOzkDuxdkE/edit#gid=2133439974

*/

#pragma once

#ifndef _DS1_OVERHAUL_LIGHT_BANK_PARAM_DEF_FILE_H_
    #define _DS1_OVERHAUL_LIGHT_BANK_PARAM_DEF_FILE_H_


#include "Param/Param.h"


// LightBank parameter
typedef struct LightBankParameter : public Param {

    int16_t
        degRotX_0 = 0, // Parallel light source: 0
        degRotY_0 = 0, // Parallel light source: 0
        colR_0 = 255, // Parallel light source: 0
        colG_0 = 255, // Parallel light source: 0
        colB_0 = 255, // Parallel light source: 0
        colA_0 = 100, // Parallel light source: 0
        degRotX_1 = 0, // Parallel light source: 1
        degRotY_1 = 0, // Parallel light source: 1
        colR_1 = 255, // Parallel light source: 1
        colG_1 = 255, // Parallel light source: 1
        colB_1 = 255, // Parallel light source: 1
        colA_1 = 100, // Parallel light source: 1
        degRotX_2 = 0, // Parallel light source: 2
        degRotY_2 = 0, // Parallel light source: 2
        colR_2 = 255, // Parallel light source: 2
        colG_2 = 255, // Parallel light source: 2
        colB_2 = 255, // Parallel light source: 2
        colA_2 = 100, // Parallel light source: 2
        colR_u = 255, // Ambient upper hemisphere
        colG_u = 255, // Ambient upper hemisphere
        colB_u = 255, // Ambient upper hemisphere
        colA_u = 100, // Ambient upper hemisphere
        colR_d = 255, // Ambient lower hemisphere
        colG_d = 255, // Ambient lower hemisphere
        colB_d = 255, // Ambient lower hemisphere
        colA_d = 100, // Ambient lower hemisphere
        degRotX_s = 0, // Parallel light source: specular
        degRotY_s = 0, // Parallel light source: specular
        colR_s = 255, // Parallel light source: specular
        colG_s = 255, // Parallel light source: specular
        colB_s = 255, // Parallel light source: specular
        colA_s = 0, // Parallel light source: specular
        envDif_colR = 255, // Environment light source: diffuse multiplication color
        envDif_colG = 255, // Environment light source: diffuse multiplication color
        envDif_colB = 255, // Environment light source: diffuse multiplication color
        envDif_colA = 100, // Environment light source: diffuse multiplication color
        envSpc_colR = 255, // Environment light source: Specular multiplication color
        envSpc_colG = 255, // Environment light source: Specular multiplication color
        envSpc_colB = 255, // Environment light source: Specular multiplication color
        envSpc_colA = 100, // Environment light source: Specular multiplication color
        envDif = 0, // Environment light source: diffuse texture ID
        envSpc_0 = 0, // Environment light source: specular 0 texture ID
        envSpc_1 = 0, // Environment light source: specular 1 texture ID
        envSpc_2 = 0, // Environment light source: Specular 2 texture ID
        envSpc_3 = 0; // Environment light source: specular 3 texture ID

    uint8_t
        pad[2] = { 0, 0 }; // Padding

} LightBankParam;



// LightBank parameter definitions file
class LightBankParamDef : public ParamDef {


public:
    static LightBankParamDef& get_instance()
    {
        static LightBankParamDef instance;
        return instance;
    }
    
    LightBankParam *data()
    {
        return (LightBankParam*)ParamDef::data();
    }

    LightBankParam *get(int index)
    {
        return (LightBankParam*)ParamDef::get(index);
    }


    // @TODO: Member data specific to this class
    
private:
    LightBankParamDef()
        : ParamDef(NULL, sizeof(LightBankParam),
                   "@TODO",
                   "LightBankParam", "Light Bank")
    {
    }
    
public:
    LightBankParamDef(LightBankParamDef const&) = delete;
    void operator=(LightBankParamDef const&) = delete;
};







#endif // _DS1_OVERHAUL_LIGHT_BANK_PARAM_DEF_FILE_H_
