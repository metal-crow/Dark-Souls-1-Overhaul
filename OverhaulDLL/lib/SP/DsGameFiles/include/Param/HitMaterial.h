/*
    DARK SOULS GAME FILE C++ LIBRARY

    Contributors to this file:
        Sean Pesce  -  C++


    Param/HitMaterial.h

    Hit Material Parameters from HitMtrlParam.paramdef


    References:
        - Burton Radons' ParamDefs Sheet
          https://docs.google.com/spreadsheets/d/1KukblWL61We64-gNIyaAShga9h8RTXYmyFs98eQhY4E/

        - Kushisushi's ParamDefs Sheet
          https://docs.google.com/spreadsheets/d/1QX6i93H4_jGZ0So9mArDECnuv1lfkaJQhXOzkDuxdkE/edit#gid=2133439974

*/

#pragma once

#ifndef DS1_FILE_LIB_HIT_MATERIAL_PARAM_DEF_FILE_H_
    #define DS1_FILE_LIB_HIT_MATERIAL_PARAM_DEF_FILE_H_


#include "Param.h"


// HitMaterial parameter
typedef struct HitMaterialParameter : public Param {
    
    float
        aiVolumeRate;

    int32_t
        spEffectIdOnHit0,
        spEffectIdOnHit1;

    uint8_t
        footEffectHeightType : 2, // HMP_FOOT_EFFECT_HEIGHT_TYPE enum type
        footEffectDirType : 2,    // HMP_FOOT_EFFECT_DIR_TYPE enum type
        floorHeightType : 2,      // HMP_FLOOR_HEIGHT_TYPE enum type
        padding0 : 2;

    uint8_t
        padding[3];

} HitMaterialParam;



// HitMaterial parameter definitions file
class HitMaterialParamDef : public ParamDef {


public:
    static HitMaterialParamDef& get_instance()
    {
        static HitMaterialParamDef instance;
        return instance;
    }
    
    HitMaterialParam *data()
    {
        return (HitMaterialParam*)ParamDef::data();
    }

    HitMaterialParam *get(int index)
    {
        return (HitMaterialParam*)ParamDef::get(index);
    }


    // @TODO: Member data specific to this class
    
private:
    HitMaterialParamDef()
        : ParamDef(NULL, sizeof(HitMaterialParam),
                   "?? ?? ?? ?? ?? ?? 00 00 02 00 ?? ?? 48 49 54 5F 4D 54 52 4C 5F 50 41 52 41 4D 5F 53 54 00",
                   "HitMtrlParam", "Hit Material")
    {
    }
    
public:
    HitMaterialParamDef(HitMaterialParamDef const&) = delete;
    void operator=(HitMaterialParamDef const&) = delete;
};







#endif // DS1_FILE_LIB_HIT_MATERIAL_PARAM_DEF_FILE_H_
