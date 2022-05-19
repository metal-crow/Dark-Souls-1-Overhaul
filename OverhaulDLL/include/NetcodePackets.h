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
    int16_t maxHp_mod;
    uint32_t walkanimtwist_unk;
    float rotation; //just use the normal rotation for this, not the weird version the game uses
    uint32_t type1_unk1;
    uint8_t type1_unk2;
    //Type 10
    int32_t player_num;
    uint8_t player_sex;
    uint8_t covenantId;
    uint16_t type10_unk1;
    uint32_t equipment_array[20];
    float type10_unk2[5];
    //Type 11
    uint16_t flags;
    uint16_t node_num; //TODO is this just player_num?
    //Type 16
    PosRotFloatVec attacker_position;
    PosRotFloatVec defender_position;
    uint16_t entitynum_defender;
    uint16_t entitynum_attacker;
    uint32_t throw_id;
    uint32_t type16_unk1;
    //Type 17
    uint16_t curSelectedMagicId;
    uint32_t curUsingItemId;
    //Type 34
    uint32_t spEffectToApply; //apply to the playerid that sent this
    uint32_t timestamp;
};
#pragma pack(pop)

#endif
