/*
    DARK SOULS GAME FILE C++ LIBRARY

    Contributors to this file:
        Sean Pesce  -  C++


    Param/QwcChange.h

    QwcChange Parameters from QwcChangeParam.paramdef


    References:
        - Burton Radons' ParamDefs Sheet
          https://docs.google.com/spreadsheets/d/1KukblWL61We64-gNIyaAShga9h8RTXYmyFs98eQhY4E/

        - Kushisushi's ParamDefs Sheet
          https://docs.google.com/spreadsheets/d/1QX6i93H4_jGZ0So9mArDECnuv1lfkaJQhXOzkDuxdkE/edit#gid=2133439974

*/

#pragma once

#ifndef DS1_FILE_LIB_QWC_CHANGE_PARAM_DEF_FILE_H_
    #define DS1_FILE_LIB_QWC_CHANGE_PARAM_DEF_FILE_H_


#include "Param.h"


// QwcChange parameter
typedef struct QwcChangeParameter : public Param {
    
    int16_t
        pcAttrB,
        pcAttrW,
        pcAttrL,
        pcAttrR,
        areaAttrB,
        areaAttrW,
        areaAttrL,
        areaAttrR;

} QwcChangeParam;



// QwcChange parameter definitions file
class QwcChangeParamDef : public ParamDef {


public:
    static QwcChangeParamDef& get_instance()
    {
        static QwcChangeParamDef instance;
        return instance;
    }
    
    QwcChangeParam *data()
    {
        return (QwcChangeParam*)ParamDef::data();
    }

    QwcChangeParam *get(int index)
    {
        return (QwcChangeParam*)ParamDef::get(index);
    }


    // @TODO: Member data specific to this class
    
private:
    QwcChangeParamDef()
        : ParamDef(NULL, sizeof(QwcChangeParam),
                   "?? ?? ?? ?? ?? ?? 01 00 01 00 ?? ?? 51 57 43 5F 43 48 41 4E 47 45 5F 50 41 52 41 4D 5F 53 54 00", 
                   "QwcChangeParam", "World Tendency Change")
    {
    }
    
public:
    QwcChangeParamDef(QwcChangeParamDef const&) = delete;
    void operator=(QwcChangeParamDef const&) = delete;
};







#endif // DS1_FILE_LIB_QWC_CHANGE_PARAM_DEF_FILE_H_
