/*
    DARK SOULS GAME FILE C++ LIBRARY

    Contributors to this file:
        Sean Pesce    -    C++


    Param/GameArea.h

    Game Area Parameters from GameAreaParam.paramdef


    References:
        - Burton Radons' ParamDefs Sheet
          https://docs.google.com/spreadsheets/d/1KukblWL61We64-gNIyaAShga9h8RTXYmyFs98eQhY4E/

        - Kushisushi's ParamDefs Sheet
          https://docs.google.com/spreadsheets/d/1QX6i93H4_jGZ0So9mArDECnuv1lfkaJQhXOzkDuxdkE/edit#gid=2133439974

*/

#pragma once

#ifndef DS1_FILE_LIB_GAME_AREA_PARAM_DEF_FILE_H_
    #define DS1_FILE_LIB_GAME_AREA_PARAM_DEF_FILE_H_


#include "Param.h"


// GameArea parameter
typedef struct GameAreaParameter : public Param {

    uint32_t
        bonusSoul_single = 0, // Seoul amount that can be acquired when an area boss is defeated (at single play)
        bonusSoul_multi = 0; // The amount of soul that can be acquired when an area boss is defeated (in multiplayer)

    int32_t
        humanityPointCountFlagIdTop = -1; // Head flag ID for managing humanity drop point (using 20 Bit)

    uint16_t
        humanityDropPoint1 = -1, // Threshold 1 for acquiring human nature
        humanityDropPoint2 = -1, // Threshold 2 for acquiring human nature
        humanityDropPoint3 = -1, // Threshold for acquiring humanity 3
        humanityDropPoint4 = -1, // Threshold for acquiring humanity 4
        humanityDropPoint5 = -1, // Threshold for acquiring humanity 5
        humanityDropPoint6 = -1, // Threshold for acquiring human nature 6
        humanityDropPoint7 = -1, // Threshold for acquiring human nature 7
        humanityDropPoint8 = -1, // Threshold 8 for acquiring human nature
        humanityDropPoint9 = -1, // Threshold for acquiring human nature 9
        humanityDropPoint10 = -1; // Threshold for acquiring humanity 10

} GameAreaParam;



// GameArea parameter definitions file
class GameAreaParamDef : public ParamDef {


public:
    static GameAreaParamDef& get_instance()
    {
        static GameAreaParamDef instance;
        return instance;
    }

    GameAreaParam *data()
    {
        return (GameAreaParam*)ParamDef::data();
    }

    GameAreaParam *get(int index)
    {
        return (GameAreaParam*)ParamDef::get(index);
    }


    // @TODO: Member data specific to this class

private:
    GameAreaParamDef()
        : ParamDef(NULL, sizeof(GameAreaParam),
                   //"44 08 00 00 ?? ?? 00 00 01 00 ?? ?? 47 41 4D 45",
                   "?? ?? ?? ?? ?? ?? 00 00 01 00 ?? ?? 47 41 4D 45 5F 41 52 45 41 5F 50 41 52 41 4D 5F 53 54 00",
                   "GameAreaParam", "Game Area")
    {
    }

public:
    GameAreaParamDef(GameAreaParamDef const&) = delete;
    void operator=(GameAreaParamDef const&) = delete;
};







#endif // DS1_FILE_LIB_GAME_AREA_PARAM_DEF_FILE_H_
