#pragma once
#ifndef NETCODEPACKETS_H
#define NETCODEPACKETS_H

#include <stddef.h>
#include <stdint.h>

#pragma pack(push, 1)
typedef struct PlayerInitPacket PlayerInitPacket;
struct PlayerInitPacket
{
    float position_x;
    float position_z;
    float position_y;
    float rotation;
    uint32_t curHp;
    uint32_t baseMaxHp;
    uint32_t curSp;
    uint32_t baseMaxSp;
    int32_t player_num;
    uint8_t player_sex;
    uint8_t covenantId;
    float type10_unk1;
    float type10_unk2;
    float type10_unk3;
    float type10_unk4;
    float type10_unk5;
    uint16_t type11_flags;
    unsigned int left_hand_slot_selected : 1;
    unsigned int right_hand_slot_selected : 1;
    uint32_t equipment_array[20];
};
#pragma pack(pop)

static const uint32_t RollbackPlayerInitPacketType = 2;

void SendPlayerInitPacket();
void RecvPlayerInitPacket(uint64_t ConnectedPlayerData);

#endif
