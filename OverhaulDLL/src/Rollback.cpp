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

void Rollback::init_saved_PlayerIns()
{
    //TODO
}

void Rollback::save()
{
    auto player_o = Game::get_PlayerIns();
    if (!player_o.has_value())
    {
        return;
    }
    uint64_t player = (uint64_t)player_o.value();

    //we pre-allocate a static playerins on boot, so we can assume all pointers are set up
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
    memcpy(to->equippedMagicList, from->equippedMagicList, sizeof(to->equippedMagicList));
    to->curSelectedMagicSlot = from->curSelectedMagicSlot;
}

void copy_PlayerGameData_AttributeInfo(PlayerGameData_AttributeInfo* to, PlayerGameData_AttributeInfo* from)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
}
void copy_ChrIns(ChrIns* to, ChrIns* from)
{
    copy_playerCtrl(to->playerCtrl, from->playerCtrl);
    copy_PadManipulator(to->padManipulator, from->padManipulator);
    to->CharaInitParamID = from->CharaInitParamID;
    to->lowerThrowAnim = from->lowerThrowAnim;
    to->upperThrowAnim = from->upperThrowAnim;
    to->curSelectedMagicId = from->curSelectedMagicId;
    to->curUsedItem = from->curUsedItem;
    copy_SpecialEffect(to->specialEffects, from->specialEffects);
    copy_QwcSpEffectEquipCtrl(to->qwcSpEffectEquipCtrl, from->qwcSpEffectEquipCtrl);
    to->curHp = from->curHp;
    to->maxHp = from->maxHp;
    to->curSp = from->curSp;
    to->maxSp = from->maxSp;
    to->damage_taken_scalar = from->damage_taken_scalar;
    to->PoisonResist = from->PoisonResist;
    to->ToxicResist = from->ToxicResist;
    to->BleedResist = from->BleedResist;
    to->CurseResist = from->CurseResist;
    to->resistPoisonTotal = from->resistPoisonTotal;
    to->resistPlagueTotal = from->resistPlagueTotal;
    to->resistBleedingTotal = from->resistBleedingTotal;
    to->resistCurseTotal = from->resistCurseTotal;
    copy_EntityThrowAnimationStatus(to->throw_animation_info, from->throw_animation_info);
}

void copy_EntityThrowAnimationStatus(EntityThrowAnimationStatus* to, EntityThrowAnimationStatus* from)
{
    to->throw_paramdef = from->throw_paramdef;
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
}

void copy_QwcSpEffectEquipCtrl(QwcSpEffectEquipCtrl* to, QwcSpEffectEquipCtrl* from)
{
    //we allow up to a max of 64 speffects
    if (from->arry_len > 64)
    {
        FATALERROR("Got %d number of speffects for QwcSpEffectEquipCtrl->arry. Only support a max of 64.");
    }
    to->arry_len = from->arry_len;
    for (int i = 0; i < from->arry_len; i++)
    {
        to->arry[i] = from->arry[i];
    }
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
}

void copy_SpecialEffect(SpecialEffect* to, SpecialEffect* from)
{
    copy_SpecialEffect_Info(to->specialEffect_Info, from->specialEffect_Info);
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
    memcpy(to->data_2, from->data_2, sizeof(to->data_2));
}

void copy_SpecialEffect_Info(SpecialEffect_Info* to, SpecialEffect_Info* from)
{
    SpecialEffect_Info* to_prev = NULL;
    do
    {
        memcpy(to->data_0, from->data_0, sizeof(to->data_0));
        to->paramRowBytes = from->paramRowBytes;
        to->prev = to_prev;

        if (from->next == NULL)
        {
            break;
        }
        else
        {
            from = from->next;
        }
        if (to->next == NULL)
        {
            FATALERROR("Unable to recursivly copy SpecialEffect_Info. to->next is null.");
        }
        else
        {
            to_prev = to;
            to = to->next;
        }
    } while (true);
}

void copy_PadManipulator(PadManipulator* to, PadManipulator* from)
{
    copy_ChrManipulator(&to->chrManipulator, &from->chrManipulator);
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
}

void copy_NetworkManipulator(NetworkManipulator* to, NetworkManipulator* from)
{
    //TODO
}

void copy_ChrManipulator(ChrManipulator* to, ChrManipulator* from)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
}

void copy_PlayerCtrl(PlayerCtrl* to, PlayerCtrl* from)
{
    copy_ChrCtrl(&to->chrCtrl, &from->chrCtrl);
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    copy_TurnAnim(to->turnAnim, from->turnAnim);
    copy_ArrowTurnAnim(to->arrowTurnAnim, from->arrowTurnAnim);
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
    memcpy(to->data_2, from->data_2, sizeof(to->data_2));
}

void copy_ArrowTurnAnim(ArrowTurnAnim* to, ArrowTurnAnim* from)
{
    copy_TurnAnim(&to->turnAnim, &from->turnAnim);
    copy_SpinJoint(to->joint_spine_2, from->joint_spine_2);
    copy_SpinJoint(to->joint_spine1_2, from->joint_spine1_2);
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
}

void copy_SpinJoint(SpinJoint* to, SpinJoint* from)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
    memcpy(to->data_2, from->data_2, sizeof(to->data_2));
}

void copy_TurnAnim(TurnAnim* to, TurnAnim* from)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
    copy_SpinJoint(to->joint_UpperRoot, from->joint_UpperRoot);
    copy_SpinJoint(to->joint_LowerRoot, from->joint_LowerRoot);
    copy_SpinJoint(to->joint_spine1_1, from->joint_spine1_1);
    copy_SpinJoint(to->joint_spine_1, from->joint_spine_1);
    copy_SpinJoint(to->joint_master, from->joint_master);
    memcpy(to->data_2, from->data_2, sizeof(to->data_2));
    memcpy(to->data_3, from->data_3, sizeof(to->data_3));
}

void copy_ChrCtrl(ChrCtrl* to, ChrCtrl* from)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    copy_ChrCtrl_AnimationQueue(to->animationQueue, from->animationQueue);
    copy_AnimationMediator(to->animationMediator, from->animationMediator);
    copy_HavokChara(to->havokChara, from->havokChara);
    copy_ActionCtrl(to->actionctrl, from->actionctrl);
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
    memcpy(to->data_2, from->data_2, sizeof(to->data_2));
    copy_WalkAnim_Twist(to->walkAnim_Twist, from->walkAnim_Twist);
    memcpy(to->data_3, from->data_3, sizeof(to->data_3));
    memcpy(to->data_4, from->data_4, sizeof(to->data_4));
}

void copy_WalkAnim_Twist(WalkAnim_Twist* to, WalkAnim_Twist* from)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
    memcpy(to->data_2, from->data_2, sizeof(to->data_2));
    copy_SpinJoint(to->Upper_Root_Joint, from->Upper_Root_Joint);
    copy_SpinJoint(to->master_joint, from->master_joint);
    copy_SpinJoint(to->neck_joint, from->neck_joint);
    memcpy(to->data_3, from->data_3, sizeof(to->data_3));
    copy_WalkAnim_Twist_Field0x228Elem(&to->walkAnim_Twist_Field0x228Elem, &from->walkAnim_Twist_Field0x228Elem);
    memcpy(to->data_4, from->data_4, sizeof(to->data_4));
}

void copy_WalkAnim_Twist_Field0x228Elem(WalkAnim_Twist_Field0x228Elem* to, WalkAnim_Twist_Field0x228Elem* from)
{
    //TODO
}

void copy_ActionCtrl(ActionCtrl* to, ActionCtrl* from)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    copy_ActionCtrl_0x30Substruct(&to->passive_state, &from->passive_state);
    copy_ActionCtrl_0x30Substruct(&to->active_state, &from->active_state);
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
    memcpy(to->data_2, from->data_2, sizeof(to->data_2));
}

void copy_ActionCtrl_0x30Substruct(ActionCtrl_0x30Substruct* to, ActionCtrl_0x30Substruct* from)
{
    copy_EzState_detail_EzStateMachineImpl(to->EzStateMachineImpl, from->EzStateMachineImpl);
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
}

void copy_EzState_detail_EzStateMachineImpl(EzState_detail_EzStateMachineImpl* to, EzState_detail_EzStateMachineImpl* from)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
    memcpy(to->data_2, from->data_2, sizeof(to->data_2));
}

void copy_HavokChara(HavokChara* to, HavokChara* from)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    copy_hkpCharacterProxy(to->char_proxy, from->char_proxy);
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
}

void copy_hkpCharacterProxy(hkpCharacterProxy* to, hkpCharacterProxy* from)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
}

void copy_AnimationMediator(AnimationMediator* to, AnimationMediator* from)
{
    for (int i = 0; i < 31; i++)
    {
        copy_AnimationMediatorStateEntry(&to->states_list[i], &from->states_list[i]);
    }
    copy_AnimationQueue(to->animationQueue, from->animationQueue);
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
}

void copy_AnimationMediatorStateEntry(AnimationMediatorStateEntry* to, AnimationMediatorStateEntry* from)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    if (from->linked_animation1 != NULL)
    {
        if (to->linked_animation1 == NULL)
        {
            FATALERROR("Unable to recursivly copy AnimationMediatorStateEntry into to->linked_animation. Pointer is null");
        }
        else
        {
            copy_AnimationMediatorStateEntry(to->linked_animation1, from->linked_animation1);
        }
    }
    if (from->linked_animation2 != NULL)
    {
        if (to->linked_animation2 == NULL)
        {
            FATALERROR("Unable to recursivly copy AnimationMediatorStateEntry into to->linked_animation. Pointer is null");
        }
        else
        {
            copy_AnimationMediatorStateEntry(to->linked_animation2, from->linked_animation2);
        }
    }
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
}

void copy_ChrCtrl_AnimationQueue(ChrCtrl_AnimationQueue* to, ChrCtrl_AnimationQueue* from)
{
    to->array_length = from->array_length;
    to->data_0 = from->data_0;
    for (int i = 0; i < from->array_length; i++)
    {
        copy_ChrCtrl_AnimationQueue_field0x8(&to->arry[i], &from->arry[i]);
    }
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
    memcpy(to->data_2, from->data_2, sizeof(to->data_2));
    to->data_3 = from->data_3;
    memcpy(to->data_4, from->data_4, sizeof(to->data_4));
    memcpy(to->data_5, from->data_5, sizeof(to->data_5));
}

void copy_ChrCtrl_AnimationQueue_field0x8(ChrCtrl_AnimationQueue_field0x8* to, ChrCtrl_AnimationQueue_field0x8* from)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
}

void copy_AnimationQueue(AnimationQueue* to, AnimationQueue* from)
{
    for (int i = 0; i < 6; i++)
    {
        copy_AnimationQueue_Entry(to->AnimationQueue_Entries[i], from->AnimationQueue_Entries[i]);
    }
}

void copy_AnimationQueue_Entry(AnimationQueue_Entry* to, AnimationQueue_Entry* from)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
    memcpy(to->data_2, from->data_2, sizeof(to->data_2));
    memcpy(to->data_3, from->data_3, sizeof(to->data_3));
}
