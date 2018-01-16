/*
    DARK SOULS GAME FILE C++ LIBRARY

    Contributors to this file:
        Sean Pesce  -  C++


    Param/ObjectAction.h

    ObjectAction Parameters from ObjectActParam.paramdef


    References:
        - Burton Radons' ParamDefs Sheet
          https://docs.google.com/spreadsheets/d/1KukblWL61We64-gNIyaAShga9h8RTXYmyFs98eQhY4E/

        - Kushisushi's ParamDefs Sheet
          https://docs.google.com/spreadsheets/d/1QX6i93H4_jGZ0So9mArDECnuv1lfkaJQhXOzkDuxdkE/edit#gid=2133439974

*/

#pragma once

#ifndef DS1_FILE_LIB_OBJECT_ACTION_PARAM_DEF_FILE_H_
    #define DS1_FILE_LIB_OBJECT_ACTION_PARAM_DEF_FILE_H_


#include "Param.h"


// ObjectAction parameter
typedef struct ObjectActionParameter : public Param {

    int32_t
        actionEnableMsgId,
        actionFailedMsgId,
        spQualifiedPassEventFlag;

    uint16_t
        validDist,
        playerAnimId,
        chrAnimId,
        spQualifiedId,
        spQualifiedId2;

    uint8_t
        objDummyId,
        objAnimId,
        validPlayerAngle,
        spQualifiedType,  // OBJACT_SP_QUALIFIED_TYPE enum type
        spQualifiedType2, // OBJACT_SP_QUALIFIED_TYPE enum type
        validObjAngle,
        chrSorbType,      // OBJACT_CHR_SORB_TYPE enum type
        eventKickTiming;  // OBJACT_EVENT_KICK_TIMING enum type

    uint8_t
        padding[2];

} ObjectActionParam;



// ObjectAction parameter definitions file
class ObjectActionParamDef : public ParamDef {


public:
    static ObjectActionParamDef& get_instance()
    {
        static ObjectActionParamDef instance;
        return instance;
    }
    
    ObjectActionParam *data()
    {
        return (ObjectActionParam*)ParamDef::data();
    }

    ObjectActionParam *get(int index)
    {
        return (ObjectActionParam*)ParamDef::get(index);
    }


    // @TODO: Member data specific to this class
    
private:
    ObjectActionParamDef()
        : ParamDef(NULL, sizeof(ObjectActionParam),
                   "?? ?? ?? ?? ?? ?? 00 00 01 00 ?? ?? 4F 42 4A 5F 41 43 54 5F 50 41 52 41 4D 5F 53 54 00", 
                   "ObjectActParam", "Object Action")
    {
    }
    
public:
    ObjectActionParamDef(ObjectActionParamDef const&) = delete;
    void operator=(ObjectActionParamDef const&) = delete;
};







#endif // DS1_FILE_LIB_OBJECT_ACTION_PARAM_DEF_FILE_H_
