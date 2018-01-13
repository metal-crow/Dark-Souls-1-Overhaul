/*
    DARK SOULS GAME FILE C++ LIBRARY

    Contributors to this file:
        Sean Pesce	-	C++


    Param/MaterialSet.h

    Material Set Parameters from EquipMtrlSetParam.paramdef


    References:
        - Burton Radons' ParamDefs Sheet
          https://docs.google.com/spreadsheets/d/1KukblWL61We64-gNIyaAShga9h8RTXYmyFs98eQhY4E/

        - Kushisushi's ParamDefs Sheet
          https://docs.google.com/spreadsheets/d/1QX6i93H4_jGZ0So9mArDECnuv1lfkaJQhXOzkDuxdkE/edit#gid=2133439974

*/

#pragma once

#ifndef _DS1_FILE_LIB_EQUP_MATERIAL_SET_PARAM_DEF_FILE_H_
    #define _DS1_FILE_LIB_EQUP_MATERIAL_SET_PARAM_DEF_FILE_H_


#include "Param.h"


// MaterialSet parameter
typedef struct MaterialSetParameter : public Param {

    int32_t
        materialId01 = -1,
        materialId02 = -1,
        materialId03 = -1,
        materialId04 = -1,
        materialId05 = -1;

    int8_t
        itemNum01 = -1,
        itemNum02 = -1,
        itemNum03 = -1,
        itemNum04 = -1,
        itemNum05 = -1;

    uint8_t
        isDisableDispNum01 : 1,
        isDisableDispNum02 : 1,
        isDisableDispNum03 : 1,
        isDisableDispNum04 : 1,
        isDisableDispNum05 : 1,
        pad0 : 3;

    uint8_t
        pad[6] = { 0, 0, 0, 0, 0, 0 };

} MaterialSetParam;



// MaterialSet parameter definitions file
class MaterialSetParamDef : public ParamDef {


public:
    static MaterialSetParamDef& get_instance()
    {
        static MaterialSetParamDef instance;
        return instance;
    }

    MaterialSetParam *data()
    {
        return (MaterialSetParam*)ParamDef::data();
    }

    MaterialSetParam *get(int index)
    {
        return (MaterialSetParam*)ParamDef::get(index);
    }


    // @TODO: Member data specific to this class

private:
    MaterialSetParamDef()
        : ParamDef(NULL, /*0xD20, 276,*/ sizeof(MaterialSetParam),
                   //"A0 2F 00 00 ?? ?? 00 00 01 00 ?? ?? 45 51 55 49",
                   "?? ?? ?? ?? ?? ?? 00 00 01 00 ?? ?? 45 51 55 49 50 5F 4D 54 52 4C 5F 53 45 54 5F 50 41 52 41 4D 5F 53 54 00",
                   "EquipMtrlSetParam", "Material Set")
    {
    }

public:
    MaterialSetParamDef(MaterialSetParamDef const&) = delete;
    void operator=(MaterialSetParamDef const&) = delete;
};







#endif // _DS1_FILE_LIB_EQUP_MATERIAL_SET_PARAM_DEF_FILE_H_
