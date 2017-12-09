/*
    DARK SOULS GAME FILE C++ LIBRARY

    Contributors to this file:
        Sean Pesce    -    C++


    Param/Draw/EnvLightTexBank.h

    Environment Lights Texture Bank Parameters as defined in EnvLightTexBankParam.paramdef


    References:
        - Burton Radons' ParamDefs Sheet
          https://docs.google.com/spreadsheets/d/1KukblWL61We64-gNIyaAShga9h8RTXYmyFs98eQhY4E/

        - Kushisushi's ParamDefs Sheet
          https://docs.google.com/spreadsheets/d/1QX6i93H4_jGZ0So9mArDECnuv1lfkaJQhXOzkDuxdkE/edit#gid=2133439974

*/

#pragma once

#ifndef _DS1_OVERHAUL_ENV_LIGHT_TEX_BANK_PARAM_DEF_FILE_H_
    #define _DS1_OVERHAUL_ENV_LIGHT_TEX_BANK_PARAM_DEF_FILE_H_


#include "Param/Param.h"


// EnvLightTexBank parameter
typedef struct EnvLightTexBankParameter : public Param {

    int8_t
        isUse = 0, // 0: Do not, 1: do
        autoUpdate = 0; // 0: Do not, 1: do

    uint8_t
        pad_0[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // dummy

    int16_t
        invMulCol = 100, // All light source colors are multiplied by the reciprocal of this value
        resNameId_Dif0 = 0, // [DIF 0] ID of texture file name (-1: none)
        invMulCol_Dif0 = 100; // [DIF 0] Multiply the light source color by the inverse of this value

    float
        sepcPow_Dif0 = 1; // [DIF 0] Specular order

    uint8_t
        pad_Dif0[8] = { 0, 0, 0, 0, 0, 0, 0, 0 }; // [DIF 0] Dummy

    int16_t
        resNameId_Spc0 = 0, // [SPC 0] ID of texture file name (-1: none)
        invMulCol_Spc0 = 100; // [SPC 0] Multiply the light source color by the inverse of this value

    float
        sepcPow_Spc0 = 1; // [SPC 0] Specular order

    uint8_t
        pad_Spc0[8] = { 0, 0, 0, 0, 0, 0, 0, 0 }; // [SPC 0] Dummy

    int16_t
        resNameId_Spc1 = 1, // [SPC1] ID of texture file name (-1: none)
        invMulCol_Spc1 = 100; // [SPC1] Multiply the source color by the inverse of this value

    float
        sepcPow_Spc1 = 1; // [SPC 1] Specular order

    uint8_t
        pad_Spc1[8] = { 0, 0, 0, 0, 0, 0, 0, 0 }; // [SPC 1] Dummy

    int16_t
        resNameId_Spc2 = 2, // [SPC 2] ID of texture file name (-1: none)
        invMulCol_Spc2 = 100; // [SPC 2] Multiply the source color by the inverse of this value

    float
        sepcPow_Spc2 = 1; // [SPC 2] Specular order

    uint8_t
        pad_Spc2[8] = { 0, 0, 0, 0, 0, 0, 0, 0 }; // [SPC 2] Dummy

    int16_t
        resNameId_Spc3 = 3, // [SPC 3] ID of texture file name (-1: none)
        invMulCol_Spc3 = 100; // [SPC 3] Multiply the light source color by the inverse of this value

    float
        sepcPow_Spc3 = 1; // [SPC 3] Specular order

    uint8_t
        pad_Spc3[8] = { 0, 0, 0, 0, 0, 0, 0, 0 }; // [SPC 3] Dummy

    int16_t
        degRotX_00 = 0, // Parallel light source: 00
        degRotY_00 = 0, // Parallel light source: 00
        colR_00 = 255, // Parallel light source: 00
        colG_00 = 255, // Parallel light source: 00
        colB_00 = 255, // Parallel light source: 00
        colA_00 = 100; // Parallel light source: 00 (0: not used)

    uint8_t
        pad_00[4] = { 0, 0, 0, 0 }; // Parallel light source: 00

    int16_t
        degRotX_01 = 0, // Parallel light source: 01
        degRotY_01 = 0, // Parallel light source: 01
        colR_01 = 255, // Parallel light source: 01
        colG_01 = 255, // Parallel light source: 01
        colB_01 = 255, // Parallel light source: 01
        colA_01 = 0; // Parallel light source: 01 (0: not used)

    uint8_t
        pad_01[4] = { 0, 0, 0, 0 }; // Parallel light source: 01

    int16_t
        degRotX_02 = 0, // Parallel light source: 02
        degRotY_02 = 0, // Parallel light source: 02
        colR_02 = 255, // Parallel light source: 02
        colG_02 = 255, // Parallel light source: 02
        colB_02 = 255, // Parallel light source: 02
        colA_02 = 0; // Parallel light source: 02 (0: not used)

    uint8_t
        pad_02[4] = { 0, 0, 0, 0 }; // Parallel light source: 02

    int16_t
        degRotX_03 = 0, // Parallel light source: 03
        degRotY_03 = 0, // Parallel light source: 03
        colR_03 = 255, // Parallel light source: 03
        colG_03 = 255, // Parallel light source: 03
        colB_03 = 255, // Parallel light source: 03
        colA_03 = 0; // Parallel light source: 03 (0: not used)

    uint8_t
        pad_03[4] = { 0, 0, 0, 0 }; // Parallel light source: 03

    int16_t
        degRotX_04 = 0, // Parallel light source: 04
        degRotY_04 = 0, // Parallel light source: 04
        colR_04 = 255, // Parallel light source: 04
        colG_04 = 255, // Parallel light source: 04
        colB_04 = 255, // Parallel light source: 04
        colA_04 = 0; // Parallel light source: 04 (0: not used)

    uint8_t
        pad_04[4] = { 0, 0, 0, 0 }; // Parallel light source: 04

    int16_t
        degRotX_05 = 0, // Parallel light source: 05
        degRotY_05 = 0, // Parallel light source: 05
        colR_05 = 255, // Parallel light source: 05
        colG_05 = 255, // Parallel light source: 05
        colB_05 = 255, // Parallel light source: 05
        colA_05 = 0; // Parallel light source: 05 (0: not used)

    uint8_t
        pad_05[4] = { 0, 0, 0, 0 }; // Parallel light source: 05

    int16_t
        degRotX_06 = 0, // Parallel light source: 06
        degRotY_06 = 0, // Parallel light source: 06
        colR_06 = 255, // Parallel light source: 06
        colG_06 = 255, // Parallel light source: 06
        colB_06 = 255, // Parallel light source: 06
        colA_06 = 0; // Parallel light source: 06 (0: not used)

    uint8_t
        pad_06[4] = { 0, 0, 0, 0 }; // Parallel light source: 06

    int16_t
        degRotX_07 = 0, // Parallel light source: 07
        degRotY_07 = 0, // Parallel light source: 07
        colR_07 = 255, // Parallel light source: 07
        colG_07 = 255, // Parallel light source: 07
        colB_07 = 255, // Parallel light source: 07
        colA_07 = 0; // Parallel light source: 07 (0: not used)

    uint8_t
        pad_07[4] = { 0, 0, 0, 0 }; // Parallel light source: 07

    int16_t
        degRotX_08 = 0, // Parallel light source: 08
        degRotY_08 = 0, // Parallel light source: 08
        colR_08 = 255, // Parallel light source: 08
        colG_08 = 255, // Parallel light source: 08
        colB_08 = 255, // Parallel light source: 08
        colA_08 = 0; // Parallel light source: 08 (0: not used)

    uint8_t
        pad_08[4] = { 0, 0, 0, 0 }; // Parallel light source: 08

    int16_t
        degRotX_09 = 0, // Parallel light source: 09
        degRotY_09 = 0, // Parallel light source: 09
        colR_09 = 255, // Parallel light source: 09
        colG_09 = 255, // Parallel light source: 09
        colB_09 = 255, // Parallel light source: 09
        colA_09 = 0; // Parallel light source: 09 (0: not used)

    uint8_t
        pad_09[4] = { 0, 0, 0, 0 }; // Parallel light source: 09

} EnvLightTexBankParam;



// EnvLightTexBank parameter definitions file
class EnvLightTexBankParamDef : public ParamDef {


public:
    static EnvLightTexBankParamDef& get_instance()
    {
        static EnvLightTexBankParamDef instance;
        return instance;
    }
    
    EnvLightTexBankParam *data()
    {
        return (EnvLightTexBankParam*)ParamDef::data();
    }

    EnvLightTexBankParam *get(int index)
    {
        return (EnvLightTexBankParam*)ParamDef::get(index);
    }


    // @TODO: Member data specific to this class
    
private:
    EnvLightTexBankParamDef()
        : ParamDef(NULL, sizeof(EnvLightTexBankParam), "@TODO", "EnvLightTexBankParam", "Environment Lights Texture Bank")
    {
    }
    
public:
    EnvLightTexBankParamDef(EnvLightTexBankParamDef const&) = delete;
    void operator=(EnvLightTexBankParamDef const&) = delete;
};







#endif // _DS1_OVERHAUL_ENV_LIGHT_TEX_BANK_PARAM_DEF_FILE_H_
