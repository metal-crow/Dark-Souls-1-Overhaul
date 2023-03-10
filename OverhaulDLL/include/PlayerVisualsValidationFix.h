#pragma once
#ifndef PLAYERVISUALSVALIDATIONFIX_H_
#define PLAYERVISUALSVALIDATIONFIX_H_

#include <cstdint>

class PlayerVisualsValidationFix
{
public:
    static void start();
    static void sanitizePacketData(uint8_t* data, uint32_t len);

private:
    static const uint64_t Validate_Type6FaceData_offset = 0x506e04;
};

#endif
