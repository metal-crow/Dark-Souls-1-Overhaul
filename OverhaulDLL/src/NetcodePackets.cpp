#include "Rollback.h"
#include "SP/memory.h"
#include "SP/memory/injection/asm/x64.h"

extern "C" {
    uint64_t sendType1NetMessage_return;
    void sendType1NetMessage_injection();
    bool sendType1NetMessage_helper();

    uint64_t readparseType1NetMessage_return;
    void readparseType1NetMessage_injection();
    bool readparseType1NetMessage_helper();

    uint64_t send_generalplayerinfo_return;
    void send_generalplayerinfo_injection();
    void send_generalplayerinfo_helper();

    uint64_t Read_GeneralPlayerData_return;
    void Read_GeneralPlayerData_injection();
    void Read_GeneralPlayerData_helper();

    uint64_t sendType10NetMessage_return;
    void sendType10NetMessage_injection();
    bool sendType10NetMessage_helper();

    uint64_t sendType10NetMessage2_return;
    void sendType10NetMessage2_injection();
    bool sendType10NetMessage2_helper();

    uint64_t readparseType10NetMessage_return;
    void readparseType10NetMessage_injection();
    bool readparseType10NetMessage_helper();

    uint64_t sendType11NetMessage_1_return;
    uint64_t sendType11NetMessage_2_return;
    uint64_t sendType11NetMessage_3_return;
    void sendType11NetMessage_1_injection();
    void sendType11NetMessage_2_injection();
    void sendType11NetMessage_3_injection();
    bool sendType11NetMessage_helper();

    uint64_t readparseType11NetMessage_return;
    void readparseType11NetMessage_injection();
    bool readparseType11NetMessage_helper();

    uint64_t sendType16NetMessage_return;
    void sendType16NetMessage_injection();
    bool sendType16NetMessage_helper();

    uint64_t readparseType16NetMessage_return;
    void readparseType16NetMessage_injection();
    bool readparseType16NetMessage_helper();

    uint64_t readparseType17NetMessage_return;
    void readparseType17NetMessage_injection();
    bool readparseType17NetMessage_helper();
}

void Rollback::NetcodeFix()
{
    uint8_t *write_address;

    /* Type 1 Packet */
    //Disable sending
    write_address = (uint8_t*)(Rollback::sendType1NetMessage_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &sendType1NetMessage_return, 2, &sendType1NetMessage_injection);
    //Disable recv
    write_address = (uint8_t*)(Rollback::readparseType1NetMessage_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &readparseType1NetMessage_return, 1, &readparseType1NetMessage_injection);

    //and use it's calling function as the base for our new main packet
    //(send)
    write_address = (uint8_t*)(Rollback::send_generalplayerinfo_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &send_generalplayerinfo_return, 2, &send_generalplayerinfo_injection);
    //(recv)
    write_address = (uint8_t*)(Rollback::Read_GeneralPlayerData_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &Read_GeneralPlayerData_return, 4, &Read_GeneralPlayerData_injection);

    /* Type 10 Packet */
    //Disable sending
    write_address = (uint8_t*)(Rollback::sendType10NetMessage_1_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &sendType10NetMessage_return, 4, &sendType10NetMessage_injection);
    write_address = (uint8_t*)(Rollback::sendType10NetMessage_2_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &sendType10NetMessage2_return, 4, &sendType10NetMessage2_injection);
    //Disable recv
    write_address = (uint8_t*)(Rollback::readparseType10NetMessage_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &readparseType10NetMessage_return, 5, &readparseType10NetMessage_injection);

    /* Type 11 Packet */
    //Disable sending
    write_address = (uint8_t*)(Rollback::sendType11NetMessage_1_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &sendType11NetMessage_1_return, 5, &sendType11NetMessage_1_injection);
    write_address = (uint8_t*)(Rollback::sendType11NetMessage_2_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &sendType11NetMessage_2_return, 2, &sendType11NetMessage_2_injection);
    write_address = (uint8_t*)(Rollback::sendType11NetMessage_3_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &sendType11NetMessage_3_return, 1, &sendType11NetMessage_3_injection);
    //Disable recv
    write_address = (uint8_t*)(Rollback::readparseType11NetMessage_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &readparseType11NetMessage_return, 0, &readparseType11NetMessage_injection);

    /* Type 16 Packet */
    //Disable sending
    write_address = (uint8_t*)(Rollback::sendType16NetMessage_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &sendType16NetMessage_return, 2, &sendType16NetMessage_injection);
    //Disable recv
    write_address = (uint8_t*)(Rollback::readparseType16NetMessage_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &readparseType16NetMessage_return, 1, &readparseType16NetMessage_injection);

    /* Type 17 Packet */
    //Disable sending

    //Disable recv
    write_address = (uint8_t*)(Rollback::readparseType17NetMessage_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &readparseType17NetMessage_return, 5, &readparseType17NetMessage_injection);

    /* Type 18 Packet */
    // normally, type18 packet receipt is the only way to do damage to another player
    // the client does see the other attacking player as a normal npc, and is capable of applying damage the normal way
    // but explicitly doesn't if the other player is a PC
    // disable that throw away check and just return 0 instead
    write_address = (uint8_t*)(Rollback::disableType18PacketEnforcement + Game::ds1_base);
    uint8_t disableType18PacketEnforcement_patch[] = { 0x48, 0x31, 0xc0, 0x90, 0x90, 0x90 }; //xor    rax,rax | nop | nop | nop
    sp::mem::patch_bytes(write_address, disableType18PacketEnforcement_patch, sizeof(disableType18PacketEnforcement_patch));

    // see the above, disable the sending of the type18 packet, since we now compute it client-side
    write_address = (uint8_t*)(Rollback::disableType18PacketSending + Game::ds1_base);
    uint8_t disableType18PacketSending_patch[] = { 0x90, 0x90, 0x90, 0x90, 0x90 }; //nop x5
    sp::mem::patch_bytes(write_address, disableType18PacketSending_patch, sizeof(disableType18PacketSending_patch));
}

bool sendType1NetMessage_helper()
{
    if (Rollback::rollbackEnabled)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool readparseType1NetMessage_helper()
{
    if (Rollback::rollbackEnabled)
    {
        return true;
    }
    else
    {
        return false;
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

bool sendType10NetMessage_helper()
{
    if (Rollback::rollbackEnabled)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool sendType10NetMessage2_helper()
{
    if (Rollback::rollbackEnabled)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool readparseType10NetMessage_helper()
{
    if (Rollback::rollbackEnabled)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool sendType11NetMessage_helper()
{
    if (Rollback::rollbackEnabled)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool readparseType11NetMessage_helper()
{
    if (Rollback::rollbackEnabled)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool sendType16NetMessage_helper()
{
    if (Rollback::rollbackEnabled)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool readparseType16NetMessage_helper()
{
    if (Rollback::rollbackEnabled)
    {
        return true;
    }
    else
    {
        return false;
    }
}

