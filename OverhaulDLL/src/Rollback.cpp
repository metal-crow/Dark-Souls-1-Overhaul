#include "Rollback.h"
#include "SP/memory.h"
#include "SP/memory/injection/asm/x64.h"
#include "DarkSoulsOverhaulMod.h"
#include "PlayerInsStruct.h"

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

void Rollback::save()
{
    auto player_o = Game::get_PlayerIns();
    if (!player_o.has_value())
    {
        return;
    }
    uint64_t player = (uint64_t)player_o.value();

    copy_PlayerIns(saved_player, player);
}

//PlayerIns---

void copy_PlayerIns(PlayerIns* to, PlayerIns* from)
{
    copy_ChrIns(to, from);
}
//---PlayerIns
