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
};
#pragma pack(pop)

static const uint32_t RollbackPlayerInitPacketType = 2;

void SendPlayerInitPacket(uint64_t ConnectedPlayerData);
void RecvPlayerInitPacket(uint64_t ConnectedPlayerData);

#endif
