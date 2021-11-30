#include "Rollback.h"
#include "SP/memory.h"
#include "SP/memory/injection/asm/x64.h"
#include "DarkSoulsOverhaulMod.h"

extern "C" {
    uint64_t sendType1NetMessage_injection_return;
    void sendType1NetMessage_injection();
    uint64_t recvType1NetMessage_injection_return;
    void recvType1NetMessage_injection();
}

void Rollback::start()
{
    ConsoleWrite("Rollback...");
    uint8_t *write_address;
    uint8_t retn_patch[] = { 0xC3 }; //x64 asm Return opcode

    //the following packets we need to just completly stop from being sent, they're now handled by the pvp primary packet
    write_address = (uint8_t*)(Rollback::sendType1NetMessage_offset + Game::ds1_base);
    //sp::mem::code::x64::inject_jmp_14b(write_address, &sendType1NetMessage_injection_return, 0, &sendType1NetMessage_injection);
    //sp::mem::patch_bytes(write_address, retn_patch, sizeof(retn_patch));
    write_address = (uint8_t*)(Rollback::recvType1NetMessage_offset + Game::ds1_base);
    //sp::mem::code::x64::inject_jmp_14b(write_address, &recvType1NetMessage_injection_return, 0, &recvType1NetMessage_injection);
    //sp::mem::patch_bytes(write_address, retn_patch, sizeof(retn_patch));

    //the following packets are handeling indirect player action that should removed and instead be computed client side, so rollbacks can invalidate them if needed
    write_address = (uint8_t*)(Rollback::sendType16NetMessage_offset + Game::ds1_base);
    //sp::mem::patch_bytes(write_address, retn_patch, sizeof(retn_patch));
    write_address = (uint8_t*)(Rollback::recvType16NetMessage_offset + Game::ds1_base);
    //sp::mem::patch_bytes(write_address, retn_patch, sizeof(retn_patch));

}
