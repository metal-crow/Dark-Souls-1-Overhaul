/*
    DARK SOULS GAME FILE C++ LIBRARY

    Contributors to this file:
        Sean Pesce	-	C++


    Param/Armor.h


    EquipParamProtector

    Equipment (Armor) Parameters from EquipParamProtector.paramdef


    References:
        - Burton Radons' ParamDefs Sheet
          https://docs.google.com/spreadsheets/d/1KukblWL61We64-gNIyaAShga9h8RTXYmyFs98eQhY4E/

        - Kushisushi's ParamDefs Sheet
          https://docs.google.com/spreadsheets/d/1QX6i93H4_jGZ0So9mArDECnuv1lfkaJQhXOzkDuxdkE/edit#gid=2133439974

*/

#pragma once

#ifndef _DS1_FILE_LIB_ARMOR_PARAM_DEF_EQUIP_PROTECTOR_H_
    #define _DS1_FILE_LIB_ARMOR_PARAM_DEF_EQUIP_PROTECTOR_H_


#include "Param.h"


// EquipParamProtector data structure (Armor parameters)
typedef struct ArmorParameter : public Param { // Armor
    int32_t  sortId = -1;
    uint32_t wanderingEquipId = 950000; // "Replacement equipment ID for the wandering ghost."
    int32_t  vagrantItemLotId = -1; // "-1: no beigrant 0: no drawing 1: ~ lottery"
    int32_t  vagrantBonusEneDropItemLotId = -1; // "Vagrant Bonus Enemy Drop Item Lot ID" / "-1: No drop 0: no drawing 1: to be selected"
    int32_t  vagrantItemEneDropItemLotId = -1;  // "Vagrant Item  Enemy Drop Item Lot ID" / "-1: No drop 0: no drawing 1: to be selected"
    int32_t  repairPrice;
    int32_t  buyPrice; // "Basic price"
    int32_t  sellValue;
    float    weight; // kg
    // "Resident Special Effect Ids" are not in an array to avoid confusion because indexing starts at 1 (really FromSoft???)
    int32_t  residentSpEffectId = -1;
    int32_t  residentSpEffectId2 = -1;
    int32_t  residentSpEffectId3 = -1;
    int32_t  materialSetId = -1; // "Material parameter ID required to strengthen weapons"
    float    partsDamageRate = 1.0f; // "Site damage rate"
    float    corectSARecover; // "Correct Super Armor Recover" (Note: "Correct" is spelled wrong) / "Super armor recovery time correction value"
    // Key for Origin Equip Pro values:
    // 0="Enhancement source armor ID", 1="Uses reinforcement armor ID 1", 2="Uses reinforcement armor ID 2", 3="Uses reinforcement armor ID 3",
    // 4="Uses reinforcement armor ID 4", 5="Uses reinforcement armor ID 5", 6="Uses reinforcement armor ID 6", 7="Uses reinforcement armor ID 7",
    // 8="Uses reinforcement armor ID 8", 9="Uses reinforcement armor ID 9", 10="Uses reinforcement armor ID 10", 11="Uses reinforcement armor ID 11",
    // 12="Uses reinforcement armor ID 12", 13="Uses reinforcement armor ID 13", 14="Uses reinforcement armor ID 14", 15="Uses reinforcement armor ID 15"
    int32_t  originEquipPro[16] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
    float    faceScaleM_ScaleX;     // Male
    float    faceScaleM_ScaleZ;     // Male
    float    faceScaleM_MaxX = 1.0f; // Male
    float    faceScaleM_MaxZ = 1.0f; // Male
    float    faceScaleF_ScaleX;     // Female
    float    faceScaleF_ScaleZ;     // Female
    float    faceScaleF_MaxX = 1.0f; // Female
    float    faceScaleF_MaxZ = 1.0f; // Female
    int32_t  qwcId = -1; // "QWC parameter ID"
    uint16_t equipModelId = 1;
    uint16_t iconIdM; // "Male menu icon ID"
    uint16_t iconIdF; // "Female menu icon ID"
    uint16_t knockBack; // "Decrease value of knockback"
    uint16_t knockbackBounceRate; // "Knock back rebound rate"
    uint16_t durability; // "Initial durability"
    uint16_t durabilityMax; // "New article durability"
    int16_t  saDurability; // "Super armor endurance"
    uint16_t defenseFlickPower; // "Used to judge the rebound of enemy's attack"
    uint16_t defensePhysical; // Physical defense
    uint16_t defenseMagic; // Magic defense
    uint16_t defenseFire; // Fire defense
    uint16_t defenseThunder; // Lightning defense
    int16_t  defenseSlash; // Slash defense
    int16_t  defenseBlow; // Strike defense
    int16_t  defenseThrust; // Thrust defense
    uint16_t resistPoison; // Poison resistance
    uint16_t resistDisease; // Toxic resistance
    uint16_t resistBlood; // Bleed resistance
    uint16_t resistCurse; // Curse resistance
    int16_t  reinforceTypeId; // "Enhanced Type ID"
    int16_t  trophySGradeId = -1; // "Related to the trophy system?" / Something to do with achievements (?)
    int16_t  shopLv = -1; // "Level that can be sold at a store" / "Shop level" (?)
    uint8_t  knockbackParamId = 1; // "ID of parameter used for knockback"
    uint8_t  flickDamageCutRate = 255; // "Used for damage attenuation rate at repelling"
    uint8_t  equipModelCategory = 5; // "Type of equipment model"
    uint8_t  equipModelGender = 1; // "Gender of the equipment model"
    uint8_t  protectorCategory; // "Category of armor"
    uint8_t  defenseMaterial = 59; // "For SE for movement/defense" (SE = sfx/sound effects? Zullie armor sounds toggle script works by setting this to 0)
    uint8_t  defenseMaterialSfx = 59; // "Defense Material Sound Effects" / "For SFX when moving / defending"
    uint8_t  partsDmgType; // "Parts Damage Type" / "Set the attack type to perform site damage judgment"
    uint8_t  defenseMaterial_Weak = 29; // "For Weak Points Damaged SE" (SE = sfx/sound effects? Zullie armor sounds toggle script works by setting this to 0)
    uint8_t  defenseMaterialSfx_Weak = 29; // "Defense Material Sound Effects (Weak)" / "For Weak Points Damaged SE"
    // Flags:
    uint64_t
        isDeposit : 1, // "Will it be deposited in the warehouse?" / Denotes if item can be put into bottomless box?
        headEquip : 1, // "Head equipment?"
        bodyEquip : 1, // "Is it equipped with a cylinder?" / Body equipment
        armEquip : 1, // "Arm armor?" / Hands/gloves armor
        legEquip : 1, // "Is it a leg fitting?" / Pants/Leggings/Boots/Shoes/Foot armor
        useFaceScale : 1, // "Do you use face scales?"

    /*
        Key for invisibleFlag bits (incomplete):

        Each flag toggles rendering of a specific piece of clothing and/or body part.

        0="Bangs ahead", 1="Bottom of bangs", 2="Sideburns", 3="Head top", 4="Head top", 5="Back hair" (back of head?), 6="Behind the back", 7="?", 8="?",
        9="", 10="Collar", 11="Around the collar", 12="?", 13="?", 14="?", 15="Hem/no hem", 16="?", 17="?", 18="?", 19="?", 20="Sleeve A", 21="Sleeve B",
        22="?", 23="?", 24="?", 25="Arm", 26="?", 27="?", 28="?", 29="?", 30="Belt", 31="?", 32="?", 33="?", 34="?", 35="?", 36="?", 37="?", 38="?",
        39="?", 40="?", 41="?", ="?", 42="?", 43="?", 44="?", 45="?", 46="?", 47="?"
    */

    // Unfortunately C++ doesn't support bit fields in an array, so this will have to do
    invisibleFlag0:  1, invisibleFlag1 : 1, invisibleFlag2 : 1, invisibleFlag3 : 1, invisibleFlag4 : 1, invisibleFlag5 : 1, invisibleFlag6 : 1, invisibleFlag7 : 1, invisibleFlag8 : 1,
        invisibleFlag9 : 1, invisibleFlag10 : 1, invisibleFlag11 : 1, invisibleFlag12 : 1, invisibleFlag13 : 1, invisibleFlag14 : 1, invisibleFlag15 : 1, invisibleFlag16 : 1, invisibleFlag17 : 1,
        invisibleFlag18 : 1, invisibleFlag19 : 1, invisibleFlag20 : 1, invisibleFlag21 : 1, invisibleFlag22 : 1, invisibleFlag23 : 1, invisibleFlag24 : 1, invisibleFlag25 : 1, invisibleFlag26 : 1,
        invisibleFlag27 : 1, invisibleFlag28 : 1, invisibleFlag29 : 1, invisibleFlag30 : 1, invisibleFlag31 : 1, invisibleFlag32 : 1, invisibleFlag33 : 1, invisibleFlag34 : 1, invisibleFlag35 : 1,
        invisibleFlag36 : 1, invisibleFlag37 : 1, invisibleFlag38 : 1, invisibleFlag39 : 1, invisibleFlag40 : 1, invisibleFlag41 : 1, invisibleFlag42 : 1, invisibleFlag43 : 1, invisibleFlag44 : 1,
        invisibleFlag45 : 1, invisibleFlag46 : 1, invisibleFlag47 : 1,

        disableMultiDropShare : 1, // "Does multi-drop sharing prohibited?" / "Disable Multiplayer Drop Share" (?)
        simpleModelForDlc : 1, // "Does a simple model exist for DLC?"
        pad_0 : 8; // Unused padding bits
    int16_t  oldSortId = -1; // "Old sort ID (-1 = not collected)"
    uint8_t  pad_1[6] = { 0, 0, 0, 0, 0, 0 }; // Unused padding bits

} ArmorParam;



// Armor parameter definitions file
class ArmorParamDef : public ParamDef {


public:
    static ArmorParamDef& get_instance()
    {
        static ArmorParamDef instance; // Guaranteed to be destroyed
        return instance;
    }

    ArmorParam *data()
    {
        return (ArmorParam*)ParamDef::data();
    }

    ArmorParam *get(int index)
    {
        return (ArmorParam*)ParamDef::get(index);
    }


    // Returns pointer to the parameter struct with the given sortId, or NULL if no match was found
    ArmorParam *get_by_sort_id(int32_t sort_id)
    {
        if (data())
            for (int i = 0; i < (int)param_count; i++)
                if (get(i)->sortId == sort_id)
                    return get(i);
        return NULL;
    }

private:
    ArmorParamDef()
        : ParamDef(NULL, /*0xF60, 324,*/ sizeof(ArmorParam),
                   //"00 35 01 00 ?? ?? 01 00 02 00 ?? ?? 45 51 55 49",
                   "?? ?? ?? ?? ?? ?? 01 00 02 00 ?? ?? 45 51 55 49 50 5F 50 41 52 41 4D 5F 50 52 4F 54 45 43 54 4F 52 5F 53 54 00",
                   "EquipParamProtector", "Armor")
    {
    }

public:
    ArmorParamDef(ArmorParamDef const&) = delete;
    void operator=(ArmorParamDef const&) = delete;
};







#endif // _DS1_FILE_LIB_ARMOR_PARAM_DEF_EQUIP_PROTECTOR_H_
