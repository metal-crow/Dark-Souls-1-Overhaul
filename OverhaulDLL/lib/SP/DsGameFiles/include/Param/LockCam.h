/*
    DARK SOULS GAME FILE C++ LIBRARY

    Contributors to this file:
        Sean Pesce	-	C++


    Param/LockCam.h

    LockCam Parameters from LockCamParam.paramdef


    References:
        - Burton Radons' ParamDefs Sheet
          https://docs.google.com/spreadsheets/d/1KukblWL61We64-gNIyaAShga9h8RTXYmyFs98eQhY4E/

        - Kushisushi's ParamDefs Sheet
          https://docs.google.com/spreadsheets/d/1QX6i93H4_jGZ0So9mArDECnuv1lfkaJQhXOzkDuxdkE/edit#gid=2133439974

*/

#pragma once

#ifndef DS1_FILE_LIB_LOCK_CAM_PARAM_DEF_FILE_H_
    #define DS1_FILE_LIB_LOCK_CAM_PARAM_DEF_FILE_H_


#include "Param.h"


// LockCam parameter
typedef struct LockCamParameter : public Param {
    float camDistTarget = 4.0f;           // For camera
    float rotRangeMinX = -40.0f;          // For camera
    float lockRotXShiftRatio = 0.6f;      // For camera
    float chrOrgOffset_Y = 1.42f;         // For camera
    float chrLockRangeMaxRadius = 15.0f;  // For lock
    float camFovY = 43.0f;                // For camera
    uint8_t pad[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
} LockCamParam;



// LockCam parameter definitions file
class LockCamParamDef : public ParamDef {


public:
    static LockCamParamDef& get_instance()
    {
        static LockCamParamDef instance;
        return instance;
    }

    LockCamParam *data()
    {
        return (LockCamParam*)ParamDef::data();
    }

    LockCamParam *get(int index)
    {
        return (LockCamParam*)ParamDef::get(index);
    }


    // @TODO: Member data specific to this class

private:
    LockCamParamDef()
        : ParamDef(NULL, /*0x1F8, 38,*/ sizeof(LockCamParam),
                   //"C0 06 00 00 ?? ?? 00 00 01 00 ?? ?? 4C 4F 43 4B",
                   "?? ?? ?? ?? ?? ?? 00 00 01 00 ?? ?? 4C 4F 43 4B 5F 43 41 4D 5F 50 41 52 41 4D 5F 53 54 00",
                   "LockCamParam", "Camera lock")
    {
    }

public:
    LockCamParamDef(LockCamParamDef const&) = delete;
    void operator=(LockCamParamDef const&) = delete;
};







#endif // DS1_FILE_LIB_LOCK_CAM_PARAM_DEF_FILE_H_
