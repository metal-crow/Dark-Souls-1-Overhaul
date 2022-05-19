#include "Rollback.h"
#include "SP/memory.h"
#include "SP/memory/injection/asm/x64.h"

extern "C" {

    uint64_t sendNetMessage_return;
    void sendNetMessage_injection();
    bool sendNetMessage_helper(uint32_t type);

    uint64_t getNetMessage_return;
    void getNetMessage_injection();
    bool getNetMessage_helper(uint32_t type);

    uint64_t send_generalplayerinfo_return;
    void send_generalplayerinfo_injection();
    void send_generalplayerinfo_helper();

    uint64_t Read_GeneralPlayerData_return;
    void Read_GeneralPlayerData_injection();
    void Read_GeneralPlayerData_helper();
}

void Rollback::NetcodeFix()
{
    uint8_t *write_address;

    /* Type 1 Packet */
    //use it's calling function as the base for our new main packet
    //(send)
    write_address = (uint8_t*)(Rollback::send_generalplayerinfo_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &send_generalplayerinfo_return, 2, &send_generalplayerinfo_injection);
    //(recv)
    write_address = (uint8_t*)(Rollback::Read_GeneralPlayerData_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &Read_GeneralPlayerData_return, 4, &Read_GeneralPlayerData_injection);

    /* Type 1,10,11,16,17,18,34,35,70 Packet */
    //Disable sending
    write_address = (uint8_t*)(Rollback::sendNetMessage_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &sendNetMessage_return, 5, &sendNetMessage_injection);
    //Disable recv
    write_address = (uint8_t*)(Rollback::getNetMessage_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &getNetMessage_return, 1, &getNetMessage_injection);

    /* Type 18 Packet */
    // normally, type18 packet receipt is the only way to do damage to another player
    // the client does see the other attacking player as a normal npc, and is capable of applying damage the normal way
    // but explicitly doesn't if the other player is a PC
    // disable that throw away check and just return 0 instead
    write_address = (uint8_t*)(Rollback::disableType18PacketEnforcement + Game::ds1_base);
    uint8_t disableType18PacketEnforcement_patch[] = { 0x48, 0x31, 0xc0, 0x90, 0x90, 0x90 }; //xor    rax,rax | nop | nop | nop
    sp::mem::patch_bytes(write_address, disableType18PacketEnforcement_patch, sizeof(disableType18PacketEnforcement_patch));
}

//return false if we don't want to have sendNetMessage send a packet
bool sendNetMessage_helper(uint32_t type)
{
    if (Rollback::rollbackEnabled)
    {
        switch (type)
        {
        case 1:
        case 10:
        case 11:
        case 16:
        case 17:
        case 18:
        case 34:
        case 35:
        case 70:
            return false;
        default:
            return true;
        }
    }
    else
    {
        return true;
    }
}

//return false if we don't want to have getNetMessage get a packet
bool getNetMessage_helper(uint32_t type)
{
    if (Rollback::rollbackEnabled)
    {
        switch (type)
        {
        case 1:
        case 10:
        case 11:
        case 16:
        case 17:
        case 18:
        case 34:
        case 35:
        case 70:
            return false;
        default:
            return true;
        }
    }
    else
    {
        return true;
    }
}

void send_generalplayerinfo_helper()
{
    //TODO send
}

void Read_GeneralPlayerData_helper()
{
    //TODO recv
}
