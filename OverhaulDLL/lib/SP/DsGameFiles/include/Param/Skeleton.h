/*
    DARK SOULS GAME FILE C++ LIBRARY

    Contributors to this file:
        Sean Pesce    -    C++


    Param/Skeleton.h

    Skeleton Parameters from SkeletonParam.paramdef


    References:
        - Burton Radons' ParamDefs Sheet
          https://docs.google.com/spreadsheets/d/1KukblWL61We64-gNIyaAShga9h8RTXYmyFs98eQhY4E/

        - Kushisushi's ParamDefs Sheet
          https://docs.google.com/spreadsheets/d/1QX6i93H4_jGZ0So9mArDECnuv1lfkaJQhXOzkDuxdkE/edit#gid=2133439974

*/

#pragma once

#ifndef _DS1_OVERHAUL_SKELETON_PARAM_DEF_FILE_H_
    #define _DS1_OVERHAUL_SKELETON_PARAM_DEF_FILE_H_


#include "Param.h"


// Skeleton parameter
typedef struct SkeletonParameter : public Param {

    float
        neckTurnGain = 0; // Head swing gain. Turn higher as soon as possible

    int16_t
        originalGroundHeightMS = 0, // Height from the origin to the ground [cm]
        minAnkleHeightMS = -30, // The lowest height at which you can raise your ankle [cm]
        maxAnkleHeightMS = 70, // Maximum height at which you can raise your ankle [cm]
        cosineMaxKneeAngle = -95, // The maximum angle at which the knee can be bent (cosine value)
        cosineMinKneeAngle = 55, // Minimum angle at which the knee can be bent
        footPlantedAnkleHeightMS = 1, // If the ankle is lower than this position it is assumed that the feet are stuck [cm]
        footRaisedAnkleHeightMS = 30, // If the ankle is higher than this position it is assumed that the foot is far away [cm]
        raycastDistanceUp = 70, // How far from the ankle to recast it [cm]
        raycastDistanceDown = 55, // How far from the ankle to recast it [cm]
        footEndLS_X = 0, // Toe position X [cm]
        footEndLS_Y = 0, // Toe position Y [cm]
        footEndLS_Z = 0, // Toe position Z [cm]
        onOffGain = 18, // Gain when turning foot foot on / off [1/100]
        groundAscendingGain = 100, // Gain when the ground level gets higher [1/100]
        groundDescendingGain = 100, // Gain when the height of the ground lowers [1/100]
        footRaisedGain = 20, // Gain when the foot goes up [1/100]
        footPlantedGain = 100, // Gain at foot contact [1/100]
        footUnlockGain = 80; // Gain when foot is locked / unlocked [1/100]

    uint8_t
        kneeAxisType = 4, // Axial direction of the knee
        useFootLocking = 0, // Do you lock the ankle / foot?
        footPlacementOn = 1, // Is foot footing effective?
        twistKneeAxisType = 1; // Axial direction ignoring rotation of knee joint for twisting

    int8_t
        neckTurnPriority = 0; // The lower the lower, the better. Does not swing by -1

    uint8_t
        neckTurnMaxAngle = 0; // Maximum turning angle of this joint. It will not stop beyond this angle

    uint8_t
        pad1[2] = { 0, 0 }; // Padding

} SkeletonParam;



// Skeleton parameter definitions file
class SkeletonParamDef : public ParamDef {


public:
    static SkeletonParamDef& get_instance()
    {
        static SkeletonParamDef instance;
        return instance;
    }

    SkeletonParam *data()
    {
        return (SkeletonParam*)ParamDef::data();
    }

    SkeletonParam *get(int index)
    {
        return (SkeletonParam*)ParamDef::get(index);
    }


    // @TODO: Member data specific to this class

private:
    SkeletonParamDef()
        : ParamDef(NULL, sizeof(SkeletonParam),
                   "E0 08 00 00 ?? ?? 01 00 01 00 ?? ?? 53 4B 45 4C",
                   "SkeletonParam", "Skeleton")
    {
    }

public:
    SkeletonParamDef(SkeletonParamDef const&) = delete;
    void operator=(SkeletonParamDef const&) = delete;
};







#endif // _DS1_OVERHAUL_SKELETON_PARAM_DEF_FILE_H_
