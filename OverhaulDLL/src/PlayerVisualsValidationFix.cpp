#include "PlayerVisualsValidationFix.h"
#include "GameData.h"
#include "SP/memory/injection/asm/x64.h"
#include "sp/memory.h"

void PlayerVisualsValidationFix::start()
{
    ConsoleWrite("Enabling PlayerVisualsValidationFix...");

    uint8_t* write_address;

    //inject code to skip validating the player face/hair/body data
    uint8_t Validate_Type6FaceData_patch[] = { 0xB0, 0x1, 0x90, 0x90, 0x90 }; //mov al, 1
    write_address = (uint8_t*)(PlayerVisualsValidationFix::Validate_Type6FaceData_offset + Game::ds1_base);
    sp::mem::patch_bytes(write_address, Validate_Type6FaceData_patch, 5);
}

float clamp(float d, float min, float max)
{
    const float t = d < min ? min : d;
    return t > max ? max : t;
}

//if the player we're sending to doesn't have the mod, clamp the hair color, eye color, and body proportions to valid amounts
void PlayerVisualsValidationFix::sanitizePacketData(uint8_t* data, uint32_t len)
{
    //type 6
    if (data[1] == 6 && len==0x120+8+1)
    {
        uint8_t* type6pktdata = data + 8 + 1;

        float* hair = (float*)(type6pktdata + 0x90);
        hair[0] = clamp(hair[0], -1.0f, 1.0f);
        hair[1] = clamp(hair[1], -1.0f, 1.0f);
        hair[2] = clamp(hair[2], -1.0f, 1.0f);
        hair[3] = clamp(hair[3], -1.0f, 1.0f);

        float* eyes = (float*)(type6pktdata + 0x90 + 0x10);
        eyes[0] = clamp(eyes[0], -1.0f, 1.0f);
        eyes[1] = clamp(eyes[1], -1.0f, 1.0f);
        eyes[2] = clamp(eyes[2], -1.0f, 1.0f);
        eyes[3] = clamp(eyes[3], -1.0f, 1.0f);
    }
}
