/*
    DARK SOULS GAME FILE C++ LIBRARY

    Contributors to this file:
        Sean Pesce	-	C++


    Param/Item.h

    Item Parameters from EquipParamGoods.paramdef


    References:
        - Burton Radons' ParamDefs Sheet
          https://docs.google.com/spreadsheets/d/1KukblWL61We64-gNIyaAShga9h8RTXYmyFs98eQhY4E/

        - Kushisushi's ParamDefs Sheet
          https://docs.google.com/spreadsheets/d/1QX6i93H4_jGZ0So9mArDECnuv1lfkaJQhXOzkDuxdkE/edit#gid=2133439974

*/

#pragma once

#ifndef _DS1_FILE_LIB_GOODS_PARAM_DEF_FILE_H_
    #define _DS1_FILE_LIB_GOODS_PARAM_DEF_FILE_H_


#include "Param.h"


// Goods parameter
typedef struct ItemParameter : public Param {

    int32_t
        refId = -1, // ID called from item
        sfxVariationId = -1; // Specify variation of SFX (Used to specify SFX in combination with ID of TimeActEditor)

    float
        weight = 1; // Weight [kg]

    int32_t
        basicPrice = 0, // Basic price
        sellValue = 0, // Selling price
        behaviorId = 0, // Set the effect that occurs when using tools
        replaceItemId = -1, // Item ID when replacing
        sortId = 0, // Sort ID (-1: not collected)
        qwcId = -1, // QWC parameters iD
        yesNoDialogMessageId = -1, // Yes No Message ID to use when displaying the dialog
        magicId = -1; // Magic ID tied to scrolls

    uint16_t
        iconId = 0, // Icon ID for menu
        modelId = 0; // Model ID

    int16_t
        shopLv = 0, // Level that can be sold at a store
        compTrophySedId = -1, // SEQ number of complete trophy
        trophySeqId = -1, // Trophy's SEQ number
        maxNum = 0; // Maximum possession number

    uint8_t
        consumeHeroPoint = 0, // Consumer humanity
        overDexterity = 0, // Skill over start value
        goodsType = 0, // Tool type
        refCategory = 0, // Category of ID of Bullet [attack, jumping tool, special]
        spEffectCategory = 0, // Since there are effects (such as enchant weapons) whose parameters fluctuate with skills, magic, items, etc., set the action for each action so that the specified effect can correspond to the effect of "power up only weapon attacks" For varistors, etc. that do not require setting, set "none"
        goodsCategory = 0, // Tool category
        goodsUseAnim = 0, // Set an animation to play when using tools
        opmeMenuType = 0, // Menu type to open when using items
        useLimitCategory = 0, // Designated to control whether it can be used depending on the special effect being applied
        replaceCategory = 0; // Condition category to be added to the call ID

    uint8_t
        vowType0 : 1,	// Can it be used with pledge 0
        vowType1 : 1,	// Can it be used under pledge 1
        vowType2 : 1,	// Can it be used in pledge 2
        vowType3 : 1,	// Can it be used in pledge 3
        vowType4 : 1,	// Can it be used under pledge 4
        vowType5 : 1,	// Can it be used in pledge 5
        vowType6 : 1,	// Can it be used under pledge 6
        vowType7 : 1,	// Can it be used with pledge 7
        vowType8 : 1,	// Is it possible to use it at pledge 8
        vowType9 : 1,	// Is it possible to use it at pledge 9
        vowType10 : 1,	// Is it possible to use with the pledge 10
        vowType11 : 1,	// Is it possible to use with pledge 11
        vowType12 : 1,	// Can it be used with pledge 12
        vowType13 : 1,	// Is it possible to use with the pledge 13
        vowType14 : 1,	// Can it be used under pledge 14
        vowType15 : 1,	// Can it be used with pledge 15
        enable_live : 1,	// Can surviving players be used?
        enable_gray : 1,	// Can you use gray ghost?
        enable_white : 1,	// Is it possible to use white ghost?
        enable_black : 1,	// Is it possible to black ghost
        enable_multi : 1,	// Can it be used during multiplayer play?
        disable_offline : 1,	// Is it unusable while offline?
        isEquip : 1,	// Whether you can equip it
        isConsume : 1,	// Will it be consumed when used (whether the number of possessions is reduced)
        isAutoEquip : 1,	// Do you want to equip it automatically when you pick it up?
        isEstablishment : 1,	// Is it an installation type item?
        isOnlyOne : 1,	// Is it possible to have only one item
        isDrop : 1,	// Do you throw items away? TRUE = discarded
        isDeposit : 1,	// Will it be deposited in the warehouse?
        isDisableHand : 1,	// Can not use if the right hand weapon is bare hands
        IsTravelItem : 1,	// It is used to judge items to travel
        isSuppleItem : 1,	// It is used to determine replenishable items
        isFullSuppleItem : 1,	// Used to determine the replenished item
        isEnhance : 1,	// Do you enchant with weapons?
        isFixItem : 1,	// Is it an item to be repaired?
        disableMultiDropShare : 1,	// Does multi-drop sharing prohibited?
        disableUseAtColiseum : 1,	// Is it prohibited to use in the field?
        disableUseAtOutOfColiseum : 1;	// Is it prohibited to use outside the battlefield?

    uint8_t
        pad[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };	// Padding

    int32_t
        vagrantItemLotId = 0, // -1: no beigrant 0: no drawing 1: ~ lottery
        vagrantBonusEneDropItemLotId = 0, // -1: No drop 0: no drawing 1: to be selected
        vagrantItemEneDropItemLotId = 0; // -1: No drop 0: no drawing 1: to be selected

} ItemParam;



// Goods parameter definitions file
class ItemParamDef : public ParamDef {


public:
    static ItemParamDef& get_instance()
    {
        static ItemParamDef instance;
        return instance;
    }

    ItemParam *data()
    {
        return (ItemParam*)ParamDef::data();
    }

    ItemParam *get(int index)
    {
        return (ItemParam*)ParamDef::get(index);
    }


    // @TODO: Member data specific to this class

private:
    ItemParamDef()
        : ParamDef(NULL, /*0xC30, 256,*/ sizeof(ItemParam),
                   "30 68 00 00 ?? ?? 01 00 01 00 ?? ?? 45 51 55 49",
                   "EquipParamGoods", "Item")
    {
    }

public:
    ItemParamDef(ItemParamDef const&) = delete;
    void operator=(ItemParamDef const&) = delete;
};







#endif // _DS1_FILE_LIB_GOODS_PARAM_DEF_FILE_H_
