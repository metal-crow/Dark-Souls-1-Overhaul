#include "Rollback.h"
#include "SP/memory.h"
#include "SP/memory/injection/asm/x64.h"
#include "DarkSoulsOverhaulMod.h"
#include "MainLoop.h"
#include "InputUtil.h"

void* malloc_(size_t size)
{
    void* out = malloc(size);
    if (out == NULL)
    {
        FATALERROR("Malloc failure. Unable to allocate %d bytes.", size);
    }
    memset(out, 0, size);
    return out;
}

extern "C" {
    uint64_t sendType1NetMessage_injection_return;
    void sendType1NetMessage_injection();
    uint64_t recvType1NetMessage_injection_return;
    void recvType1NetMessage_injection();
}

PlayerIns* Rollback::saved_playerins = NULL;

bool Rollback::bsave = false;
bool Rollback::bload = false;
bool rollback_test(void* unused)
{
    if (Rollback::bsave)
    {
        Rollback::save();
        Rollback::bsave = false;
    }

    if (Rollback::bload)
    {
        Rollback::load();
        Rollback::bload = false;
    }

    return true;
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

    //init out copy of the playerins struct, for saving/restoring with rollback
    Rollback::saved_playerins = init_PlayerIns();

    //TEST save/restore with a hotkey
    MainLoop::setup_mainloop_callback(rollback_test, NULL, "rollback_test");
}

void Rollback::save()
{
    auto player_o = Game::get_PlayerIns();
    if (!player_o.has_value())
    {
        return;
    }
    PlayerIns* player = (PlayerIns*)player_o.value();

    //we pre-allocate a static playerins on boot, so we can assume all pointers are set up
    copy_PlayerIns(Rollback::saved_playerins, player, false);
}

void Rollback::load()
{
    auto player_o = Game::get_PlayerIns();
    if (!player_o.has_value())
    {
        return;
    }
    PlayerIns* player = (PlayerIns*)player_o.value();

    copy_PlayerIns(player, Rollback::saved_playerins, true);
}

void copy_PlayerIns(PlayerIns* to, PlayerIns* from, bool to_game)
{
    copy_ChrIns(&to->chrins, &from->chrins, to_game);
    copy_PlayerGameData(to->playergamedata, from->playergamedata);
    copy_RingEquipCtrl(to->ringequipctrl, from->ringequipctrl, to_game);
    copy_WeaponEquipCtrl(to->weaponequipctrl, from->weaponequipctrl, to_game);
    copy_ProEquipCtrl(to->proequipctrl, from->proequipctrl, to_game);
    to->curSelectedMagicId = from->curSelectedMagicId;
    to->curUsedItem = from->curUsedItem;
    to->itemId = from->itemId;
    to->override_equipped_magicId = from->override_equipped_magicId;
    copy_ChrAsm(to->chrasm, from->chrasm);
}

PlayerIns* init_PlayerIns()
{
    PlayerIns* local_PlayerIns = (PlayerIns*)malloc_(sizeof(PlayerIns));

    ChrIns* pChrIns = init_ChrIns();
    local_PlayerIns->chrins = *pChrIns;
    free(pChrIns);
    local_PlayerIns->playergamedata = init_PlayerGameData();
    local_PlayerIns->ringequipctrl = init_RingEquipCtrl();
    local_PlayerIns->weaponequipctrl = init_WeaponEquipCtrl();
    local_PlayerIns->proequipctrl = init_ProEquipCtrl();
    local_PlayerIns->chrasm = init_ChrAsm();

    return local_PlayerIns;
}

void copy_ChrAsm(ChrAsm* to, ChrAsm* from)
{
    to->equipped_weapon_style = from->equipped_weapon_style;
    to->l_hand_equipped_index = from->equipped_weapon_style;
    to->r_hand_equipped_index = from->r_hand_equipped_index;
    memcpy(to->equip_items, from->equip_items, sizeof(to->equip_items));
}

ChrAsm* init_ChrAsm()
{
    ChrAsm* local_ChrAsm = (ChrAsm*)malloc_(sizeof(ChrAsm));
    return local_ChrAsm;
}

void copy_ProEquipCtrl(ProEquipCtrl* to, ProEquipCtrl* from, bool to_game)
{
    copy_SpecialEffect(to->spEffectList, from->spEffectList, to_game);
    //there should always be 5 armors (4 equip and a hair)
    if (from->array_len != 5)
    {
        FATALERROR("Got %d for ProEquipCtrl->array_len, should be 5", from->array_len);
    }
    to->array_len = 5;
    to->equipped_armors_ids[0] = from->equipped_armors_ids[0];
    to->equipped_armors_ids[1] = from->equipped_armors_ids[1];
    to->equipped_armors_ids[2] = from->equipped_armors_ids[2];
    to->equipped_armors_ids[3] = from->equipped_armors_ids[3];
    to->equipped_armors_ids[4] = from->equipped_armors_ids[4];
}

ProEquipCtrl* init_ProEquipCtrl()
{
    ProEquipCtrl* local_ProEquipCtrl = (ProEquipCtrl*)malloc_(sizeof(ProEquipCtrl));
    uint32_t* local_ProEquipCtrl_equipped_armors_ids = (uint32_t*)malloc_(sizeof(uint32_t)*5);

    local_ProEquipCtrl->spEffectList = init_SpecialEffect();
    local_ProEquipCtrl->equipped_armors_ids = local_ProEquipCtrl_equipped_armors_ids;

    return local_ProEquipCtrl;
}

void copy_WeaponEquipCtrl(WeaponEquipCtrl* to, WeaponEquipCtrl* from, bool to_game)
{
    copy_SpecialEffect(to->spEffectList, from->spEffectList, to_game);
    //there should always be 2 weapons
    if (from->array_len != 2)
    {
        FATALERROR("Got %d for WeaponEquipCtrl->array_len", from->array_len);
    }
    to->array_len = 2;
    to->equipped_weapons_ids[0] = from->equipped_weapons_ids[0];
    to->equipped_weapons_ids[1] = from->equipped_weapons_ids[1];
}

WeaponEquipCtrl* init_WeaponEquipCtrl()
{
    WeaponEquipCtrl* local_WeaponEquipCtrl = (WeaponEquipCtrl*)malloc_(sizeof(WeaponEquipCtrl));
    uint32_t* local_WeaponEquipCtrl_equipped_weapons_ids = (uint32_t*)malloc_(sizeof(uint32_t)*2);

    local_WeaponEquipCtrl->spEffectList = init_SpecialEffect();
    local_WeaponEquipCtrl->equipped_weapons_ids = local_WeaponEquipCtrl_equipped_weapons_ids;

    return local_WeaponEquipCtrl;
}

void copy_RingEquipCtrl(RingEquipCtrl* to, RingEquipCtrl* from, bool to_game)
{
    copy_SpecialEffect(to->spEffectList, from->spEffectList, to_game);
    //there should always be 2 rings
    if (from->array_len != 2)
    {
        FATALERROR("Got %d for RingEquipCtrl->array_len", from->array_len);
    }
    to->array_len = 2;
    to->equipped_rings_ids[0] = from->equipped_rings_ids[0];
    to->equipped_rings_ids[1] = from->equipped_rings_ids[1];
}

RingEquipCtrl* init_RingEquipCtrl()
{
    RingEquipCtrl* local_RingEquipCtrl = (RingEquipCtrl*)malloc_(sizeof(RingEquipCtrl));
    uint32_t* local_RingEquipCtrl_equipped_rings_ids = (uint32_t*)malloc_(sizeof(uint32_t)*2);

    local_RingEquipCtrl->spEffectList = init_SpecialEffect();
    local_RingEquipCtrl->equipped_rings_ids = local_RingEquipCtrl_equipped_rings_ids;

    return local_RingEquipCtrl;
}

void copy_PlayerGameData(PlayerGameData* to, PlayerGameData* from)
{
    copy_PlayerGameData_AttributeInfo(&to->attribs, &from->attribs);
    copy_EquipGameData(&to->equipGameData, &from->equipGameData);
    copy_PlayerGameData_ChrProperties(&to->ChrProperties, &from->ChrProperties);
}

PlayerGameData* init_PlayerGameData()
{
    PlayerGameData* local_PlayerGameData = (PlayerGameData*)malloc_(sizeof(PlayerGameData));

    EquipGameData* pEquipGameData = init_EquipGameData();
    local_PlayerGameData->equipGameData = *pEquipGameData;
    free(pEquipGameData);

    return local_PlayerGameData;
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

EquipGameData* init_EquipGameData()
{
    EquipGameData* local_EquipGameData = (EquipGameData*)malloc_(sizeof(EquipGameData));

    local_EquipGameData->equipMagicData = init_EquipMagicData();

    return local_EquipGameData;
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

EquipMagicData* init_EquipMagicData()
{
    EquipMagicData* local_EquipMagicData = (EquipMagicData*)malloc_(sizeof(EquipMagicData));
    return local_EquipMagicData;
}

void copy_PlayerGameData_AttributeInfo(PlayerGameData_AttributeInfo* to, PlayerGameData_AttributeInfo* from)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
}

void copy_ChrIns(ChrIns* to, ChrIns* from, bool to_game)
{
    copy_PlayerCtrl(to->playerCtrl, from->playerCtrl, to_game);
    copy_PadManipulator(to->padManipulator, from->padManipulator);
    to->CharaInitParamID = from->CharaInitParamID;
    to->lowerThrowAnim = from->lowerThrowAnim;
    to->upperThrowAnim = from->upperThrowAnim;
    to->curSelectedMagicId = from->curSelectedMagicId;
    to->curUsedItem = from->curUsedItem;
    copy_SpecialEffect(to->specialEffects, from->specialEffects, to_game);
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

ChrIns* init_ChrIns()
{
    ChrIns* local_ChrIns = (ChrIns*)malloc_(sizeof(ChrIns));

    local_ChrIns->playerCtrl = init_PlayerCtrl();
    local_ChrIns->padManipulator = init_PadManipulator();
    local_ChrIns->specialEffects = init_SpecialEffect();
    local_ChrIns->qwcSpEffectEquipCtrl = init_QwcSpEffectEquipCtrl();
    local_ChrIns->throw_animation_info = init_EntityThrowAnimationStatus();

    return local_ChrIns;
}

void copy_EntityThrowAnimationStatus(EntityThrowAnimationStatus* to, EntityThrowAnimationStatus* from)
{
    to->throw_paramdef = from->throw_paramdef;
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
}

EntityThrowAnimationStatus* init_EntityThrowAnimationStatus()
{
    EntityThrowAnimationStatus* local_EntityThrowAnimationStatus = (EntityThrowAnimationStatus*)malloc_(sizeof(EntityThrowAnimationStatus));
    return local_EntityThrowAnimationStatus;
}

void copy_QwcSpEffectEquipCtrl(QwcSpEffectEquipCtrl* to, QwcSpEffectEquipCtrl* from)
{
    //we allow up to a max of 64 speffects
    if (from->arry_len > 64)
    {
        FATALERROR("Got %d number of speffects for QwcSpEffectEquipCtrl->arry. Only support a max of 64.");
    }
    to->arry_len = from->arry_len;
    for (size_t i = 0; i < from->arry_len; i++)
    {
        to->arry[i] = from->arry[i];
    }
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
}

QwcSpEffectEquipCtrl* init_QwcSpEffectEquipCtrl()
{
    QwcSpEffectEquipCtrl* local_QwcSpEffectEquipCtrl = (QwcSpEffectEquipCtrl*)malloc_(sizeof(QwcSpEffectEquipCtrl));
    uint32_t* local_QwcSpEffectEquipCtrl_arry = (uint32_t*)malloc_(sizeof(uint32_t)*64);

    local_QwcSpEffectEquipCtrl->arry = local_QwcSpEffectEquipCtrl_arry;

    return local_QwcSpEffectEquipCtrl;
}

void copy_SpecialEffect(SpecialEffect* to, SpecialEffect* from, bool to_game)
{
    copy_SpecialEffect_Info(to->specialEffect_Info, from->specialEffect_Info, to_game);
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
    memcpy(to->data_2, from->data_2, sizeof(to->data_2));
}

SpecialEffect* init_SpecialEffect()
{
    SpecialEffect* local_SpecialEffect = (SpecialEffect*)malloc_(sizeof(SpecialEffect));

    local_SpecialEffect->specialEffect_Info = init_SpecialEffect_Info();

    return local_SpecialEffect;
}

static const size_t max_preallocated_SpecialEffect_Info = 64;

void copy_SpecialEffect_Info(SpecialEffect_Info* to, SpecialEffect_Info* from_, bool to_game)
{
    SpecialEffect_Info* from = from_;

    if (!to_game)
    {
        size_t to_index = 0;
        while (from)
        {
            if (to_index >= max_preallocated_SpecialEffect_Info)
            {
                ConsoleWrite("Unable to recursivly copy SpecialEffect_Info from the game. Out of space.");
                break;
            }
            memcpy(to->data_0, from->data_0, sizeof(to->data_0));
            to->paramRowBytes = from->paramRowBytes;

            if (from->next != NULL)
            {
                to->next = to + sizeof(SpecialEffect_Info);
            }
            else
            {
                to->next = NULL;
            }
            //don't need to handle prev since it should just be as expected

            from = from->next;
            to += sizeof(SpecialEffect_Info);
            to_index += 1;
        }
    }
    else
    {
        while (from)
        {
            memcpy(to->data_0, from->data_0, sizeof(to->data_0));
            to->paramRowBytes = from->paramRowBytes;

            //handle if the game's list isn't long enough, and we need to alloc more slots
            if (from->next != NULL && to->next == NULL)
            {
                ConsoleWrite("Out of space on game side for SpecialEffect_Info");
                break;
                //TODO handle game mallocing
                //to->next = (SpecialEffect_Info*)Game::game_malloc(sizeof(SpecialEffect_Info), 8, (void*)*(uint64_t*)(0x141C04F30)); //internal_heap_3
                //to->next->next = NULL;
                //to->next->prev = to;
            }

            //handle if the game's list is too long, and we need to free it's extra slots
            if (from->next == NULL && to->next != NULL)
            {
                SpecialEffect_Info* entry_to_free = to->next;
                to->next = NULL;
                while (entry_to_free)
                {
                    SpecialEffect_Info* next = entry_to_free->next;
                    //TODO handle game freeing
                    //Game::game_free(entry_to_free, sizeof(SpecialEffect_Info));
                    entry_to_free = next;
                }
                break;
            }

            from = from->next;
            to = to->next;
        }
    }
}

SpecialEffect_Info* init_SpecialEffect_Info()
{
    //this is a linked list, so pre-allocate a max of 64 in the list for speffects
    SpecialEffect_Info* local_SpecialEffect_Info = (SpecialEffect_Info*)malloc_(sizeof(SpecialEffect_Info)*max_preallocated_SpecialEffect_Info);

    return local_SpecialEffect_Info;
}

void copy_PadManipulator(PadManipulator* to, PadManipulator* from)
{
    copy_ChrManipulator(&to->chrManipulator, &from->chrManipulator);
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
}

PadManipulator* init_PadManipulator()
{
    PadManipulator* local_PadManipulator = (PadManipulator*)malloc_(sizeof(PadManipulator));
    return local_PadManipulator;
}

void copy_NetworkManipulator(NetworkManipulator* to, NetworkManipulator* from)
{
    //TODO
}

void copy_ChrManipulator(ChrManipulator* to, ChrManipulator* from)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
}

void copy_PlayerCtrl(PlayerCtrl* to, PlayerCtrl* from, bool to_game)
{
    copy_ChrCtrl(&to->chrCtrl, &from->chrCtrl, to_game);
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    copy_TurnAnim(to->turnAnim, from->turnAnim);
    copy_ArrowTurnAnim(to->arrowTurnAnim, from->arrowTurnAnim);
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
    memcpy(to->data_2, from->data_2, sizeof(to->data_2));
}

PlayerCtrl* init_PlayerCtrl()
{
    PlayerCtrl* local_PlayerCtrl = (PlayerCtrl*)malloc_(sizeof(PlayerCtrl));

    ChrCtrl* pChrCtrl = init_ChrCtrl();
    local_PlayerCtrl->chrCtrl = *pChrCtrl;
    free(pChrCtrl);
    local_PlayerCtrl->turnAnim = init_TurnAnim();
    local_PlayerCtrl->arrowTurnAnim = init_ArrowTurnAnim();

    return local_PlayerCtrl;
}

void copy_ArrowTurnAnim(ArrowTurnAnim* to, ArrowTurnAnim* from)
{
    copy_TurnAnim(&to->turnAnim, &from->turnAnim);
    copy_SpinJoint(to->joint_spine_2, from->joint_spine_2);
    copy_SpinJoint(to->joint_spine1_2, from->joint_spine1_2);
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
}

ArrowTurnAnim* init_ArrowTurnAnim()
{
    ArrowTurnAnim* local_ArrowTurnAnim = (ArrowTurnAnim*)malloc_(sizeof(ArrowTurnAnim));

    TurnAnim* pTurnAnim = init_TurnAnim();
    local_ArrowTurnAnim->turnAnim = *pTurnAnim;
    free(pTurnAnim);
    local_ArrowTurnAnim->joint_spine_2 = init_SpinJoint();
    local_ArrowTurnAnim->joint_spine1_2 = init_SpinJoint();

    return local_ArrowTurnAnim;
}

void copy_SpinJoint(SpinJoint* to, SpinJoint* from)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
    memcpy(to->data_2, from->data_2, sizeof(to->data_2));
}

SpinJoint* init_SpinJoint()
{
    SpinJoint* local_SpinJoint = (SpinJoint*)malloc_(sizeof(SpinJoint));
    return local_SpinJoint;
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

TurnAnim* init_TurnAnim()
{
    TurnAnim* local_TurnAnim = (TurnAnim*)malloc_(sizeof(TurnAnim));

    local_TurnAnim->joint_UpperRoot = init_SpinJoint();
    local_TurnAnim->joint_LowerRoot = init_SpinJoint();
    local_TurnAnim->joint_spine1_1 = init_SpinJoint();
    local_TurnAnim->joint_spine_1 = init_SpinJoint();
    local_TurnAnim->joint_master = init_SpinJoint();

    return local_TurnAnim;
}

void copy_ChrCtrl(ChrCtrl* to, ChrCtrl* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    copy_ChrCtrl_AnimationQueue(to->animationQueue, from->animationQueue, to_game);
    copy_AnimationMediator(to->animationMediator, from->animationMediator);
    copy_HavokChara(to->havokChara, from->havokChara);
    copy_ActionCtrl(to->actionctrl, from->actionctrl);
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
    memcpy(to->data_2, from->data_2, sizeof(to->data_2));
    copy_WalkAnim_Twist(to->walkAnim_Twist, from->walkAnim_Twist);
    memcpy(to->data_3, from->data_3, sizeof(to->data_3));
    memcpy(to->data_4, from->data_4, sizeof(to->data_4));
}

ChrCtrl* init_ChrCtrl()
{
    ChrCtrl* local_ChrCtrl = (ChrCtrl*)malloc_(sizeof(ChrCtrl));

    local_ChrCtrl->animationQueue = init_ChrCtrl_AnimationQueue();
    local_ChrCtrl->animationMediator = init_AnimationMediator();
    local_ChrCtrl->havokChara = init_HavokChara();
    local_ChrCtrl->actionctrl = init_ActionCtrl();
    local_ChrCtrl->walkAnim_Twist = init_WalkAnim_Twist();

    return local_ChrCtrl;
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

WalkAnim_Twist* init_WalkAnim_Twist()
{
    WalkAnim_Twist* local_WalkAnim_Twist = (WalkAnim_Twist*)malloc_(sizeof(WalkAnim_Twist));

    local_WalkAnim_Twist->Upper_Root_Joint = init_SpinJoint();
    local_WalkAnim_Twist->master_joint = init_SpinJoint();
    local_WalkAnim_Twist->neck_joint = init_SpinJoint();
    WalkAnim_Twist_Field0x228Elem* pWalkAnim_Twist_Field0x228Elem = init_WalkAnim_Twist_Field0x228Elem();
    local_WalkAnim_Twist->walkAnim_Twist_Field0x228Elem = *pWalkAnim_Twist_Field0x228Elem;
    free(pWalkAnim_Twist_Field0x228Elem);

    return local_WalkAnim_Twist;
}

void copy_WalkAnim_Twist_Field0x228Elem(WalkAnim_Twist_Field0x228Elem* to, WalkAnim_Twist_Field0x228Elem* from)
{
    //TODO
}

WalkAnim_Twist_Field0x228Elem* init_WalkAnim_Twist_Field0x228Elem()
{
    WalkAnim_Twist_Field0x228Elem* local_WalkAnim_Twist_Field0x228Elem = (WalkAnim_Twist_Field0x228Elem*)malloc_(sizeof(WalkAnim_Twist_Field0x228Elem));
    //TODO
    return local_WalkAnim_Twist_Field0x228Elem;
}

void copy_ActionCtrl(ActionCtrl* to, ActionCtrl* from)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    copy_ActionCtrl_0x30Substruct(&to->passive_state, &from->passive_state);
    copy_ActionCtrl_0x30Substruct(&to->active_state, &from->active_state);
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
    memcpy(to->data_2, from->data_2, sizeof(to->data_2));
}

ActionCtrl* init_ActionCtrl()
{
    ActionCtrl* local_ActionCtrl = (ActionCtrl*)malloc_(sizeof(ActionCtrl));

    local_ActionCtrl->passive_state.EzStateMachineImpl = init_EzState_detail_EzStateMachineImpl();
    local_ActionCtrl->active_state.EzStateMachineImpl = init_EzState_detail_EzStateMachineImpl();

    return local_ActionCtrl;
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

EzState_detail_EzStateMachineImpl* init_EzState_detail_EzStateMachineImpl()
{
    EzState_detail_EzStateMachineImpl* local_EzState_detail_EzStateMachineImpl = (EzState_detail_EzStateMachineImpl*)malloc_(sizeof(EzState_detail_EzStateMachineImpl));
    return local_EzState_detail_EzStateMachineImpl;
}

void copy_HavokChara(HavokChara* to, HavokChara* from)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    copy_hkpCharacterProxy(to->char_proxy, from->char_proxy);
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
}

HavokChara* init_HavokChara()
{
    HavokChara* local_HavokChara = (HavokChara*)malloc_(sizeof(HavokChara));

    local_HavokChara->char_proxy = init_hkpCharacterProxy();

    return local_HavokChara;
}

void copy_hkpCharacterProxy(hkpCharacterProxy* to, hkpCharacterProxy* from)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
}

hkpCharacterProxy* init_hkpCharacterProxy()
{
    hkpCharacterProxy* local_hkpCharacterProxy = (hkpCharacterProxy*)malloc_(sizeof(hkpCharacterProxy));
    return local_hkpCharacterProxy;
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

AnimationMediator* init_AnimationMediator()
{
    AnimationMediator* local_AnimationMediator = (AnimationMediator*)malloc_(sizeof(AnimationMediator));

    for (int i = 0; i < 31; i++)
    {
        AnimationMediatorStateEntry* pAnimationMediatorStateEntry = init_AnimationMediatorStateEntry();
        local_AnimationMediator->states_list[i] = *pAnimationMediatorStateEntry;
        free(pAnimationMediatorStateEntry);
    }
    local_AnimationMediator->animationQueue = init_AnimationQueue();

    return local_AnimationMediator;
}

void copy_AnimationMediatorStateEntry(AnimationMediatorStateEntry* to, AnimationMediatorStateEntry* from)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
}

AnimationMediatorStateEntry* init_AnimationMediatorStateEntry()
{
    AnimationMediatorStateEntry* local_AnimationMediatorStateEntry = (AnimationMediatorStateEntry*)malloc_(sizeof(AnimationMediatorStateEntry));
    return local_AnimationMediatorStateEntry;
}

void copy_ChrCtrl_AnimationQueue(ChrCtrl_AnimationQueue* to, ChrCtrl_AnimationQueue* from, bool to_game)
{
    //we allow up to a max of 32 ChrCtrl_AnimationQueueEntry entries in the array
    if (from->array_length > 32)
    {
        FATALERROR("Got %d number of ChrCtrl_AnimationQueueEntry entries for ChrCtrl_AnimationQueue->arry. Only support a max of 32.", from->array_length);
    }
    to->array_length = from->array_length;
    to->data_0 = from->data_0;
    for (size_t i = 0; i < from->array_length; i++)
    {
        copy_ChrCtrl_AnimationQueueEntry(&to->arry[i], &from->arry[i], to_game);
    }

    copy_ChrCtrl_AnimationQueue_field0x10(to->field0x10, from->field0x10);
    copy_hkaAnimatedSkeleton(to->HkaAnimatedSkeleton, from->HkaAnimatedSkeleton);
    copy_ChrCtrl_AnimationQueue_field0x20(to->field0x20, from->field0x20, to_game);
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
    memcpy(to->data_2, from->data_2, sizeof(to->data_2));
    to->data_3 = from->data_3;
    memcpy(to->data_4, from->data_4, sizeof(to->data_4));
    memcpy(to->data_5, from->data_5, sizeof(to->data_5));
}

ChrCtrl_AnimationQueue* init_ChrCtrl_AnimationQueue()
{
    ChrCtrl_AnimationQueue* local_ChrCtrl_AnimationQueue = (ChrCtrl_AnimationQueue*)malloc_(sizeof(ChrCtrl_AnimationQueue));
    ChrCtrl_AnimationQueueEntry* local_AnimationQueueEntry_Array = (ChrCtrl_AnimationQueueEntry*)malloc_(sizeof(ChrCtrl_AnimationQueueEntry) * 32);

    local_ChrCtrl_AnimationQueue->arry = local_AnimationQueueEntry_Array;
    for (size_t i = 0; i < 32; i++)
    {
        ChrCtrl_AnimationQueueEntry* local_AnimationQueueEntry = init_ChrCtrl_AnimationQueueEntry();
        local_ChrCtrl_AnimationQueue->arry[i] = *local_AnimationQueueEntry;
        free(local_AnimationQueueEntry);
    }
    local_ChrCtrl_AnimationQueue->field0x10 = init_ChrCtrl_AnimationQueue_field0x10();
    local_ChrCtrl_AnimationQueue->HkaAnimatedSkeleton = init_hkaAnimatedSkeleton();
    local_ChrCtrl_AnimationQueue->field0x20 = init_ChrCtrl_AnimationQueue_field0x20();

    return local_ChrCtrl_AnimationQueue;
}

void copy_ChrCtrl_AnimationQueue_field0x20(ChrCtrl_AnimationQueue_field0x20* to, ChrCtrl_AnimationQueue_field0x20* from, bool to_game)
{
    if (!to_game)
    {
        uint32_t field0x28_len = *(uint32_t*)(from->padding_0 + 0x30);
        if (field0x28_len != 61)
        {
            FATALERROR("ChrCtrl_AnimationQueue_field0x20 arrays are of size %d. Expected 61.", field0x28_len);
        }
    }
    memcpy(to->field0x8, from->field0x8, 0x30 * 64);
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    memcpy(to->field0x18, from->field0x18, 0x30 * 64);
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
    memcpy(to->field0x28, from->field0x28, 4 * 64);
    memcpy(to->data_2, from->data_2, sizeof(to->data_2));
    memcpy(to->data_3, from->data_3, sizeof(to->data_3));
}

ChrCtrl_AnimationQueue_field0x20* init_ChrCtrl_AnimationQueue_field0x20()
{
    ChrCtrl_AnimationQueue_field0x20* local_ChrCtrl_AnimationQueue_field0x20 = (ChrCtrl_AnimationQueue_field0x20*)malloc_(sizeof(ChrCtrl_AnimationQueue_field0x20));

    //the lengths seem to be constant 64 (61 + 3)
    local_ChrCtrl_AnimationQueue_field0x20->field0x8 = malloc_(0x30 * 64);
    local_ChrCtrl_AnimationQueue_field0x20->field0x18 = malloc_(0x30 * 64);
    local_ChrCtrl_AnimationQueue_field0x20->field0x28 = (uint32_t*)malloc_(4 * 64);

    return local_ChrCtrl_AnimationQueue_field0x20;
}

void copy_hkaAnimatedSkeleton(hkaAnimatedSkeleton* to, hkaAnimatedSkeleton* from)
{
    to->data_0 = from->data_0;
    if (from->animCtrl_list_len > 32)
    {
        FATALERROR("Got %d number of hkaAnimatedSkeleton entries for hkaAnimatedSkeleton->animCtrl_list. Only support a max of 32.", from->animCtrl_list_len);
    }
    to->animCtrl_list_len = from->animCtrl_list_len;
    for (uint32_t i = 0; i < to->animCtrl_list_len; i++)
    {
        copy_hkaDefaultAnimationControl(to->animCtrl_list[i], from->animCtrl_list[i]);
    }
    to->data_1 = from->data_1;
    to->data_2 = from->data_2;
}

hkaAnimatedSkeleton* init_hkaAnimatedSkeleton()
{
    hkaAnimatedSkeleton* local_hkaAnimatedSkeleton = (hkaAnimatedSkeleton*)malloc_(sizeof(hkaAnimatedSkeleton));
    hkaDefaultAnimationControl** local_hkaAnimatedSkeleton_animCtrl_list = (hkaDefaultAnimationControl**)malloc_(sizeof(hkaDefaultAnimationControl*) * 32);
    for (int i = 0; i < 32; i++)
    {
        local_hkaAnimatedSkeleton_animCtrl_list[i] = init_hkaDefaultAnimationControl();
    }
    local_hkaAnimatedSkeleton->animCtrl_list = local_hkaAnimatedSkeleton_animCtrl_list;

    return local_hkaAnimatedSkeleton;
}

void copy_hkaDefaultAnimationControl(hkaDefaultAnimationControl* to, hkaDefaultAnimationControl* from)
{
    copy_hkaAnimationControl(&to->HkaAnimationControl, &from->HkaAnimationControl);
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
}

hkaDefaultAnimationControl* init_hkaDefaultAnimationControl()
{
    hkaDefaultAnimationControl* local_hkaDefaultAnimationControl = (hkaDefaultAnimationControl*)malloc_(sizeof(hkaDefaultAnimationControl));

    hkaAnimationControl* local_hkaAnimationControl = init_hkaAnimationControl();
    local_hkaDefaultAnimationControl->HkaAnimationControl = *local_hkaAnimationControl;
    free(local_hkaAnimationControl);

    return local_hkaDefaultAnimationControl;
}

void copy_hkaAnimationControl(hkaAnimationControl* to, hkaAnimationControl* from)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));

    if (from->field0x18_cap > 64)
    {
        FATALERROR("Got %d number of hkaAnimationControl->field0x18_len entries. Only support 64.", from->field0x18_cap);
    }
    to->field0x18_len = from->field0x18_len;
    to->field0x18_cap = from->field0x18_cap;
    memcpy(to->field0x18, from->field0x18, to->field0x18_len);

    if (from->field0x28_cap > 64)
    {
        FATALERROR("Got %d number of hkaAnimationControl->field0x28_len entries. Only support 64.", from->field0x28_cap);
    }
    to->field0x28_len = from->field0x28_len;
    to->field0x28_cap = from->field0x28_cap;
    memcpy(to->field0x28, from->field0x28, to->field0x28_len);

    copy_hkaAnimationBinding(to->HkaAnimationBinding, from->HkaAnimationBinding);

    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
}

hkaAnimationControl* init_hkaAnimationControl()
{
    hkaAnimationControl* local_hkaAnimationControl = (hkaAnimationControl*)malloc_(sizeof(hkaAnimationControl));

    local_hkaAnimationControl->field0x18 = (uint8_t*)malloc_(sizeof(uint8_t) * 64);
    local_hkaAnimationControl->field0x28 = (uint8_t*)malloc_(sizeof(uint8_t) * 64);
    local_hkaAnimationControl->HkaAnimationBinding = init_hkaAnimationBinding();

    return local_hkaAnimationControl;
}

void copy_hkaAnimationBinding(hkaAnimationBinding* to, hkaAnimationBinding* from)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
}

hkaAnimationBinding* init_hkaAnimationBinding()
{
    hkaAnimationBinding* local_hkaAnimationBinding = (hkaAnimationBinding*)malloc_(sizeof(hkaAnimationBinding));

    return local_hkaAnimationBinding;
}

void copy_ChrCtrl_AnimationQueue_field0x10(ChrCtrl_AnimationQueue_field0x10* to, ChrCtrl_AnimationQueue_field0x10* from)
{
    if (from->array1_len != 4)
    {
        FATALERROR("Got %d number of ChrCtrl_AnimationQueue_field0x10->array1 entries. Only support 4.", from->array1_len);
    }
    if (from->array2_len != 8)
    {
        FATALERROR("Got %d number of ChrCtrl_AnimationQueue_field0x10->array2 entries. Only support 8.", from->array2_len);
    }
    to->array1_len = from->array1_len;
    to->array2_len = from->array2_len;
    memcpy(to->arry2, from->arry2, sizeof(ChrCtrl_AnimationQueue_field0x10_field0x10arrayelem) * to->array2_len);

    //adjust the pointers, since they always point to elems in array2
    for (size_t i = 0; i < to->array1_len; i++)
    {
        if (from->arry1[i] == NULL)
        {
            to->arry1[i] = NULL;
        }
        else
        {
            uint64_t offset = (uint64_t)(from->arry1[i]) - (uint64_t)(from->arry2);
            if (offset > sizeof(ChrCtrl_AnimationQueue_field0x10_field0x10arrayelem*) * to->array2_len)
            {
                FATALERROR("Got invalid offset for ChrCtrl_AnimationQueue_field0x10->array1 entry. Got %x", offset);
            }
            to->arry1[i] = (ChrCtrl_AnimationQueue_field0x10_field0x10arrayelem*)(offset + (uint64_t)(to->arry2)); //preserve the offset, just change base
        }
    }
}

ChrCtrl_AnimationQueue_field0x10* init_ChrCtrl_AnimationQueue_field0x10()
{
    ChrCtrl_AnimationQueue_field0x10* local_ChrCtrl_AnimationQueue_field0x10 = (ChrCtrl_AnimationQueue_field0x10*)malloc_(sizeof(ChrCtrl_AnimationQueue_field0x10));

    local_ChrCtrl_AnimationQueue_field0x10->arry1 = (ChrCtrl_AnimationQueue_field0x10_field0x10arrayelem**)malloc_(sizeof(ChrCtrl_AnimationQueue_field0x10_field0x10arrayelem*) * 4); //allow storage of 4
    local_ChrCtrl_AnimationQueue_field0x10->arry2 = (ChrCtrl_AnimationQueue_field0x10_field0x10arrayelem*)malloc_(sizeof(ChrCtrl_AnimationQueue_field0x10_field0x10arrayelem) * 8); //allow storage of 8

    return local_ChrCtrl_AnimationQueue_field0x10;
}

void copy_ChrCtrl_AnimationQueueEntry(ChrCtrl_AnimationQueueEntry* to, ChrCtrl_AnimationQueueEntry* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));

    if (to_game)
    {
        if (to->animEntryInfo == NULL && from->animEntryInfo != NULL)
        {
            to->animEntryInfo = (AnimationQueueEntry_AnimationInfo*)Game::game_malloc(sizeof(AnimationQueueEntry_AnimationInfo), 8, (void*)*(uint64_t*)(0x141C04F30)); //internal_heap_3
            copy_AnimationQueueEntry_AnimationInfo(to->animEntryInfo, from->animEntryInfo);
        }
        else if (from->animEntryInfo == NULL)
        {
            //TODO need to free this to prevent memory leak
            to->animEntryInfo = NULL;
        }
        else if (to->animEntryInfo != NULL && from->animEntryInfo != NULL)
        {
            copy_AnimationQueueEntry_AnimationInfo(to->animEntryInfo, from->animEntryInfo);
        }
    }
    else
    {
        if (to->animEntryInfo == NULL && from->animEntryInfo != NULL)
        {
            to->animEntryInfo = init_AnimationQueueEntry_AnimationInfo();
            copy_AnimationQueueEntry_AnimationInfo(to->animEntryInfo, from->animEntryInfo);
        }
        else if (from->animEntryInfo == NULL)
        {
            free(to->animEntryInfo);
            to->animEntryInfo = NULL;
        }
        else if (to->animEntryInfo != NULL && from->animEntryInfo != NULL)
        {
            copy_AnimationQueueEntry_AnimationInfo(to->animEntryInfo, from->animEntryInfo);
        }
    }

    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
}

ChrCtrl_AnimationQueueEntry* init_ChrCtrl_AnimationQueueEntry()
{
    ChrCtrl_AnimationQueueEntry* local_AnimationQueueEntry = (ChrCtrl_AnimationQueueEntry*)malloc_(sizeof(ChrCtrl_AnimationQueueEntry));

    local_AnimationQueueEntry->animEntryInfo = init_AnimationQueueEntry_AnimationInfo();

    return local_AnimationQueueEntry;
}

void copy_AnimationQueueEntry_AnimationInfo(AnimationQueueEntry_AnimationInfo* to, AnimationQueueEntry_AnimationInfo* from)
{
    //since this object is being allocated in-game as needed by us, we need to save everything in it. can't rely on stuff we don't care about being in there already
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    to->hkaAnimationBinding = from->hkaAnimationBinding;
    to->unk = from->unk;
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
    to->unk2 = from->unk2;
}

AnimationQueueEntry_AnimationInfo* init_AnimationQueueEntry_AnimationInfo()
{
    AnimationQueueEntry_AnimationInfo* local_AnimationQueueEntry_AnimationInfo = (AnimationQueueEntry_AnimationInfo*)malloc_(sizeof(AnimationQueueEntry_AnimationInfo));

    return local_AnimationQueueEntry_AnimationInfo;
}

void copy_AnimationQueue(AnimationQueue* to, AnimationQueue* from)
{
    for (int i = 0; i < 6; i++)
    {
        copy_AnimationQueue_Entry(to->AnimationQueue_Entries[i], from->AnimationQueue_Entries[i]);
    }
}

AnimationQueue* init_AnimationQueue()
{
    AnimationQueue* local_AnimationQueue = (AnimationQueue*)malloc_(sizeof(AnimationQueue));

    for (int i = 0; i < 6; i++)
    {
        local_AnimationQueue->AnimationQueue_Entries[i] = init_AnimationQueue_Entry();
    }

    return local_AnimationQueue;
}

void copy_AnimationQueue_Entry(AnimationQueue_Entry* to, AnimationQueue_Entry* from)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
    memcpy(to->data_2, from->data_2, sizeof(to->data_2));
    memcpy(to->data_3, from->data_3, sizeof(to->data_3));
}

AnimationQueue_Entry* init_AnimationQueue_Entry()
{
    AnimationQueue_Entry* local_AnimationQueue_Entry = (AnimationQueue_Entry*)malloc_(sizeof(AnimationQueue_Entry));
    return local_AnimationQueue_Entry;
}
