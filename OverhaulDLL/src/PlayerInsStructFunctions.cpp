#include "PlayerInsStructFunctions.h"
#include "Rollback.h"
#include "AttachSysSlotStructsFunctions.h"
#include "PadManipulatorStructFunctions.h"
#include "StateSerializer.h"

typedef void* falloc(uint64_t, uint64_t, uint32_t);


void copy_PlayerIns(PlayerIns* to, const PlayerIns* from, StateTarget target)
{
    Game::SuspendThreads();

    copy_ChrIns(&to->chrins, &from->chrins, target);
    copy_PlayerGameData(to->playergamedata, from->playergamedata);
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    memcpy(&to->unk_7a8, &from->unk_7a8, 16);
    memcpy(&to->unk_7d0, &from->unk_7d0, 8);
    copy_RingEquipCtrl(to->ringequipctrl, from->ringequipctrl, target);
    copy_WeaponEquipCtrl(to->weaponequipctrl, from->weaponequipctrl, target);
    copy_ProEquipCtrl(to->proequipctrl, from->proequipctrl, target);
    to->curSelectedMagicId = from->curSelectedMagicId;
    to->curUsedItem = from->curUsedItem;
    to->override_itemId = from->override_itemId;
    to->override_equipped_magicId = from->override_equipped_magicId;
    to->using_override = from->using_override;
    copy_ChrAsm(to->chrasm, from->chrasm);
    //copy_ChrAsmModelRes(to->chrAsmModelRes, from->chrAsmModelRes, target); //This is dynamically re-drawn every frame by the game
    copy_ChrAsmModel(to->chrAsmModel, from->chrAsmModel, target);
    memcpy(&to->headSize, &from->headSize, 24);
    memcpy(&to->unk_880, &from->unk_880, 0x50);
    memcpy(to->data_5, from->data_5, sizeof(to->data_5));
    memcpy(to->data_5a, from->data_5a, sizeof(to->data_5a));
    memcpy(to->data_6, from->data_6, sizeof(to->data_6));

    Game::ResumeThreads();
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


void copy_ChrAsm(ChrAsm* to, const ChrAsm* from)
{
    to->equipped_weapon_style = from->equipped_weapon_style;
    to->l_hand_equipped_index = from->l_hand_equipped_index;
    to->r_hand_equipped_index = from->r_hand_equipped_index;
    to->l_arrow_equipped_index = from->l_arrow_equipped_index;
    to->r_arrow_equipped_index = from->r_arrow_equipped_index;
    to->l_bolt_equipped_index = from->l_bolt_equipped_index;
    to->r_bolt_equipped_index = from->r_bolt_equipped_index;
    memcpy(to->equip_items, from->equip_items, sizeof(to->equip_items));
    to->transformProtectorId = from->transformProtectorId;
    to->transformProtectorId_HalfCamo = from->transformProtectorId_HalfCamo;
    to->unk_7c = from->unk_7c;
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


void copy_ChrAsmModelRes(ChrAsmModelRes* to, const ChrAsmModelRes* from, StateTarget target)
{
    memcpy(&to->magic_state, &from->magic_state, 0x28);
    for (size_t i = 0; i < 14; i++)
    {
        copy_ChrAsmModelRes_Elem(&to->arry[i], &from->arry[i], target);
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


void copy_ChrAsmModelRes_Elem(ChrAsmModelRes_Elem* to, const ChrAsmModelRes_Elem* from, StateTarget target)
{
    to->curModelId = from->curModelId;
    to->newModelId = from->newModelId;
    to->PartsbndFileCap2 = NULL; //this should always be null since it's just tmp storage for 1 frame
    memcpy(&to->unk_18, &from->unk_18, 0x38);
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


void copy_ChrAsmModel(ChrAsmModel* to, const ChrAsmModel* from, StateTarget target)
{
    memcpy(&to->unk_8, &from->unk_8, 0x10);
    memcpy(to->fieldE0, from->fieldE0, sizeof(to->fieldE0));
    memcpy(&to->unk_68, &from->unk_68, 0x18);
    to->data_3a = from->data_3a;
    memcpy(&to->unk_a0, &from->unk_a0, 0x28);
    memcpy(&to->hairColor, &from->hairColor, 0x80);
    memcpy(to->filecap_array, from->filecap_array, sizeof(to->filecap_array));
    to->unk_1c8 = from->unk_1c8;
    memcpy(to->unk_1c9, from->unk_1c9, sizeof(to->unk_1c9));
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


void copy_ProEquipCtrl(ProEquipCtrl* to, const ProEquipCtrl* from, StateTarget target)
{
    copy_SpecialEffect(to->spEffectList, from->spEffectList, target);
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


void copy_WeaponEquipCtrl(WeaponEquipCtrl* to, const WeaponEquipCtrl* from, StateTarget target)
{
    copy_SpecialEffect(to->spEffectList, from->spEffectList, target);
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


void copy_RingEquipCtrl(RingEquipCtrl* to, const RingEquipCtrl* from, StateTarget target)
{
    copy_SpecialEffect(to->spEffectList, from->spEffectList, target);
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


void copy_PlayerGameData(PlayerGameData* to, const PlayerGameData* from)
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


void copy_PlayerGameData_ChrProperties(PlayerGameData_ChrProperties* to, const PlayerGameData_ChrProperties* from)
{
    memcpy(to, from, sizeof(PlayerGameData_ChrProperties));
}


void copy_EquipGameData(EquipGameData* to, const EquipGameData* from)
{
    memcpy(to->EquipItemToInventoryIndexMap, from->EquipItemToInventoryIndexMap, sizeof(to->EquipItemToInventoryIndexMap)); //Part of input
    memcpy(to->EquipItemToInventoryIndexMap_index_updated, from->EquipItemToInventoryIndexMap_index_updated, sizeof(to->EquipItemToInventoryIndexMap_index_updated)); //Part of input
    copy_ChrAsm(&to->chrasm, &from->chrasm);
    copy_EquipMagicData(to->equipMagicData, from->equipMagicData);
    copy_EquipItemData(&to->equippedItemsInQuickbar, &from->equippedItemsInQuickbar);
    to->amountOfItemUsedFromInventory = from->amountOfItemUsedFromInventory;
    to->itemInventoryIdCurrentlyBeingUsedFromInventory = from->itemInventoryIdCurrentlyBeingUsedFromInventory;
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


void copy_EquipItemData(EquipItemData* to, const EquipItemData* from)
{
    memcpy(to->quickbar, from->quickbar, sizeof(to->quickbar));
    to->selectedQuickbarItem = from->selectedQuickbarItem;
}


void copy_EquipMagicData(EquipMagicData* to, const EquipMagicData* from)
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


void copy_PlayerGameData_AttributeInfo(PlayerGameData_AttributeInfo* to, const PlayerGameData_AttributeInfo* from)
{
    memcpy(to, from, sizeof(PlayerGameData_AttributeInfo));
}


void copy_ChrIns(ChrIns* to, const ChrIns* from, StateTarget target)
{
    //copy_ChrIns_field0x18(to->field0x18, from->field0x18);
    copy_PlayerCtrl(to->playerCtrl, from->playerCtrl, target);
    copy_PadManipulator(to->padManipulator, from->padManipulator);
    to->CharaInitParamID = from->CharaInitParamID;
    memcpy(&to->unk_16c, &from->unk_16c, 0x10);
    to->lowerThrowAnim = from->lowerThrowAnim;
    to->upperThrowAnim = from->upperThrowAnim;
    memcpy(to->player_handing_state, from->player_handing_state, sizeof(uint32_t)*3);
    to->curToughness = from->curToughness;
    to->maxToughness = from->maxToughness;
    to->toughnessUnk1 = from->toughnessUnk1;
    to->toughnessRefillTimer = from->toughnessRefillTimer;
    to->toughnessUnk2 = from->toughnessUnk2;
    to->curSelectedMagicId = from->curSelectedMagicId;
    to->curUsedItem = from->curUsedItem;
    copy_SpecialEffect(to->specialEffects, from->specialEffects, target);
    copy_QwcSpEffectEquipCtrl(to->qwcSpEffectEquipCtrl, from->qwcSpEffectEquipCtrl);
    memcpy(&to->unk_288, &from->unk_288, 0x48);
    copy_ChrIns_field0x2c8(to->field0x2c8, from->field0x2c8);
    memcpy(&to->unk_2d8, &from->unk_2d8, 0x98);
    copy_HitIns(to->hitins_1, from->hitins_1);
    copy_HitIns(to->hitins_2, from->hitins_2);
    memcpy(&to->unk_380, &from->unk_380, 8);
    copy_ChrAttachSys(&to->chrattachsys, &from->chrattachsys, target);
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
    copy_EntityThrowAnimationStatus(to->throw_animation_info, from->throw_animation_info, target);
    memcpy(&to->unk_450, &from->unk_450, 0x18);
    memcpy(&to->unk_470, &from->unk_470, 0x50);
    memcpy(&to->unk_4d8, &from->unk_4d8, 0x18);
    memcpy(&to->unk_518, &from->unk_518, 0x58);
}

ChrIns* init_ChrIns()
{
    ChrIns* local_ChrIns = (ChrIns*)malloc_(sizeof(ChrIns));

    //local_ChrIns->field0x18 = init_ChrIns_field0x18();
    local_ChrIns->playerCtrl = init_PlayerCtrl();
    local_ChrIns->padManipulator = init_PadManipulator();
    local_ChrIns->player_handing_state = (uint32_t*)malloc_(sizeof(uint32_t)*3);
    local_ChrIns->specialEffects = init_SpecialEffect();
    local_ChrIns->qwcSpEffectEquipCtrl = init_QwcSpEffectEquipCtrl();
    local_ChrIns->field0x2c8 = init_ChrIns_field0x2c8();
    local_ChrIns->hitins_1 = init_HitIns();
    local_ChrIns->hitins_2 = init_HitIns();
    auto local_chrattach_sys = init_ChrAttachSys();
    local_ChrIns->chrattachsys = *local_chrattach_sys;
    free(local_chrattach_sys);
    local_ChrIns->throw_animation_info = init_EntityThrowAnimationStatus();

    return local_ChrIns;
}

void free_ChrIns(ChrIns* to, bool freeself)
{
    //free_ChrIns_field0x18(to->field0x18);
    free_PlayerCtrl(to->playerCtrl);
    free_PadManipulator(to->padManipulator);
    free(to->player_handing_state);
    free_SpecialEffect(to->specialEffects);
    free_QwcSpEffectEquipCtrl(to->qwcSpEffectEquipCtrl);
    free_ChrIns_field0x2c8(to->field0x2c8);
    free_HitIns(to->hitins_1);
    free_HitIns(to->hitins_2);
    free_ChrAttachSys(&to->chrattachsys, false);
    free_EntityThrowAnimationStatus(to->throw_animation_info);

    if (freeself)
    {
        free(to);
    }
}


void copy_ChrAttachSys(ChrAttachSys* to, const ChrAttachSys* from, StateTarget target)
{
    if (to->SysSlots != NULL && from->SysSlots != NULL)
    {
        //free_AttachSysSlot recursively frees the whole chain AND any embedded dynamic arrays
        free_AttachSysSlot(to->SysSlots, target);
        to->SysSlots = NULL;
    }
    if (to->SysSlots == NULL && from->SysSlots != NULL)
    {
        to->SysSlots = init_AttachSysSlot((AttachSysSlotType)from->SysSlots->slotType, target);
    }
    if (to->SysSlots != NULL)
    {
        copy_AttachSysSlot(&(to->SysSlots), from->SysSlots, target);
    }
}

ChrAttachSys* init_ChrAttachSys()
{
    ChrAttachSys* local = (ChrAttachSys*)malloc_(sizeof(ChrAttachSys));

    local->SysSlots = NULL;

    return local;
}

void free_ChrAttachSys(ChrAttachSys* to, bool freeself)
{
    //ChrAttachSys is always managed as part of our local rollback snapshots, so targets are always MSVC-heap
    free_AttachSysSlot(to->SysSlots, StateTarget::ToLocal);
    to->SysSlots = NULL;

    if (freeself)
    {
        free(to);
    }
}

void copy_ChrIns_field0x18(ChrIns_field0x18* to, const ChrIns_field0x18* from)
{
    memcpy(to, from, sizeof(ChrIns_field0x18));
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


void copy_ChrIns_field0x2c8(ChrIns_field0x2c8* to, const ChrIns_field0x2c8* from)
{
    memcpy(&to->unk_8, &from->unk_8, 0x18);
    to->unk_28 = from->unk_28;
    to->unk_2c = from->unk_2c;
    memcpy(&to->unk_38, &from->unk_38, 0x10);
    to->unk_50 = from->unk_50;
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


void copy_EntityThrowAnimationStatus(EntityThrowAnimationStatus* to, const EntityThrowAnimationStatus* from, StateTarget target)
{
    to->playerins_parent = from->playerins_parent;
    to->throw_paramdef = from->throw_paramdef;
    memcpy(&to->unk_18, &from->unk_18, 0x40);

    if (to->throwSelfEsc == NULL && from->throwSelfEsc != NULL)
    {
        switch (target) {
        case StateTarget::ToGame:
            to->throwSelfEsc = (ThrowSelfEsc*)Game::game_malloc(sizeof(ThrowSelfEsc), 8, *(uint64_t*)Game::internal_heap_2);
            break;
        case StateTarget::ToLocal:
        case StateTarget::Copy:
            to->throwSelfEsc = init_ThrowSelfEsc();
            break;
        }
    }
    if (to->throwSelfEsc != NULL && from->throwSelfEsc == NULL)
    {
        switch (target)
        {
        case StateTarget::ToGame:
            Game::game_free(to->throwSelfEsc);
            break;
        case StateTarget::ToLocal:
        case StateTarget::Copy:
            free_ThrowSelfEsc(to->throwSelfEsc);
            break;
        }
        to->throwSelfEsc = NULL;
    }
    if (to->throwSelfEsc != NULL && from->throwSelfEsc != NULL)
    {
        copy_ThrowSelfEsc(to->throwSelfEsc, from->throwSelfEsc);
    }

    memcpy(&to->unk_60, &from->unk_60, 0x50);
}

EntityThrowAnimationStatus* init_EntityThrowAnimationStatus()
{
    EntityThrowAnimationStatus* local_EntityThrowAnimationStatus = (EntityThrowAnimationStatus*)malloc_(sizeof(EntityThrowAnimationStatus));
    local_EntityThrowAnimationStatus->throwSelfEsc = NULL;
    return local_EntityThrowAnimationStatus;
}

void free_EntityThrowAnimationStatus(EntityThrowAnimationStatus* to)
{
    if (to->throwSelfEsc != NULL)
    {
        free(to->throwSelfEsc);
    }
    free(to);
}


void copy_ThrowSelfEsc(ThrowSelfEsc* to, const ThrowSelfEsc* from)
{
    memcpy(&to->m_cycleTime, &from->m_cycleTime, 0x18);
}

ThrowSelfEsc* init_ThrowSelfEsc()
{
    ThrowSelfEsc* local_ThrowSelfEsc = (ThrowSelfEsc*)malloc_(sizeof(ThrowSelfEsc));
    return local_ThrowSelfEsc;
}

void free_ThrowSelfEsc(ThrowSelfEsc* to)
{
    free(to);
}


void copy_QwcSpEffectEquipCtrl(QwcSpEffectEquipCtrl* to, const QwcSpEffectEquipCtrl* from)
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
    to->unk_24 = from->unk_24;
    to->unk_28 = from->unk_28;
    to->unk_2c = from->unk_2c;
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


void copy_SpecialEffect(SpecialEffect* to, const SpecialEffect* from, StateTarget target)
{
    copy_SpecialEffect_Info(to->specialEffect_Info, from->specialEffect_Info, target);
    to->speffectIsBeingRun = from->speffectIsBeingRun;
    memcpy(to->unk_11, from->unk_11, sizeof(to->unk_11));
    to->unk_14 = from->unk_14;
    to->flags = from->flags;
    to->unk_24 = from->unk_24;
    to->debugActivateSpEffect = from->debugActivateSpEffect;
    to->unk_34 = from->unk_34;
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



void copy_SpecialEffect_Info(SpecialEffect_Info* to, const SpecialEffect_Info* from, StateTarget target)
{
    if (target != StateTarget::ToGame)
    {
        size_t to_index = 0;
        while (from)
        {
            if (to_index >= max_preallocated_SpecialEffect_Info)
            {
                FATALERROR("Unable to recursivly copy SpecialEffect_Info from the game. Out of space.");
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
                //This warning is invalid since game_malloc cannot return null
                #pragma warning(push)
                #pragma warning(disable:6011)
                to->next = (SpecialEffect_Info*)Game::game_malloc(sizeof(SpecialEffect_Info), 8, *(uint64_t*)Game::internal_heap_3);
                to->next->next = NULL;
                to->next->prev = to;
                #pragma warning(pop)
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


void copy_PlayerCtrl(PlayerCtrl* to, const PlayerCtrl* from, StateTarget target)
{
    copy_ChrCtrl(&to->chrCtrl, &from->chrCtrl, target);
    memcpy(&to->unk_300, &from->unk_300, 8);
    copy_TurnAnim(to->turnAnim, from->turnAnim);
    copy_ArrowTurnAnim(to->arrowTurnAnim, from->arrowTurnAnim);
    memcpy(&to->unk_330, &from->unk_330, 8);
    memcpy(&to->movement_related_flags, &from->movement_related_flags, 24);
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


void copy_ArrowTurnAnim(ArrowTurnAnim* to, const ArrowTurnAnim* from)
{
    copy_TurnAnim(&to->turnAnim, &from->turnAnim);
    copy_SpinJoint(to->joint_spine_2, from->joint_spine_2);
    copy_SpinJoint(to->joint_spine1_2, from->joint_spine1_2);
    memcpy(&to->unk_1b8, &from->unk_1b8, 8);
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


void copy_SpinJoint(SpinJoint* to, const SpinJoint* from)
{
    to->unk_8 = from->unk_8;
    memcpy(&to->spin_bone_index, &from->spin_bone_index, 96);
    memcpy(&to->disableUpdate, &from->disableUpdate, 8);
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


void copy_TurnAnim(TurnAnim* to, const TurnAnim* from)
{
    to->unk_8 = from->unk_8;
    memcpy(&to->turnL, &from->turnL, 0x28);
    copy_SpinJoint(to->joint_UpperRoot, from->joint_UpperRoot);
    copy_SpinJoint(to->joint_LowerRoot, from->joint_LowerRoot);
    copy_SpinJoint(to->joint_spine1_1, from->joint_spine1_1);
    copy_SpinJoint(to->joint_spine_1, from->joint_spine_1);
    copy_SpinJoint(to->joint_master, from->joint_master);
    memcpy(&to->unk_70, &from->unk_70, 0x120);
    to->unk_198 = from->unk_198;
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


void copy_ChrCtrl(ChrCtrl* to, const ChrCtrl* from, StateTarget target)
{
    to->unk_8 = from->unk_8;
    copy_ChrCtrl_AnimationQueue(to->animationQueue, from->animationQueue, target);
    copy_AnimationMediator(to->animationMediator, from->animationMediator);
    copy_HavokChara(to->havokChara, from->havokChara, target);
    copy_ActionCtrl(to->actionctrl, from->actionctrl, target);
    to->unk_80 = from->unk_80;
    memcpy(&to->unk_90, &from->unk_90, 0x118);
    copy_WalkAnim_Twist(to->walkAnim_Twist, from->walkAnim_Twist, target);
    memcpy(&to->movement_enabled, &from->movement_enabled, 0xe0);
    memcpy(&to->MapHitDisableDebugging, &from->MapHitDisableDebugging, 0x60);
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


void copy_WalkAnim_Twist(WalkAnim_Twist* to, const WalkAnim_Twist* from, StateTarget target)
{
    to->unk_8 = from->unk_8;
    memcpy(&to->walkF, &from->walkF, 0x1b8);
    memcpy(&to->unk_1d8, &from->unk_1d8, 16);
    copy_SpinJoint(to->Upper_Root_Joint, from->Upper_Root_Joint);
    copy_SpinJoint(to->master_joint, from->master_joint);
    copy_SpinJoint(to->neck_joint, from->neck_joint);
    memcpy(&to->turn_lower_body, &from->turn_lower_body, 0x28);
    copy_WalkAnim_Twist_Field0x228Elem(&to->walkAnim_Twist_Field0x228Elem, &from->walkAnim_Twist_Field0x228Elem, target);
    memcpy(&to->unk_258, &from->unk_258, 0x48);
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


void copy_WalkAnim_Twist_Field0x228Elem(WalkAnim_Twist_Field0x228Elem* to, const WalkAnim_Twist_Field0x228Elem* from, StateTarget target)
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
        if (target == StateTarget::ToGame)
        {
            to->field0x10 = (WalkAnim_Twist_Field0x228Elem_field0x10elem**)(*(falloc*)*(uint64_t*)(*((uint64_t*)to->padding_1[0]) + 0x50))(to->padding_1[0], (to->field0x10_cap) * 8, 8);
            memset(to->field0x10, 0, to->field0x10_cap * 8);
        }
        else
        {
            FATALERROR("Why is the local WalkAnim_Twist_Field0x228Elem->field0x10 null");
        }
    }

    //this array is allocated and also pre-populated with pointers in it's entries
    for (size_t i = 0; i < to->field0x10_cap; i++)
    {
        if (from->field0x10[i] == NULL && to->field0x10[i] != NULL)
        {
            if (target != StateTarget::ToGame)
            {
                free(to->field0x10[i]);
            }
            to->field0x10[i] = NULL;
        }
        if (to->field0x10[i] == NULL && from->field0x10[i] != NULL)
        {
            //need to manually alloc the entry for the game
            if (target == StateTarget::ToGame)
            {
                to->field0x10[i] = (WalkAnim_Twist_Field0x228Elem_field0x10elem*)(*(falloc*)*(uint64_t*)(*((uint64_t*)to->padding_1[0]) + 0x50))(to->padding_1[0], 4 * 4, 4);
            }
            else
            {
                to->field0x10[i] = (WalkAnim_Twist_Field0x228Elem_field0x10elem*)malloc_(sizeof(WalkAnim_Twist_Field0x228Elem_field0x10elem));
            }
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


void copy_WalkAnim_Twist_Field0x228Elem_field0x10elem(WalkAnim_Twist_Field0x228Elem_field0x10elem* to, const WalkAnim_Twist_Field0x228Elem_field0x10elem* from)
{
    to->unk_0 = from->unk_0;
    to->unk_4 = from->unk_4;
    to->unk_8 = from->unk_8;
    to->unk_c = from->unk_c;
}


void copy_ActionCtrl(ActionCtrl* to, const ActionCtrl* from, StateTarget target)
{
    to->unk_8 = from->unk_8;
    copy_ActionCtrl_0x30Substruct(&to->passive_state, &from->passive_state, target);
    copy_ActionCtrl_0x30Substruct(&to->active_state, &from->active_state, target);
    memcpy(&to->ezStatePassiveState, &from->ezStatePassiveState, 0x4d0);
    memcpy(&to->unk_548, &from->unk_548, 0x18);
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


void copy_ActionCtrl_0x30Substruct(ActionCtrl_0x30Substruct* to, const ActionCtrl_0x30Substruct* from, StateTarget target)
{
    copy_EzState_detail_EzStateMachineImpl(to->EzStateMachineImpl, from->EzStateMachineImpl, target);
    memcpy(&to->actionIsValid, &from->actionIsValid, 0x18);
}


void copy_EzState_detail_EzStateMachineImpl(EzState_detail_EzStateMachineImpl* to, const EzState_detail_EzStateMachineImpl* from, StateTarget target)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));

    if (from->unk2 != NULL) { FATALERROR("EzState_detail_EzStateMachineImpl->unk2 = %p, not null", from->unk2); }

    if (from->padding_unk3[1] != NULL) { FATALERROR("EzState_detail_EzStateMachineImpl->field0x38.arry = %p, not null", from->padding_unk3[1]); }

    copy_EzStateRegisterSet(&to->EzStateRegisterSet1, &from->EzStateRegisterSet1, target);

    if (from->unk4 != NULL) { FATALERROR("EzState_detail_EzStateMachineImpl->unk4 = %p, not null", from->unk4); }

    copy_EzStateRegisterSet(&to->EzStateRegisterSet2, &from->EzStateRegisterSet2, target);

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


void copy_EzStateRegisterSet(EzStateRegisterSet* to, const EzStateRegisterSet* from, StateTarget target)
{
    if (target != StateTarget::ToGame && from->arry_cur != NULL && from->arry != NULL)
    {
        size_t arry_size = (from->arry_cur - (uint64_t)from->arry) / sizeof(EzStateRegister);
        if (arry_size != 8)
        {
            FATALERROR("EzStateRegisterSet (%p) has a size of %d, not 8", from, arry_size);
        }
    }
    if (from->arry == NULL)
    {
        FATALERROR("EzStateRegisterSet->arry is null");
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


void copy_HavokChara(HavokChara* to, const HavokChara* from, StateTarget target)
{
    memcpy(&to->RotAngleUnkWep, &from->RotAngleUnkWep, 0x38);
    copy_hkpCharacterProxy(to->char_proxy, from->char_proxy, target);
    //should we copy capsule_shape_1/2 and physShapePhantomIns_1/2?
    memcpy(&to->unk_60, &from->unk_60, 0x98);
    memcpy(&to->unk_100, &from->unk_100, 0xe8);
    memcpy(&to->unk_1f0, &from->unk_1f0, 0x58);
    memcpy(&to->unk_258, &from->unk_258, 0x38);
}

HavokChara* init_HavokChara()
{
    HavokChara* local_HavokChara = (HavokChara*)malloc_(sizeof(HavokChara));

    local_HavokChara->char_proxy = init_hkpCharacterProxy(StateTarget::ToLocal);

    return local_HavokChara;
}

void free_HavokChara(HavokChara* to)
{
    free_hkpCharacterProxy(to->char_proxy);
    free_HitIns(to->padding_hitIns);

    free(to);
}

//CharacterProxy is not in hkpWorld, it's handled by game code. So we can save/restore it from the Chr
void copy_hkpCharacterProxy(hkpCharacterProxy* to, const hkpCharacterProxy* from, StateTarget target)
{
    to->unk_8 = from->unk_8;
    to->unk_c = from->unk_c;
    memcpy(&to->m_velocity, &from->m_velocity, 0x20);

    //since the phantoms are never destroyed due to our graveyard mechanism, it's safe to just use the raw pointer. it should always be valid
    to->HkpSimpleShapePhantom = from->HkpSimpleShapePhantom;

    memcpy(&to->m_dynamicFriction, &from->m_dynamicFriction, 0x40);
    memcpy(&to->m_maxSlopeCosine, &from->m_maxSlopeCosine, 24);
}

hkpCharacterProxy* init_hkpCharacterProxy(StateTarget target)
{
    if (target != StateTarget::ToGame)
    {
        hkpCharacterProxy* local_hkpCharacterProxy = (hkpCharacterProxy*)malloc_(sizeof(hkpCharacterProxy));
        return local_hkpCharacterProxy;
    }
    else
    {
        FATALERROR("in-game alloc hkpCharacterProxy");
    }
}

void free_hkpCharacterProxy(hkpCharacterProxy* to)
{
    free(to);
}


void copy_HitIns(HitIns* to, const HitIns* from)
{
    if (to == NULL || from == NULL)
    {
        return;
    }
    to->unk_8 = from->unk_8;
    to->unk_c = from->unk_c;
    memcpy(&to->unk_18, &from->unk_18, 0x48);
    to->unk_70 = from->unk_70;
    to->unk_72 = from->unk_72;
    to->unk_74 = from->unk_74;
    to->unk_98 = from->unk_98;
    to->unk_9a = from->unk_9a;
    to->unk_9c = from->unk_9c;
    to->unk_b0 = from->unk_b0;
    to->BackReadState = from->BackReadState;
    to->TargetBackreadState = from->TargetBackreadState;
    memcpy(to->unk_b3, from->unk_b3, sizeof(to->unk_b3));
    to->IsDispHitRigid = from->IsDispHitRigid;
    memcpy(to->unk_c1, from->unk_c1, sizeof(to->unk_c1));
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


void copy_AnimationMediator(AnimationMediator* to, const AnimationMediator* from)
{
    for (int i = 0; i < 31; i++)
    {
        copy_AnimationMediatorStateEntry(&to->states_list[i], &from->states_list[i]);
    }
    copy_AnimationQueue(to->animationQueue, from->animationQueue);
    memcpy(&to->unk_1468, &from->unk_1468, 0x28);
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


void copy_AnimationMediatorStateEntry(AnimationMediatorStateEntry* to, const AnimationMediatorStateEntry* from)
{
    memcpy(&to->animationId, &from->animationId, 0x90);
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


void copy_ChrCtrl_AnimationQueue(ChrCtrl_AnimationQueue* to, const ChrCtrl_AnimationQueue* from, StateTarget target)
{
    //we allow up to a max of 32 ChrCtrl_AnimationQueueEntry entries in the array
    if (from->array_length > 32)
    {
        FATALERROR("Got %d number of ChrCtrl_AnimationQueueEntry entries for ChrCtrl_AnimationQueue->arry. Only support a max of 32.", from->array_length);
    }
    to->array_length = from->array_length;
    to->unk_4 = from->unk_4;
    for (size_t i = 0; i < from->array_length; i++)
    {
        copy_ChrCtrl_AnimationQueueEntry(&to->arry[i], &from->arry[i], target);
    }

    copy_ChrCtrl_AnimationQueue_field0x10(to->field0x10, from->field0x10);
    copy_hkaAnimatedSkeleton(to->HkaAnimatedSkeleton, from->HkaAnimatedSkeleton);
    copy_ChrCtrl_AnimationQueue_field0x20(to->field0x20, from->field0x20, target);
    memcpy(&to->genderSpecificAnimationOffset, &from->genderSpecificAnimationOffset, 0x10);
    memcpy(&to->unk_70, &from->unk_70, 0x10);
    to->unk_88 = from->unk_88;
    memcpy(&to->unk_a8, &from->unk_a8, 0x20);
    memcpy(&to->unk_d8, &from->unk_d8, 0x18);
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


void copy_ChrCtrl_AnimationQueue_field0x20(ChrCtrl_AnimationQueue_field0x20* to, const ChrCtrl_AnimationQueue_field0x20* from, StateTarget target)
{
    if (target != StateTarget::ToGame && from->padding_0 != NULL)
    {
        uint32_t field0x28_len = *(uint32_t*)(from->padding_0 + 0x30);
        if (field0x28_len != 61)
        {
            FATALERROR("ChrCtrl_AnimationQueue_field0x20 arrays are of size %d. Expected 61.", field0x28_len);
        }
    }
    memcpy(to->field0x8, from->field0x8, 0x30 * 64);
    memcpy(&to->field0x8_len, &from->field0x8_len, 8);
    memcpy(to->field0x18, from->field0x18, 0x30 * 64);
    memcpy(&to->field0x18_len, &from->field0x18_len, 8);
    memcpy(to->field0x28, from->field0x28, 4 * 64);
    memcpy(&to->field0x28_len, &from->field0x28_len, 0x10);
    memcpy(&to->unk_48, &from->unk_48, 8);
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

void copy_hkaAnimatedSkeleton(hkaAnimatedSkeleton* to, const hkaAnimatedSkeleton* from)
{
    to->unk_8 = from->unk_8;
    to->unk_c = from->unk_c;
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
    to->unk_38 = from->unk_38;
    to->unk_3c = from->unk_3c;
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

void copy_hkaDefaultAnimationControl(hkaDefaultAnimationControl* to, const hkaDefaultAnimationControl* from)
{
    copy_hkaAnimationControl(&to->HkaAnimationControl, &from->HkaAnimationControl);
    memcpy(&to->weight, &from->weight, 0x50);
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

void copy_hkaAnimationControl(hkaAnimationControl* to, const hkaAnimationControl* from)
{
    to->unk_8 = from->unk_8;
    to->unk_c = from->unk_c;
    to->curTimeInAnimation = from->curTimeInAnimation;
    to->weight = from->weight;

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

    to->unk_48 = from->unk_48;
    to->unk_4c = from->unk_4c;
    to->unk_50 = from->unk_50;
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


void copy_ChrCtrl_AnimationQueue_field0x10(ChrCtrl_AnimationQueue_field0x10* to, const ChrCtrl_AnimationQueue_field0x10* from)
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


void copy_ChrCtrl_AnimationQueueEntry(ChrCtrl_AnimationQueueEntry* to, const ChrCtrl_AnimationQueueEntry* from, StateTarget target)
{
    memcpy(&to->unk_0, &from->unk_0, 8);
    copy_hkaDefaultAnimationControl(to->defaultAnimationControl, from->defaultAnimationControl);
    to->HvkAnim_AnimInfoArrayElem = from->HvkAnim_AnimInfoArrayElem;
    memcpy(&to->unk_28, &from->unk_28, 0x50);
}

ChrCtrl_AnimationQueueEntry* init_ChrCtrl_AnimationQueueEntry()
{
    ChrCtrl_AnimationQueueEntry* local_AnimationQueueEntry = (ChrCtrl_AnimationQueueEntry*)malloc_(sizeof(ChrCtrl_AnimationQueueEntry));

    local_AnimationQueueEntry->defaultAnimationControl = init_hkaDefaultAnimationControl();

    return local_AnimationQueueEntry;
}

void free_ChrCtrl_AnimationQueueEntry(ChrCtrl_AnimationQueueEntry* to, bool freeself)
{
    free_hkaDefaultAnimationControl(to->defaultAnimationControl);

    if (freeself)
    {
        free(to);
    }
}


void copy_AnimationQueue(AnimationQueue* to, const AnimationQueue* from)
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


void copy_AnimationQueue_Entry(AnimationQueue_Entry* to, const AnimationQueue_Entry* from)
{
    memcpy(&to->unk_0, &from->unk_0, 8);
    memcpy(to->field0x10, from->field0x10, sizeof(to->field0x10));
    copy_AnimationQueue_Entry_sub1(&to->sub1, &from->sub1);
    memcpy(&to->unk_140, &from->unk_140, 8);

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

    memcpy(&to->unk_168, &from->unk_168, 0x18);
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


void copy_AnimationQueue_Entry_sub1(AnimationQueue_Entry_sub1* to, const AnimationQueue_Entry_sub1* from)
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


void copy_AnimationQueue_Entry_sub1_field0x10(AnimationQueue_Entry_sub1_field0x10* to, const AnimationQueue_Entry_sub1_field0x10* from)
{
    to->parent_AnimationQueue_Entry_field0x10 = from->parent_AnimationQueue_Entry_field0x10;
    to->unk_8 = from->unk_8;
}

// ---- forward declarations (tree has no cycles; fwd-decl avoids ordering) -----
static void serialize_ChrAsm(StateVisitor&, const ChrAsm*);
static void serialize_ChrAsmModel(StateVisitor&, const ChrAsmModel*);
static void serialize_ProEquipCtrl(StateVisitor&, const ProEquipCtrl*);
static void serialize_WeaponEquipCtrl(StateVisitor&, const WeaponEquipCtrl*);
static void serialize_RingEquipCtrl(StateVisitor&, const RingEquipCtrl*);
static void serialize_PlayerGameData(StateVisitor&, const PlayerGameData*);
static void serialize_EquipGameData(StateVisitor&, const EquipGameData*);
static void serialize_EquipItemData(StateVisitor&, const EquipItemData*);
static void serialize_EquipMagicData(StateVisitor&, const EquipMagicData*);
static void serialize_ChrIns(StateVisitor&, const ChrIns*);
static void serialize_ChrAttachSys(StateVisitor&, const ChrAttachSys*);
static void serialize_ChrIns_field0x2c8(StateVisitor&, const ChrIns_field0x2c8*);
static void serialize_EntityThrowAnimationStatus(StateVisitor&, const EntityThrowAnimationStatus*);
static void serialize_ThrowSelfEsc(StateVisitor&, const ThrowSelfEsc*);
static void serialize_QwcSpEffectEquipCtrl(StateVisitor&, const QwcSpEffectEquipCtrl*);
static void serialize_SpecialEffect(StateVisitor&, const SpecialEffect*);
static void serialize_SpecialEffect_Info(StateVisitor&, const SpecialEffect_Info*);
static void serialize_PlayerCtrl(StateVisitor&, const PlayerCtrl*);
static void serialize_ArrowTurnAnim(StateVisitor&, const ArrowTurnAnim*);
static void serialize_SpinJoint(StateVisitor&, const SpinJoint*);
static void serialize_TurnAnim(StateVisitor&, const TurnAnim*);
static void serialize_ChrCtrl(StateVisitor&, const ChrCtrl*);
static void serialize_WalkAnim_Twist(StateVisitor&, const WalkAnim_Twist*);
static void serialize_WalkAnim_Twist_Field0x228Elem(StateVisitor&, const WalkAnim_Twist_Field0x228Elem*);
static void serialize_WalkAnim_Twist_Field0x228Elem_field0x10elem(StateVisitor&, const WalkAnim_Twist_Field0x228Elem_field0x10elem*);
static void serialize_ActionCtrl(StateVisitor&, const ActionCtrl*);
static void serialize_ActionCtrl_0x30Substruct(StateVisitor&, const ActionCtrl_0x30Substruct*);
static void serialize_EzState_detail_EzStateMachineImpl(StateVisitor&, const EzState_detail_EzStateMachineImpl*);
static void serialize_EzStateRegisterSet(StateVisitor&, const EzStateRegisterSet*);
static void serialize_HavokChara(StateVisitor&, const HavokChara*);
static void serialize_hkpCharacterProxy(StateVisitor&, const hkpCharacterProxy*);
static void serialize_HitIns(StateVisitor&, const HitIns*);
static void serialize_AnimationMediator(StateVisitor&, const AnimationMediator*);
static void serialize_AnimationMediatorStateEntry(StateVisitor&, const AnimationMediatorStateEntry*);
static void serialize_ChrCtrl_AnimationQueue(StateVisitor&, const ChrCtrl_AnimationQueue*);
static void serialize_ChrCtrl_AnimationQueue_field0x20(StateVisitor&, const ChrCtrl_AnimationQueue_field0x20*);
static void serialize_hkaAnimatedSkeleton(StateVisitor&, const hkaAnimatedSkeleton*);
static void serialize_hkaDefaultAnimationControl(StateVisitor&, const hkaDefaultAnimationControl*);
static void serialize_hkaAnimationControl(StateVisitor&, const hkaAnimationControl*);
static void serialize_ChrCtrl_AnimationQueue_field0x10(StateVisitor&, const ChrCtrl_AnimationQueue_field0x10*);
static void serialize_ChrCtrl_AnimationQueueEntry(StateVisitor&, const ChrCtrl_AnimationQueueEntry*);
static void serialize_AnimationQueue(StateVisitor&, const AnimationQueue*);
static void serialize_AnimationQueue_Entry(StateVisitor&, const AnimationQueue_Entry*);
static void serialize_AnimationQueue_Entry_sub1(StateVisitor&, const AnimationQueue_Entry_sub1*);
static void serialize_AnimationQueue_Entry_sub1_field0x10(StateVisitor&, const AnimationQueue_Entry_sub1_field0x10*);

// ---- leaf / small structs ---------------------------------------------------

static void serialize_ChrAsm(StateVisitor& v, const ChrAsm* a)
{
    v.begin("ChrAsm");
    v.field("equipped_weapon_style", a->equipped_weapon_style);
    v.field("l_hand_equipped_index", a->l_hand_equipped_index);
    v.field("r_hand_equipped_index", a->r_hand_equipped_index);
    v.field("l_arrow_equipped_index", a->l_arrow_equipped_index);
    v.field("r_arrow_equipped_index", a->r_arrow_equipped_index);
    v.field("l_bolt_equipped_index", a->l_bolt_equipped_index);
    v.field("r_bolt_equipped_index", a->r_bolt_equipped_index);
    v.blob("equip_items", a->equip_items, sizeof(a->equip_items));
    v.field("transformProtectorId", a->transformProtectorId);
    v.field("transformProtectorId_HalfCamo", a->transformProtectorId_HalfCamo);
    v.field("unk_7c", a->unk_7c);
    v.end();
}

static void serialize_ChrAsmModel(StateVisitor& v, const ChrAsmModel* m)
{
    v.begin("ChrAsmModel");
    v.blob("unk_8", &m->unk_8, 0x10);
    v.blob("fieldE0", m->fieldE0, sizeof(m->fieldE0));
    v.blob("unk_68", &m->unk_68, 0x18);
    v.field("data_3a", m->data_3a);
    v.blob("unk_a0", &m->unk_a0, 0x28);
    v.blob("hairColor", &m->hairColor, 0x80);
    v.blob("filecap_array", m->filecap_array, sizeof(m->filecap_array));
    v.field("unk_1c8", m->unk_1c8);
    v.blob("unk_1c9", m->unk_1c9, sizeof(m->unk_1c9));
    v.end();
}

static void serialize_ProEquipCtrl(StateVisitor& v, const ProEquipCtrl* p)
{
    v.begin("ProEquipCtrl");
    serialize_SpecialEffect(v, p->spEffectList);
    v.field("array_len", p->array_len);
    for (int i = 0; i < 5; i++) v.field("equipped_armors_ids", p->equipped_armors_ids[i]);
    v.end();
}

static void serialize_WeaponEquipCtrl(StateVisitor& v, const WeaponEquipCtrl* p)
{
    v.begin("WeaponEquipCtrl");
    serialize_SpecialEffect(v, p->spEffectList);
    v.field("array_len", p->array_len);
    for (int i = 0; i < 2; i++) v.field("equipped_weapons_ids", p->equipped_weapons_ids[i]);
    v.end();
}

static void serialize_RingEquipCtrl(StateVisitor& v, const RingEquipCtrl* p)
{
    v.begin("RingEquipCtrl");
    serialize_SpecialEffect(v, p->spEffectList);
    v.field("array_len", p->array_len);
    for (int i = 0; i < 2; i++) v.field("equipped_rings_ids", p->equipped_rings_ids[i]);
    v.end();
}

static void serialize_EquipItemData(StateVisitor& v, const EquipItemData* e)
{
    v.begin("EquipItemData");
    v.blob("quickbar", e->quickbar, sizeof(e->quickbar));
    v.field("selectedQuickbarItem", e->selectedQuickbarItem);
    v.end();
}

static void serialize_EquipMagicData(StateVisitor& v, const EquipMagicData* e)
{
    v.begin("EquipMagicData");
    v.blob("equippedMagicList", e->equippedMagicList, sizeof(e->equippedMagicList));
    v.field("curSelectedMagicSlot", e->curSelectedMagicSlot);
    v.end();
}

static void serialize_EquipGameData(StateVisitor& v, const EquipGameData* e)
{
    v.begin("EquipGameData");
    v.blob("EquipItemToInventoryIndexMap", e->EquipItemToInventoryIndexMap, sizeof(e->EquipItemToInventoryIndexMap));
    v.blob("EquipItemToInventoryIndexMap_index_updated", e->EquipItemToInventoryIndexMap_index_updated, sizeof(e->EquipItemToInventoryIndexMap_index_updated));
    serialize_ChrAsm(v, &e->chrasm);
    serialize_EquipMagicData(v, e->equipMagicData);
    serialize_EquipItemData(v, &e->equippedItemsInQuickbar);
    v.field("amountOfItemUsedFromInventory", e->amountOfItemUsedFromInventory);
    v.field("itemInventoryIdCurrentlyBeingUsedFromInventory", e->itemInventoryIdCurrentlyBeingUsedFromInventory);
    v.end();
}

// copy_PlayerGameData_AttributeInfo / _ChrProperties memcpy the whole struct, so
// these blob it for full, bit-exact coverage (their dedicated prints delegate here).
static void serialize_PlayerGameData_AttributeInfo(StateVisitor& v, const PlayerGameData_AttributeInfo* a)
{
    v.begin("PlayerGameData_AttributeInfo");
    v.blob("AttributeInfo", a, sizeof(*a));
    v.end();
}

static void serialize_PlayerGameData_ChrProperties(StateVisitor& v, const PlayerGameData_ChrProperties* c)
{
    v.begin("PlayerGameData_ChrProperties");
    v.blob("ChrProperties", c, sizeof(*c));
    v.end();
}

// These are NOT part of the rollback snapshot (copy_PlayerIns skips chrAsmModelRes;
// copy_ChrIns skips field0x18). Provided only so their existing debug print_X can
// delegate to a serializer; not called from serialize_PlayerIns.
static void serialize_ChrAsmModelRes_Elem(StateVisitor& v, const ChrAsmModelRes_Elem* e)
{
    v.begin("ChrAsmModelRes_Elem");
    v.field("curModelId", e->curModelId);
    v.field("newModelId", e->newModelId);
    v.end();
}

static void serialize_ChrAsmModelRes(StateVisitor& v, const ChrAsmModelRes* m)
{
    v.begin("ChrAsmModelRes");
    v.field("magic_state", m->magic_state);
    for (int i = 0; i < 14; i++) serialize_ChrAsmModelRes_Elem(v, &m->arry[i]);
    v.end();
}

static void serialize_ChrIns_field0x18(StateVisitor& v, const ChrIns_field0x18* f)
{
    v.begin("ChrIns_field0x18");
    v.field("throw_animId", f->throw_animId);
    v.field("animId", f->animId);
    v.field("IsEnableAnimLoop", f->IsEnableAnimLoop);
    v.end();
}

static void serialize_PlayerGameData(StateVisitor& v, const PlayerGameData* p)
{
    v.begin("PlayerGameData");
    serialize_PlayerGameData_AttributeInfo(v, &p->attribs);
    serialize_EquipGameData(v, &p->equipGameData);
    serialize_PlayerGameData_ChrProperties(v, &p->ChrProperties);
    v.end();
}

static void serialize_ThrowSelfEsc(StateVisitor& v, const ThrowSelfEsc* t)
{
    v.begin("ThrowSelfEsc");
    v.blob("m_cycleTime", &t->m_cycleTime, 0x18);   // copy memcpy's 0x18 from m_cycleTime
    v.end();
}

static void serialize_EntityThrowAnimationStatus(StateVisitor& v, const EntityThrowAnimationStatus* e)
{
    v.begin("EntityThrowAnimationStatus");
    v.ptr_flag("playerins_parent", (void*)e->playerins_parent);   // heap parent ptr
    v.ptr_flag("throw_paramdef", (void*)e->throw_paramdef);       // loaded-param ptr
    v.blob("unk_18", &e->unk_18, 0x40);
    if (e->throwSelfEsc)
    {
        v.field("throwSelfEsc_present", true);
        serialize_ThrowSelfEsc(v, e->throwSelfEsc);
    }
    else
    {
        v.field("throwSelfEsc_present", false);
    }
    v.blob("unk_60", &e->unk_60, 0x50);
    v.end();
}

static void serialize_QwcSpEffectEquipCtrl(StateVisitor& v, const QwcSpEffectEquipCtrl* q)
{
    v.begin("QwcSpEffectEquipCtrl");
    v.count("arry", q->arry_len);
    for (size_t i = 0; i < q->arry_len; i++) v.field("arry", q->arry[i]);
    v.field("unk_24", q->unk_24);
    v.field("unk_28", q->unk_28);
    v.field("unk_2c", q->unk_2c);
    v.end();
}

static void serialize_SpecialEffect_Info(StateVisitor& v, const SpecialEffect_Info* s)
{
    // linked list; walk via ->next (capped) mirroring copy_SpecialEffect_Info
    v.begin("SpecialEffect_Info");
    size_t n = 0;
    const SpecialEffect_Info* cur = s;
    while (cur && n < max_preallocated_SpecialEffect_Info)
    {
        v.blob("data_0", cur->data_0, sizeof(cur->data_0));
        v.ptr_flag("paramRowBytes", (void*)cur->paramRowBytes);   // loaded-param ptr
        cur = cur->next;
        n++;
    }
    v.count("nodes", n);
    v.end();
}

static void serialize_SpecialEffect(StateVisitor& v, const SpecialEffect* s)
{
    v.begin("SpecialEffect");
    serialize_SpecialEffect_Info(v, s->specialEffect_Info);
    v.field("speffectIsBeingRun", s->speffectIsBeingRun);
    v.blob("unk_11", s->unk_11, sizeof(s->unk_11));
    v.field("unk_14", s->unk_14);
    v.field("flags", s->flags);
    v.field("unk_24", s->unk_24);
    v.field("debugActivateSpEffect", s->debugActivateSpEffect);
    v.field("unk_34", s->unk_34);
    v.end();
}

static void serialize_ChrAttachSys(StateVisitor& v, const ChrAttachSys* c)
{
    // Shallow per the harness design: presence + slot type only. Full AttachSysSlot
    // expansion is deferred (see rollback-test-harness memo). copy_ChrAttachSys
    // rebuilds the whole slot chain; if attach state ever desyncs, add a
    // serialize_AttachSysSlot in the AttachSysSlot module and recurse here.
    v.begin("ChrAttachSys");
    if (c->SysSlots)
    {
        v.field("SysSlots_present", true);
        v.field("slotType", c->SysSlots->slotType);
    }
    else
    {
        v.field("SysSlots_present", false);
    }
    v.end();
}

static void serialize_ChrIns_field0x2c8(StateVisitor& v, const ChrIns_field0x2c8* f)
{
    v.begin("ChrIns_field0x2c8");
    v.blob("unk_8", &f->unk_8, 0x18);
    v.field("unk_28", f->unk_28);
    v.field("unk_2c", f->unk_2c);
    v.blob("unk_38", &f->unk_38, 0x10);
    v.field("unk_50", f->unk_50);
    v.end();
}

static void serialize_HitIns(StateVisitor& v, const HitIns* h)
{
    if (h == NULL) { v.field("HitIns_null", true); return; }
    v.begin("HitIns");
    v.field("unk_8", h->unk_8);
    v.field("unk_c", h->unk_c);
    v.blob("unk_18", &h->unk_18, 0x48);
    v.field("unk_70", h->unk_70);
    v.field("unk_72", h->unk_72);
    v.field("unk_74", h->unk_74);
    v.field("unk_98", h->unk_98);
    v.field("unk_9a", h->unk_9a);
    v.field("unk_9c", h->unk_9c);
    v.field("unk_b0", h->unk_b0);
    v.field("BackReadState", h->BackReadState);
    v.field("TargetBackreadState", h->TargetBackreadState);
    v.blob("unk_b3", h->unk_b3, sizeof(h->unk_b3));
    v.field("IsDispHitRigid", h->IsDispHitRigid);
    v.blob("unk_c1", h->unk_c1, sizeof(h->unk_c1));
    v.end();
}

static void serialize_SpinJoint(StateVisitor& v, const SpinJoint* s)
{
    v.begin("SpinJoint");
    v.field("unk_8", s->unk_8);
    v.blob("spin_bone_index", &s->spin_bone_index, 96);
    v.blob("disableUpdate", &s->disableUpdate, 8);
    v.end();
}

static void serialize_TurnAnim(StateVisitor& v, const TurnAnim* t)
{
    v.begin("TurnAnim");
    v.field("unk_8", t->unk_8);
    v.blob("turnL", &t->turnL, 0x28);
    serialize_SpinJoint(v, t->joint_UpperRoot);
    serialize_SpinJoint(v, t->joint_LowerRoot);
    serialize_SpinJoint(v, t->joint_spine1_1);
    serialize_SpinJoint(v, t->joint_spine_1);
    serialize_SpinJoint(v, t->joint_master);
    v.blob("unk_70", &t->unk_70, 0x120);
    v.field("unk_198", t->unk_198);
    v.end();
}

static void serialize_ArrowTurnAnim(StateVisitor& v, const ArrowTurnAnim* a)
{
    v.begin("ArrowTurnAnim");
    serialize_TurnAnim(v, &a->turnAnim);
    serialize_SpinJoint(v, a->joint_spine_2);
    serialize_SpinJoint(v, a->joint_spine1_2);
    v.blob("unk_1b8", &a->unk_1b8, 8);
    v.end();
}

static void serialize_hkpCharacterProxy(StateVisitor& v, const hkpCharacterProxy* h)
{
    v.begin("hkpCharacterProxy");
    v.field("unk_8", h->unk_8);
    v.field("unk_c", h->unk_c);
    v.blob("m_velocity", &h->m_velocity, 0x20);
    v.ptr_flag("HkpSimpleShapePhantom", h->HkpSimpleShapePhantom);   // heap phantom ptr
    v.blob("m_dynamicFriction", &h->m_dynamicFriction, 0x40);
    v.blob("m_maxSlopeCosine", &h->m_maxSlopeCosine, 24);
    v.end();
}

static void serialize_HavokChara(StateVisitor& v, const HavokChara* h)
{
    v.begin("HavokChara");
    v.blob("RotAngleUnkWep", &h->RotAngleUnkWep, 0x38);
    serialize_hkpCharacterProxy(v, h->char_proxy);
    v.blob("unk_60", &h->unk_60, 0x98);
    v.blob("unk_100", &h->unk_100, 0xe8);
    v.blob("unk_1f0", &h->unk_1f0, 0x58);
    v.blob("unk_258", &h->unk_258, 0x38);
    v.end();
}

// ---- EzState / ActionCtrl ---------------------------------------------------

static void serialize_EzStateRegisterSet(StateVisitor& v, const EzStateRegisterSet* e)
{
    v.begin("EzStateRegisterSet");
    for (size_t i = 0; i < 8; i++)
    {
        v.blob("register", e->arry[i].data_0, sizeof(EzStateRegister));
    }
    v.end();
}

static void serialize_EzState_detail_EzStateMachineImpl(StateVisitor& v, const EzState_detail_EzStateMachineImpl* e)
{
    v.begin("EzStateMachineImpl");
    v.blob("data_0", e->data_0, sizeof(e->data_0));
    serialize_EzStateRegisterSet(v, &e->EzStateRegisterSet1);
    serialize_EzStateRegisterSet(v, &e->EzStateRegisterSet2);
    v.blob("data_1", e->data_1, sizeof(e->data_1));
    v.blob("data_2", e->data_2, sizeof(e->data_2));
    v.end();
}

static void serialize_ActionCtrl_0x30Substruct(StateVisitor& v, const ActionCtrl_0x30Substruct* a)
{
    v.begin("ActionCtrl_0x30Substruct");
    serialize_EzState_detail_EzStateMachineImpl(v, a->EzStateMachineImpl);
    v.blob("actionIsValid", &a->actionIsValid, 0x18);
    v.end();
}

static void serialize_ActionCtrl(StateVisitor& v, const ActionCtrl* a)
{
    v.begin("ActionCtrl");
    v.field("unk_8", a->unk_8);
    serialize_ActionCtrl_0x30Substruct(v, &a->passive_state);
    serialize_ActionCtrl_0x30Substruct(v, &a->active_state);
    v.blob("ezStatePassiveState_region", &a->ezStatePassiveState, 0x4d0);
    v.blob("unk_548", &a->unk_548, 0x18);
    v.end();
}

// ---- animation chain --------------------------------------------------------

static void serialize_hkaAnimationControl(StateVisitor& v, const hkaAnimationControl* h)
{
    v.begin("hkaAnimationControl");
    v.field("unk_8", h->unk_8);
    v.field("unk_c", h->unk_c);
    v.field("curTimeInAnimation", h->curTimeInAnimation);
    v.field("weight", h->weight);
    v.field("field0x18_len", h->field0x18_len);
    v.field("field0x18_cap", h->field0x18_cap);
    v.blob("field0x18", h->field0x18, h->field0x18_len);   // copy memcpy's _len bytes
    v.field("field0x28_len", h->field0x28_len);
    v.field("field0x28_cap", h->field0x28_cap);
    v.blob("field0x28", h->field0x28, h->field0x28_len);
    v.ptr_flag("HkaAnimationBinding", (void*)h->HkaAnimationBinding);   // loaded anim resource
    v.field("unk_48", h->unk_48);
    v.field("unk_4c", h->unk_4c);
    v.field("unk_50", h->unk_50);
    v.end();
}

static void serialize_hkaDefaultAnimationControl(StateVisitor& v, const hkaDefaultAnimationControl* h)
{
    v.begin("hkaDefaultAnimationControl");
    serialize_hkaAnimationControl(v, &h->HkaAnimationControl);
    v.blob("weight_region", &h->weight, 0x50);
    v.end();
}

static void serialize_hkaAnimatedSkeleton(StateVisitor& v, const hkaAnimatedSkeleton* h)
{
    v.begin("hkaAnimatedSkeleton");
    v.field("unk_8", h->unk_8);
    v.field("unk_c", h->unk_c);
    v.count("animCtrl_list", h->animCtrl_list_len);
    for (uint32_t i = 0; i < h->animCtrl_list_len; i++)
    {
        serialize_hkaDefaultAnimationControl(v, h->animCtrl_list[i]);
    }
    v.field("data_1", h->data_1);
    v.field("unk_38", h->unk_38);
    v.field("unk_3c", h->unk_3c);
    v.end();
}

static void serialize_ChrCtrl_AnimationQueue_field0x20(StateVisitor& v, const ChrCtrl_AnimationQueue_field0x20* f)
{
    v.begin("ChrCtrl_AnimationQueue_field0x20");
    v.blob("field0x8", f->field0x8, 0x30 * 64);
    v.blob("field0x8_lens", &f->field0x8_len, 8);
    v.blob("field0x18", f->field0x18, 0x30 * 64);
    v.blob("field0x18_lens", &f->field0x18_len, 8);
    v.blob("field0x28", f->field0x28, 4 * 64);
    v.blob("field0x28_lens", &f->field0x28_len, 0x10);
    v.blob("unk_48", &f->unk_48, 8);
    v.end();
}

static void serialize_ChrCtrl_AnimationQueue_field0x10(StateVisitor& v, const ChrCtrl_AnimationQueue_field0x10* f)
{
    v.begin("ChrCtrl_AnimationQueue_field0x10");
    v.field("array1_len", f->array1_len);
    v.field("array2_len", f->array2_len);
    v.blob("arry2", f->arry2, sizeof(ChrCtrl_AnimationQueue_field0x10_field0x10arrayelem) * f->array2_len);
    for (size_t i = 0; i < f->array1_len; i++)
    {
        v.ptr_index("arry1", f->arry1[i], f->arry2, sizeof(ChrCtrl_AnimationQueue_field0x10_field0x10arrayelem));
    }
    v.end();
}

static void serialize_ChrCtrl_AnimationQueueEntry(StateVisitor& v, const ChrCtrl_AnimationQueueEntry* e)
{
    v.begin("ChrCtrl_AnimationQueueEntry");
    v.blob("unk_0", &e->unk_0, 8);
    serialize_hkaDefaultAnimationControl(v, e->defaultAnimationControl);
    v.ptr_flag("HvkAnim_AnimInfoArrayElem", (void*)e->HvkAnim_AnimInfoArrayElem);   // loaded animbnd
    v.blob("unk_28", &e->unk_28, 0x50);
    v.end();
}

static void serialize_ChrCtrl_AnimationQueue(StateVisitor& v, const ChrCtrl_AnimationQueue* q)
{
    v.begin("ChrCtrl_AnimationQueue");
    v.field("array_length", q->array_length);
    v.field("unk_4", q->unk_4);
    v.count("arry", q->array_length);
    for (size_t i = 0; i < q->array_length; i++)
    {
        serialize_ChrCtrl_AnimationQueueEntry(v, &q->arry[i]);
    }
    serialize_ChrCtrl_AnimationQueue_field0x10(v, q->field0x10);
    serialize_hkaAnimatedSkeleton(v, q->HkaAnimatedSkeleton);
    serialize_ChrCtrl_AnimationQueue_field0x20(v, q->field0x20);
    v.blob("genderSpecificAnimationOffset_region", &q->genderSpecificAnimationOffset, 0x10);
    v.blob("unk_70_region", &q->unk_70, 0x10);
    v.field("unk_88", q->unk_88);
    v.blob("unk_a8", &q->unk_a8, 0x20);
    v.blob("unk_d8", &q->unk_d8, 0x18);
    v.end();
}

static void serialize_AnimationMediatorStateEntry(StateVisitor& v, const AnimationMediatorStateEntry* e)
{
    v.begin("AnimationMediatorStateEntry");
    v.blob("animationId_region", &e->animationId, 0x90);
    v.blob("data_1", e->data_1, sizeof(e->data_1));
    v.end();
}

static void serialize_AnimationQueue_Entry_sub1_field0x10(StateVisitor& v, const AnimationQueue_Entry_sub1_field0x10* f)
{
    v.begin("sub1_field0x10");
    v.field("parent_AnimationQueue_Entry_field0x10", f->parent_AnimationQueue_Entry_field0x10);
    v.field("unk_8", f->unk_8);
    v.end();
}

static void serialize_AnimationQueue_Entry_sub1(StateVisitor& v, const AnimationQueue_Entry_sub1* s)
{
    v.begin("AnimationQueue_Entry_sub1");
    v.field("field0x10_cap", s->field0x10_cap);
    v.field("unk", s->unk);
    v.field("field0x10_len", s->field0x10_len);
    for (size_t i = 0; i < s->field0x10_len; i++)
    {
        if (s->field0x10[i] != NULL)
        {
            serialize_AnimationQueue_Entry_sub1_field0x10(v, s->field0x10[i]);
        }
    }
    v.end();
}

static void serialize_AnimationQueue_Entry(StateVisitor& v, const AnimationQueue_Entry* e)
{
    v.begin("AnimationQueue_Entry");
    v.blob("unk_0", &e->unk_0, 8);
    v.blob("field0x10", e->field0x10, sizeof(e->field0x10));
    serialize_AnimationQueue_Entry_sub1(v, &e->sub1);
    v.blob("unk_140", &e->unk_140, 8);
    size_t len = ((uint64_t)e->chained_animations_array_end - (uint64_t)e->chained_animations_array_start) / 8;
    v.count("chained_animations_array", len);
    for (size_t i = 0; i < len; i++) v.field("chained", e->chained_animations_array_start[i]);
    v.blob("unk_168", &e->unk_168, 0x18);
    v.end();
}

static void serialize_AnimationQueue(StateVisitor& v, const AnimationQueue* q)
{
    v.begin("AnimationQueue");
    for (int i = 0; i < 6; i++) serialize_AnimationQueue_Entry(v, q->AnimationQueue_Entries[i]);
    v.end();
}

static void serialize_AnimationMediator(StateVisitor& v, const AnimationMediator* m)
{
    v.begin("AnimationMediator");
    for (int i = 0; i < 31; i++) serialize_AnimationMediatorStateEntry(v, &m->states_list[i]);
    serialize_AnimationQueue(v, m->animationQueue);
    v.blob("unk_1468", &m->unk_1468, 0x28);
    v.end();
}

// ---- WalkAnim_Twist ---------------------------------------------------------

static void serialize_WalkAnim_Twist_Field0x228Elem_field0x10elem(StateVisitor& v, const WalkAnim_Twist_Field0x228Elem_field0x10elem* e)
{
    v.begin("Field0x228Elem_field0x10elem");
    v.field("unk_0", e->unk_0);
    v.field("unk_4", e->unk_4);
    v.field("unk_8", e->unk_8);
    v.field("unk_c", e->unk_c);
    v.end();
}

static void serialize_WalkAnim_Twist_Field0x228Elem(StateVisitor& v, const WalkAnim_Twist_Field0x228Elem* f)
{
    v.begin("WalkAnim_Twist_Field0x228Elem");
    v.field("field0x10_cap", f->field0x10_cap);
    v.field("unk", f->unk);
    v.field("field0x10_len", f->field0x10_len);
    for (size_t i = 0; i < f->field0x10_cap; i++)
    {
        if (f->field0x10[i] != NULL)
        {
            v.field("slot_present", true);
            serialize_WalkAnim_Twist_Field0x228Elem_field0x10elem(v, f->field0x10[i]);
        }
        else
        {
            v.field("slot_present", false);
        }
    }
    v.end();
}

static void serialize_WalkAnim_Twist(StateVisitor& v, const WalkAnim_Twist* w)
{
    v.begin("WalkAnim_Twist");
    v.field("unk_8", w->unk_8);
    v.blob("walkF_region", &w->walkF, 0x1b8);
    v.blob("unk_1d8", &w->unk_1d8, 16);
    serialize_SpinJoint(v, w->Upper_Root_Joint);
    serialize_SpinJoint(v, w->master_joint);
    serialize_SpinJoint(v, w->neck_joint);
    v.blob("turn_lower_body_region", &w->turn_lower_body, 0x28);
    serialize_WalkAnim_Twist_Field0x228Elem(v, &w->walkAnim_Twist_Field0x228Elem);
    v.blob("unk_258", &w->unk_258, 0x48);
    v.end();
}

// ---- ChrCtrl / PlayerCtrl ---------------------------------------------------

static void serialize_ChrCtrl(StateVisitor& v, const ChrCtrl* c)
{
    v.begin("ChrCtrl");
    v.field("unk_8", c->unk_8);
    serialize_ChrCtrl_AnimationQueue(v, c->animationQueue);
    serialize_AnimationMediator(v, c->animationMediator);
    serialize_HavokChara(v, c->havokChara);
    serialize_ActionCtrl(v, c->actionctrl);
    v.field("unk_80", c->unk_80);
    v.blob("unk_90", &c->unk_90, 0x118);
    serialize_WalkAnim_Twist(v, c->walkAnim_Twist);
    v.blob("movement_enabled_region", &c->movement_enabled, 0xe0);
    v.blob("MapHitDisableDebugging_region", &c->MapHitDisableDebugging, 0x60);
    v.end();
}

static void serialize_PlayerCtrl(StateVisitor& v, const PlayerCtrl* p)
{
    v.begin("PlayerCtrl");
    serialize_ChrCtrl(v, &p->chrCtrl);
    v.blob("unk_300", &p->unk_300, 8);
    serialize_TurnAnim(v, p->turnAnim);
    serialize_ArrowTurnAnim(v, p->arrowTurnAnim);
    v.blob("unk_330", &p->unk_330, 8);
    v.blob("movement_related_flags_region", &p->movement_related_flags, 24);
    v.end();
}

// ---- ChrIns -----------------------------------------------------------------

static void serialize_ChrIns(StateVisitor& v, const ChrIns* c)
{
    v.begin("ChrIns");
    serialize_PlayerCtrl(v, c->playerCtrl);
    serialize_PadManipulator(v, c->padManipulator);
    v.field("CharaInitParamID", c->CharaInitParamID);
    v.blob("unk_16c", &c->unk_16c, 0x10);
    v.blob("lowerThrowAnim", &c->lowerThrowAnim, sizeof(c->lowerThrowAnim));
    v.blob("upperThrowAnim", &c->upperThrowAnim, sizeof(c->upperThrowAnim));
    v.blob("player_handing_state", c->player_handing_state, sizeof(uint32_t) * 3);
    v.field("curToughness", c->curToughness);
    v.field("maxToughness", c->maxToughness);
    v.field("toughnessUnk1", c->toughnessUnk1);
    v.field("toughnessRefillTimer", c->toughnessRefillTimer);
    v.field("toughnessUnk2", c->toughnessUnk2);
    v.field("curSelectedMagicId", c->curSelectedMagicId);
    v.blob("curUsedItem", &c->curUsedItem, sizeof(c->curUsedItem));
    serialize_SpecialEffect(v, c->specialEffects);
    serialize_QwcSpEffectEquipCtrl(v, c->qwcSpEffectEquipCtrl);
    v.blob("unk_288", &c->unk_288, 0x48);
    serialize_ChrIns_field0x2c8(v, c->field0x2c8);
    v.blob("unk_2d8", &c->unk_2d8, 0x98);
    serialize_HitIns(v, c->hitins_1);
    serialize_HitIns(v, c->hitins_2);
    v.blob("unk_380", &c->unk_380, 8);
    serialize_ChrAttachSys(v, &c->chrattachsys);
    v.field("curHp", c->curHp);
    v.field("maxHp", c->maxHp);
    v.field("curSp", c->curSp);
    v.field("maxSp", c->maxSp);
    v.field("damage_taken_scalar", c->damage_taken_scalar);
    v.field("PoisonResist", c->PoisonResist);
    v.field("ToxicResist", c->ToxicResist);
    v.field("BleedResist", c->BleedResist);
    v.field("CurseResist", c->CurseResist);
    v.field("resistPoisonTotal", c->resistPoisonTotal);
    v.field("resistPlagueTotal", c->resistPlagueTotal);
    v.field("resistBleedingTotal", c->resistBleedingTotal);
    v.field("resistCurseTotal", c->resistCurseTotal);
    serialize_EntityThrowAnimationStatus(v, c->throw_animation_info);
    v.blob("unk_450", &c->unk_450, 0x18);
    v.blob("unk_470", &c->unk_470, 0x50);
    v.blob("unk_4d8", &c->unk_4d8, 0x18);
    v.blob("unk_518", &c->unk_518, 0x58);
    v.end();
}

// ---- top level --------------------------------------------------------------

void serialize_PlayerIns(StateVisitor& v, const PlayerIns* p)
{
    v.begin("PlayerIns");
    serialize_ChrIns(v, &p->chrins);
    serialize_PlayerGameData(v, p->playergamedata);
    v.blob("data_0", p->data_0, sizeof(p->data_0));
    v.blob("unk_7a8", &p->unk_7a8, 16);
    v.blob("unk_7d0", &p->unk_7d0, 8);
    serialize_RingEquipCtrl(v, p->ringequipctrl);
    serialize_WeaponEquipCtrl(v, p->weaponequipctrl);
    serialize_ProEquipCtrl(v, p->proequipctrl);
    v.field("curSelectedMagicId", p->curSelectedMagicId);
    v.blob("curUsedItem", &p->curUsedItem, sizeof(p->curUsedItem));
    v.field("override_itemId", p->override_itemId);
    v.field("override_equipped_magicId", p->override_equipped_magicId);
    v.field("using_override", p->using_override);
    serialize_ChrAsm(v, p->chrasm);
    // chrAsmModelRes intentionally skipped (copy_PlayerIns skips it; redrawn each frame)
    serialize_ChrAsmModel(v, p->chrAsmModel);
    v.blob("headSize_region", &p->headSize, 24);
    v.blob("unk_880", &p->unk_880, 0x50);
    v.blob("data_5", p->data_5, sizeof(p->data_5));
    v.blob("data_5a", p->data_5a, sizeof(p->data_5a));
    v.blob("data_6", p->data_6, sizeof(p->data_6));
    v.end();
}

uint64_t hash_PlayerIns(PlayerIns* p)
{
    StateVisitor v(StateVisitor::Mode::Hash);
    serialize_PlayerIns(v, p);
    return v.digest();
}

std::string print_PlayerIns(PlayerIns* to)
{
    StateVisitor v(StateVisitor::Mode::Print);
    serialize_PlayerIns(v, to);
    return v.text();
}
