/*
    DARK SOULS GAME FILE C++ LIBRARY

    Contributors to this file:
        Sean Pesce    -    C++


    Param/Draw/ToneCorrectBank.h

    Tone Correction Bank Parameters from ToneCorrectBankParam.paramdef


    References:
        - Burton Radons' ParamDefs Sheet
          https://docs.google.com/spreadsheets/d/1KukblWL61We64-gNIyaAShga9h8RTXYmyFs98eQhY4E/

        - Kushisushi's ParamDefs Sheet
          https://docs.google.com/spreadsheets/d/1QX6i93H4_jGZ0So9mArDECnuv1lfkaJQhXOzkDuxdkE/edit#gid=2133439974

*/

#pragma once

#ifndef DS1_FILE_LIB_TONE_CORRECT_BANK_PARAM_DEF_FILE_H_
    #define DS1_FILE_LIB_TONE_CORRECT_BANK_PARAM_DEF_FILE_H_


#include "Param/Param.h"


// ToneCorrectBank parameter
typedef struct ToneCorrectBankParameter : public Param {

    float
        brightnessR = 1, // Luminance
        brightnessG = 1, // Luminance
        brightnessB = 1, // Luminance
        contrastR = 1, // contrast
        contrastG = 1, // contrast
        contrastB = 1, // contrast
        saturation = 1, // saturation
        hue = 0; // Hue

} ToneCorrectBankParam;



// ToneCorrectBank parameter definitions file
class ToneCorrectBankParamDef : public ParamDef {


public:
    static ToneCorrectBankParamDef& get_instance()
    {
        static ToneCorrectBankParamDef instance;
        return instance;
    }

    ToneCorrectBankParam *data()
    {
        return (ToneCorrectBankParam*)ParamDef::data();
    }

    ToneCorrectBankParam *get(int index)
    {
        return (ToneCorrectBankParam*)ParamDef::get(index);
    }


    // @TODO: Member data specific to this class

private:
    ToneCorrectBankParamDef()
        : ParamDef(NULL, sizeof(ToneCorrectBankParam),
                   "@TODO",
                   "ToneCorrectBankParam", "Tone Correction Bank")
    {
    }

public:
    ToneCorrectBankParamDef(ToneCorrectBankParamDef const&) = delete;
    void operator=(ToneCorrectBankParamDef const&) = delete;
};







#endif // DS1_FILE_LIB_TONE_CORRECT_BANK_PARAM_DEF_FILE_H_
