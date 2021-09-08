#include "SpellDesyncFixes.h"
#include "DarkSoulsOverhaulMod.h"
#include "SP/memory/injection/asm/x64.h"
#include <vector>
#include <mutex>

typedef struct {
    const uint32_t magic = 0x7fc00001; //magic number used to signify this is a spell packet
    uint32_t owner;
    uint32_t target;
    uint32_t target_alt;
    uint32_t bullet_number;
    const uint32_t padding_a = 0;
    const uint64_t padding_b[2] = { 0,0 };
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


class BulletNetworkInfo
{
public:
    uint32_t owner_id;
    uint32_t target_id;
    uint8_t number;
};

std::mutex BulletNetworkInfo_Array_mtx; //ensure that we don't drop a bullet that arrives right when we're writing the array

std::vector<BulletNetworkInfo*> BulletNetworkInfo_Array;

void SpellDesync::start() {
    ConsoleWrite("Fixing Spell Desync...");

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

// Send out the custom packet which says a homing bullet has been fired (i.e the actual HSM orb has left the caster and is mid-air).
// By entering this function we know the owner of the bullet is the PC and the target is another entity
void homing_spell_trigger_injection_helper_function(uint32_t target, uint8_t bulletNum)
{
    if (Mod::get_mode() == ModMode::Compatability)
    {
        return;
    }

    CustomSpellPacketData homingPkt;

    auto owner_o = Game::get_pc_playernum();
    if (!owner_o.has_value())
    {
        ConsoleWrite("WARNING. Unable to get bullet owner");
        return;
    }
    homingPkt.owner = owner_o.value();

    //handle if the target is a PC or an NPC
    if (target > Game::PC_Handle && target < Game::PC_Handle + 10)
    {
        auto target_o = Game::convert_handle_to_playernum(target);
        if (target_o.has_value())
        {
            homingPkt.target = target_o.value();
            homingPkt.target_alt = -1;
        }
        else
        {
            ConsoleWrite("WARNING. Unable to get bullet target");
            return;
        }
    }
    else
    {
        homingPkt.target = -1;
        homingPkt.target_alt = target;
    }

    homingPkt.bullet_number = bulletNum;

    ConsoleWrite("Send bullet %x %x %x %d", homingPkt.owner, homingPkt.target, homingPkt.target_alt, homingPkt.bullet_number);
    //Send the custom packet
    sendType1NetMessage(&homingPkt);
}


// Take in the custom spell type 1 packet, and extract it's data
void type1_p2pPacket_parse_injection_helper_function(CustomSpellPacketData* bullet_packet)
{
    if (Mod::get_mode() == ModMode::Compatability)
    {
        return;
    }

    BulletNetworkInfo* bullet = new BulletNetworkInfo();

    //Convert from player numbers to handles
    uint32_t ownerHandle = Game::convert_playernum_to_handle(bullet_packet->owner).value_or(0);
    if (ownerHandle == 0) {
        ConsoleWrite("WARNING. Unable to convert Owner PlayerNo(%d) to a handle", bullet_packet->owner);
        delete bullet;
        return;
    }
    bullet->owner_id = ownerHandle;

    //either convert to handle or use the given handle
    if (bullet_packet->target != -1)
    {
        uint32_t targetHandle = Game::convert_playernum_to_handle(bullet_packet->target).value_or(0);
        if (targetHandle == 0)
        {
            ConsoleWrite("WARNING. Unable to convert Target PlayerNo(%d) to a handle", bullet_packet->target);
            delete bullet;
            return;
        }
        bullet->target_id = targetHandle;
    }
    else
    {
        bullet->target_id = bullet_packet->target_alt;
    }

    bullet->number = (uint8_t)bullet_packet->bullet_number;

    //verify we can find this bullet in our bullet list
    if (!Game::find_bullet(bullet->owner_id, bullet->number).has_value())
    {
        ConsoleWrite("WARNING: Got bullet %x %x %d but unable to find in list.", bullet->owner_id, bullet->target_id, bullet->number);
        delete bullet;
        return;
    }

    BulletNetworkInfo_Array_mtx.lock();
    BulletNetworkInfo_Array.push_back(bullet);
    BulletNetworkInfo_Array_mtx.unlock();
    ConsoleWrite("Got bullet %x %x %d", bullet->owner_id, bullet->target_id, bullet->number);
}


// Check if the current bullet being checked to fire has been received as a network packet
void homing_spell_checkIfTriggered_injection_helper_function(uint8_t* firing_bullet, uint32_t* bulletParamEntry)
{
    if (Mod::get_mode() == ModMode::Compatability)
    {
        return;
    }

    uint32_t bullet_owner = *(uint32_t*)(firing_bullet + 0x9C);
    uint8_t bullet_num = *(uint8_t*)(firing_bullet + 0x8);

    //always allow if the owner is the PC
    if (bullet_owner == Game::PC_Handle) {
        return;
    }

    //check if this bullet was received as a packet
    BulletNetworkInfo_Array_mtx.lock();
    std::vector<BulletNetworkInfo*>::iterator iter = BulletNetworkInfo_Array.begin();
    while (iter != BulletNetworkInfo_Array.end())
    {
        BulletNetworkInfo* bullet = *iter;
        //if the owner and bullet number match
        if (bullet_owner == bullet->owner_id && bullet_num == bullet->number)
        {
            //set the target
            *bulletParamEntry = bullet->target_id;

            ConsoleWrite("Firing bullet %x %x %d", bullet_owner, *bulletParamEntry, bullet_num);

            //remove from the list
            delete bullet;
            iter = BulletNetworkInfo_Array.erase(iter);
            BulletNetworkInfo_Array_mtx.unlock();
            return;
        }
        else
        {
            iter++;
        }
    }
    BulletNetworkInfo_Array_mtx.unlock();

    //deny any homing bullet from another player that we don't get a packet for, to prevent the client from firing it off
    if (bullet_owner > Game::PC_Handle && bullet_owner < Game::PC_Handle + 10) {
        *bulletParamEntry = -1;
        return;
    }
}
