#include "Rollback.h"
#include "NetcodePackets.h"
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
    void Read_GeneralPlayerData_helper(uint64_t NetworkManipulator);
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

typedef uint32_t get_AnimationData_FUNC(ActionCtrl* actionctrl, uint32_t i);
get_AnimationData_FUNC* get_AnimationData = (get_AnimationData_FUNC*)0x140385d00;

typedef uint32_t PlayerCtrl_Get_WalkAnimTwist_unk_FUNC(PlayerCtrl* playerctrl);
PlayerCtrl_Get_WalkAnimTwist_unk_FUNC* PlayerCtrl_Get_WalkAnimTwist_unk = (PlayerCtrl_Get_WalkAnimTwist_unk_FUNC*)0x14037c4a0;

typedef uint32_t FUN_14050ff50_FUNC(float param_1, void* param_2, byte param_3, int param_4, byte param_5);
FUN_14050ff50_FUNC* FUN_14050ff50 = (FUN_14050ff50_FUNC*)0x14050ff50;

typedef uint16_t compress_gamedata_flags_FUNC(uint64_t equipgamedata);
compress_gamedata_flags_FUNC* compress_gamedata_flags = (compress_gamedata_flags_FUNC*)0x140747900;

typedef uint32_t get_currently_selected_magic_id_FUNC(PlayerIns* playerins);
get_currently_selected_magic_id_FUNC* get_currently_selected_magic_id = (get_currently_selected_magic_id_FUNC*)0x140360650;

typedef bool sendNetMessageToAllPlayers_FUNC(uint64_t sessionMan, uint32_t type, void* data, int size);
sendNetMessageToAllPlayers_FUNC* sendNetMessageToAllPlayers = (sendNetMessageToAllPlayers_FUNC*)0x1405098a0;

typedef uint32_t getNetMessage_FUNC(uint64_t session_man, uint64_t ConnectedPlayerData, uint32_t type, void* data_out, int max_size);
getNetMessage_FUNC* getNetMessage = (getNetMessage_FUNC*)0x140509560;

void send_generalplayerinfo_helper()
{
    MainPacket pkt;

    auto playerins_o = Game::get_PlayerIns();
    if (!playerins_o.has_value() || playerins_o.value() == NULL)
    {
        ConsoleWrite("ERROR: Tried to get playerins for send_generalplayerinfo_helper, but unable to.");
        return;
    }
    PlayerIns* playerins = (PlayerIns*)playerins_o.value();

    //Load Type 1
    pkt.position_x = *(float*)(((uint64_t)playerins->chrins.playerCtrl->chrCtrl.havokChara) + 0x10);
    pkt.position_z = *(float*)(((uint64_t)playerins->chrins.playerCtrl->chrCtrl.havokChara) + 0x14);
    pkt.position_y = *(float*)(((uint64_t)playerins->chrins.playerCtrl->chrCtrl.havokChara) + 0x18);
    pkt.ezStateActiveState = get_AnimationData(playerins->chrins.playerCtrl->chrCtrl.actionctrl, 1);
    pkt.ezStatePassiveState = get_AnimationData(playerins->chrins.playerCtrl->chrCtrl.actionctrl, 0);
    pkt.curHp = playerins->chrins.curHp;
    pkt.maxHp = playerins->chrins.curHp;
    pkt.walkanimtwist_unk = PlayerCtrl_Get_WalkAnimTwist_unk(playerins->chrins.playerCtrl);
    pkt.rotation = *(float*)(((uint64_t)playerins->chrins.playerCtrl->chrCtrl.havokChara) + 0x4);

    uint32_t type1_unk1 = FUN_14050ff50(*(float*)(((uint64_t)playerins) + 0x954), NULL, 0xd, 5, 1);
    pkt.type1_unk1 = pkt.type1_unk1 & 0xffffe000;
    pkt.type1_unk1 = pkt.type1_unk1 | type1_unk1 & 0x1fff;
    type1_unk1 = FUN_14050ff50(*(float*)(((uint64_t)playerins) + 0x958), NULL, 0xd, 5, 1);
    pkt.type1_unk1 = pkt.type1_unk1 & 0xfc001fff;
    pkt.type1_unk1 = pkt.type1_unk1 | (type1_unk1 & 0x1fff) << 0xd;

    pkt.type1_unk2 = *(uint8_t*)(((uint64_t)playerins) + 0x94c);

    //Load Type 10
    uint64_t playergamedata = *(uint64_t*)(*(uint64_t*)(Game::game_data_man) + 0x10);
    uint64_t attribs = playergamedata + 0x10;
    uint64_t equipgamedata = playergamedata + 0x280;
    pkt.player_num = *(int32_t*)(attribs + 0);
    pkt.player_sex = *(uint8_t*)(attribs + 0xba);
    pkt.covenantId = *(uint8_t*)(attribs + 0x103);
    for (int i = 0; i < 0x14; i++)
    {
        pkt.equipment_array[i] = Game::get_equipped_inventory((uint64_t)playerins, (InventorySlots)i);
    }
    pkt.type10_unk1 = *(float*)(equipgamedata + 0x108);
    pkt.type10_unk2 = *(float*)(equipgamedata + 0x10C);
    pkt.type10_unk3 = *(float*)(equipgamedata + 0x110);
    pkt.type10_unk4 = *(float*)(equipgamedata + 0x114);
    pkt.type10_unk5 = *(float*)(equipgamedata + 0x118);

    //Load Type 11
    pkt.flags = compress_gamedata_flags(equipgamedata);
    pkt.node_num = (uint16_t)(*(uint32_t*)(((uint64_t)(&playerins->chrins)) + 0xdc));

    //Load Type 16
    //TODO need runtime debugging here

    //Load Type 17
    pkt.curSelectedMagicId = get_currently_selected_magic_id(playerins);
    pkt.curUsingItemId = (playerins->chrins).curUsedItem.itemId;

    //Load Type 34
    //TODO this is a bit complicated

    //send out packet
    sendNetMessageToAllPlayers(*(uint64_t*)Game::session_man_imp, Rollback::RollbackSinglePacketType, &pkt, sizeof(pkt));
}

void Read_GeneralPlayerData_helper(uint64_t NetworkManipulator)
{
    //read in packet for the given connected player
    MainPacket pkt;

    uint64_t GeneralPlayerData = *(uint64_t*)(NetworkManipulator + 0x270);
    uint64_t ConnectedPlayerData = GeneralPlayerData + 0x30;
    getNetMessage(*(uint64_t*)Game::session_man_imp, ConnectedPlayerData, Rollback::RollbackSinglePacketType, &pkt, sizeof(pkt));

    //temporary, will use this with GGPO later
    Rollback::LoadRemotePlayerPacket(&pkt);
}

typedef void PlayerIns_SetHp_FUNC(void* playerins, uint64_t curHp);
PlayerIns_SetHp_FUNC* PlayerIns_SetHp = (PlayerIns_SetHp_FUNC*)0x1403206c0;

typedef void ChrAsm_Set_Equipped_Items_FromNetwork_FUNC(void* EquipGameData, uint32_t index, uint32_t given_item_id, int param_4, bool param_5);
ChrAsm_Set_Equipped_Items_FromNetwork_FUNC* ChrAsm_Set_Equipped_Items_FromNetwork = (ChrAsm_Set_Equipped_Items_FromNetwork_FUNC*)0x140743b60;

typedef void set_playergamedata_flags_FUNC(void* EquipGameData, uint16_t net_data);
set_playergamedata_flags_FUNC* set_playergamedata_flags = (set_playergamedata_flags_FUNC*)0x140747870;

typedef ChrIns* getEntity_FUNC(uint64_t WorldChrMan, uint32_t entityNum);
getEntity_FUNC* getEntity = (getEntity_FUNC*)0x1425a71a3;

typedef void* getThrowParamFromThrowId_FUNC(uint32_t throw_id);
getThrowParamFromThrowId_FUNC* getThrowParamFromThrowId = (getThrowParamFromThrowId_FUNC*)0x140534b20;

typedef void putAttackerIntoThrowAnimation_FUNC(uint64_t param_1);
putAttackerIntoThrowAnimation_FUNC* putAttackerIntoThrowAnimation = (putAttackerIntoThrowAnimation_FUNC*)0x1403ad660;

typedef void putDefenderIntoThrowAnimation_FUNC(uint64_t param_1, byte param_2);
putDefenderIntoThrowAnimation_FUNC* putDefenderIntoThrowAnimation = (putDefenderIntoThrowAnimation_FUNC*)0x1403ad760;

void Rollback::LoadRemotePlayerPacket(MainPacket* pkt)
{
    auto playerins_o = Game::get_connected_player(0);// player_id);
    if (!playerins_o.has_value())
    {
        ConsoleWrite("ERROR: Tried to get playerins for LoadRemotePlayerPacket, but unable to.");
        return;
    }
    PlayerIns* playerins = (PlayerIns*)playerins_o.value();

    //Type 1
    //TODO need to runtime debug
    playerins->chrins.maxHp = pkt->maxHp;
    *(uint32_t*)((uint64_t)(&playerins->playergamedata->attribs) + 8) = pkt->maxHp;

    PlayerIns_SetHp(playerins, pkt->curHp);
    //if ((0 < uVar14) && (uVar7 == 0)) {
    //thunk_FUN_1421710bc(pNVar2);
    //}
    *(uint32_t*)((uint64_t)(&playerins->playergamedata->attribs) + 4) = pkt->curHp;

    //Type 10
    if (*(uint32_t*)((uint64_t)(&playerins->playergamedata->attribs) + 0) != pkt->player_num)
    {
        FATALERROR("Remote player attribs playernum doesn't match the packet playernum.");
        return;
    }
    *(uint8_t*)((uint64_t)(&playerins->playergamedata->attribs) + 0xba) = pkt->player_sex;
    *(uint8_t*)((uint64_t)(&playerins->playergamedata->attribs) + 0x103) = pkt->covenantId;
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

    //Type 11
    set_playergamedata_flags((void*)equipgamedata, pkt->flags);

    //Type 16
    if (pkt->throw_id != -1)
    {
        ChrIns* attacker = getEntity(*(uint64_t*)Game::world_chr_man_imp, pkt->entitynum_attacker);
        ChrIns* defender = getEntity(*(uint64_t*)Game::world_chr_man_imp, pkt->entitynum_defender);
        uint64_t attacker_throw_info = (uint64_t)attacker->throw_animation_info;
        uint64_t defender_throw_info = (uint64_t)defender->throw_animation_info;

        //position
        *(float*)((uint64_t)(attacker->playerCtrl->chrCtrl.actionctrl) + 0x4e0 + 0) = pkt->attacker_position.X;
        *(float*)((uint64_t)(attacker->playerCtrl->chrCtrl.actionctrl) + 0x4e0 + 4) = pkt->attacker_position.Y;
        *(float*)((uint64_t)(attacker->playerCtrl->chrCtrl.actionctrl) + 0x4e0 + 8) = pkt->attacker_position.Z;
        *(float*)((uint64_t)(attacker->playerCtrl->chrCtrl.actionctrl) + 0x4e0 + 0xc) = 1;

        *(float*)((uint64_t)(defender->playerCtrl->chrCtrl.actionctrl) + 0x4e0 + 0) = pkt->defender_position.X;
        *(float*)((uint64_t)(defender->playerCtrl->chrCtrl.actionctrl) + 0x4e0 + 4) = pkt->defender_position.Y;
        *(float*)((uint64_t)(defender->playerCtrl->chrCtrl.actionctrl) + 0x4e0 + 8) = pkt->defender_position.Z;
        *(float*)((uint64_t)(defender->playerCtrl->chrCtrl.actionctrl) + 0x4e0 + 0xc) = 1;

        *(float*)(attacker_throw_info + 0x70 + 0) = pkt->attacker_position.X;
        *(float*)(attacker_throw_info + 0x70 + 4) = pkt->attacker_position.Z;
        *(float*)(attacker_throw_info + 0x70 + 8) = pkt->attacker_position.Y;
        *(float*)(attacker_throw_info + 0x70 + 0xc) = 1;

        *(float*)(defender_throw_info + 0x70 + 0) = pkt->defender_position.X;
        *(float*)(defender_throw_info + 0x70 + 4) = pkt->defender_position.Z;
        *(float*)(defender_throw_info + 0x70 + 8) = pkt->defender_position.Y;
        *(float*)(defender_throw_info + 0x70 + 0xc) = 1;

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
        *(uint32_t*)(attacker_throw_info + 0x24) = *(uint32_t*)(uint64_t)(defender + 8);
        *(uint32_t*)(defender_throw_info + 0x24) = *(uint32_t*)(uint64_t)(attacker + 8);

        //putIntoThrowAnimation
        putAttackerIntoThrowAnimation(attacker_throw_info);
        putDefenderIntoThrowAnimation(defender_throw_info, 1);
    }
}
