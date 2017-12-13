/*
    DARK SOULS GAME FILE C++ LIBRARY

    Contributors to this file:
        Sean Pesce	-	C++


    Param/MenuColor.h

    Menu Color Parameters from MenuColorParam.paramdef


    References:
        - Burton Radons' ParamDefs Sheet
          https://docs.google.com/spreadsheets/d/1KukblWL61We64-gNIyaAShga9h8RTXYmyFs98eQhY4E/

        - Kushisushi's ParamDefs Sheet
          https://docs.google.com/spreadsheets/d/1QX6i93H4_jGZ0So9mArDECnuv1lfkaJQhXOzkDuxdkE/edit#gid=2133439974

*/

#pragma once

#ifndef _DS1_FILE_LIB_MENU_COLOR_TABLE_PARAM_DEF_FILE_H_
    #define _DS1_FILE_LIB_MENU_COLOR_TABLE_PARAM_DEF_FILE_H_


#include "Param.h"


// MenuColor parameter
typedef struct MenuColorParameter : public Param {

    uint8_t
        R = 0, // Red
        G = 0, // Green
        B = 0, // Blue
        A = 0; // Alpha

} MenuColorParam;



// MenuColor parameter definitions file
class MenuColorParamDef : public ParamDef {


public:
    static MenuColorParamDef& get_instance()
    {
        static MenuColorParamDef instance;
        return instance;
    }

    MenuColorParam *data()
    {
        return (MenuColorParam*)ParamDef::data();
    }

    MenuColorParam *get(int index)
    {
        return (MenuColorParam*)ParamDef::get(index);
    }


    // @TODO: Member data specific to this class

private:
    MenuColorParamDef()
        : ParamDef(NULL, /*0x3FC, 81,*/ sizeof(MenuColorParam),
                   "40 05 00 00 ?? ?? 01 00 01 00 ?? ?? 4D 45 4E 55",
                   "MenuColorTableParam", "Menu Color")
    {
    }

public:
    MenuColorParamDef(MenuColorParamDef const&) = delete;
    void operator=(MenuColorParamDef const&) = delete;
};







#endif // _DS1_FILE_LIB_MENU_COLOR_TABLE_PARAM_DEF_FILE_H_
