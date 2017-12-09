/*
    DARK SOULS GAME FILE C++ LIBRARY

    Contributors to this file:
        Sean Pesce    -    C++


    Param/Accessory.h

    Accessory Parameters from EquipAccessoryParam.paramdef


    References:
        - Burton Radons' ParamDefs Sheet
          https://docs.google.com/spreadsheets/d/1KukblWL61We64-gNIyaAShga9h8RTXYmyFs98eQhY4E/

        - Kushisushi's ParamDefs Sheet
          https://docs.google.com/spreadsheets/d/1QX6i93H4_jGZ0So9mArDECnuv1lfkaJQhXOzkDuxdkE/edit#gid=2133439974

*/

#pragma once

#ifndef _DS1_OVERHAUL_ACCESSORY_PARAM_DEF_FILE_H_
    #define _DS1_OVERHAUL_ACCESSORY_PARAM_DEF_FILE_H_


#include "Param.h"


// Accessory parameter
typedef struct AccessoryParameter : public Param {

    int32_t
        refId = -1, // ID to call from accessories
        sfxVariationId = -1; // Specify variation of SFX (Used to specify SFX in combination with ID of TimeActEditor)

    float
        weight = 1; // Weight [kg]

    int32_t
        behaviorId = 0, // Behavior ID (= Skill)
        basicPrice = 0, // Basic price
        sellValue = 0, // Selling price
        sortId = 0, // Sort ID
        qwcId = -1; // QWC ID

    uint16_t
        equipModelId = 0, // Equipment model number
        iconId = 0; // Menu icon ID

    int16_t
        shopLv = 0, // Level that can be sold at a store
        trophySGradeId = -1, // ??
        trophySeqId = -1; // Trophy's SEQ number

    uint8_t
        equipModelCategory = 0, // Equipment model type
        equipModelGender = 0, // Equipment model sex
        accessoryCategory = 0, // Armor category
        refCategory = 0, // Category of ID of dropping [attack, jumping tool, special]
        spEffectCategory = 0; // Since there are effects (such as enchant weapons) whose parameters fluctuate with skills, magic, items, etc., set the action for each action so that the specified effect can correspond to the effect of "power up only weapon attacks" For varistors, etc. that do not require setting, set "none"

    uint8_t
        pad = 0; // Padding

    int32_t
        vagrantItemLotId = 0, // -1: no beigrant 0: no drawing 1: ~ lottery
        vagrantBonusEneDropItemLotId = 0, // -1: No drop 0: no drawing 1: to be selected
        vagrantItemEneDropItemLotId = 0; // -1: No drop 0: no drawing 1: to be selected

    uint8_t
        isDeposit : 1, // Will it be deposited in the warehouse?
        isEquipOutBrake : 1, // Do you break when you equip it with it?
        disableMultiDropShare : 1; // Does multi-drop sharing prohibited?

    uint8_t
        pad1[3] = { 0, 0, 0 }; // Padding

} AccessoryParam;



// Accessory parameter definitions file
class AccessoryParamDef : public ParamDef {


public:
    static AccessoryParamDef& get_instance()
    {
        static AccessoryParamDef instance;
        return instance;
    }
    
    AccessoryParam *data()
    {
        return (AccessoryParam*)ParamDef::data();
    }

    AccessoryParam *get(int index)
    {
        return (AccessoryParam*)ParamDef::get(index);
    }


    // @TODO: Member data specific to this class
    
private:
    AccessoryParamDef()
        : ParamDef(NULL, sizeof(AccessoryParam),
                   "60 0C 00 00 ?? ?? 01 00 01 00 ?? ?? 45 51 55 49",
                   "EquipAccessoryParam", "Accessory")
    {
    }
    
public:
    AccessoryParamDef(AccessoryParamDef const&) = delete;
    void operator=(AccessoryParamDef const&) = delete;
};







#endif // _DS1_OVERHAUL_ACCESSORY_PARAM_DEF_FILE_H_
