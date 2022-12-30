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
    void send_generalplayerinfo_helper(PlayerIns* p);

    uint64_t Read_GeneralPlayerData_return;
    void Read_GeneralPlayerData_injection();
    uint64_t Read_GeneralPlayerData_helper(uint64_t NetworkManipulator);
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
    //TODO make this a toggle
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

typedef uint32_t get_AnimationData_FUNC(ActionCtrl* actionctrl, uint32_t i);
get_AnimationData_FUNC* get_AnimationData = (get_AnimationData_FUNC*)0x1403853c0;

typedef uint16_t compress_gamedata_flags_FUNC(uint64_t equipgamedata);
compress_gamedata_flags_FUNC* compress_gamedata_flags = (compress_gamedata_flags_FUNC*)0x14074a5e0;

typedef uint32_t get_currently_selected_magic_id_FUNC(PlayerIns* playerins);
get_currently_selected_magic_id_FUNC* get_currently_selected_magic_id = (get_currently_selected_magic_id_FUNC*)0x14035fd10;

typedef bool sendNetMessageToAllPlayers_FUNC(uint64_t sessionMan, uint32_t type, void* data, int size);
sendNetMessageToAllPlayers_FUNC* sendNetMessageToAllPlayers = (sendNetMessageToAllPlayers_FUNC*)0x14050b880;

typedef uint32_t getNetMessage_FUNC(uint64_t session_man, uint64_t ConnectedPlayerData, uint32_t type, void* data_out, int max_size);
getNetMessage_FUNC* getNetMessage = (getNetMessage_FUNC*)0x14050b540;

typedef void GetTimestamp_FUNC(void* timestamp_out);
GetTimestamp_FUNC* GetTimestamp = (GetTimestamp_FUNC*)0x140d9cad0;

typedef uint16_t GetEntityNumForThrow_FUNC(void* WorldChrManImp, void* playerIns);
GetEntityNumForThrow_FUNC* GetEntityNumForThrow = (GetEntityNumForThrow_FUNC*)0x142847c6a;

void send_generalplayerinfo_helper(PlayerIns* playerins)
{
    MainPacket pkt;

    if (playerins == NULL)
    {
        ConsoleWrite("ERROR: playerins value at send_generalplayerinfo_helper is NULL.");
        return;
    }

    //Load Type 1
    pkt.position_x = *(float*)(((uint64_t)playerins->chrins.playerCtrl->chrCtrl.havokChara) + 0x10);
    pkt.position_z = *(float*)(((uint64_t)playerins->chrins.playerCtrl->chrCtrl.havokChara) + 0x14);
    pkt.position_y = *(float*)(((uint64_t)playerins->chrins.playerCtrl->chrCtrl.havokChara) + 0x18);
    pkt.ezStateActiveState = get_AnimationData(playerins->chrins.playerCtrl->chrCtrl.actionctrl, 1);
    pkt.ezStatePassiveState = get_AnimationData(playerins->chrins.playerCtrl->chrCtrl.actionctrl, 0);
    pkt.curHp = playerins->chrins.curHp;
    pkt.maxHp = playerins->chrins.maxHp;
    pkt.rotation = *(float*)(((uint64_t)playerins->chrins.playerCtrl->chrCtrl.havokChara) + 0x4);
    pkt.movement_direction_vals[0] = *(float*)(((uint64_t)(&playerins->chrins.padManipulator->chrManipulator)) + 0x10 + 0);
    pkt.movement_direction_vals[1] = *(float*)(((uint64_t)(&playerins->chrins.padManipulator->chrManipulator)) + 0x10 + 8);

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

    //Load Type 16
    uint64_t cur_throw_ptr = *(uint64_t*)((*(uint64_t*)Game::throw_man) + 0x68);
    if (cur_throw_ptr != NULL && *(uint64_t*)cur_throw_ptr != NULL)
    {
        uint64_t cur_throw = *(uint64_t*)cur_throw_ptr;
        PlayerIns* atk_playerins = (*(PlayerIns**)(*(uint64_t*)(cur_throw + 0) + 0x8));
        PlayerIns* def_playerins = (*(PlayerIns**)(*(uint64_t*)(cur_throw + 8) + 0x8));

        pkt.attacker_position = *(PosRotFloatVec*)(*(uint64_t*)(cur_throw + 0) + 0x70);
        pkt.attacker_position.Rotation = *(float*)(((uint64_t)atk_playerins->chrins.playerCtrl->chrCtrl.havokChara) + 0x4);

        pkt.defender_position = *(PosRotFloatVec*)(*(uint64_t*)(cur_throw + 8) + 0x70);
        pkt.defender_position.Rotation = *(float*)(((uint64_t)def_playerins->chrins.playerCtrl->chrCtrl.havokChara) + 0x4);

        pkt.entitynum_defender = GetEntityNumForThrow(*(void**)Game::world_chr_man_imp, def_playerins);
        pkt.entitynum_attacker = GetEntityNumForThrow(*(void**)Game::world_chr_man_imp, atk_playerins);
        pkt.throw_id = *(uint16_t*)(cur_throw + 0x10);
    }
    else
    {
        pkt.throw_id = -1;
    }

    //Load Type 17
    pkt.curSelectedMagicId = get_currently_selected_magic_id(playerins);
    pkt.curUsingItemId = (playerins->chrins).curUsedItem.itemId;

    //Load Type 76
    //need to have this a list of all speffects currently applied to the PC
    //don't need timestamp because we'll refresh every packet
    SpecialEffect_Info* specialEffectInfo = playerins->chrins.specialEffects->specialEffect_Info;
    size_t i = 0;
    while (specialEffectInfo != NULL)
    {
        uint32_t specialEffectInfo_id = *(uint32_t*)((uint64_t)specialEffectInfo + 0x30);
        if (i >= sizeof(pkt.spEffectToApply) / sizeof(uint32_t))
        {
            FATALERROR("pkt.spEffectToApply size of 15 is insufficent.");
        }
        pkt.spEffectToApply[i] = specialEffectInfo_id;
        i++;
        specialEffectInfo = specialEffectInfo->next;
    }
    for (; i < sizeof(pkt.spEffectToApply) / sizeof(uint32_t); i++)
    {
        pkt.spEffectToApply[i] = -1;
    }

    //send out packet
    sendNetMessageToAllPlayers(*(uint64_t*)Game::session_man_imp, Rollback::RollbackSinglePacketType, &pkt, sizeof(pkt));
}

typedef PlayerIns* getPlayerInsForConnectedPlayerData_FUNC(void* worldchrman, void* ConnectedPlayerData);
getPlayerInsForConnectedPlayerData_FUNC* getPlayerInsForConnectedPlayerData = (getPlayerInsForConnectedPlayerData_FUNC*)0x140371d90;

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

        //temporary, will use this with GGPO later
        Rollback::LoadRemotePlayerPacket(&pkt, playerins);
    }

    return Rollback::rollbackEnabled;
}

typedef void PlayerIns_SetHp_FUNC(void* playerins, uint64_t curHp);
PlayerIns_SetHp_FUNC* PlayerIns_SetHp = (PlayerIns_SetHp_FUNC*)0x140322910;

typedef void ChrAsm_Set_Equipped_Items_FromNetwork_FUNC(void* EquipGameData, uint32_t index, uint32_t given_item_id, int param_4, bool param_5);
ChrAsm_Set_Equipped_Items_FromNetwork_FUNC* ChrAsm_Set_Equipped_Items_FromNetwork = (ChrAsm_Set_Equipped_Items_FromNetwork_FUNC*)0x140746840;

typedef void set_playergamedata_flags_FUNC(void* EquipGameData, uint16_t net_data);
set_playergamedata_flags_FUNC* set_playergamedata_flags = (set_playergamedata_flags_FUNC*)0x14074a550;

typedef ChrIns* getEntity_FUNC(uint64_t WorldChrMan, uint32_t entityNum);
getEntity_FUNC* getEntity = (getEntity_FUNC*)0x1428611e2;

typedef void* getThrowParamFromThrowId_FUNC(uint32_t throw_id);
getThrowParamFromThrowId_FUNC* getThrowParamFromThrowId = (getThrowParamFromThrowId_FUNC*)0x140537210;

typedef void putAttackerIntoThrowAnimation_FUNC(uint64_t param_1);
putAttackerIntoThrowAnimation_FUNC* putAttackerIntoThrowAnimation = (putAttackerIntoThrowAnimation_FUNC*)0x1403acc70;

typedef void putDefenderIntoThrowAnimation_FUNC(uint64_t param_1, byte param_2);
putDefenderIntoThrowAnimation_FUNC* putDefenderIntoThrowAnimation = (putDefenderIntoThrowAnimation_FUNC*)0x1403acd70;

typedef void Apply_SpeffectSync_FromNetwork_FUNC(void* chrins, uint32_t speffect_id, uint32_t timestamp, float const);
Apply_SpeffectSync_FromNetwork_FUNC* Apply_SpeffectSync_FromNetwork = (Apply_SpeffectSync_FromNetwork_FUNC*)0x142f69ca9;

typedef void ActionCtrl_ApplyEzState_FUNC(ActionCtrl* actionctrl, uint32_t unk, uint32_t ezState);
ActionCtrl_ApplyEzState_FUNC* ActionCtrl_ApplyEzState = (ActionCtrl_ApplyEzState_FUNC*)0x140385440;

typedef void ChrCtrl_Func_30_FUNC(ChrCtrl* param_1, float FrameTime_const);
ChrCtrl_Func_30_FUNC* ChrCtrl_Func_30 = (ChrCtrl_Func_30_FUNC*)0x14037c250;

void Rollback::LoadRemotePlayerPacket(MainPacket* pkt, PlayerIns* playerins)
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
    *(float*)(((uint64_t)playerins->chrins.playerCtrl->chrCtrl.havokChara) + 0x4) = pkt->rotation;
    *(float*)(((uint64_t)(&playerins->chrins.padManipulator->chrManipulator)) + 0x10 + 0) = pkt->movement_direction_vals[0];
    *(float*)(((uint64_t)(&playerins->chrins.padManipulator->chrManipulator)) + 0x10 + 4) = 0.0f;
    *(float*)(((uint64_t)(&playerins->chrins.padManipulator->chrManipulator)) + 0x10 + 8) = pkt->movement_direction_vals[1];
    *(float*)(((uint64_t)(&playerins->chrins.padManipulator->chrManipulator)) + 0x10 + 0xc) = 0.0f;

    //Type 10
    *(uint32_t*)((uint64_t)(&playerins->playergamedata->attribs) + 0) = pkt->player_num;
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
    if (pkt->curUsingItemId != -1)
    {
        (playerins->chrins).curUsedItem.itemId = pkt->curUsingItemId;
        (playerins->chrins).curUsedItem.amountUsed = 1;
    }

    // Type 76
    //remove all old speffects, and apply these as new ones
    uint64_t timestamp_data[2];
    GetTimestamp(&timestamp_data);
    uint32_t timestamp = (uint32_t)(timestamp_data[0] / 10000 >> 4);
    for (size_t i = 0; i < sizeof(pkt->spEffectToApply) / sizeof(uint32_t); i++)
    {
        if (pkt->spEffectToApply[i] != -1)
        {
            Apply_SpeffectSync_FromNetwork(playerins, pkt->spEffectToApply[i], timestamp & 0x3fffffff, 1.0f);
        }
    }
}
