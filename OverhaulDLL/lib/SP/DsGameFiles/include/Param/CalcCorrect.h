/*
    DARK SOULS GAME FILE C++ LIBRARY

    Contributors to this file:
        Sean Pesce    -    C++


    Param/CalcCorrect.h

    Calculation Correction Parameters from CalcCorrectParam.paramdef


    References:
        - Burton Radons' ParamDefs Sheet
          https://docs.google.com/spreadsheets/d/1KukblWL61We64-gNIyaAShga9h8RTXYmyFs98eQhY4E/

        - Kushisushi's ParamDefs Sheet
          https://docs.google.com/spreadsheets/d/1QX6i93H4_jGZ0So9mArDECnuv1lfkaJQhXOzkDuxdkE/edit#gid=2133439974

*/

#pragma once

#ifndef _DS1_OVERHAUL_CALC_CORRECT_PARAM_DEF_FILE_H_
    #define _DS1_OVERHAUL_CALC_CORRECT_PARAM_DEF_FILE_H_


#include "Param.h"


// CalcCorrect parameter
typedef struct CalcCorrectParameter : public Param {

    float
        stageMaxVal0 = 0, // Those whose specification states "nth-order threshold [point]"
        stageMaxVal1 = 0, // Those whose specification states "nth-order threshold [point]"
        stageMaxVal2 = 0, // Those whose specification states "nth-order threshold [point]"
        stageMaxVal3 = 0, // Those whose specification states "nth-order threshold [point]"
        stageMaxVal4 = 0, // Those whose specification states "nth-order threshold [point]"
        stageMaxGrowVal0 = 0, // Those whose specification states "nth order threshold [coefficient]"
        stageMaxGrowVal1 = 0, // Those whose specification states "nth order threshold [coefficient]"
        stageMaxGrowVal2 = 0, // Those whose specification states "nth order threshold [coefficient]"
        stageMaxGrowVal3 = 0, // Those whose specification states "nth order threshold [coefficient]"
        stageMaxGrowVal4 = 0, // Those whose specification states "nth order threshold [coefficient]"
        adjPt_maxGrowVal0 = 0, // Adjustment factor
        adjPt_maxGrowVal1 = 0, // Adjustment factor
        adjPt_maxGrowVal2 = 0, // Adjustment factor
        adjPt_maxGrowVal3 = 0, // Adjustment factor
        adjPt_maxGrowVal4 = 0, // Adjustment factor
        init_inclination_soul = 0, // Gradual slope {alpha}1 of the initial growth of Soul
        adjustment_value = 0, // Growth soul adjustment early in the year 2
        boundry_inclination_soul = 0, // Influence on slope of graph after growth soul threshold {alpha} 3
        boundry_value = 0; // Growth soul threshold t

    uint8_t
        pad[4] = { 0, 0, 0, 0 }; // Padding

} CalcCorrectParam;



// CalcCorrect parameter definitions file
class CalcCorrectParamDef : public ParamDef {


public:
    static CalcCorrectParamDef& get_instance()
    {
        static CalcCorrectParamDef instance;
        return instance;
    }

    CalcCorrectParam *data()
    {
        return (CalcCorrectParam*)ParamDef::data();
    }

    CalcCorrectParam *get(int index)
    {
        return (CalcCorrectParam*)ParamDef::get(index);
    }


    // @TODO: Member data specific to this class

private:
    CalcCorrectParamDef()
        : ParamDef(NULL, sizeof(CalcCorrectParam),
                   "90 09 00 00 ?? ?? 01 00 01 00 ?? ?? 43 41 43 4C",
                   "CalcCorrectParam", "Calculation Correction")
    {
    }

public:
    CalcCorrectParamDef(CalcCorrectParamDef const&) = delete;
    void operator=(CalcCorrectParamDef const&) = delete;
};







#endif // _DS1_OVERHAUL_CALC_CORRECT_PARAM_DEF_FILE_H_
