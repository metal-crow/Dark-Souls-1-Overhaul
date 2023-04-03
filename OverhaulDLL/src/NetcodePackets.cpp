#include "Rollback.h"
#include "NetcodePackets.h"
#include "GameData.h"
#include "SP/memory.h"
#include "SP/memory/injection/asm/x64.h"
#include <unordered_set>

extern "C" {
    uint64_t sendNetMessage_return;
    void sendNetMessage_injection();
    bool sendNetMessage_helper(void* session_man, uint64_t ConnectedPlayerData, uint32_t type, uint8_t* data);

    uint64_t getNetMessage_return;
    void getNetMessage_injection();
    bool getNetMessage_helper(void* session_man, uint64_t ConnectedPlayerData, uint32_t type);

    uint64_t disableType18PacketEnforcement_return;
    void disableType18PacketEnforcement_injection();
    uint64_t disableType18PacketEnforcement_helper(PlayerIns* pc);

    uint64_t fixPhantomBulletGenIssue_return;
    void fixPhantomBulletGenIssue_injection();
    uint64_t fixPhantomBulletGenIssue_helper(PlayerIns* pc);

    uint64_t fixRollTypeComputing_return;
    void fixRollTypeComputing_injection();
    bool fixRollTypeComputing_helper(PlayerIns* pc);
}

void Rollback::NetcodeFix()
{
    uint8_t *write_address;

    /* Type 1,10,11,16,17,18,34,35,70 Packet */
    //Disable sending
    write_address = (uint8_t*)(Rollback::sendNetMessage_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &sendNetMessage_return, 25, &sendNetMessage_injection);
    //Disable recv
    write_address = (uint8_t*)(Rollback::getNetMessage_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &getNetMessage_return, 12, &getNetMessage_injection);

    /* Type 18 Packet */
    // normally, type18 packet receipt is the only way to do damage to another player
    // the client does see the other attacking player as a normal npc, and is capable of applying damage the normal way
    // but explicitly doesn't if the other player is a PC
    // disable that throw away check and just return 0 instead
    // Do this by modifying the PlayerIns_Is_NetworkedPlayer function to always return false for phantoms. This fixes the check everywhere it's done
    write_address = (uint8_t*)(Rollback::disableType18PacketEnforcement_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &disableType18PacketEnforcement_return, 4, &disableType18PacketEnforcement_injection);

    // With the above type 18 packet fix, we have a bug where followup bullets from flamesurge don't get emitted
    // It checks the PlayerIns_Is_NetworkedPlayer func and fails if it returns false
    // Patch this to be an exception and return true instead of false
    write_address = (uint8_t*)(Rollback::fixPhantomBulletGenIssue_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &fixPhantomBulletGenIssue_return, 0, &fixPhantomBulletGenIssue_injection);

    // Fix the code in PlayerIns_ComputeChanges to compute the roll type for other players even if they're networked
    write_address = (uint8_t*)(Rollback::fixRollTypeComputing_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &fixRollTypeComputing_return, 0, &fixRollTypeComputing_injection);

    //allow our packet to be received even if the other playerins isn't available
    //just have the function always return true
    //write_address = (uint8_t*)(Rollback::isPacketTypeValid_offset + Game::ds1_base);
    //uint8_t mov_r8b_1[] = { 0x41, 0xb0, 0x01 };
    //sp::mem::patch_bytes(write_address, mov_r8b_1, 3);

    //cause the new playerins to be created with a PadManipulator, instead of a NetworkManipulator
    //TODO make not temporary
    write_address = (uint8_t*)(Game::ds1_base + 0x27ba147);
    uint8_t patch1[] = { 0xC7, 0x44, 0x24, 0x28, 0x01, 0x00, 0x00, 0x00 };
    sp::mem::patch_bytes(write_address, patch1, sizeof(patch1));
}

//return false if we don't want to have sendNetMessage send a packet
bool sendNetMessage_helper(void* session_man, uint64_t ConnectedPlayerData, uint32_t type, uint8_t* data)
{
    if (Rollback::rollbackEnabled || Rollback::networkTest)
    {
        if (type == 10)
        {
            SendPlayerInitPacket(ConnectedPlayerData);
        }

        switch (type)
        {
        case 1:
        //TODO
        //case 10:
        //case 11:
        case 16:
        case 17:
        case 18:
        case 76:
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

void SendPlayerInitPacket(uint64_t ConnectedPlayerData)
{
    //TODO temp solution. will need to send this on player join
    PlayerInitPacket pkt;
    auto player_o = Game::get_PlayerIns();
    if (player_o.has_value() && player_o.value() != NULL)
    {
        PlayerIns* playerins = (PlayerIns*)player_o.value();

        pkt.position_x = *(float*)(((uint64_t)playerins->chrins.playerCtrl->chrCtrl.havokChara) + 0x10);
        pkt.position_z = *(float*)(((uint64_t)playerins->chrins.playerCtrl->chrCtrl.havokChara) + 0x14);
        pkt.position_y = *(float*)(((uint64_t)playerins->chrins.playerCtrl->chrCtrl.havokChara) + 0x18);
        pkt.rotation = *(float*)(((uint64_t)playerins->chrins.playerCtrl->chrCtrl.havokChara) + 0x4);
        pkt.curHp = playerins->chrins.curHp;
        pkt.baseMaxHp = *(uint32_t*)(((uint64_t)&playerins->playergamedata->attribs) + 0xC);
        pkt.curSp = playerins->chrins.curSp;
        pkt.baseMaxSp = *(uint32_t*)(((uint64_t)&playerins->playergamedata->attribs) + 0x28);

        sendNetMessage(*(uint64_t*)Game::session_man_imp, ConnectedPlayerData, RollbackPlayerInitPacketType, &pkt, sizeof(pkt));
    }
}

//return false if we don't want to have getNetMessage get a packet
bool getNetMessage_helper(void* session_man, uint64_t ConnectedPlayerData, uint32_t type)
{
    if (Rollback::rollbackEnabled || Rollback::networkTest)
    {
        if (type == 10)
        {
            RecvPlayerInitPacket(ConnectedPlayerData);
        }

        switch (type)
        {
        case 1:
        //TODO
        //case 10:
        //case 11:
        case 16:
        case 17:
        case 18:
        case 76:
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

void RecvPlayerInitPacket(uint64_t ConnectedPlayerData)
{
    PlayerInitPacket pkt;
    uint32_t res = getNetMessage(*(uint64_t*)Game::session_man_imp, ConnectedPlayerData, RollbackPlayerInitPacketType, &pkt, sizeof(pkt));
    if (res == sizeof(pkt))
    {
        PlayerIns* playerins = getPlayerInsForConnectedPlayerData(*(void**)Game::world_chr_man_imp, (void*)ConnectedPlayerData);
        if (playerins != NULL)
        {
            *(float*)(((uint64_t)playerins->chrins.playerCtrl->chrCtrl.havokChara) + 0x10) = pkt.position_x;
            *(float*)(((uint64_t)playerins->chrins.playerCtrl->chrCtrl.havokChara) + 0x14) = pkt.position_z;
            *(float*)(((uint64_t)playerins->chrins.playerCtrl->chrCtrl.havokChara) + 0x18) = pkt.position_y;
            *(float*)(((uint64_t)playerins->chrins.playerCtrl->chrCtrl.havokChara) + 0x4) = pkt.rotation;
            playerins->chrins.curHp = pkt.curHp;
            *(uint32_t*)(((uint64_t)&playerins->playergamedata->attribs) + 0xC) = pkt.baseMaxHp;
            playerins->chrins.curSp = pkt.curSp;
            *(uint32_t*)(((uint64_t)&playerins->playergamedata->attribs) + 0x28) = pkt.baseMaxSp;
        }
    }
}

uint64_t disableType18PacketEnforcement_helper(PlayerIns* pc)
{
    if (!Rollback::rollbackEnabled && !Rollback::networkTest)
    {
        return 1;
    }

    //return 0 if this is a phantom
    uint32_t handle = *(uint32_t*)((uint64_t)(&pc->chrins) + 8);
    if (handle >= Game::PC_Handle && handle < Game::PC_Handle + 10)
    {
        return 0;
    }

    return 1;
}

uint64_t fixPhantomBulletGenIssue_helper(PlayerIns* pc)
{
    //return 0 to do nothing, return 1 if we need to fix the bullet bug
    if (!Rollback::rollbackEnabled && !Rollback::networkTest)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

bool fixRollTypeComputing_helper(PlayerIns* pc)
{
    if (!Rollback::rollbackEnabled && !Rollback::networkTest)
    {
        return false;
    }
    else
    {
        return true;
    }
}
