/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Ainsley Harriott  -  Reverse engineering, Anti-Cheat Identification
        metal-crow  -  Reverse engineering, Anti-Cheat Identification
*/

#include "AntiAntiCheat.h"
#include "GameData.h"
#include "SP/memory/injection/asm/x64.h"
#include <string.h>
#include <unordered_set>

const uint64_t game_invalid_data_checks[] = {
    0x7f2c44,
    0x7f2c5f,
    0x7f2c7c,
    0x7f2c99,
    0x7f2cb6,
    0x7f2ce4,
    0x7f2d12,
    0x7f2d40,
    0x7f2d6e,
    0x7f2dda,
    0x7f2e4a,
    0x7f2e67,
    0x7f2ea6,
    0x7f2ec6,
    0x7f2ee8,
};

extern "C" {
    uint64_t game_write_playerdata_to_flatbuffer_injection_return;
    void game_write_playerdata_to_flatbuffer_injection();
    uint64_t game_write_playerdata_to_flatbuffer_injection_helper(uint32_t*, uint64_t);

    uint64_t construct_flatbuffer_from_PlayerStatus_MemberFlags_injection_return;
    void construct_flatbuffer_from_PlayerStatus_MemberFlags_injection();
    void construct_flatbuffer_from_PlayerStatus_MemberFlags_injection_helper(uint64_t);

    uint64_t finish_construct_flatbuffer_from_PlayerStatus_MemberFlags_injection_return;
    void finish_construct_flatbuffer_from_PlayerStatus_MemberFlags_injection();
    void finish_construct_flatbuffer_from_PlayerStatus_MemberFlags_injection_helper();

    uint64_t set_MemberFlags_bitflag_injection_return;
    void set_MemberFlags_bitflag_injection();
    bool set_MemberFlags_bitflag_injection_helper(uint64_t flagid_);
}

enum MemberFlags_IdentifiersEnum
{
    AreaId = 0x0,
    ClearCount = 0x1,
    DeathCount = 0x2,
    MultiplayerCount = 0x3,
    CoopSuccessCount = 0x4,
    New_Name_5 = 0x5,
    New_Name_6 = 0x6,
    RegulationVersion = 0x7,
    SoulLevel = 0x8,
    SoulCount = 0x9,
    SoulMemory = 0xa,
    Archetype = 0xb,
    HP = 0xc,
    MaxHp = 0xd,
    BaseMaxHp_1 = 0xe,
    Mp = 0xf,
    MaxMp = 0x10,
    BaseMaxHp_2 = 0x11,
    Vitality = 0x12,
    Attunement = 0x13,
    Endurance = 0x14,
    Strength = 0x15,
    Dexterity = 0x16,
    Resistance = 0x17,
    Intelligence = 0x18,
    Force = 0x19,
    ItemDiscoveryRate = 0x1a,
    attackR1 = 0x1b,
    attackR2 = 0x1c,
    attackL1 = 0x1d,
    attackL2 = 0x1e,
    MaxWeaponLevel = 0x1f,
    EstusLevel = 0x20,
    RankingRegistration = 0x21,
    PlayTime = 0x22,
    HumanityCount = 0x23,
    Left_Hand_1 = 0x24,
    Left_Hand_2 = 0x25,
    Right_Hand_1 = 0x26,
    Right_Hand_2 = 0x27,
    ArmorHeadInv = 0x28,
    ArmorBodyInv = 0x29,
    ArmorArmsInv = 0x2a,
    ArmorLegsInv = 0x2b,
    LeftHandHeldWeaponSlot = 0x2c,
    RightHandHeldWeaponSlot = 0x2d,
    Stamina = 0x2e,
    MaxStamina = 0x2f,
    BaseMaxStamina = 0x30,
    WeaponinSlot0 = 0x31,
    WeaponinSlot2 = 0x32,
    WeaponinSlot1 = 0x33,
    WeaponinSlot3 = 0x34,
    ArmorinSlot0 = 0x35,
    ArmorinSlot1 = 0x36,
    ArmorinSlot2 = 0x37,
    ArmorinSlot3 = 0x38,
    RinginSlot0 = 0x39,
    RinginSlot1 = 0x3a,
    IteminQuickbar0 = 0x3b,
    IteminQuickbar1 = 0x3c,
    IteminQuickbar2 = 0x3d,
    IteminQuickbar3 = 0x3e,
    IteminQuickbar4 = 0x3f,
    IteminArrowBoltSlot0 = 0x40,
    IteminArrowBoltSlot1 = 0x41,
    IteminArrowBoltSlot2 = 0x42,
    IteminArrowBoltSlot3 = 0x43,
    EquippedSpell1 = 0x44,
    EquippedSpell2 = 0x45,
    EquippedSpell3 = 0x46,
    EquippedSpell4 = 0x47,
    EquippedSpell5 = 0x48,
    EquippedSpell6 = 0x49,
    EquippedSpell7 = 0x4a,
    EquippedSpell8 = 0x4b,
    EquippedSpell9 = 0x4c,
    EquippedSpell10 = 0x4d,
    EquippedSpell11 = 0x4e,
    EquippedSpell12 = 0x4f,
    SessionRoleID = 0x50,
    New_Name_81 = 0x51,
    New_Name_82 = 0x52,
    RegionMatchmaking = 0x53,
    isPlayerHollow = 0x54,
    PlayerSex = 0x55,
    isPlayerHuman = 0x56,
    New_Name_87 = 0x57,
    New_Name_88 = 0x58,
    New_Name_89 = 0x59,
    hasInvasionTimeLimit = 0x5a,
    New_Name_91 = 0x5b,
    Const0 = 0x5c,
    New_Name_93 = 0x5d,
    defSAToughnessTotal = 0x5e,
    MaxEquipLoad = 0x5f,
    CharacterName = 0x60,
    New_Name_97 = 0x61,
    New_Name_98 = 0x62,
    recentMPAreasVisited = 0x63,
    New_Name_100 = 0x64,
    MpRegion = 0x65,
    SessionNatType = 0x66,
    CovenantId = 0x67,
    inSession = 0x68,
    SessionState = 0x69,
    NormalDefenses = 0x6a,
    NormalResists = 0x6b,
    CovenantLevel = 0x6c,
    New_Name_109 = 0x6d,
    New_Name_110 = 0x6e,
    New_Name_111 = 0x6f,
    New_Name_112 = 0x70,
    New_Name_113 = 0x71,
    New_Name_114 = 0x72,
    New_Name_115 = 0x73,
    New_Name_116 = 0x74,
    New_Name_117 = 0x75,
    New_Name_118 = 0x76,
};

void AntiAntiCheat::start() {
    ConsoleWrite("Disabling built in AntiCheat...");
    uint8_t *write_address;

    //TODO patch out the hash comparison checks (compare with input to always be true)

    //TODO patch out the conditional jmps to the start of anti-cheat functions to disable them

    //Disable what appear to be cheat detection things
    //change the setting of the bool so that it always stays false
    //i don't see any situation where this bool is actually _read_, but better safe
    uint8_t patch_false = 0 ;
    for (auto patch_loc : game_invalid_data_checks)
    {
        bool res = sp::mem::patch_bytes((void*)(Game::ds1_base+patch_loc+6), &patch_false, 1);
        if (!res) FATALERROR("Unable to disable invalid game data check for addr %x", patch_loc);
    }
    
    //Before sending to the server make sure the data is correct
    write_address = (uint8_t*)(AntiAntiCheat::game_write_playerdata_to_flatbuffer_injection_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &game_write_playerdata_to_flatbuffer_injection_return, 0, &game_write_playerdata_to_flatbuffer_injection);

    //Prevent RequestUpdatePlayerStatus from sending anything we don't want it too
    //fix all but the mandatory flags from the inputdata when it's read by construct_flatbuffer_from_PlayerStatus_MemberFlags 
    write_address = (uint8_t*)(AntiAntiCheat::construct_flatbuffer_from_PlayerStatus_MemberFlags_injection_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &construct_flatbuffer_from_PlayerStatus_MemberFlags_injection_return, 1, &construct_flatbuffer_from_PlayerStatus_MemberFlags_injection);

    write_address = (uint8_t*)(AntiAntiCheat::finish_construct_flatbuffer_from_PlayerStatus_MemberFlags_injection_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &finish_construct_flatbuffer_from_PlayerStatus_MemberFlags_injection_return, 1, &finish_construct_flatbuffer_from_PlayerStatus_MemberFlags_injection);

    //SendBuild_RequestNotifyUserLogMessage_GeneralRequestTask (the other function besides construct_flatbuffer_from_PlayerStatus_MemberFlags that puts flags in the packet)
    //do we need to remove anything here? it seems safe

    //Prevent any member flags from being set ANYWHERE that are not required
    write_address = (uint8_t*)(AntiAntiCheat::set_MemberFlags_bitflag_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &set_MemberFlags_bitflag_injection_return, 1, &set_MemberFlags_bitflag_injection);
}


//use to store the original values of the memberflags before we edited them, since touching them directly can break stuff
//save the game's true value here on function entry, overwrite the game values so the flatbuffer sees our values, then restore to this on function exit
void* memberflags_ptr = NULL;
uint64_t memberflags_orig[4];

void set_value_in_MemberFlags(uint64_t input_data, MemberFlags_IdentifiersEnum flag, void* value)
{
    if (flag < MemberFlags_IdentifiersEnum::Stamina)
    {
        *(uint32_t*)(input_data + 0x20 + 4*flag) = *(uint32_t*)value;
        return;
    }
    uint32_t flag_2 = flag - 0x2E;
    if (flag_2 < 0x23)
    {
        *(uint32_t*)(input_data + 0xD8 + 4*flag_2) = *(uint32_t*)value;
        return;
    }
    uint32_t flag_4 = flag - 0x53;
    if (flag_4 < 0xA)
    {
        *(uint8_t*)(input_data + 0x170 + 1*flag_4) = *(uint8_t*)value;
        return;
    }
    uint32_t flag_5 = flag - 0x5D;
    if (flag_5 < 0x3)
    {
        *(float*)(input_data + 0x17C + 4*flag_5) = *(float*)value;
        return;
    }
    if (flag == MemberFlags_IdentifiersEnum::NormalResists)
    {
        *(uint64_t*)(input_data + 0x274) = *(uint64_t*)value;
        *(uint32_t*)(input_data + 0x27c) = *(uint32_t*)((uint64_t)value+8);
        return;
    }
    if (flag == MemberFlags_IdentifiersEnum::NormalDefenses)
    {
        *(uint32_t*)(input_data + 0x258) = *(uint32_t*)((uint64_t)value + 0);
        *(uint32_t*)(input_data + 0x25c) = *(uint32_t*)((uint64_t)value + 4);
        *(uint32_t*)(input_data + 0x260) = *(uint32_t*)((uint64_t)value + 8);
        *(uint32_t*)(input_data + 0x264) = *(uint32_t*)((uint64_t)value + 12);
        *(uint64_t*)(input_data + 0x268) = *(uint64_t*)((uint64_t)value + 16);
        *(uint32_t*)(input_data + 0x270) = *(uint32_t*)((uint64_t)value + 24);
        return;
    }
    if (flag == MemberFlags_IdentifiersEnum::CovenantLevel)
    {
        *(uint32_t*)(input_data + 0x280) = *(uint32_t*)((uint64_t)value + 0);
        *(uint32_t*)(input_data + 0x284) = *(uint32_t*)((uint64_t)value + 4);
        *(uint32_t*)(input_data + 0x288) = *(uint32_t*)((uint64_t)value + 8);
        *(uint32_t*)(input_data + 0x28C) = *(uint32_t*)((uint64_t)value + 12);
        *(uint64_t*)(input_data + 0x290) = *(uint64_t*)((uint64_t)value + 16);
        *(uint32_t*)(input_data + 0x298) = *(uint32_t*)((uint64_t)value + 24);
        return;
    }
    if (flag == MemberFlags_IdentifiersEnum::New_Name_110)
    {
        *(uint64_t*)(input_data + 0x29c) = *(uint64_t*)((uint64_t)value + 0);
        *(uint16_t*)(input_data + 0x2a4) = *(uint16_t*)((uint64_t)value + 8);
        *(uint8_t*)(input_data + 0x2a6) = *(uint8_t*)((uint64_t)value + 10);
        return;
    }
    if (flag == MemberFlags_IdentifiersEnum::New_Name_111)
    {
        *(uint64_t*)(input_data + 0x2a8) = *(uint64_t*)((uint64_t)value + 0);
        *(uint64_t*)(input_data + 0x2a8+8) = *(uint64_t*)((uint64_t)value + 8);
        *(uint64_t*)(input_data + 0x2a8+16) = *(uint64_t*)((uint64_t)value + 16);
        *(uint32_t*)(input_data + 0x2a8+24) = *(uint32_t*)((uint64_t)value + 24);
        return;
    }

    FATALERROR("%s Invalid flag=%d", __FUNCTION__, flag);
}

void compute_MemberFlags_bitflag(uint64_t* bitflags_and, MemberFlags_IdentifiersEnum flag)
{
    bitflags_and[(flag >> 0x6)] |= 1ULL << (flag & 0x3f);
    bitflags_and[2 + (flag >> 0x6)] |= 1ULL << (flag & 0x3f);
}

void construct_flatbuffer_from_PlayerStatus_MemberFlags_injection_helper(uint64_t input_data)
{
    input_data = input_data + 0x78; //&input_data->memberflags

    //save the old values
    memberflags_ptr = (void*)input_data;
    memcpy_s(memberflags_orig, sizeof(memberflags_orig), memberflags_ptr, 32);

    //allow only some mandatory data to be sent
    uint64_t membitflags_allowed[4];
    memset(membitflags_allowed, 0, sizeof(membitflags_allowed));

    //needed in invade
    compute_MemberFlags_bitflag(membitflags_allowed, MemberFlags_IdentifiersEnum::AreaId);
    compute_MemberFlags_bitflag(membitflags_allowed, MemberFlags_IdentifiersEnum::MpRegion);
    compute_MemberFlags_bitflag(membitflags_allowed, MemberFlags_IdentifiersEnum::RegionMatchmaking);
    compute_MemberFlags_bitflag(membitflags_allowed, MemberFlags_IdentifiersEnum::CovenantId);
    compute_MemberFlags_bitflag(membitflags_allowed, MemberFlags_IdentifiersEnum::hasInvasionTimeLimit);
    //needed to be invaded
    compute_MemberFlags_bitflag(membitflags_allowed, MemberFlags_IdentifiersEnum::SoulLevel);
    compute_MemberFlags_bitflag(membitflags_allowed, MemberFlags_IdentifiersEnum::ClearCount);
    compute_MemberFlags_bitflag(membitflags_allowed, MemberFlags_IdentifiersEnum::MaxWeaponLevel);
    compute_MemberFlags_bitflag(membitflags_allowed, MemberFlags_IdentifiersEnum::isPlayerHuman);
    compute_MemberFlags_bitflag(membitflags_allowed, MemberFlags_IdentifiersEnum::RegulationVersion);
    compute_MemberFlags_bitflag(membitflags_allowed, MemberFlags_IdentifiersEnum::SessionState);

    //extra checks, to be safe

    uint32_t* arraystart = *(uint32_t**)(input_data + 0x228);
    uint32_t* arrayend = *(uint32_t**)(input_data + 0x230);
    while (arraystart != arrayend)
    {
        //this check probably isn't needed but just to be safe
        if (*arraystart < 0 || *arraystart > 8)
        {
            FATALERROR("UNKNOWN SessionState VALUE: %d", *arraystart);
        }
        arraystart += 1;
    }
    uint32_t RegulationVersion = *(uint32_t*)(input_data + 0x20 + (MemberFlags_IdentifiersEnum::RegulationVersion * 4));
    if (RegulationVersion != 0 && RegulationVersion != 1040000)
    {
        FATALERROR("UNKNOWN RegulationVersion VALUE: %d", RegulationVersion);
    }
    uint8_t isPlayerHuman = *(uint8_t*)(input_data + 0x170 + ((MemberFlags_IdentifiersEnum::isPlayerHuman-0x53) * 1));
    if (isPlayerHuman != 0 && isPlayerHuman != 1)
    {
        FATALERROR("UNKNOWN isPlayerHuman VALUE: %d", isPlayerHuman);
    }

    //unset all the bitflags we don't whitelist
    //this ensures we only remove data, instead of adding data
    uint64_t membitflags_resultant[4];
    membitflags_resultant[0] = memberflags_orig[0] & membitflags_allowed[0];
    membitflags_resultant[1] = memberflags_orig[1] & membitflags_allowed[1];
    membitflags_resultant[2] = memberflags_orig[2] & membitflags_allowed[2];
    membitflags_resultant[3] = memberflags_orig[3] & membitflags_allowed[3];
    memcpy_s((void*)input_data, 32, membitflags_resultant, sizeof(membitflags_resultant));
}

void finish_construct_flatbuffer_from_PlayerStatus_MemberFlags_injection_helper()
{
    if (memberflags_ptr == NULL)
    {
        FATALERROR("%s has memberflags_ptr as null", __FUNCTION__);
        return;
    }
    //restore the old values and be safe to ensure we don't use a stale pointer
    memcpy_s(memberflags_ptr, 32, memberflags_orig, sizeof(memberflags_orig));
    memberflags_ptr = NULL;
}


const uint32_t hackerFlag = 0x1770;
const std::unordered_set<uint32_t> known_good_hash_vals_array_x64 = { 0x0, 0x37A, 0x3DE, 0x0AF0, 0x0E74, 0x29D6, 0x13C5E, 0x1895C, 0x4C4EBA, 0x0F1EB4, 1, 0x317, 0xa8f, 0x3e7 };

// we want to edit this at the latest point possible, so we don't mess up any other parts of the game
// this changes only the value that's in the flatbuffer
uint64_t game_write_playerdata_to_flatbuffer_injection_helper(uint32_t* array_start, uint64_t array_len)
{
    size_t i = 0;

    while (i < array_len)
    {
        if (array_start[i] == hackerFlag)
        {
            //remove this element
            for (size_t j = i; j < array_len; j++)
            {
                array_start[j] = array_start[j + 1];
            }
            array_len -= 1;
            continue;
        }

        if (known_good_hash_vals_array_x64.count(array_start[i]) == 0)
        {
            FATALERROR("Unknown element in flags array: %d=%x", i, array_start[i]);
        }

        i++;
    }

    return array_len;
}

//return false if we don't want the flag to be set
bool set_MemberFlags_bitflag_injection_helper(uint64_t flagid_)
{
    MemberFlags_IdentifiersEnum flagid = static_cast<MemberFlags_IdentifiersEnum>(flagid_);

    switch (flagid)
    {
    case MemberFlags_IdentifiersEnum::AreaId:
    case MemberFlags_IdentifiersEnum::MpRegion:
    case MemberFlags_IdentifiersEnum::RegionMatchmaking:
    case MemberFlags_IdentifiersEnum::CovenantId:
    case MemberFlags_IdentifiersEnum::hasInvasionTimeLimit:
    case MemberFlags_IdentifiersEnum::SoulLevel:
    case MemberFlags_IdentifiersEnum::ClearCount:
    case MemberFlags_IdentifiersEnum::MaxWeaponLevel:
    case MemberFlags_IdentifiersEnum::isPlayerHuman:
    case MemberFlags_IdentifiersEnum::RegulationVersion:
    case MemberFlags_IdentifiersEnum::SessionState:
        return true;
    default:
        return false;
    }
}
