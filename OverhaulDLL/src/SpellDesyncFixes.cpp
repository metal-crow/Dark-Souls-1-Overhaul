#include "SpellDesyncFixes.h"
#include "DarkSoulsOverhaulMod.h"
#include "SP/memory/injection/asm/x64.h"

bool SpellDesync::enabled = true;

typedef struct CustomSpellPacketData_Struct {
    const uint32_t magic = 0x7fc00001; //magic number used to signify this is a spell packet
    uint32_t owner;
    uint32_t target;
    uint32_t bullet_number;
    const uint64_t padding[3] = { 0,0,0 };
    const uint8_t include_last_2_vals = 0; //must be at +40
} CustomSpellPacketData;

extern "C" {
    uint64_t homing_spell_trigger_injection_return;
    void homing_spell_trigger_injection();
    void homing_spell_trigger_injection_helper_function(uint32_t, uint8_t);

    uint64_t type1_p2pPacket_parse_injection_return;
    void type1_p2pPacket_parse_injection();
    void type1_p2pPacket_parse_injection_helper_function(CustomSpellPacketData*);

    uint64_t homing_spell_checkIfTriggered_injection_return;
    void homing_spell_checkIfTriggered_injection();
    void homing_spell_checkIfTriggered_injection_helper_function(uint8_t*, uint32_t*);
}

uint32_t received_SpellData_count = 0;

typedef struct BulletNetworkInfo_Struct {
    uint32_t owner_id;
    uint32_t target_id;
    uint8_t number;
} BulletNetworkInfo;

BulletNetworkInfo BulletNetworkInfo_Array[10];

void SpellDesync::start() {
    global::cmd_out << Mod::output_prefix << "Fixing Spell Desync...\n";

    //inject into the spell casting function to trigger a packet sending, which includes the custom needed data
    uint8_t *write_address = (uint8_t*)(SpellDesync::homing_spell_trigger_injection_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &homing_spell_trigger_injection_return, 1, &homing_spell_trigger_injection);

    //inject into the type1 packet parsing function and handle if we detect a magic specific packet
    write_address = (uint8_t*)(SpellDesync::type1_p2pPacket_parsing_injection_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &type1_p2pPacket_parse_injection_return, 4, &type1_p2pPacket_parse_injection);

    //inject into the function that determines if the homing bullet should be fired, and sets the target id to fire it
    write_address = (uint8_t*)(SpellDesync::homing_spell_checkIfTriggered_injection_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &homing_spell_checkIfTriggered_injection_return, 2, &homing_spell_checkIfTriggered_injection);
}


typedef bool sendType1NetMessage_Typedef(CustomSpellPacketData* data);
sendType1NetMessage_Typedef* sendType1NetMessage = (sendType1NetMessage_Typedef*)0x1405031f0;

// Send out the custom packet which says a homing bullet has been fired.
// By entering this function we know the owner of the bullet is the PC and the target is another player entity
void homing_spell_trigger_injection_helper_function(uint32_t target, uint8_t bulletNum)
{
    if (!SpellDesync::enabled)
    {
        return;
    }

    CustomSpellPacketData homingPkt;
    char error[100];

    auto owner_o = Game::get_pc_playernum();
    auto target_o = Game::convert_handle_to_playernum(target);
    if (!owner_o.has_value() || !target_o.has_value())
    {
        global::cmd_out << "WARNING. Unable to get bullet owner or target\n";
        return;
    }

    homingPkt.owner = owner_o.value();

    homingPkt.target = target_o.value();
    //handle error case
    if (homingPkt.target == -1) {
        snprintf(error, 100, "WARNING. Unable to convert Handle(%x) to a player number\n", target);
        global::cmd_out << error;
        return;
    }

    homingPkt.bullet_number = bulletNum;

    //Send the custom packet
    sendType1NetMessage(&homingPkt);
}


// Take in the custom spell type 1 packet, and extract it's data
void type1_p2pPacket_parse_injection_helper_function(CustomSpellPacketData* bullet_packet)
{
    if (!SpellDesync::enabled)
    {
        return;
    }

    char error[100];

    if (received_SpellData_count < 10)
    {
        //Convert from player numbers to handles
        uint32_t ownerHandle = Game::convert_playernum_to_handle(bullet_packet->owner).value_or(0);
        if (ownerHandle == 0) {
            snprintf(error, 100, "WARNING. Unable to convert Owner PlayerNo(%d) to a handle\n", bullet_packet->owner);
            global::cmd_out << error;
            return;
        }
        BulletNetworkInfo_Array[received_SpellData_count].owner_id = ownerHandle;

        uint32_t targetHandle = Game::convert_playernum_to_handle(bullet_packet->target).value_or(0);
        if (targetHandle == 0) {
            snprintf(error, 100, "WARNING. Unable to convert Target PlayerNo(%d) to a handle\n", bullet_packet->target);
            global::cmd_out << error;
            return;
        }
        BulletNetworkInfo_Array[received_SpellData_count].target_id = targetHandle;

        BulletNetworkInfo_Array[received_SpellData_count].number = (uint8_t)bullet_packet->bullet_number;
        received_SpellData_count++;
    }
}


// Check if the current bullet being checked to fire has been received as a network packet
void homing_spell_checkIfTriggered_injection_helper_function(uint8_t* bullet, uint32_t* bulletParamEntry)
{
    if (!SpellDesync::enabled)
    {
        return;
    }

    uint32_t bullet_owner = *(uint32_t*)(bullet + 0x9C);
    uint8_t bullet_num = *(uint8_t*)(bullet + 0x8);

    //always allow if the owner is the PC
    if (bullet_owner == Game::PC_Handle) {
        return;
    }

    //check if this bullet was received as a packet
    for (uint32_t i = 0; i < received_SpellData_count; i++) {
        //if the owner and bullet number match
        if (bullet_owner == BulletNetworkInfo_Array[i].owner_id && bullet_num == BulletNetworkInfo_Array[i].number)
        {
            //set the target
            *bulletParamEntry = BulletNetworkInfo_Array[i].target_id;

            //remove from the list and shift the rest to fill in spot
            for (uint32_t j = i; j < received_SpellData_count - 1; j++)
            {
                BulletNetworkInfo_Array[j] = BulletNetworkInfo_Array[j + 1];
            }
            received_SpellData_count--;
            return;
        }
    }

    //deny any homing bullet from another player that we don't get a packet for
    if (bullet_owner > Game::PC_Handle && bullet_owner < Game::PC_Handle + 10) {
        *bulletParamEntry = -1;
        return;
    }
}
