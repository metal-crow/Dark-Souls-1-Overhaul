/*
    DARK SOULS GAME FILE C++ LIBRARY

    Contributors to this file:
        Sean Pesce  -  C++


    Param/QwcJudge.h

    QwcJudge Parameters from QwcJudgeParam.paramdef


    References:
        - Burton Radons' ParamDefs Sheet
          https://docs.google.com/spreadsheets/d/1KukblWL61We64-gNIyaAShga9h8RTXYmyFs98eQhY4E/

        - Kushisushi's ParamDefs Sheet
          https://docs.google.com/spreadsheets/d/1QX6i93H4_jGZ0So9mArDECnuv1lfkaJQhXOzkDuxdkE/edit#gid=2133439974

*/

#pragma once

#ifndef DS1_FILE_LIB_QWC_JUDGE_PARAM_DEF_FILE_H_
    #define DS1_FILE_LIB_QWC_JUDGE_PARAM_DEF_FILE_H_


#include "Param.h"


// QwcJudge parameter
typedef struct QwcJudgeParameter : public Param {
    
    int16_t
        pcJudgeUnderWB,
        pcJudgeTopWB,
        pcJudgeUnderLR,
        pcJudgeTopLR,
        areaJudgeUnderWB,
        areaJudgeTopWB,
        areaJudgeUnderLR,
        areaJudgeTopLR;

} QwcJudgeParam;



// QwcJudge parameter definitions file
class QwcJudgeParamDef : public ParamDef {


public:
    static QwcJudgeParamDef& get_instance()
    {
        static QwcJudgeParamDef instance;
        return instance;
    }
    
    QwcJudgeParam *data()
    {
        return (QwcJudgeParam*)ParamDef::data();
    }

    QwcJudgeParam *get(int index)
    {
        return (QwcJudgeParam*)ParamDef::get(index);
    }


    // @TODO: Member data specific to this class
    
private:
    QwcJudgeParamDef()
        : ParamDef(NULL, sizeof(QwcJudgeParam),
                   "?? ?? ?? ?? ?? ?? 01 00 01 00 ?? ?? 51 57 43 5F 4A 55 44 47 45 5F 50 41 52 41 4D 5F 53 54 00", 
                   "QwcJudgeParam", "World Tendency Judge")
    {
    }
    
public:
    QwcJudgeParamDef(QwcJudgeParamDef const&) = delete;
    void operator=(QwcJudgeParamDef const&) = delete;
};







#endif // DS1_FILE_LIB_QWC_JUDGE_PARAM_DEF_FILE_H_
