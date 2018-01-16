/*
    DARK SOULS GAME FILE C++ LIBRARY

    Contributors to this file:
        Sean Pesce    -    C++


    Param/Draw/DofBank.h

    Depth of Field Bank Parameters as defined in DofBank.paramdef


    There are 11 DofBank Parambnds, stored in the following DCX files:
        /param/DrawParam/a10_DrawParam.parambnd.dcx
        /param/DrawParam/a11_DrawParam.parambnd.dcx
        /param/DrawParam/a12_DrawParam.parambnd.dcx
        /param/DrawParam/a13_DrawParam.parambnd.dcx
        /param/DrawParam/a14_DrawParam.parambnd.dcx
        /param/DrawParam/a15_DrawParam.parambnd.dcx     // a15 contains 2 DofBank param files: m15_DoFBank.param and m15_1_DoFBank.param
        /param/DrawParam/a16_DrawParam.parambnd.dcx
        /param/DrawParam/a17_DrawParam.parambnd.dcx
        /param/DrawParam/a18_DrawParam.parambnd.dcx
        /param/DrawParam/default_DrawParam.parambnd.dcx

    There is also a 12th DofBank Parambnd, but its contents don't
    appear to get loaded into memory. It can be found in:
        /param/DrawParam/a99_DrawParam.parambnd.dcx


    All 12 *_DofBank.param files start with the same 56 bytes.

    @TODO: Create a derived ParamDef class for DrawParams (currenty this library
    only supports GameParams)


    References:
        - Burton Radons' ParamDefs Sheet
          https://docs.google.com/spreadsheets/d/1KukblWL61We64-gNIyaAShga9h8RTXYmyFs98eQhY4E/

        - Kushisushi's ParamDefs Sheet
          https://docs.google.com/spreadsheets/d/1QX6i93H4_jGZ0So9mArDECnuv1lfkaJQhXOzkDuxdkE/edit#gid=2133439974

*/

#pragma once

#ifndef DS1_FILE_LIB_DOF_BANK_PARAM_DEF_FILE_H_
    #define DS1_FILE_LIB_DOF_BANK_PARAM_DEF_FILE_H_


#include "Param/Param.h"


// DoF Bank parameter
typedef struct DofBankParameter : public Param {

    float
        farDofBegin = 60,  // Distance at which depth of field begins blurring far away
        farDofEnd = 360; // Distance at which depth of field is far away at depth of field

    uint8_t
        farDofMul = 100; // Depth of field blurring (0 will not blur)

    uint8_t
        pad_0[3] = { 0, 0, 0 }; // Padding

    float
        nearDofBegin = 3, // Distance at which the depth of field is close to blur starts (It is farther than the end distance)
        nearDofEnd = 0; // Distance at which the depth of field is close to the depth of field (closer than the starting distance)

    uint8_t
        nearDofMul = 100; // Depth of field blurring (0 will not blur)

    uint8_t
        pad_1[3] = { 0, 0, 0 }; // Padding

    float
        dispersionSq = 5; // Increasing the value increases the depth of field's blur

} DofBankParam;


/*

        Unique AoBs:

        default_DoFBank.param:
        "30 0A 00 00 ?? ?? 00 00  01 00 ?? ?? 44 4F 46 5F
         42 41 4E 4B 00 20 20 20  20 20 20 20 20 20 20 20
         20 20 20 20 20 20 20 20  20 20 20 20 00 02 00 00
         00 00 00 00 30 03 00 00  00 00 00 00 01 00 00 00
         4C 03 00 00 00 00 00 00  02 00 00 00 68 03 00 00"


         m10_DoFBank.param:
         "30 0A 00 00 ?? ?? 00 00  01 00 ?? ?? 44 4F 46 5F
          42 41 4E 4B 00 20 20 20  20 20 20 20 20 20 20 20
          20 20 20 20 20 20 20 20  20 20 20 20 00 02 00 00
          00 00 00 00 30 03 00 00  30 0A 00 00 01 00 00 00
          4C 03 00 00 00 00 00 00  02 00 00 00 68 03 00 00
          33"


         m11_DoFBank.param:
          "30 0A 00 00 ?? ?? 00 00  01 00 ?? ?? 44 4F 46 5F
           42 41 4E 4B 00 20 20 20  20 20 20 20 20 20 20 20
           20 20 20 20 20 20 20 20  20 20 20 20 00 02 00 00
           00 00 00 00 30 03 00 00  30 0A 00 00 01 00 00 00
           4C 03 00 00 37 0A 00 00  02 00 00 00 68 03 00 00
           3E 0A 00 00 03 00 00 00  84 03 00 00 00 00 00 00
           04 00 00 00 A0 03 00 00  00 00 00 00 05 00 00 00
           BC 03 00 00 00 00 00 00  06 00 00 00 D8 03 00 00
           00 00 00 00 07 00 00 00  F4 03 00 00 00 00 00 00
           08 00 00 00 10 04 00 00  00 00 00 00 09 00 00 00
           2C 04 00 00 00 00 00 00  0A 00 00 00 48 04 00 00
           00 00 00 00 0B 00 00 00  64 04 00 00 00 00 00 00
           0C 00 00 00 80 04 00 00  00 00 00 00 0D 00 00 00
           9C 04 00 00 00 00 00 00  0E 00 00 00 B8 04 00 00
           00 00 00 00 0F 00 00 00  D4 04 00 00 00 00 00 00
           10 00 00 00 F0 04 00 00  00 00 00 00 11 00 00 00
           0C 05 00 00 00 00 00 00  12 00 00 00 28 05 00 00
           00 00 00 00 13 00 00 00  44 05 00 00 49 0A 00 00
           14 00 00 00 60 05 00 00  00"


         m12_DoFBank.param:
         "30 0A 00 00 ?? ?? 00 00  01 00 ?? ?? 44 4F 46 5F
          42 41 4E 4B 00 20 20 20  20 20 20 20 20 20 20 20
          20 20 20 20 20 20 20 20  20 20 20 20 00 02 00 00
          00 00 00 00 30 03 00 00  30 0A 00 00 01 00 00 00
          4C 03 00 00 00 00 00 00  02 00 00 00 68 03 00 00
          00 00 00 00 03 00 00 00  84 03 00 00 00 00 00 00
          04 00 00 00 A0 03 00 00  00 00 00 00 05 00 00 00
          BC 03 00 00 00 00 00 00  06 00 00 00 D8 03 00 00
          00 00 00 00 07 00 00 00  F4 03 00 00 00 00 00 00
          08 00 00 00 10 04 00 00  00 00 00 00 09 00 00 00
          2C 04 00 00 00 00 00 00  0A 00 00 00 48 04 00 00
          00"


         m13_DoFBank.param:
         "30 0A 00 00 ?? ?? 00 00  01 00 ?? ?? 44 4F 46 5F
          42 41 4E 4B 00 20 20 20  20 20 20 20 20 20 20 20
          20 20 20 20 20 20 20 20  20 20 20 20 00 02 00 00
          00 00 00 00 30 03 00 00  30 0A 00 00 01 00 00 00
          4C 03 00 00 00 00 00 00  02 00 00 00 68 03 00 00
          00 00 00 00 03 00 00 00  84 03 00 00 37 0A"


         m14_DoFBank.param:
         "30 0A 00 00 ?? ?? 00 00  01 00 ?? ?? 44 4F 46 5F
          42 41 4E 4B 00 20 20 20  20 20 20 20 20 20 20 20
          20 20 20 20 20 20 20 20  20 20 20 20 00 02 00 00
          00 00 00 00 30 03 00 00  00 00 00 00 01 00 00 00
          4C 03 00 00 30"


         m15_DoFBank.param:
         "30 0A 00 00 ?? ?? 00 00  01 00 ?? ?? 44 4F 46 5F
          42 41 4E 4B 00 20 20 20  20 20 20 20 20 20 20 20
          20 20 20 20 20 20 20 20  20 20 20 20 00 02 00 00
          00 00 00 00 30 03 00 00  30 0A 00 00 01 00 00 00
          4C 03 00 00 00 00 00 00  02 00 00 00 68 03 00 00
          00 00 00 00 03 00 00 00  84 03 00 00 00 00 00 00
          04 00 00 00 A0 03 00 00  00 00 00 00 05 00 00 00
          BC 03 00 00 00 00 00 00  06 00 00 00 D8 03 00 00
          00 00 00 00 07 00 00 00  F4 03 00 00 00 00 00 00
          08 00 00 00 10 04 00 00  00 00 00 00 09 00 00 00
          2C 04 00 00 00 00 00 00  0A 00 00 00 48 04 00 00
          37 0A 00 00 0B 00 00 00  64 04 00 00 3E 0A 00 00
          0C 00 00 00 80 04 00 00  4D 0A 00 00 0D 00 00 00
          9C 04 00 00 00 00 00 00  0E 00 00 00 B8 04 00 00
          00 00 00 00 0F 00 00 00  D4 04 00 00 00 00 00 00
          10 00 00 00 F0 04 00 00  00 00 00 00 11 00 00 00
          0C 05 00 00 00 00 00 00  12 00 00 00 28 05 00 00
          00 00 00 00 13 00 00 00  44 05 00 00 00 00 00 00
          14 00 00 00 60 05 00 00  00 00 00 00 15 00 00 00
          7C 05 00 00 00 00 00 00  16 00 00 00 98 05 00 00
          00 00 00 00 17 00 00 00  B4 05 00 00 00 00 00 00
          18 00 00 00 D0 05 00 00  00 00 00 00 19 00 00 00
          EC 05 00 00 61 0A 00 00  1A 00 00 00 08 06 00 00
          71"


          m15_1_DoFBank.param:
          "30 0A 00 00 ?? ?? 00 00  01 00 ?? ?? 44 4F 46 5F
           42 41 4E 4B 00 20 20 20  20 20 20 20 20 20 20 20
           20 20 20 20 20 20 20 20  20 20 20 20 00 02 00 00
           00 00 00 00 30 03 00 00  30 0A 00 00 01 00 00 00
           4C 03 00 00 00 00 00 00  02 00 00 00 68 03 00 00
           00 00 00 00 03 00 00 00  84 03 00 00 00 00 00 00
           04 00 00 00 A0 03 00 00  00 00 00 00 05 00 00 00
           BC 03 00 00 00 00 00 00  06 00 00 00 D8 03 00 00
           00 00 00 00 07 00 00 00  F4 03 00 00 00 00 00 00
           08 00 00 00 10 04 00 00  00 00 00 00 09 00 00 00
           2C 04 00 00 00 00 00 00  0A 00 00 00 48 04 00 00
           37 0A 00 00 0B 00 00 00  64 04 00 00 3E 0A 00 00
           0C 00 00 00 80 04 00 00  4D 0A 00 00 0D 00 00 00
           9C 04 00 00 00 00 00 00  0E 00 00 00 B8 04 00 00
           00 00 00 00 0F 00 00 00  D4 04 00 00 00 00 00 00
           10 00 00 00 F0 04 00 00  00 00 00 00 11 00 00 00
           0C 05 00 00 00 00 00 00  12 00 00 00 28 05 00 00
           00 00 00 00 13 00 00 00  44 05 00 00 00 00 00 00
           14 00 00 00 60 05 00 00  00 00 00 00 15 00 00 00
           7C 05 00 00 00 00 00 00  16 00 00 00 98 05 00 00
           00 00 00 00 17 00 00 00  B4 05 00 00 00 00 00 00
           18 00 00 00 D0 05 00 00  00 00 00 00 19 00 00 00
           EC 05 00 00 61 0A 00 00  1A 00 00 00 08 06 00 00
           6E"


         m16_DoFBank.param:
         "30 0A 00 00 ?? ?? 00 00  01 00 ?? ?? 44 4F 46 5F
          42 41 4E 4B 00 20 20 20  20 20 20 20 20 20 20 20
          20 20 20 20 20 20 20 20  20 20 20 20 00 02 00 00
          00 00 00 00 30 03 00 00  30 0A 00 00 01 00 00 00
          4C 03 00 00 37 0A 00 00  02 00 00 00 68 03 00 00
          3E 0A 00 00 03 00 00 00  84 03 00 00 00 00 00 00
          04 00 00 00 A0 03 00 00  00 00 00 00 05 00 00 00
          BC 03 00 00 00 00 00 00  06 00 00 00 D8 03 00 00
          00 00 00 00 07 00 00 00  F4 03 00 00 00 00 00 00
          08 00 00 00 10 04 00 00  00 00 00 00 09 00 00 00
          2C 04 00 00 00 00 00 00  0A 00 00 00 48 04 00 00
          00 00 00 00 0B 00 00 00  64 04 00 00 00 00 00 00
          0C 00 00 00 80 04 00 00  00 00 00 00 0D 00 00 00
          9C 04 00 00 00 00 00 00  0E 00 00 00 B8 04 00 00
          00 00 00 00 0F 00 00 00  D4 04 00 00 00 00 00 00
          10 00 00 00 F0 04 00 00  00 00 00 00 11 00 00 00
          0C 05 00 00 00 00 00 00  12 00 00 00 28 05 00 00
          00 00 00 00 13 00 00 00  44 05 00 00 49 0A 00 00
          14 00 00 00 60 05 00 00  56"


         m17_DoFBank.param:
         "30 0A 00 00 ?? ?? 00 00  01 00 ?? ?? 44 4F 46 5F
          42 41 4E 4B 00 20 20 20  20 20 20 20 20 20 20 20
          20 20 20 20 20 20 20 20  20 20 20 20 00 02 00 00
          00 00 00 00 30 03 00 00  30 0A 00 00 01 00 00 00
          4C 03 00 00 37 0A 00 00  02 00 00 00 68 03 00 00
          3E 0A 00 00 03 00 00 00  84 03 00 00 43"


         m18_DoFBank.param:
         "30 0A 00 00 ?? ?? 00 00  01 00 ?? ?? 44 4F 46 5F
          42 41 4E 4B 00 20 20 20  20 20 20 20 20 20 20 20
          20 20 20 20 20 20 20 20  20 20 20 20 00 02 00 00
          00 00 00 00 30 03 00 00  30 0A 00 00 01 00 00 00
          4C 03 00 00 33"



                  NOTE: m99_DoFBank.param does not seem to
                  get loaded into the game's memory
         m99_DoFBank.param:
         "30 0A 00 00 ?? ?? 00 00  01 00 ?? ?? 44 4F 46 5F
          42 41 4E 4B 00 20 20 20  20 20 20 20 20 20 20 20
          20 20 20 20 20 20 20 20  20 20 20 20 00 02 00 00
          00 00 00 00 30 03 00 00  30 0A 00 00 01 00 00 00
          4C 03 00 00 39"

*/


// DofBank parameter definitions file
class DofBankParamDef : public ParamDef {


public:
    static DofBankParamDef& get_instance()
    {
        static DofBankParamDef instance;
        return instance;
    }

    DofBankParam *data()
    {
        return (DofBankParam*)ParamDef::data();
    }

    DofBankParam *get(int index)
    {
        return (DofBankParam*)ParamDef::get(index);
    }


    // @TODO: Member data specific to this class

private:
    // There are 11 DoF Bank param files

   /*DofBankParamDef()
       : ParamDef(NULL, sizeof(DofBankParam), @TODO, "default_DofBankParam", "Depth of Field Bank (Default)")
   {
   }*/
    DofBankParamDef()
        : ParamDef(NULL, sizeof(DofBankParam), "@TODO", "@TODO_DofBankParam", "Depth of Field Bank (@TODO)")
    {
    }

public:
    DofBankParamDef(DofBankParamDef const&) = delete;
    void operator=(DofBankParamDef const&) = delete;
};







#endif // DS1_FILE_LIB_DOF_BANK_PARAM_DEF_FILE_H_
