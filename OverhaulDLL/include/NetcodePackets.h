#pragma once
#ifndef NETCODEPACKETS_H
#define NETCODEPACKETS_H

#include <stddef.h>
#include <stdint.h>

typedef struct MainPacket MainPacket;
typedef struct PosRotFloatVec PosRotFloatVec;

#pragma pack(push, 1)
struct PosRotFloatVec
{
    float X;
    float Z;
    float Y;
    float Rotation;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct MainPacket
{
    //Type 1
    float position_x;
    float position_y;
    float position_z;
    uint32_t ezStateActiveState;
    uint32_t ezStatePassiveState;
    int16_t curHp;
    int16_t maxHp; //just use the normal max hp for this
    float rotation; //just use the normal rotation for this, not the weird version the game uses
    float atkAngle;
    float movement_direction_vals[2]; //this can theoretically be compressed, we don't need the full float range
    //Type 10
    int32_t player_num;
    uint8_t player_sex;
    uint8_t covenantId;
    uint32_t equipment_array[20];
    float type10_unk1;
    float type10_unk2;
    float type10_unk3;
    float type10_unk4;
    float type10_unk5;
    //Type 11
    uint16_t flags;
    //Type 16
    PosRotFloatVec attacker_position;
    PosRotFloatVec defender_position;
    uint16_t entitynum_defender;
    uint16_t entitynum_attacker;
    uint32_t throw_id; //if this is -1, don't use any of these vals
    //Type 17
    uint16_t curSelectedMagicId;
    uint32_t curUsingItemId;
    //Type 76
    uint32_t spEffectToApply[15]; //apply to the playerid that sent this
};
#pragma pack(pop)

#endif
