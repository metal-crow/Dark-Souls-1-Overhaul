/*
    DARK SOULS GAME FILE C++ LIBRARY

    Contributors to this file:
        Sean Pesce  -  C++


    Param/FaceGen.h

    FaceGen Parameters from FaceGenParam.paramdef


    References:
        - Burton Radons' ParamDefs Sheet
          https://docs.google.com/spreadsheets/d/1KukblWL61We64-gNIyaAShga9h8RTXYmyFs98eQhY4E/

        - Kushisushi's ParamDefs Sheet
          https://docs.google.com/spreadsheets/d/1QX6i93H4_jGZ0So9mArDECnuv1lfkaJQhXOzkDuxdkE/edit#gid=2133439974

*/

#pragma once

#ifndef _DS1_OVERHAUL_FACE_GENERATION_PARAM_DEF_FILE_H_
    #define _DS1_OVERHAUL_FACE_GENERATION_PARAM_DEF_FILE_H_


#include "Param.h"


// FaceGen parameter
typedef struct FaceGenParameter : public Param {
    uint8_t
        faceGeoData[50],
        faceTexData[50],
        hairStyle      = 0,
        hairColor_Base = 0,
        hairColor_R    = 0,
        hairColor_G    = 0,
        hairColor_B    = 0,
        eyeColor_R     = 0,
        eyeColor_G     = 0,
        eyeColor_B     = 0,
        padding[20];
} FaceGenParam;



// FaceGen parameter definitions file
class FaceGenParamDef : public ParamDef {


public:
    static FaceGenParamDef& get_instance()
    {
        static FaceGenParamDef instance;
        return instance;
    }
    
    FaceGenParam *data()
    {
        return (FaceGenParam*)ParamDef::data();
    }

    FaceGenParam *get(int index)
    {
        return (FaceGenParam*)ParamDef::get(index);
    }


    // @TODO: Member data specific to this class
    
private:
    FaceGenParamDef()
        : ParamDef(NULL, sizeof(FaceGenParam),
                   //"10 56 00 00 ?? ?? 00 00 01 00 ?? ?? 46 41 43 45",
                   "?? ?? ?? ?? ?? ?? 00 00 01 00 ?? ?? 46 41 43 45 5F 50 41 52 41 4D 5F 53 54 00",
                   "FaceGenParam", "Face Generation")
    {
    }
    
public:
    FaceGenParamDef(FaceGenParamDef const&) = delete;
    void operator=(FaceGenParamDef const&) = delete;
};







#endif // _DS1_OVERHAUL_FACE_GENERATION_PARAM_DEF_FILE_H_
