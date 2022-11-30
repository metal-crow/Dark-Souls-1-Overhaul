#include "PlayerInsStructFunctions.h"
#include "Rollback.h"

typedef void* falloc(uint64_t, uint64_t, uint32_t);

void copy_PlayerIns(PlayerIns* to, PlayerIns* from, bool to_game)
{
    copy_ChrIns(&to->chrins, &from->chrins, to_game);
    copy_PlayerGameData(to->playergamedata, from->playergamedata);
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
    to->data_2 = from->data_2;
    copy_RingEquipCtrl(to->ringequipctrl, from->ringequipctrl, to_game);
    copy_WeaponEquipCtrl(to->weaponequipctrl, from->weaponequipctrl, to_game);
    copy_ProEquipCtrl(to->proequipctrl, from->proequipctrl, to_game);
    to->curSelectedMagicId = from->curSelectedMagicId;
    to->curUsedItem = from->curUsedItem;
    to->itemId = from->itemId;
    to->override_equipped_magicId = from->override_equipped_magicId;
    copy_ChrAsm(to->chrasm, from->chrasm);
    copy_ChrAsmModelRes(to->chrAsmModelRes, from->chrAsmModelRes, to_game);
    copy_ChrAsmModel(to->chrAsmModel, from->chrAsmModel, to_game);
    memcpy(to->data_3, from->data_3, sizeof(to->data_3));
    memcpy(to->data_4, from->data_4, sizeof(to->data_4));
    memcpy(to->data_5, from->data_5, sizeof(to->data_5));
    memcpy(to->data_6, from->data_6, sizeof(to->data_6));
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
    local_PlayerIns->chrAsmModelRes = init_ChrAsmModelRes();
    local_PlayerIns->chrAsmModel = init_ChrAsmModel();

    return local_PlayerIns;
}

void free_PlayerIns(PlayerIns* to)
{
    free_ChrIns(&to->chrins, false);
    free_PlayerGameData(to->playergamedata);
    free_RingEquipCtrl(to->ringequipctrl);
    free_WeaponEquipCtrl(to->weaponequipctrl);
    free_ProEquipCtrl(to->proequipctrl);
    free_ChrAsm(to->chrasm);
    free_ChrAsmModelRes(to->chrAsmModelRes);
    free_ChrAsmModel(to->chrAsmModel);

    free(to);
}

void copy_ChrAsm(ChrAsm* to, ChrAsm* from)
{
    to->equipped_weapon_style = from->equipped_weapon_style;
    to->l_hand_equipped_index = from->l_hand_equipped_index;
    to->r_hand_equipped_index = from->r_hand_equipped_index;
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    memcpy(to->equip_items, from->equip_items, sizeof(to->equip_items));
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
}

ChrAsm* init_ChrAsm()
{
    ChrAsm* local_ChrAsm = (ChrAsm*)malloc_(sizeof(ChrAsm));
    return local_ChrAsm;
}

void free_ChrAsm(ChrAsm* to)
{
    free(to);
}

void copy_ChrAsmModelRes(ChrAsmModelRes* to, ChrAsmModelRes* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    for (size_t i = 0; i < 14; i++)
    {
        copy_ChrAsmModelRes_Elem(&to->arry[i], &from->arry[i], to_game);
    }
}

ChrAsmModelRes* init_ChrAsmModelRes()
{
    ChrAsmModelRes* local_ChrAsmModelRes = (ChrAsmModelRes*)malloc_(sizeof(ChrAsmModelRes));

    for (size_t i = 0; i < 14; i++)
    {
        ChrAsmModelRes_Elem* local_elem = init_ChrAsmModelRes_Elem();
        local_ChrAsmModelRes->arry[i] = *local_elem;
        free(local_elem);
    }

    return local_ChrAsmModelRes;
}

void free_ChrAsmModelRes(ChrAsmModelRes* to)
{
    for (size_t i = 0; i < 14; i++)
    {
        free_ChrAsmModelRes_Elem(&to->arry[i], false);
    }
    free(to);
}

void copy_ChrAsmModelRes_Elem(ChrAsmModelRes_Elem* to, ChrAsmModelRes_Elem* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
}

ChrAsmModelRes_Elem* init_ChrAsmModelRes_Elem()
{
    ChrAsmModelRes_Elem* local_ChrAsmModelRes_Elem = (ChrAsmModelRes_Elem*)malloc_(sizeof(ChrAsmModelRes_Elem));
    return local_ChrAsmModelRes_Elem;
}

void free_ChrAsmModelRes_Elem(ChrAsmModelRes_Elem* to, bool freeself)
{
    if (freeself)
    {
        free(to);
    }
}

void copy_ChrAsmModel(ChrAsmModel* to, ChrAsmModel* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
    memcpy(to->data_2, from->data_2, sizeof(to->data_2));
    memcpy(to->data_3, from->data_3, sizeof(to->data_3));
    memcpy(to->data_4, from->data_4, sizeof(to->data_4));
    memcpy(to->data_5, from->data_5, sizeof(to->data_5));
    to->data_6 = from->data_6;
}

ChrAsmModel* init_ChrAsmModel()
{
    ChrAsmModel* local_ChrAsmModel = (ChrAsmModel*)malloc_(sizeof(ChrAsmModel));
    return local_ChrAsmModel;
}

void free_ChrAsmModel(ChrAsmModel* to)
{
    free(to);
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
    uint32_t* local_ProEquipCtrl_equipped_armors_ids = (uint32_t*)malloc_(sizeof(uint32_t) * 5);

    local_ProEquipCtrl->spEffectList = init_SpecialEffect();
    local_ProEquipCtrl->equipped_armors_ids = local_ProEquipCtrl_equipped_armors_ids;

    return local_ProEquipCtrl;
}

void free_ProEquipCtrl(ProEquipCtrl* to)
{
    free_SpecialEffect(to->spEffectList);
    free(to->equipped_armors_ids);

    free(to);
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
    uint32_t* local_WeaponEquipCtrl_equipped_weapons_ids = (uint32_t*)malloc_(sizeof(uint32_t) * 2);

    local_WeaponEquipCtrl->spEffectList = init_SpecialEffect();
    local_WeaponEquipCtrl->equipped_weapons_ids = local_WeaponEquipCtrl_equipped_weapons_ids;

    return local_WeaponEquipCtrl;
}

void free_WeaponEquipCtrl(WeaponEquipCtrl* to)
{
    free_SpecialEffect(to->spEffectList);
    free(to->equipped_weapons_ids);

    free(to);
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
    uint32_t* local_RingEquipCtrl_equipped_rings_ids = (uint32_t*)malloc_(sizeof(uint32_t) * 2);

    local_RingEquipCtrl->spEffectList = init_SpecialEffect();
    local_RingEquipCtrl->equipped_rings_ids = local_RingEquipCtrl_equipped_rings_ids;

    return local_RingEquipCtrl;
}

void free_RingEquipCtrl(RingEquipCtrl* to)
{
    free_SpecialEffect(to->spEffectList);
    free(to->equipped_rings_ids);

    free(to);
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

void free_PlayerGameData(PlayerGameData* to)
{
    free_EquipGameData(&to->equipGameData, false);

    free(to);
}

void copy_PlayerGameData_ChrProperties(PlayerGameData_ChrProperties* to, PlayerGameData_ChrProperties* from)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
}

void copy_EquipGameData(EquipGameData* to, EquipGameData* from)
{
    copy_ChrAsm(&to->chrasm, &from->chrasm);
    copy_EquipMagicData(to->equipMagicData, from->equipMagicData);
    copy_EquipItemData(&to->equippedItemsInQuickbar, &from->equippedItemsInQuickbar);
}

EquipGameData* init_EquipGameData()
{
    EquipGameData* local_EquipGameData = (EquipGameData*)malloc_(sizeof(EquipGameData));

    local_EquipGameData->equipMagicData = init_EquipMagicData();

    return local_EquipGameData;
}

void free_EquipGameData(EquipGameData* to, bool freeself)
{
    free_EquipMagicData(to->equipMagicData);

    if (freeself)
    {
        free(to);
    }
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

void free_EquipMagicData(EquipMagicData* to)
{
    free(to);
}

void copy_PlayerGameData_AttributeInfo(PlayerGameData_AttributeInfo* to, PlayerGameData_AttributeInfo* from)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
}

void copy_ChrIns(ChrIns* to, ChrIns* from, bool to_game)
{
    //copy_ChrIns_field0x18(to->field0x18, from->field0x18);
    copy_PlayerCtrl(to->playerCtrl, from->playerCtrl, to_game);
    copy_PadManipulator(to->padManipulator, from->padManipulator);
    to->CharaInitParamID = from->CharaInitParamID;
    memcpy(to->data_5, from->data_5, sizeof(to->data_5));
    to->lowerThrowAnim = from->lowerThrowAnim;
    to->upperThrowAnim = from->upperThrowAnim;
    to->curSelectedMagicId = from->curSelectedMagicId;
    to->curUsedItem = from->curUsedItem;
    copy_SpecialEffect(to->specialEffects, from->specialEffects, to_game);
    copy_QwcSpEffectEquipCtrl(to->qwcSpEffectEquipCtrl, from->qwcSpEffectEquipCtrl);
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    copy_ChrIns_field0x2c8(to->field0x2c8, from->field0x2c8);
    memcpy(to->data_0a, from->data_0a, sizeof(to->data_0a));
    copy_HitIns(to->hitins_1, from->hitins_1);
    copy_HitIns(to->hitins_2, from->hitins_2);
    to->data_0b = from->data_0b;
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
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
    memcpy(to->data_2, from->data_2, sizeof(to->data_2));
    memcpy(to->data_3, from->data_3, sizeof(to->data_3));
    memcpy(to->data_4, from->data_4, sizeof(to->data_4));
}

ChrIns* init_ChrIns()
{
    ChrIns* local_ChrIns = (ChrIns*)malloc_(sizeof(ChrIns));

    //local_ChrIns->field0x18 = init_ChrIns_field0x18();
    local_ChrIns->playerCtrl = init_PlayerCtrl();
    local_ChrIns->padManipulator = init_PadManipulator();
    local_ChrIns->specialEffects = init_SpecialEffect();
    local_ChrIns->qwcSpEffectEquipCtrl = init_QwcSpEffectEquipCtrl();
    local_ChrIns->field0x2c8 = init_ChrIns_field0x2c8();
    local_ChrIns->hitins_1 = init_HitIns();
    local_ChrIns->hitins_2 = init_HitIns();
    local_ChrIns->throw_animation_info = init_EntityThrowAnimationStatus();

    return local_ChrIns;
}

void free_ChrIns(ChrIns* to, bool freeself)
{
    //free_ChrIns_field0x18(to->field0x18);
    free_PlayerCtrl(to->playerCtrl);
    free_PadManipulator(to->padManipulator);
    free_SpecialEffect(to->specialEffects);
    free_QwcSpEffectEquipCtrl(to->qwcSpEffectEquipCtrl);
    free_ChrIns_field0x2c8(to->field0x2c8);
    free_HitIns(to->hitins_1);
    free_HitIns(to->hitins_2);
    free_EntityThrowAnimationStatus(to->throw_animation_info);

    if (freeself)
    {
        free(to);
    }
}

void copy_ChrIns_field0x18(ChrIns_field0x18* to, ChrIns_field0x18* from)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
}

ChrIns_field0x18* init_ChrIns_field0x18()
{
    ChrIns_field0x18* local_ChrIns_field0x18 = (ChrIns_field0x18*)malloc_(sizeof(ChrIns_field0x18));
    return local_ChrIns_field0x18;
}

void free_ChrIns_field0x18(ChrIns_field0x18* to)
{
    free(to);
}

void copy_ChrIns_field0x2c8(ChrIns_field0x2c8* to, ChrIns_field0x2c8* from)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    to->data_1 = from->data_1;
    memcpy(to->data_2, from->data_2, sizeof(to->data_2));
    to->data_3 = from->data_3;
}

ChrIns_field0x2c8* init_ChrIns_field0x2c8()
{
    ChrIns_field0x2c8* local_ChrIns_field0x2c8 = (ChrIns_field0x2c8*)malloc_(sizeof(ChrIns_field0x2c8));
    return local_ChrIns_field0x2c8;
}

void free_ChrIns_field0x2c8(ChrIns_field0x2c8* to)
{
    free(to);
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

void free_EntityThrowAnimationStatus(EntityThrowAnimationStatus* to)
{
    free(to);
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
    uint32_t* local_QwcSpEffectEquipCtrl_arry = (uint32_t*)malloc_(sizeof(uint32_t) * 64);

    local_QwcSpEffectEquipCtrl->arry = local_QwcSpEffectEquipCtrl_arry;

    return local_QwcSpEffectEquipCtrl;
}

void free_QwcSpEffectEquipCtrl(QwcSpEffectEquipCtrl* to)
{
    free(to->arry);

    free(to);
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

void free_SpecialEffect(SpecialEffect* to)
{
    free_SpecialEffect_Info(to->specialEffect_Info);

    free(to);
}

static const size_t max_preallocated_SpecialEffect_Info = 64;

void copy_SpecialEffect_Info(SpecialEffect_Info* to, SpecialEffect_Info* from, bool to_game)
{
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
                to->next = (SpecialEffect_Info*)((uint64_t)(to) + sizeof(SpecialEffect_Info));
            }
            else
            {
                to->next = NULL;
            }
            //don't need to handle prev since it should just be as expected

            from = from->next;
            to = (SpecialEffect_Info*)((uint64_t)(to) + sizeof(SpecialEffect_Info));
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
                to->next = (SpecialEffect_Info*)Game::game_malloc(sizeof(SpecialEffect_Info), 8, (void*)*(uint64_t*)(0x141b68f20)); //internal_heap_3
                to->next->next = NULL;
                to->next->prev = to;
            }

            //handle if the game's list is too long, and we need to free it's extra slots
            if (from->next == NULL && to->next != NULL)
            {
                SpecialEffect_Info* entry_to_free = to->next;
                to->next = NULL;
                while (entry_to_free)
                {
                    SpecialEffect_Info* next = entry_to_free->next;
                    Game::game_free_alt(entry_to_free);
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

void free_SpecialEffect_Info(SpecialEffect_Info* to)
{
    free(to);
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

void free_PadManipulator(PadManipulator* to)
{
    free(to);
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

void free_PlayerCtrl(PlayerCtrl* to)
{
    free_ChrCtrl(&to->chrCtrl, false);
    free_TurnAnim(to->turnAnim, true);
    free_ArrowTurnAnim(to->arrowTurnAnim);

    free(to);
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

void free_ArrowTurnAnim(ArrowTurnAnim* to)
{
    free_TurnAnim(&to->turnAnim, false);
    free_SpinJoint(to->joint_spine_2);
    free_SpinJoint(to->joint_spine1_2);

    free(to);
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

void free_SpinJoint(SpinJoint* to)
{
    free(to);
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

void free_TurnAnim(TurnAnim* to, bool freeself)
{
    free_SpinJoint(to->joint_UpperRoot);
    free_SpinJoint(to->joint_LowerRoot);
    free_SpinJoint(to->joint_spine1_1);
    free_SpinJoint(to->joint_spine_1);
    free_SpinJoint(to->joint_master);

    if (freeself)
    {
        free(to);
    }
}

void copy_ChrCtrl(ChrCtrl* to, ChrCtrl* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    copy_ChrCtrl_AnimationQueue(to->animationQueue, from->animationQueue, to_game);
    copy_AnimationMediator(to->animationMediator, from->animationMediator);
    copy_HavokChara(to->havokChara, from->havokChara, to_game);
    copy_ActionCtrl(to->actionctrl, from->actionctrl, to_game);
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

void free_ChrCtrl(ChrCtrl* to, bool freeself)
{
    free_ChrCtrl_AnimationQueue(to->animationQueue);
    free_AnimationMediator(to->animationMediator);
    free_HavokChara(to->havokChara);
    free_ActionCtrl(to->actionctrl);
    free_WalkAnim_Twist(to->walkAnim_Twist);

    if (freeself)
    {
        free(to);
    }
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

void free_WalkAnim_Twist(WalkAnim_Twist* to)
{
    free_SpinJoint(to->Upper_Root_Joint);
    free_SpinJoint(to->master_joint);
    free_SpinJoint(to->neck_joint);
    free_WalkAnim_Twist_Field0x228Elem(&to->walkAnim_Twist_Field0x228Elem, false);

    free(to);
}

void copy_WalkAnim_Twist_Field0x228Elem(WalkAnim_Twist_Field0x228Elem* to, WalkAnim_Twist_Field0x228Elem* from)
{
    to->field0x10_cap = from->field0x10_cap;
    to->unk = from->unk;
    to->field0x10_len = from->field0x10_len;
    if (to->field0x10_cap > 8)
    {
        FATALERROR("WalkAnim_Twist_Field0x228Elem->field0x10_cap is %d, max supported is 8 entries", to->field0x10_cap);
    }

    if (to->field0x10_cap > 0 && to->field0x10 == NULL)
    {
        //need to manually alloc the array for the game
        ConsoleWrite("WalkAnim_Twist_Field0x228Elem(%p)->field0x10 alloc", to);
        to->field0x10 = (WalkAnim_Twist_Field0x228Elem_field0x10elem**)(*(falloc*)*(uint64_t*)(*((uint64_t*)to->padding_1[0]) + 0x50))(to->padding_1[0], (to->field0x10_cap) * 8, 8);
        memset(to->field0x10, 0, to->field0x10_cap * 8);
    }

    //this array is allocated and also pre-populated with pointers in it's entries
    for (size_t i = 0; i < to->field0x10_cap; i++)
    {
        if (from->field0x10[i] == NULL && to->field0x10[i] != NULL)
        {
            to->field0x10[i] = NULL;
        }
        if (to->field0x10[i] == NULL && from->field0x10[i] != NULL)
        {
            //need to manually alloc the entry for the game
            ConsoleWrite("WalkAnim_Twist_Field0x228Elem->field0x10 entry alloc");
            to->field0x10[i] = (WalkAnim_Twist_Field0x228Elem_field0x10elem*)(*(falloc*)*(uint64_t*)(*((uint64_t*)to->padding_1[0]) + 0x50))(to->padding_1[0], 4 * 4, 4);
        }
        if (to->field0x10[i] != NULL && from->field0x10[i] != NULL)
        {
            copy_WalkAnim_Twist_Field0x228Elem_field0x10elem(to->field0x10[i], from->field0x10[i]);
        }
    }
}

WalkAnim_Twist_Field0x228Elem* init_WalkAnim_Twist_Field0x228Elem()
{
    WalkAnim_Twist_Field0x228Elem* local_WalkAnim_Twist_Field0x228Elem = (WalkAnim_Twist_Field0x228Elem*)malloc_(sizeof(WalkAnim_Twist_Field0x228Elem));

    local_WalkAnim_Twist_Field0x228Elem->field0x10 = (WalkAnim_Twist_Field0x228Elem_field0x10elem**)malloc_(sizeof(WalkAnim_Twist_Field0x228Elem_field0x10elem*) * 8);
    for (size_t i = 0; i < 8; i++)
    {
        local_WalkAnim_Twist_Field0x228Elem->field0x10[i] = (WalkAnim_Twist_Field0x228Elem_field0x10elem*)malloc_(sizeof(WalkAnim_Twist_Field0x228Elem_field0x10elem));
    }

    return local_WalkAnim_Twist_Field0x228Elem;
}

void free_WalkAnim_Twist_Field0x228Elem(WalkAnim_Twist_Field0x228Elem* to, bool freeself)
{
    for (size_t i = 0; i < 8; i++)
    {
        free(to->field0x10[i]);
    }
    free(to->field0x10);

    if (freeself)
    {
        free(to);
    }
}

void copy_WalkAnim_Twist_Field0x228Elem_field0x10elem(WalkAnim_Twist_Field0x228Elem_field0x10elem* to, WalkAnim_Twist_Field0x228Elem_field0x10elem* from)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
}

void copy_ActionCtrl(ActionCtrl* to, ActionCtrl* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    copy_ActionCtrl_0x30Substruct(&to->passive_state, &from->passive_state, to_game);
    copy_ActionCtrl_0x30Substruct(&to->active_state, &from->active_state, to_game);
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

void free_ActionCtrl(ActionCtrl* to)
{
    free_EzState_detail_EzStateMachineImpl(to->passive_state.EzStateMachineImpl);
    free_EzState_detail_EzStateMachineImpl(to->active_state.EzStateMachineImpl);

    free(to);
}

void copy_ActionCtrl_0x30Substruct(ActionCtrl_0x30Substruct* to, ActionCtrl_0x30Substruct* from, bool to_game)
{
    copy_EzState_detail_EzStateMachineImpl(to->EzStateMachineImpl, from->EzStateMachineImpl, to_game);
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
}

void copy_EzState_detail_EzStateMachineImpl(EzState_detail_EzStateMachineImpl* to, EzState_detail_EzStateMachineImpl* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));

    if (from->unk2 != NULL) { FATALERROR("EzState_detail_EzStateMachineImpl->unk2 = %p, not null", from->unk2); }

    if (from->padding_unk3[1] != NULL) { FATALERROR("EzState_detail_EzStateMachineImpl->field0x38.arry = %p, not null", from->padding_unk3[1]); }

    copy_EzStateRegisterSet(&to->EzStateRegisterSet1, &from->EzStateRegisterSet1, to_game);

    if (from->unk4 != NULL) { FATALERROR("EzState_detail_EzStateMachineImpl->unk4 = %p, not null", from->unk4); }

    copy_EzStateRegisterSet(&to->EzStateRegisterSet2, &from->EzStateRegisterSet2, to_game);

    memcpy(to->data_1, from->data_1, sizeof(to->data_1));

    if (from->padding_MessageQueue[3] != NULL) { FATALERROR("EzState_detail_EzStateMachineImpl->MessageQueue.arry = %p, not null", from->padding_MessageQueue[3]); }

    memcpy(to->data_2, from->data_2, sizeof(to->data_2));
}

EzState_detail_EzStateMachineImpl* init_EzState_detail_EzStateMachineImpl()
{
    EzState_detail_EzStateMachineImpl* local_EzState_detail_EzStateMachineImpl = (EzState_detail_EzStateMachineImpl*)malloc_(sizeof(EzState_detail_EzStateMachineImpl));

    EzStateRegisterSet* local_EzStateRegisterSet = init_EzStateRegisterSet();
    local_EzState_detail_EzStateMachineImpl->EzStateRegisterSet1 = *local_EzStateRegisterSet;
    free(local_EzStateRegisterSet);

    local_EzStateRegisterSet = init_EzStateRegisterSet();
    local_EzState_detail_EzStateMachineImpl->EzStateRegisterSet2 = *local_EzStateRegisterSet;
    free(local_EzStateRegisterSet);

    return local_EzState_detail_EzStateMachineImpl;
}

void free_EzState_detail_EzStateMachineImpl(EzState_detail_EzStateMachineImpl* to)
{
    free_EzStateRegisterSet(&to->EzStateRegisterSet1, false);
    free_EzStateRegisterSet(&to->EzStateRegisterSet2, false);

    free(to);
}

void copy_EzStateRegisterSet(EzStateRegisterSet* to, EzStateRegisterSet* from, bool to_game)
{
    if (!to_game)
    {
        size_t arry_size = (from->arry_cur - (uint64_t)from->arry) / sizeof(EzStateRegister);
        if (arry_size != 8)
        {
            FATALERROR("EzStateRegisterSet (%p) has a size of %d, not 8", from, arry_size);
        }
    }
    for (size_t i = 0; i < 8; i++)
    {
        memcpy(to->arry[i].data_0, from->arry[i].data_0, sizeof(EzStateRegister));
    }
    //don't need to save arry_cur and end, since they shouldn't change
}

EzStateRegisterSet* init_EzStateRegisterSet()
{
    EzStateRegisterSet* local_EzStateRegisterSet = (EzStateRegisterSet*)malloc_(sizeof(EzStateRegisterSet));

    //array size is const 8
    local_EzStateRegisterSet->arry = (EzStateRegister*)malloc_(8 * sizeof(EzStateRegister));

    return local_EzStateRegisterSet;
}

void free_EzStateRegisterSet(EzStateRegisterSet* to, bool freeself)
{
    free(to->arry);

    if (freeself)
    {
        free(to);
    }
}

void copy_HavokChara(HavokChara* to, HavokChara* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    copy_hkpCharacterProxy(to->char_proxy, from->char_proxy);
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
    memcpy(to->data_2, from->data_2, sizeof(to->data_2));
    memcpy(to->data_3, from->data_3, sizeof(to->data_3));
    memcpy(to->data_4, from->data_4, sizeof(to->data_4));
}

HavokChara* init_HavokChara()
{
    HavokChara* local_HavokChara = (HavokChara*)malloc_(sizeof(HavokChara));

    local_HavokChara->char_proxy = init_hkpCharacterProxy();

    return local_HavokChara;
}

void free_HavokChara(HavokChara* to)
{
    free_hkpCharacterProxy(to->char_proxy);
    free_HitIns(to->padding_hitIns);

    free(to);
}

void copy_HitIns(HitIns* to, HitIns* from)
{
    to->data_0 = from->data_0;
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
    to->data_2 = from->data_2;
    to->data_3 = from->data_3;
    to->data_4 = from->data_4;
    to->data_5 = from->data_5;
}

HitIns* init_HitIns()
{
    HitIns* local_HitIns = (HitIns*)malloc_(sizeof(HitIns));

    return local_HitIns;
}

void free_HitIns(HitIns* to)
{
    free(to);
}

void copy_hkpCharacterProxy(hkpCharacterProxy* to, hkpCharacterProxy* from)
{
    to->data_0 = from->data_0;
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
    copy_hkpSimpleShapePhantom(to->HkpSimpleShapePhantom, from->HkpSimpleShapePhantom);
    memcpy(to->data_2, from->data_2, sizeof(to->data_2));
    memcpy(to->data_3, from->data_3, sizeof(to->data_3));
}

hkpCharacterProxy* init_hkpCharacterProxy()
{
    hkpCharacterProxy* local_hkpCharacterProxy = (hkpCharacterProxy*)malloc_(sizeof(hkpCharacterProxy));

    local_hkpCharacterProxy->HkpSimpleShapePhantom = init_hkpSimpleShapePhantom();

    return local_hkpCharacterProxy;
}

void free_hkpCharacterProxy(hkpCharacterProxy* to)
{
    free_hkpSimpleShapePhantom(to->HkpSimpleShapePhantom);
    free(to);
}

void copy_hkpSimpleShapePhantom(hkpSimpleShapePhantom* to, hkpSimpleShapePhantom* from)
{
    copy_hkpSimpleShapePhantom_field0x30(to->field0x30, from->field0x30);
}

hkpSimpleShapePhantom* init_hkpSimpleShapePhantom()
{
    hkpSimpleShapePhantom* local_hkpSimpleShapePhantom = (hkpSimpleShapePhantom*)malloc_(sizeof(hkpSimpleShapePhantom));

    local_hkpSimpleShapePhantom->field0x30 = init_hkpSimpleShapePhantom_field0x30();

    return local_hkpSimpleShapePhantom;
}

void free_hkpSimpleShapePhantom(hkpSimpleShapePhantom* to)
{
    free_hkpSimpleShapePhantom_field0x30(to->field0x30);
    free(to);
}

void copy_hkpSimpleShapePhantom_field0x30(hkpSimpleShapePhantom_field0x30* to, hkpSimpleShapePhantom_field0x30* from)
{
    memcpy(to->position, from->position, sizeof(to->position));
}

hkpSimpleShapePhantom_field0x30* init_hkpSimpleShapePhantom_field0x30()
{
    hkpSimpleShapePhantom_field0x30* local_hkpSimpleShapePhantom_field0x30 = (hkpSimpleShapePhantom_field0x30*)malloc_(sizeof(hkpSimpleShapePhantom_field0x30));

    return local_hkpSimpleShapePhantom_field0x30;
}

void free_hkpSimpleShapePhantom_field0x30(hkpSimpleShapePhantom_field0x30* to)
{
    free(to);
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

void free_AnimationMediator(AnimationMediator* to)
{
    for (int i = 0; i < 31; i++)
    {
        free_AnimationMediatorStateEntry(&to->states_list[i], false);
    }
    free_AnimationQueue(to->animationQueue);

    free(to);
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

void free_AnimationMediatorStateEntry(AnimationMediatorStateEntry* to, bool freeself)
{
    if (freeself)
    {
        free(to);
    }
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

void free_ChrCtrl_AnimationQueue(ChrCtrl_AnimationQueue* to)
{
    for (size_t i = 0; i < 32; i++)
    {
        free_ChrCtrl_AnimationQueueEntry(&to->arry[i], false);
    }
    free(to->arry);
    free_ChrCtrl_AnimationQueue_field0x10(to->field0x10);
    free_hkaAnimatedSkeleton(to->HkaAnimatedSkeleton);
    free_ChrCtrl_AnimationQueue_field0x20(to->field0x20);

    free(to);
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

void free_ChrCtrl_AnimationQueue_field0x20(ChrCtrl_AnimationQueue_field0x20* to)
{
    free(to->field0x8);
    free(to->field0x18);
    free(to->field0x28);

    free(to);
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

void free_hkaAnimatedSkeleton(hkaAnimatedSkeleton* to)
{
    for (int i = 0; i < 32; i++)
    {
        free_hkaDefaultAnimationControl(to->animCtrl_list[i]);
    }
    free(to->animCtrl_list);

    free(to);
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

void free_hkaDefaultAnimationControl(hkaDefaultAnimationControl* to)
{
    free_hkaAnimationControl(&to->HkaAnimationControl, false);

    free(to);
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

    to->HkaAnimationBinding = from->HkaAnimationBinding;

    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
}

hkaAnimationControl* init_hkaAnimationControl()
{
    hkaAnimationControl* local_hkaAnimationControl = (hkaAnimationControl*)malloc_(sizeof(hkaAnimationControl));

    local_hkaAnimationControl->field0x18 = (uint8_t*)malloc_(sizeof(uint8_t) * 64);
    local_hkaAnimationControl->field0x28 = (uint8_t*)malloc_(sizeof(uint8_t) * 64);

    return local_hkaAnimationControl;
}

void free_hkaAnimationControl(hkaAnimationControl* to, bool freeself)
{
    free(to->field0x18);
    free(to->field0x28);

    if (freeself)
    {
        free(to);
    }
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

void free_ChrCtrl_AnimationQueue_field0x10(ChrCtrl_AnimationQueue_field0x10* to)
{
    free(to->arry1);
    free(to->arry2);

    free(to);
}

void copy_ChrCtrl_AnimationQueueEntry(ChrCtrl_AnimationQueueEntry* to, ChrCtrl_AnimationQueueEntry* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    copy_hkaDefaultAnimationControl(to->defaultAnimationControl, from->defaultAnimationControl);
    to->HvkAnim_AnimInfoArrayElem = from->HvkAnim_AnimInfoArrayElem;
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
}

ChrCtrl_AnimationQueueEntry* init_ChrCtrl_AnimationQueueEntry()
{
    ChrCtrl_AnimationQueueEntry* local_AnimationQueueEntry = (ChrCtrl_AnimationQueueEntry*)malloc_(sizeof(ChrCtrl_AnimationQueueEntry));

    local_AnimationQueueEntry->defaultAnimationControl = init_hkaDefaultAnimationControl();

    return local_AnimationQueueEntry;
}

void free_ChrCtrl_AnimationQueueEntry(ChrCtrl_AnimationQueueEntry* to, bool freeself)
{
    free(to->defaultAnimationControl);

    if (freeself)
    {
        free(to);
    }
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

void free_AnimationQueue(AnimationQueue* to)
{
    for (int i = 0; i < 6; i++)
    {
        free_AnimationQueue_Entry(to->AnimationQueue_Entries[i]);
    }

    free(to);
}

void copy_AnimationQueue_Entry(AnimationQueue_Entry* to, AnimationQueue_Entry* from)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
    copy_AnimationQueue_Entry_sub1(&to->sub1, &from->sub1);
    memcpy(to->data_2, from->data_2, sizeof(to->data_2));

    size_t i = 0;
    size_t from_len = ((uint64_t)from->chained_animations_array_end - (uint64_t)from->chained_animations_array_start) / 8;
    size_t to_cap = ((uint64_t)from->chained_animations_array_capEnd - (uint64_t)from->chained_animations_array_start) / 8;
    if (from_len > to_cap)
    {
        FATALERROR("AnimationQueue_Entry->chained_animations_array has a length of %d, but we can only output %d", from_len, to_cap);
    }
    for (i = 0; i < from_len; i++)
    {
        to->chained_animations_array_start[i] = from->chained_animations_array_start[i];
    }
    to->chained_animations_array_end = &to->chained_animations_array_start[i];

    memcpy(to->data_3, from->data_3, sizeof(to->data_3));
}

AnimationQueue_Entry* init_AnimationQueue_Entry()
{
    AnimationQueue_Entry* local_AnimationQueue_Entry = (AnimationQueue_Entry*)malloc_(sizeof(AnimationQueue_Entry));

    AnimationQueue_Entry_sub1* local_AnimationQueue_Entry_sub1 = init_AnimationQueue_Entry_sub1();
    local_AnimationQueue_Entry->sub1 = *local_AnimationQueue_Entry_sub1;
    free(local_AnimationQueue_Entry_sub1);

    local_AnimationQueue_Entry->chained_animations_array_start = (uint64_t*)malloc_(sizeof(uint64_t) * 16);
    local_AnimationQueue_Entry->chained_animations_array_capEnd = &local_AnimationQueue_Entry->chained_animations_array_start[16];

    return local_AnimationQueue_Entry;
}

void free_AnimationQueue_Entry(AnimationQueue_Entry* to)
{
    free_AnimationQueue_Entry_sub1(&to->sub1, false);
    free(to->chained_animations_array_start);

    free(to);
}

void copy_AnimationQueue_Entry_sub1(AnimationQueue_Entry_sub1* to, AnimationQueue_Entry_sub1* from)
{
    to->field0x10_cap = from->field0x10_cap;
    to->unk = from->unk;
    to->field0x10_len = from->field0x10_len;
    if (to->field0x10_cap > 8)
    {
        FATALERROR("AnimationQueue_Entry->field0x10_cap is %d, max supported is 8 entries", to->field0x10_cap);
    }

    if (to->field0x10_cap > 0 && to->field0x10 == NULL)
    {
        //need to manually alloc the array for the game
        ConsoleWrite("AnimationQueue_Entry(%p)->field0x10 alloc", to);
        to->field0x10 = (AnimationQueue_Entry_sub1_field0x10**)(*(falloc*)*(uint64_t*)(*((uint64_t*)to->padding_1[0]) + 0x50))(to->padding_1[0], (to->field0x10_cap) * 8, 8);
        memset(to->field0x10, 0, to->field0x10_cap * 8);
    }

    //this array is allocated and not pre-populated with pointers in it's entries
    for (size_t i = 0; i < to->field0x10_len; i++)
    {
        if (from->field0x10[i] == NULL)
        {
            FATALERROR("Why is AnimationQueue_Entry->field0x10[%d] null?", i);
        }
        if (to->field0x10[i] == NULL)
        {
            //need to manually alloc the entry for the game
            ConsoleWrite("AnimationQueue_Entry->field0x10 entry alloc");
            to->field0x10[i] = (AnimationQueue_Entry_sub1_field0x10*)(*(falloc*)*(uint64_t*)(*((uint64_t*)to->padding_1[0]) + 0x50))(to->padding_1[0], 2 * 8, 8);
        }
        copy_AnimationQueue_Entry_sub1_field0x10(to->field0x10[i], from->field0x10[i]);
    }
}

AnimationQueue_Entry_sub1* init_AnimationQueue_Entry_sub1()
{
    AnimationQueue_Entry_sub1* local_AnimationQueue_Entry_sub1 = (AnimationQueue_Entry_sub1*)malloc_(sizeof(AnimationQueue_Entry_sub1));

    local_AnimationQueue_Entry_sub1->field0x10 = (AnimationQueue_Entry_sub1_field0x10**)malloc_(sizeof(AnimationQueue_Entry_sub1_field0x10*) * 8);
    for (size_t i = 0; i < 8; i++)
    {
        local_AnimationQueue_Entry_sub1->field0x10[i] = (AnimationQueue_Entry_sub1_field0x10*)malloc_(sizeof(AnimationQueue_Entry_sub1_field0x10));
    }

    return local_AnimationQueue_Entry_sub1;
}

void free_AnimationQueue_Entry_sub1(AnimationQueue_Entry_sub1* to, bool freeself)
{
    for (size_t i = 0; i < 8; i++)
    {
        free(to->field0x10[i]);
    }
    free(to->field0x10);

    if (freeself)
    {
        free(to);
    }
}

void copy_AnimationQueue_Entry_sub1_field0x10(AnimationQueue_Entry_sub1_field0x10* to, AnimationQueue_Entry_sub1_field0x10* from)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
}
