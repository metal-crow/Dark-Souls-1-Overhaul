/*
    DARK SOULS GAME FILE C++ LIBRARY

    Contributors to this file:
        Sean Pesce	-	C++


    Param/ShopLineup.h

    ShopLineup Parameters from ShopLineupParam.paramdef


    References:
        - Burton Radons' ParamDefs Sheet
          https://docs.google.com/spreadsheets/d/1KukblWL61We64-gNIyaAShga9h8RTXYmyFs98eQhY4E/

        - Kushisushi's ParamDefs Sheet
          https://docs.google.com/spreadsheets/d/1QX6i93H4_jGZ0So9mArDECnuv1lfkaJQhXOzkDuxdkE/edit#gid=2133439974

*/

#pragma once

#ifndef DS1_FILE_LIB_SHOP_LINEUP_PARAM_DEF_FILE_H_
    #define DS1_FILE_LIB_SHOP_LINEUP_PARAM_DEF_FILE_H_


#include "Param.h"


// ShopLineup parameter
typedef struct ShopLineupParameter : public Param {

    int32_t
        equipId = 0, // ID of the equipment being sold
        value = -1, // Selling price
        mtrlId = -1, // Material ID required for purchase
        eventFlag = -1, // The event flag value holding the number
        qwcId = -1; // ID of QWC parameter

    int16_t
        sellQuantity = -1; // Number of items sold

    uint8_t
        shopType = 0, // Shop type
        equipType = 0; // Types of equipment being sold

    uint8_t pad_0[8] = { 0, 0, 0, 0, 0, 0, 0, 0 }; // Padding

} ShopLineupParam;



// ShopLineup parameter definitions file
class ShopLineupParamDef : public ParamDef {


public:
    static ShopLineupParamDef& get_instance()
    {
        static ShopLineupParamDef instance;
        return instance;
    }

    ShopLineupParam *data()
    {
        return (ShopLineupParam*)ParamDef::data();
    }

    ShopLineupParam *get(int index)
    {
        return (ShopLineupParam*)ParamDef::get(index);
    }


    // @TODO: Member data specific to this class

private:
    ShopLineupParamDef()
        : ParamDef(NULL, /*0x1284, 391,*/ sizeof(ShopLineupParam),
                   //"70 43 00 00 ?? ?? 01 00 01 00 ?? ?? 53 48 4F 50",
                   "?? ?? ?? ?? ?? ?? 01 00 01 00 ?? ?? 53 48 4F 50 5F 4C 49 4E 45 55 50 5F 50 41 52 41 4D 00",
                   "ShopLineupParam", "Shop Lineup")
    {
    }

public:
    ShopLineupParamDef(ShopLineupParamDef const&) = delete;
    void operator=(ShopLineupParamDef const&) = delete;
};







#endif // DS1_FILE_LIB_SHOP_LINEUP_PARAM_DEF_FILE_H_
