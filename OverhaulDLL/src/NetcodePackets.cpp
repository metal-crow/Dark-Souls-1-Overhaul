#include "Rollback.h"
#include "NetcodePackets.h"
#include "GameData.h"
#include "SP/memory.h"
#include "SP/memory/injection/asm/x64.h"
#include <unordered_set>

extern "C" {
    uint64_t sendNetMessage_return;
    void sendNetMessage_injection();
    bool sendNetMessage_helper(void* session_man, uint64_t ConnectedPlayerData, uint32_t type);

    uint64_t getNetMessage_return;
    void getNetMessage_injection();
    bool getNetMessage_helper(void* session_man, uint64_t ConnectedPlayerData, uint32_t type);

    uint64_t send_generalplayerinfo_return;
    void send_generalplayerinfo_injection();
    void send_generalplayerinfo_helper(PlayerIns* p);

    uint64_t Read_GeneralPlayerData_return;
    void Read_GeneralPlayerData_injection();
    uint64_t Read_GeneralPlayerData_helper(uint64_t NetworkManipulator);

    uint64_t disableType18PacketEnforcement_return;
    void disableType18PacketEnforcement_injection();
    uint64_t disableType18PacketEnforcement_helper(PlayerIns* pc);

    uint64_t fixPhantomBulletGenIssue_return;
    void fixPhantomBulletGenIssue_injection();
    uint64_t fixPhantomBulletGenIssue_helper(PlayerIns* pc);
}
void send_HandshakePacketExtra(uint64_t ConnectedPlayerData);
void recv_HandshakePacketExtra(uint64_t ConnectedPlayerData);

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
    sp::mem::code::x64::inject_jmp_14b(write_address, &sendNetMessage_return, 25, &sendNetMessage_injection);
    //Disable recv
    write_address = (uint8_t*)(Rollback::getNetMessage_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &getNetMessage_return, 12, &getNetMessage_injection);

    /* Type 18 Packet */
    // normally, type18 packet receipt is the only way to do damage to another player
    // the client does see the other attacking player as a normal npc, and is capable of applying damage the normal way
    // but explicitly doesn't if the other player is a PC
    // disable that throw away check and just return 0 instead
    // Do this by modifying the PlayerIns_Is_PC function to always return false for phantoms. This fixes the check everywhere it's done
    write_address = (uint8_t*)(Rollback::disableType18PacketEnforcement_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &disableType18PacketEnforcement_return, 4, &disableType18PacketEnforcement_injection);

    // With the above type 18 packet fix, we have a bug where followup bullets from flamesurge don't get emitted
    // It checks the PlayerIns_Is_PC func and fails if it returns false
    // Patch this to be an exception and return true instead of false
    write_address = (uint8_t*)(Rollback::fixPhantomBulletGenIssue_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &fixPhantomBulletGenIssue_return, 0, &fixPhantomBulletGenIssue_injection);

    //allow our packet to be received even if the other playerins isn't available
    //just have the function always return true
    write_address = (uint8_t*)(Rollback::isPacketTypeValid_offset + Game::ds1_base);
    uint8_t mov_r8b_1[] = { 0x41, 0xb0, 0x01 };
    sp::mem::patch_bytes(write_address, mov_r8b_1, 3);
}

//return false if we don't want to have sendNetMessage send a packet
bool sendNetMessage_helper(void* session_man, uint64_t ConnectedPlayerData, uint32_t type)
{
    if (Rollback::rollbackEnabled)
    {
        if (type == 10)
        {
            // Our rollback packet, via hooking type 1, doesn't start arriving until a bit later then the game would like
            // which means we don't set up certain flags, that are set by type 10 and 11, in time and the session fails
            // Add an additional packet type that is sent as part of the join handshake set, that does their jobs
            send_HandshakePacketExtra(ConnectedPlayerData);
        }

        switch (type)
        {
        case 1:
        case 10:
        case 11:
        case 16:
        case 17:
        case 18:
        case 76:
        case 35:
        case 70:
            return false;
        case Rollback::RollbackSinglePacketType:
        default:
            return true;
        }
    }
    else
    {
        if (type == Rollback::RollbackSinglePacketType)
        {
            return false;
        }
        else
        {
            return true;
        }
    }
}

//return false if we don't want to have getNetMessage get a packet
bool getNetMessage_helper(void* session_man, uint64_t ConnectedPlayerData, uint32_t type)
{
    if (Rollback::rollbackEnabled)
    {
        if (type == 10)
        {
            // For the additional packet sent as part of the join handshake set, read it
            recv_HandshakePacketExtra(ConnectedPlayerData);
        }

        switch (type)
        {
        case 1:
        case 10:
        case 11:
        case 16:
        case 17:
        case 18:
        case 76:
        case 35:
        case 70:
            return false;
        case Rollback::RollbackSinglePacketType:
        default:
            return true;
        }
    }
    else
    {
        if (type == Rollback::RollbackSinglePacketType)
        {
            return false;
        }
        else
        {
            return true;
        }
    }
}

void Rollback::BuildRemotePlayerPacket(PlayerIns* playerins, MainPacket* pkt)
{
    if (playerins == NULL)
    {
        ConsoleWrite("ERROR: playerins value at BuildRemotePlayerPacket is NULL.");
        return;
    }

    //Load Type 1
    pkt->position_x = *(float*)(((uint64_t)playerins->chrins.playerCtrl->chrCtrl.havokChara) + 0x10);
    pkt->position_z = *(float*)(((uint64_t)playerins->chrins.playerCtrl->chrCtrl.havokChara) + 0x14);
    pkt->position_y = *(float*)(((uint64_t)playerins->chrins.playerCtrl->chrCtrl.havokChara) + 0x18);
    pkt->ezStateActiveState = get_AnimationData(playerins->chrins.playerCtrl->chrCtrl.actionctrl, 1);
    pkt->ezStatePassiveState = get_AnimationData(playerins->chrins.playerCtrl->chrCtrl.actionctrl, 0);
    pkt->curHp = playerins->chrins.curHp;
    pkt->maxHp = playerins->chrins.maxHp;
    pkt->rotation = *(float*)(((uint64_t)playerins->chrins.playerCtrl->chrCtrl.havokChara) + 0x4);
    pkt->atkAngle = *(float*)(((uint64_t)playerins->chrins.playerCtrl->chrCtrl.havokChara) + 0);
    pkt->movement_direction_vals[0] = *(float*)(((uint64_t)(&playerins->chrins.padManipulator->chrManipulator)) + 0x10 + 0);
    pkt->movement_direction_vals[1] = *(float*)(((uint64_t)(&playerins->chrins.padManipulator->chrManipulator)) + 0x10 + 8);

    //Load Type 10
    uint64_t playergamedata = *(uint64_t*)(*(uint64_t*)(Game::game_data_man)+0x10);
    uint64_t attribs = playergamedata + 0x10;
    uint64_t equipgamedata = playergamedata + 0x280;
    for (int i = 0; i < 0x14; i++)
    {
        pkt->equipment_array[i] = Game::get_equipped_inventory((uint64_t)playerins, (InventorySlots)i);
    }
    pkt->type10_unk1 = *(float*)(equipgamedata + 0x108);
    pkt->type10_unk2 = *(float*)(equipgamedata + 0x10C);
    pkt->type10_unk3 = *(float*)(equipgamedata + 0x110);
    pkt->type10_unk4 = *(float*)(equipgamedata + 0x114);
    pkt->type10_unk5 = *(float*)(equipgamedata + 0x118);

    //Load Type 11
    pkt->flags = compress_gamedata_flags(equipgamedata);

    //Load Type 16
    uint64_t cur_throw_ptr = *(uint64_t*)((*(uint64_t*)Game::throw_man) + 0x68);
    if (cur_throw_ptr != NULL && *(uint64_t*)cur_throw_ptr != NULL)
    {
        uint64_t cur_throw = *(uint64_t*)cur_throw_ptr;
        PlayerIns* atk_playerins = (*(PlayerIns**)(*(uint64_t*)(cur_throw + 0) + 0x8));
        PlayerIns* def_playerins = (*(PlayerIns**)(*(uint64_t*)(cur_throw + 8) + 0x8));

        pkt->attacker_position = *(PosRotFloatVec*)(*(uint64_t*)(cur_throw + 0) + 0x70);
        pkt->attacker_position.Rotation = *(float*)(((uint64_t)atk_playerins->chrins.playerCtrl->chrCtrl.havokChara) + 0x4);

        pkt->defender_position = *(PosRotFloatVec*)(*(uint64_t*)(cur_throw + 8) + 0x70);
        pkt->defender_position.Rotation = *(float*)(((uint64_t)def_playerins->chrins.playerCtrl->chrCtrl.havokChara) + 0x4);

        pkt->entitynum_defender = GetEntityNumForThrow(*(void**)Game::world_chr_man_imp, def_playerins);
        pkt->entitynum_attacker = GetEntityNumForThrow(*(void**)Game::world_chr_man_imp, atk_playerins);
        pkt->throw_id = *(uint16_t*)(cur_throw + 0x10);
    }
    else
    {
        pkt->throw_id = -1;
    }

    //Load Type 17
    pkt->curSelectedMagicId = get_currently_selected_magic_id(playerins);
    pkt->curUsingItemId = (playerins->chrins).curUsedItem.itemId;

    //Load Type 76
    //need to have this a list of all speffects currently applied to the PC
    //don't need timestamp because we'll refresh every packet
    SpecialEffect_Info* specialEffectInfo = playerins->chrins.specialEffects->specialEffect_Info;
    size_t i = 0;
    while (specialEffectInfo != NULL)
    {
        uint32_t specialEffectInfo_id = *(uint32_t*)((uint64_t)specialEffectInfo + 0x30);
        if (i >= sizeof(pkt->spEffectToApply) / sizeof(uint32_t))
        {
            FATALERROR("pkt.spEffectToApply size of 15 is insufficent.");
        }
        pkt->spEffectToApply[i] = specialEffectInfo_id;
        i++;
        specialEffectInfo = specialEffectInfo->next;
    }
    for (; i < sizeof(pkt->spEffectToApply) / sizeof(uint32_t); i++)
    {
        pkt->spEffectToApply[i] = -1;
    }
}

void send_generalplayerinfo_helper(PlayerIns* playerins)
{
    MainPacket pkt;

    Rollback::BuildRemotePlayerPacket(playerins, &pkt);

    //send out packet
    sendNetMessageToAllPlayers(*(uint64_t*)Game::session_man_imp, Rollback::RollbackSinglePacketType, &pkt, sizeof(pkt));
}

void send_HandshakePacketExtra(uint64_t ConnectedPlayerData)
{
    HandshakePacketExtra pkt;
    auto gamedata_o = Game::get_host_player_gamedata();
    if (!gamedata_o.has_value())
    {
        FATALERROR("Unable to get player game data in send_HandshakePacketExtra");
    }
    uint64_t playergamedata = (uint64_t)gamedata_o.value();

    //type 10
    uint64_t attribs = playergamedata + 0x10;
    uint64_t equipgamedata = playergamedata + 0x280;
    pkt.player_num = *(int32_t*)(attribs + 0);
    pkt.player_sex = *(uint8_t*)(attribs + 0xba);
    pkt.covenantId = *(uint8_t*)(attribs + 0x103);
    for (int i = 0; i < 0x14; i++)
    {
        pkt.equipment_array[i] = *(uint32_t*)((equipgamedata + 0x80 + 0x24) + (i * 4));
    }
    pkt.type10_unk1 = *(float*)(equipgamedata + 0x108);
    pkt.type10_unk2 = *(float*)(equipgamedata + 0x10C);
    pkt.type10_unk3 = *(float*)(equipgamedata + 0x110);
    pkt.type10_unk4 = *(float*)(equipgamedata + 0x114);
    pkt.type10_unk5 = *(float*)(equipgamedata + 0x118);

    //Load Type 11
    pkt.flags = compress_gamedata_flags(equipgamedata);

    sendNetMessage(*(uint64_t*)Game::session_man_imp, ConnectedPlayerData, Rollback::RollbackSinglePacketType, &pkt, sizeof(pkt));
}

uint64_t Read_GeneralPlayerData_helper(uint64_t NetworkManipulator)
{
    //read in packet for the given connected player
    MainPacket pkt;
    PlayerIns* playerins;

    uint64_t GeneralPlayerData = *(uint64_t*)(NetworkManipulator + 0x270);
    uint64_t ConnectedPlayerData = GeneralPlayerData + 0x28;
    uint32_t res = getNetMessage(*(uint64_t*)Game::session_man_imp, ConnectedPlayerData, Rollback::RollbackSinglePacketType, &pkt, sizeof(pkt));

    if (res == sizeof(pkt))
    {
        playerins = getPlayerInsForConnectedPlayerData(*(void**)Game::world_chr_man_imp, (void*)ConnectedPlayerData);
        if (playerins == NULL)
        {
            FATALERROR("Unable to get PlayerIns for the ConnectedPlayerData %p", ConnectedPlayerData);
        }

        uint32_t session_player_num = Game::get_SessionPlayerNumber_For_ConnectedPlayerData(ConnectedPlayerData);

        Rollback::LoadRemotePlayerPacket(&pkt, playerins, session_player_num);
    }

    return Rollback::rollbackEnabled;
}

void recv_HandshakePacketExtra(uint64_t ConnectedPlayerData)
{
    HandshakePacketExtra pkt;
    uint32_t res = getNetMessage(*(uint64_t*)Game::session_man_imp, ConnectedPlayerData, Rollback::RollbackSinglePacketType, &pkt, sizeof(pkt));

    if (res == sizeof(pkt))
    {
        int32_t session_player_num = Game::get_SessionPlayerNumber_For_ConnectedPlayerData(ConnectedPlayerData);
        if (session_player_num == -1)
        {
            ConsoleWrite("Warning: Got -1 for session_player_num in recv_HandshakePacketExtra. Connection will fail.");
            return;
        }
        PlayerGameData* playergamedata = (PlayerGameData*)(*(uint64_t*)((*(uint64_t*)Game::game_data_man) + 0x18) + (0x660 * session_player_num));

        //Type 10
        *(uint32_t*)((uint64_t)(&playergamedata->attribs) + 0) = pkt.player_num;
        uint32_t chrType = *(uint32_t*)(((uint64_t)&playergamedata->attribs) + 0x94);
        uint8_t* playerAttribsSet = (uint8_t*)((uint64_t)(playergamedata) + 0x612);
        *(uint8_t*)((uint64_t)(&playergamedata->attribs) + 0xba) = pkt.player_sex;
        Set_Player_Sex_Specific_Attribs(&playergamedata->equipGameData, pkt.player_sex, chrType);
        *(uint8_t*)((uint64_t)(&playergamedata->attribs) + 0x103) = pkt.covenantId;
        for (uint32_t i = 0; i < 20; i++)
        {
            ChrAsm_Set_Equipped_Items_FromNetwork(&playergamedata->equipGameData, i, pkt.equipment_array[i], -1, false);
        }
        uint64_t equipgamedata = (uint64_t)&playergamedata->equipGameData;
        *(float*)(equipgamedata + 0x108) = pkt.type10_unk1;
        *(float*)(equipgamedata + 0x10C) = pkt.type10_unk2;
        *(float*)(equipgamedata + 0x110) = pkt.type10_unk3;
        *(float*)(equipgamedata + 0x114) = pkt.type10_unk4;
        *(float*)(equipgamedata + 0x118) = pkt.type10_unk5;
        uint8_t* on_pkt10_recv = (uint8_t*)((*(uint64_t*)((*(uint64_t*)Game::game_data_man) + 0x28)) + session_player_num);
        *on_pkt10_recv |= 0x4;

        *playerAttribsSet = 1;

        //Type 11
        set_playergamedata_flags((void*)equipgamedata, pkt.flags);
        *on_pkt10_recv |= 0x8;
    }
}

void Rollback::LoadRemotePlayerPacket(MainPacket* pkt, PlayerIns* playerins, int32_t session_player_num)
{
    //Type 1
    *(float*)(((uint64_t)playerins->chrins.playerCtrl->chrCtrl.havokChara) + 0x10) = pkt->position_x;
    *(float*)(((uint64_t)playerins->chrins.playerCtrl->chrCtrl.havokChara) + 0x14) = pkt->position_z;
    *(float*)(((uint64_t)playerins->chrins.playerCtrl->chrCtrl.havokChara) + 0x18) = pkt->position_y;
    ActionCtrl_ApplyEzState(playerins->chrins.playerCtrl->chrCtrl.actionctrl, 0x0, pkt->ezStatePassiveState);
    ActionCtrl_ApplyEzState(playerins->chrins.playerCtrl->chrCtrl.actionctrl, 0x1, pkt->ezStateActiveState);
    PlayerIns_SetHp(playerins, pkt->curHp);
    *(uint32_t*)((uint64_t)(&playerins->playergamedata->attribs) + 4) = pkt->curHp;
    playerins->chrins.maxHp = pkt->maxHp;
    *(uint32_t*)((uint64_t)(&playerins->playergamedata->attribs) + 8) = pkt->maxHp;
    *(uint32_t*)((uint64_t)(&playerins->playergamedata->attribs) + 0xC) = pkt->maxHp;
    *(float*)(((uint64_t)playerins->chrins.playerCtrl->chrCtrl.havokChara) + 0x4) = pkt->rotation;
    *(float*)(((uint64_t)playerins->chrins.playerCtrl->chrCtrl.havokChara) + 0) = pkt->atkAngle;
    *(float*)(((uint64_t)(&playerins->chrins.padManipulator->chrManipulator)) + 0x10 + 0) = pkt->movement_direction_vals[0];
    *(float*)(((uint64_t)(&playerins->chrins.padManipulator->chrManipulator)) + 0x10 + 4) = 0.0f;
    *(float*)(((uint64_t)(&playerins->chrins.padManipulator->chrManipulator)) + 0x10 + 8) = pkt->movement_direction_vals[1];
    *(float*)(((uint64_t)(&playerins->chrins.padManipulator->chrManipulator)) + 0x10 + 0xc) = 0.0f;

    //Type 10
    uint32_t chrType = *(uint32_t*)(((uint64_t)&playerins->playergamedata->attribs) + 0x94);
    uint8_t* playerAttribsSet = (uint8_t*)((uint64_t)(playerins->playergamedata) + 0x612);
    for (uint32_t i = 0; i < 20; i++)
    {
        ChrAsm_Set_Equipped_Items_FromNetwork(&playerins->playergamedata->equipGameData, i, pkt->equipment_array[i], -1, false);
    }
    uint64_t equipgamedata = (uint64_t)&playerins->playergamedata->equipGameData;
    *(float*)(equipgamedata + 0x108) = pkt->type10_unk1;
    *(float*)(equipgamedata + 0x10C) = pkt->type10_unk2;
    *(float*)(equipgamedata + 0x110) = pkt->type10_unk3;
    *(float*)(equipgamedata + 0x114) = pkt->type10_unk4;
    *(float*)(equipgamedata + 0x118) = pkt->type10_unk5;
    *playerAttribsSet = 1;

    //Type 11
    set_playergamedata_flags((void*)equipgamedata, pkt->flags);

    if (session_player_num != -1)
    {
        uint8_t* on_pkt10_recv = (uint8_t*)((*(uint64_t*)((*(uint64_t*)Game::game_data_man) + 0x28)) + session_player_num);
        *on_pkt10_recv |= 0x4;
        *on_pkt10_recv |= 0x8;
    }

    //Type 16
    if (pkt->throw_id != -1)
    {
        ChrIns* attacker = getEntity(*(uint64_t*)Game::world_chr_man_imp, pkt->entitynum_attacker);
        ChrIns* defender = getEntity(*(uint64_t*)Game::world_chr_man_imp, pkt->entitynum_defender);
        uint64_t attacker_throw_info = (uint64_t)attacker->throw_animation_info;
        uint64_t defender_throw_info = (uint64_t)defender->throw_animation_info;

        //position    
        *(float*)(((uint64_t)attacker->playerCtrl->chrCtrl.havokChara) + 0x10) = pkt->attacker_position.X;
        *(float*)(((uint64_t)attacker->playerCtrl->chrCtrl.havokChara) + 0x14) = pkt->attacker_position.Z;
        *(float*)(((uint64_t)attacker->playerCtrl->chrCtrl.havokChara) + 0x18) = pkt->attacker_position.Y;
        
        *(float*)(((uint64_t)defender->playerCtrl->chrCtrl.havokChara) + 0x10) = pkt->defender_position.X;
        *(float*)(((uint64_t)defender->playerCtrl->chrCtrl.havokChara) + 0x14) = pkt->defender_position.Z;
        *(float*)(((uint64_t)defender->playerCtrl->chrCtrl.havokChara) + 0x18) = pkt->defender_position.Y;
        
        //rotation
        *(float*)((uint64_t)(attacker->playerCtrl->chrCtrl.actionctrl) + 0x500 + 0) = 0;
        *(float*)((uint64_t)(attacker->playerCtrl->chrCtrl.actionctrl) + 0x500 + 4) = pkt->attacker_position.Rotation;
        *(float*)((uint64_t)(attacker->playerCtrl->chrCtrl.actionctrl) + 0x500 + 8) = 0;
        *(float*)((uint64_t)(attacker->playerCtrl->chrCtrl.actionctrl) + 0x500 + 0xc) = 0;
                
        *(float*)((uint64_t)(defender->playerCtrl->chrCtrl.actionctrl) + 0x500 + 0) = 0;
        *(float*)((uint64_t)(defender->playerCtrl->chrCtrl.actionctrl) + 0x500 + 4) = pkt->defender_position.Rotation;
        *(float*)((uint64_t)(defender->playerCtrl->chrCtrl.actionctrl) + 0x500 + 8) = 0;
        *(float*)((uint64_t)(defender->playerCtrl->chrCtrl.actionctrl) + 0x500 + 0xc) = 0;
        
        //override_nonthrow_positionAndRotation
        *(uint8_t*)((uint64_t)(attacker->playerCtrl->chrCtrl.actionctrl) + 0x520) = 1;
        *(uint8_t*)((uint64_t)(defender->playerCtrl->chrCtrl.actionctrl) + 0x520) = 1;

        //unknown
        *(uint8_t*)(attacker_throw_info + 0x28) = 1;
        *(uint8_t*)(defender_throw_info + 0x28) = 1;

        //throw state
        *(uint8_t*)(attacker_throw_info + 0x20) = 1;
        *(uint8_t*)(defender_throw_info + 0x20) = 2;

        //throw_id
        *(uint32_t*)(attacker_throw_info + 0x10 + 8) = pkt->throw_id;
        *(uint32_t*)(defender_throw_info + 0x10 + 8) = pkt->throw_id;

        //throw paramdef
        *(void**)(attacker_throw_info + 0x10 + 0) = getThrowParamFromThrowId(pkt->throw_id);
        *(void**)(defender_throw_info + 0x10 + 0) = getThrowParamFromThrowId(pkt->throw_id);

        //ThrowPairHandle
        *(uint32_t*)(attacker_throw_info + 0x24) = *(uint32_t*)((uint64_t)(defender) + 8);
        *(uint32_t*)(defender_throw_info + 0x24) = *(uint32_t*)((uint64_t)(attacker) + 8);

        //putIntoThrowAnimation
        putAttackerIntoThrowAnimation(attacker_throw_info);
        putDefenderIntoThrowAnimation(defender_throw_info, 1);

        //force the game to recheck the animation mediator and set the animation queue. Otherwise it'll miss this data being set and overwrite it start of next frame.
        ChrCtrl_Func_30(&defender->playerCtrl->chrCtrl, FRAMETIME);
    }

    //Type 17
    (playerins->chrins).curSelectedMagicId = pkt->curSelectedMagicId;
    PlayerIns_Update_curSelectedMagicId(playerins, pkt->curSelectedMagicId);
    if (pkt->curUsingItemId != -1)
    {
        (playerins->chrins).curUsedItem.itemId = pkt->curUsingItemId;
        (playerins->curUsedItem).itemId = pkt->curUsingItemId;
        (playerins->curUsedItem).amountUsed = 1;
        (playerins->chrins).curUsedItem.amountUsed = 1;
    }

    // Type 76
    std::unordered_set<uint32_t> packetSpEffectToApply(pkt->spEffectToApply, &pkt->spEffectToApply[15]);
    packetSpEffectToApply.erase(-1);
    //extend or remove current speffects
    SpecialEffect_Info* head = playerins->chrins.specialEffects->specialEffect_Info;
    while (head != 0)
    {
        uint32_t speffect_id = *(uint32_t*)(((uint64_t)head) + 0x30);
        //if we still have the speffect, reextend it's life
        if (packetSpEffectToApply.count(speffect_id))
        {
            *(float*)((uint64_t)head) = 1.0f;
            packetSpEffectToApply.erase(speffect_id);
            head = head->next;
        }
        //otherwise remove the effect
        else
        {
            head = SpecialEffect_Remove_SpecialEffectInfo(playerins->chrins.specialEffects, head, 0x1);
        }
    }
    //finally add the effects we don't already have
    for (uint32_t speffect_id : packetSpEffectToApply)
    {
        Apply_Speffect(playerins->chrins.specialEffects, speffect_id, 1.0f);
    }
}

uint64_t disableType18PacketEnforcement_helper(PlayerIns* pc)
{
    if (!Rollback::rollbackEnabled)
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
    if (!Rollback::rollbackEnabled)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
