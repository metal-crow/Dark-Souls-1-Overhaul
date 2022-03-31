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

void copy_PlayerIns(PlayerIns* to, PlayerIns* from)
{
    copy_ChrIns(&to->chrins, &from->chrins);
    copy_PlayerGameData(to->playergamedata, from->playergamedata);
    copy_RingEquipCtrl(to->ringequipctrl, from->ringequipctrl);
    copy_WeaponEquipCtrl(to->weaponequipctrl, from->weaponequipctrl);
    copy_ProEquipCtrl(to->proequipctrl, from->proequipctrl);
    to->curSelectedMagicId = from->curSelectedMagicId;
    to->curUsedItem = from->curUsedItem;
    to->itemId = from->itemId;
    to->override_equipped_magicId = from->override_equipped_magicId;
    copy_ChrAsm(to->chrasm, from->chrasm);
}

void copy_ChrAsm(ChrAsm* to, ChrAsm* from)
{
    to->equipped_weapon_style = from->equipped_weapon_style;
    to->l_hand_equipped_index = from->equipped_weapon_style;
    to->r_hand_equipped_index = from->r_hand_equipped_index;
    memcpy(to->equip_items, from->equip_items, sizeof(to->equip_items));
}

void copy_ProEquipCtrl(ProEquipCtrl* to, ProEquipCtrl* from)
{
    copy_SpecialEffect(to->spEffectList, from->spEffectList);
    //there should always be 5 armors (4 equip and a hair)
    if (from->array_len != 5)
    {
        FATALERROR("Got %d for ProEquipCtrl->array_len", from->array_len);
    }
    to->array_len = 5;
    to->equipped_armors_ids[0] = from->equipped_armors_ids[0];
    to->equipped_armors_ids[1] = from->equipped_armors_ids[1];
    to->equipped_armors_ids[2] = from->equipped_armors_ids[2];
    to->equipped_armors_ids[3] = from->equipped_armors_ids[3];
    to->equipped_armors_ids[4] = from->equipped_armors_ids[4];
}

void copy_WeaponEquipCtrl(WeaponEquipCtrl* to, WeaponEquipCtrl* from)
{
    copy_SpecialEffect(to->spEffectList, from->spEffectList);
    //there should always be 2 weapons
    if (from->array_len != 2)
    {
        FATALERROR("Got %d for WeaponEquipCtrl->array_len", from->array_len);
    }
    to->array_len = 2;
    to->equipped_weapons_ids[0] = from->equipped_weapons_ids[0];
    to->equipped_weapons_ids[1] = from->equipped_weapons_ids[1];
}

void copy_RingEquipCtrl(RingEquipCtrl* to, RingEquipCtrl* from)
{
    copy_SpecialEffect(to->spEffectList, from->spEffectList);
    //there should always be 2 rings
    if (from->array_len != 2)
    {
        FATALERROR("Got %d for RingEquipCtrl->array_len", from->array_len);
    }
    to->array_len = 2;
    to->equipped_rings_ids[0] = from->equipped_rings_ids[0];
    to->equipped_rings_ids[1] = from->equipped_rings_ids[1];
}

void copy_PlayerGameData(PlayerGameData* to, PlayerGameData* from)
{
    copy_PlayerGameData_AttributeInfo(&to->attribs, &from->attribs);
    copy_EquipGameData(&to->equipGameData, &from->equipGameData);
    copy_PlayerGameData_ChrProperties(&to->ChrProperties, &from->ChrProperties);
}

void copy_PlayerGameData_ChrProperties(PlayerGameData_ChrProperties* to, PlayerGameData_ChrProperties* from)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
}

void copy_EquipGameData(EquipGameData* to, EquipGameData* from)
{
    copy_EquipMagicData(to->equipMagicData, from->equipMagicData);
    copy_EquipItemData(&to->equippedItemsInQuickbar, &from->equippedItemsInQuickbar);
}

void copy_EquipItemData(EquipItemData* to, EquipItemData* from)
{
    memcpy(to->quickbar, from->quickbar, sizeof(to->quickbar));
}

void copy_EquipMagicData(EquipMagicData* to, EquipMagicData* from)
{

}
