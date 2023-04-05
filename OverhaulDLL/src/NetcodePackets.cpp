#include "Rollback.h"
#include "NetcodePackets.h"
#include "GameData.h"
#include "SP/memory.h"
#include "SP/memory/injection/asm/x64.h"
#include <unordered_set>

const uint64_t Rollback::PlayerIns_Is_NetworkedPlayer_offsets[] = {
    //0x1400d81c5,
    //0x1400d826f,
    //0x14014c3d3,
    //0x14014c42e,
    //0x1402fccc0,
    //0x14030ef67,
    //0x14031f4e1,
    0x140322eda, //needed for stamina calculation
    //0x140324271,
    //0x1403242c5,
    //0x140324322,
    //0x140324529,
    //0x140324975,
    //0x140325ac2,
    //0x140327520,
    //0x140327ca9,
    //0x140329ce7,
    //0x14032b0ca,
    //0x14032e0e7,
    //0x14032ef69,
    //0x140342e1b,
    //0x140353c0a,
    //0x140353c29,
    //0x14035614a,
    //0x1403561e2,
    //0x14035689a,
    //0x14035a79b,
    //0x14035a7bb,
    //0x14035a7db,
    //0x14035a7f0,
    //0x140360476,
    //0x14036ede3,
    //0x140370266,
    //0x140370591,
    //0x14037922a,
    //0x1403792d9,
    //0x140395a78,
    //0x140406776,
    //0x14040689e,
    //0x1404229c5,
    //0x140507a30,
    //0x140507dcb,
    //0x140509036,
    //0x140630a20,
    //0x140659899,
    //0x140660ce5,
    //0x140cd1f9b,
    //0x140dfa3f9,
    //0x14108d2db,
    //0x141103c74,
    //0x1411ccf00
};

const uint64_t Rollback::PlayerIns_IsHostPlayerIns_offsets[] = {
    //0x1400d8101,
    //0x1400d81b5,
    //0x140120fd9,
    //0x14014c2e9,
    //0x14014c314,
    //0x14014c3e3,
    //0x14014c43e,
    //0x1402fccb0,
    //0x14030f30d,
    //0x14030f3ca,
    //0x14031d780,
    //0x14031f25c,
    //0x14031f4ce,
    //0x14031f68f,
    //0x14031f95a,
    //0x140320884,
    //0x140321429,
    //0x140321669,
    //0x1403218c5,
    //0x1403226fc,
    //0x14032425d,
    //0x1403242df,
    //0x140324308,
    //0x140324502,
    //0x1403248fb,
    //0x140324a68,
    //0x140324ab1,
    //0x140324f5d,
    //0x14032500b,
    //0x14032575b,
    //0x1403258ca,
    //0x140325ab2,
    //0x140326cd1,
    //0x140326d8c,
    //0x140327235,
    //0x140327425,
    //0x14032748c,
    //0x140327c69,
    //0x1403280bb,
    //0x140328141,
    //0x1403287c1,
    //0x140328c17,
    //0x140328dd2,
    //0x140329cd7,
    //0x140329e8d,
    //0x14032a839,
    //0x14032a863,
    //0x14032ad56,
    //0x14032add1,
    //0x14032b0ba,
    //0x14032b9f6,
    //0x14032c5fc,
    //0x14032c65c,
    //0x14032c69c,
    //0x14032cf64,
    //0x14032df14,
    //0x14032e3f6,
    //0x14032e8ac,
    //0x14032ef4f,
    //0x14032f15a,
    //0x14032f5f3,
    //0x14032ff23,
    //0x14033021b,
    //0x1403358b2,
    //0x1403418b9,
    //0x140341ebf,
    //0x140342a00,
    //0x140342e0a,
    //0x1403431b7,
    //0x1403431fa,
    //0x14034bd12,
    //0x140353bcb,
    //0x140355e4a,
    //0x14035613a,
    //0x1403561c3,
    //0x140356bf6,
    //0x140356c28,
    //0x140356c79,
    //0x140356d13,
    //0x140356d51,
    0x140356ec7, //needed to compute roll type
    //0x140357077,
    //0x140357263,
    //0x1403575bd,
    //0x14035772e,
    //0x1403577e9,
    //0x1403579a1,
    //0x140357dc4,
    //0x140358219,
    //0x140358bd2,
    //0x140358c9c,
    //0x140358ee3,
    //0x140359193,
    //0x140359b07,
    //0x140359e9d,
    //0x140359fb7,
    //0x14035a032,
    //0x14035a0ec,
    //0x14035a16c,
    //0x14035a1ac,
    //0x14035a23c,
    //0x14035a319,
    //0x14035a78b,
    //0x14035a7cb,
    //0x14035c93c,
    //0x14035cd6e,
    //0x14035df19,
    //0x14035e0be,
    //0x14035e27a,
    //0x14035fa07,
    //0x140360456,
    //0x14036062b,
    //0x14036066c,
    //0x1403616a4,
    //0x140361b02,
    //0x140361b27,
    //0x140361ba3,
    //0x140361bd4,
    //0x140361c3f,
    //0x140361c75,
    //0x140361da3,
    //0x14036220e,
    //0x1403623d2,
    //0x14036f056,
    //0x140370252,
    //0x14037057d,
    //0x140373dd6,
    //0x140373df2,
    //0x140379219,
    //0x140382584,
    //0x1403909d4,
    //0x140392f71,
    //0x140393427,
    //0x1403971b5,
    //0x1403abd0c,
    //0x1403ae1b4,
    //0x1403be220,
    //0x1403be2ec,
    //0x14041bbb6,
    //0x14041bf68,
    //0x140480d54,
    //0x140491ff1,
    //0x140492041,
    //0x1404ac973,
    //0x1404cc02e,
    //0x1404cc159,
    //0x1404cc1f3,
    //0x1404cca0a,
    //0x140507a20,
    //0x140507dbb,
    //0x140575643,
    //0x1406598e5,
    //0x140660e84,
    //0x140670179,
    //0x14074511f,
    //0x140745438,
    //0x140dfa774,
    //0x140dfabed,
    //0x140dfc59e,
    //0x1410b8e88,
    //0x1410b8fdd,
    //0x1410b91b1,
    //0x1410b91f4,
    //0x1410b9300,
    //0x1410b936c,
    //0x1410b9478,
    //0x1410b96a1,
    //0x1410b96e4,
    //0x1410b9741,
    //0x1410b9928,
    //0x1410b9c4c,
    //0x1410b9cb9,
    //0x1410b9e58,
    //0x14110cab4,
};

extern "C" {
    uint64_t sendNetMessage_return;
    void sendNetMessage_injection();
    bool sendNetMessage_helper(void* session_man, uint64_t ConnectedPlayerData, uint32_t type, uint8_t* data);

    uint64_t getNetMessage_return;
    void getNetMessage_injection();
    bool getNetMessage_helper(void* session_man, uint64_t ConnectedPlayerData, uint32_t type);

    bool PlayerIns_Is_NetworkedPlayer_helper(PlayerIns* pc);
    bool WorldChrManImp_IsHostPlayerIns_helper(PlayerIns* pc);
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

    //Correct a number of checks related to PlayerIns_Is_NetworkedPlayer and WorldChrManImp_IsHostPlayerIns
    //These are used to determine
    // - if an attack should just hit or be ignored in favor of waiting for type18 packet
    // - other stuff
    //Instead of modifying the functions directly, modify the calls to them to use a different pointer
    uint64_t PlayerIns_Is_NetworkedPlayer_helper_addr = (uint64_t)&PlayerIns_Is_NetworkedPlayer_helper; //trampoline location
    write_address = (uint8_t*)(Rollback::PlayerIns_PlayerIns_Is_NetworkedPlayer_trampoline_offset + Game::ds1_base);
    sp::mem::patch_bytes(write_address, (uint8_t*)&PlayerIns_Is_NetworkedPlayer_helper_addr, sizeof(uint64_t));
    write_address = (uint8_t*)(Rollback::EnemyIns_PlayerIns_Is_NetworkedPlayer_trampoline_offset + Game::ds1_base);
    sp::mem::patch_bytes(write_address, (uint8_t*)&PlayerIns_Is_NetworkedPlayer_helper_addr, sizeof(uint64_t));
    write_address = (uint8_t*)(Rollback::ReplayGhostIns_PlayerIns_Is_NetworkedPlayer_trampoline_offset + Game::ds1_base);
    sp::mem::patch_bytes(write_address, (uint8_t*)&PlayerIns_Is_NetworkedPlayer_helper_addr, sizeof(uint64_t));

    uint64_t WorldChrManImp_IsHostPlayerIns_helper_addr = (uint64_t)&WorldChrManImp_IsHostPlayerIns_helper;
    write_address = (uint8_t*)(Rollback::PlayerIns_WorldChrManImp_IsHostPlayerIns_trampoline_offset + Game::ds1_base);
    sp::mem::patch_bytes(write_address, (uint8_t*)&WorldChrManImp_IsHostPlayerIns_helper_addr, sizeof(uint64_t));
    write_address = (uint8_t*)(Rollback::EnemyIns_WorldChrManImp_IsHostPlayerIns_trampoline_offset + Game::ds1_base);
    sp::mem::patch_bytes(write_address, (uint8_t*)&WorldChrManImp_IsHostPlayerIns_helper_addr, sizeof(uint64_t));
    write_address = (uint8_t*)(Rollback::ReplayGhostIns_WorldChrManImp_IsHostPlayerIns_trampoline_offset + Game::ds1_base);
    sp::mem::patch_bytes(write_address, (uint8_t*)&WorldChrManImp_IsHostPlayerIns_helper_addr, sizeof(uint64_t));

    //call our helpers as a very far away vtable entry. this allows us to patch, instead of inject
    uint8_t call_IsNetworkedPlayer_trampoline_addr[6] = { 0xff, 0x90, 0x02, 0x17, 0x00, 0x00 }; //call QWORD PTR [rax+0x1702]. RAX is the playerins vtable, +0x1702 is our trampoline offset
    uint8_t call_IsHostPlayerIns_trampoline_addr[6] = { 0xff, 0x90, 0x0A, 0x17, 0x00, 0x00 }; //call QWORD PTR [rax+0x170A].
    for (uint64_t patch_loc : PlayerIns_Is_NetworkedPlayer_offsets)
    {
        write_address = (uint8_t*)(patch_loc);
        sp::mem::patch_bytes(write_address, call_IsNetworkedPlayer_trampoline_addr, sizeof(call_IsNetworkedPlayer_trampoline_addr));
    }
    for (uint64_t patch_loc : PlayerIns_IsHostPlayerIns_offsets)
    {
        write_address = (uint8_t*)(patch_loc);
        sp::mem::patch_bytes(write_address, call_IsHostPlayerIns_trampoline_addr, sizeof(call_IsHostPlayerIns_trampoline_addr));
    }

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

bool PlayerIns_Is_NetworkedPlayer_helper(PlayerIns* pc)
{
    bool res = PlayerIns_Is_NetworkedPlayer(pc);

    if (Rollback::rollbackEnabled || Rollback::networkTest)
    {
        //return false if this is a phantom
        uint32_t handle = *(uint32_t*)((uint64_t)(&pc->chrins) + 8);
        if (handle >= Game::PC_Handle && handle < Game::PC_Handle + 10)
        {
            res = false;
        }
    }

    return res;
}

bool WorldChrManImp_IsHostPlayerIns_helper(PlayerIns* pc)
{
    PlayerIns* hostPc = *(PlayerIns**)((*((uint64_t*)Game::world_chr_man_imp)) + 0x68);

    if (!Rollback::rollbackEnabled && !Rollback::networkTest)
    {
        return hostPc == pc;
    }
    else
    {
        uint32_t playerHandle = *(uint32_t*)(((uint64_t)pc) + 8);
        return hostPc == pc || (playerHandle >= Game::PC_Handle && playerHandle < Game::PC_Handle + 10);
    }
}
