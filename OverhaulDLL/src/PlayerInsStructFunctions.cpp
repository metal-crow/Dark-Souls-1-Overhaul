#include "PlayerInsStructFunctions.h"
#include "Rollback.h"
#include "AttachSysSlotStructsFunctions.h"
#include "FrpgHavokManImpStructFunctions.h"

typedef void* falloc(uint64_t, uint64_t, uint32_t);

std::string print_PlayerIns(PlayerIns* to)
{
    Game::SuspendThreads();

    std::string out = "PlayerIns\n";
    out += print_ChrIns(&to->chrins);
    out += print_PlayerGameData(to->playergamedata);

    out += "Unknown data 0:";
    for (size_t i = 0; i < sizeof(to->data_0); i++)
    {
        out += std::to_string(to->data_0[i]);
        out += " ";
    }
    out += "\n";

    out += "Unknown data 1:";
    for (size_t i = 0; i < sizeof(to->data_1); i++)
    {
        out += std::to_string(to->data_1[i]);
        out += " ";
    }
    out += "\n";

    out += "Unknown data 2:" + std::to_string(to->data_2) + "\n";
    out += print_RingEquipCtrl(to->ringequipctrl);
    out += print_WeaponEquipCtrl(to->weaponequipctrl);
    out += print_ProEquipCtrl(to->proequipctrl);
    out += "curSelectedMagicId:" + std::to_string(to->curSelectedMagicId) + "\n";
    out += "curUsedItem:" + std::to_string(to->curUsedItem.amountUsed) + "," + std::to_string(to->curUsedItem.itemId) + "\n";
    out += "override_equipped_magicId:" + std::to_string(to->override_equipped_magicId) + "\n";
    out += print_ChrAsm(to->chrasm);
    out += print_ChrAsmModelRes(to->chrAsmModelRes);
    out += print_ChrAsmModel(to->chrAsmModel);

    out += "Unknown data 3:";
    for (size_t i = 0; i < sizeof(to->data_3)/4; i++)
    {
        out += std::to_string((float)to->data_3[i]);
        out += " ";
    }
    out += "\n";

    out += "Unknown data 4:";
    for (size_t i = 0; i < sizeof(to->data_4); i++)
    {
        out += std::to_string(to->data_4[i]);
        out += " ";
    }
    out += "\n";

    out += "Unknown data 5:";
    for (size_t i = 0; i < sizeof(to->data_5); i++)
    {
        out += std::to_string(to->data_5[i]);
        out += " ";
    }
    out += "\n";

    out += "Unknown data 6:";
    for (size_t i = 0; i < sizeof(to->data_6); i++)
    {
        out += std::to_string(to->data_6[i]);
        out += " ";
    }
    out += "\n";

    Game::ResumeThreads();

    return out;
}

void copy_PlayerIns(PlayerIns* to, const PlayerIns* from, bool to_game)
{
    Game::SuspendThreads();

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
    to->override_itemId = from->override_itemId;
    to->override_equipped_magicId = from->override_equipped_magicId;
    to->using_override = from->using_override;
    copy_ChrAsm(to->chrasm, from->chrasm);
    //copy_ChrAsmModelRes(to->chrAsmModelRes, from->chrAsmModelRes, to_game); //This is dynamically re-drawn every frame by the game
    copy_ChrAsmModel(to->chrAsmModel, from->chrAsmModel, to_game);
    memcpy(to->data_3, from->data_3, sizeof(to->data_3));
    memcpy(to->data_4, from->data_4, sizeof(to->data_4));
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

std::string print_ChrAsm(ChrAsm* to)
{
    std::string out = "ChrAsm\n";
    out += "equipped_weapon_style:" + std::to_string(to->equipped_weapon_style) + "\n";
    out += "l_hand_equipped_index:" + std::to_string(to->l_hand_equipped_index) + "\n";
    out += "r_hand_equipped_index:" + std::to_string(to->r_hand_equipped_index) + "\n";

    out += "Unknown data_0:";
    for (size_t i = 0; i < sizeof(to->data_0); i++)
    {
        out += std::to_string(to->data_0[i]);
        out += " ";
    }
    out += "\n";

    out += "Unknown equip_items:";
    for (size_t i = 0; i < sizeof(to->equip_items)/4; i++)
    {
        out += std::to_string(to->equip_items[i]);
        out += " ";
    }
    out += "\n";

    out += "Unknown data_1:";
    for (size_t i = 0; i < sizeof(to->data_1); i++)
    {
        out += std::to_string(to->data_1[i]);
        out += " ";
    }
    out += "\n";

    return out;
}

void copy_ChrAsm(ChrAsm* to, const ChrAsm* from)
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

std::string print_ChrAsmModelRes(ChrAsmModelRes* to)
{
    std::string out = "ChrAsmModelRes\n";
    out += "Unknown data 0:";
    for (size_t i = 0; i < sizeof(to->data_0); i++)
    {
        out += std::to_string(to->data_0[i]);
        out += " ";
    }
    out += "\n";

    for (size_t i = 0; i < 14; i++)
    {
        out += print_ChrAsmModelRes_Elem(&to->arry[i]);
    }

    return out;
}

void copy_ChrAsmModelRes(ChrAsmModelRes* to, const ChrAsmModelRes* from, bool to_game)
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

std::string print_ChrAsmModelRes_Elem(ChrAsmModelRes_Elem* to)
{
    std::string out = "ChrAsmModelRes_Elem\n";
    out += "Unknown data_0:" + std::to_string(to->data_0) + "\n";

    out += "Unknown data 1:";
    for (size_t i = 0; i < sizeof(to->data_1); i++)
    {
        out += std::to_string(to->data_1[i]);
        out += " ";
    }
    out += "\n";

    return out;
}

void copy_ChrAsmModelRes_Elem(ChrAsmModelRes_Elem* to, const ChrAsmModelRes_Elem* from, bool to_game)
{
    to->data_0 = from->data_0;
    to->PartsbndFileCap2 = NULL; //this should always be null since it's just tmp storage for 1 frame
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

std::string print_ChrAsmModel(ChrAsmModel* to)
{
    std::string out = "ChrAsmModel\n";
    out += "Unknown data_0:";
    for (size_t i = 0; i < sizeof(to->data_0); i++)
    {
        out += std::to_string(to->data_0[i]);
        out += " ";
    }
    out += "\n";

    out += "Unknown data_1:";
    for (size_t i = 0; i < sizeof(to->data_1); i++)
    {
        out += std::to_string(to->data_1[i]);
        out += " ";
    }
    out += "\n";

    out += "Unknown data_2:";
    for (size_t i = 0; i < sizeof(to->data_2); i++)
    {
        out += std::to_string(to->data_2[i]);
        out += " ";
    }
    out += "\n";

    out += "Unknown data_3:";
    for (size_t i = 0; i < sizeof(to->data_3); i++)
    {
        out += std::to_string(to->data_3[i]);
        out += " ";
    }
    out += "\n";

    out += "Unknown data_4:";
    for (size_t i = 0; i < sizeof(to->data_4); i++)
    {
        out += std::to_string(to->data_4[i]);
        out += " ";
    }
    out += "\n";

    out += "Unknown data_5:";
    for (size_t i = 0; i < sizeof(to->data_5)/8; i++)
    {
        out += std::to_string(to->data_5[i]);
        out += " ";
    }
    out += "\n";

    out += "Unknown data_6:" + std::to_string(to->data_6) + "\n";

    return out;
}

void copy_ChrAsmModel(ChrAsmModel* to, const ChrAsmModel* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
    memcpy(to->data_2, from->data_2, sizeof(to->data_2));
    to->data_3a = from->data_3a;
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

std::string print_ProEquipCtrl(ProEquipCtrl* to)
{
    std::string out = "ProEquipCtrl\n";
    out += print_SpecialEffect(to->spEffectList);
    out += "equipped_armors_ids:" +
        std::to_string(to->equipped_armors_ids[0]) + "," +
        std::to_string(to->equipped_armors_ids[1]) + "," +
        std::to_string(to->equipped_armors_ids[2]) + "," +
        std::to_string(to->equipped_armors_ids[3]) + "," +
        std::to_string(to->equipped_armors_ids[4]) + "\n";

    return out;
}

void copy_ProEquipCtrl(ProEquipCtrl* to, const ProEquipCtrl* from, bool to_game)
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

std::string print_WeaponEquipCtrl(WeaponEquipCtrl* to)
{
    std::string out = "WeaponEquipCtrl\n";
    out += print_SpecialEffect(to->spEffectList);
    out += "equipped_weapons_ids:" +
        std::to_string(to->equipped_weapons_ids[0]) + "," +
        std::to_string(to->equipped_weapons_ids[1]) + "\n";

    return out;
}

void copy_WeaponEquipCtrl(WeaponEquipCtrl* to, const WeaponEquipCtrl* from, bool to_game)
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

std::string print_RingEquipCtrl(RingEquipCtrl* to)
{
    std::string out = "RingEquipCtrl\n";
    out += print_SpecialEffect(to->spEffectList);
    out += "equipped_rings_ids:" +
        std::to_string(to->equipped_rings_ids[0]) + "," +
        std::to_string(to->equipped_rings_ids[1]) + "\n";

    return out;
}

void copy_RingEquipCtrl(RingEquipCtrl* to, const RingEquipCtrl* from, bool to_game)
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

std::string print_PlayerGameData(PlayerGameData* to)
{
    std::string out = "PlayerGameData\n";

    out += print_PlayerGameData_AttributeInfo(&to->attribs);
    out += print_EquipGameData(&to->equipGameData);
    out += print_PlayerGameData_ChrProperties(&to->ChrProperties);

    return out;
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

std::string print_PlayerGameData_ChrProperties(PlayerGameData_ChrProperties* to)
{
    std::string out = "PlayerGameData_ChrProperties\n";

    out += "Unknown data 0:";
    for (size_t i = 0; i < sizeof(to->data_0); i++)
    {
        out += std::to_string(to->data_0[i]);
        out += " ";
    }
    out += "\n";

    return out;
}

void copy_PlayerGameData_ChrProperties(PlayerGameData_ChrProperties* to, const PlayerGameData_ChrProperties* from)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
}

std::string print_EquipGameData(EquipGameData* to)
{
    std::string out = "EquipGameData\n";

    out += "equippedItemIndexes:";
    for (size_t i = 0; i < sizeof(to->EquipItemToInventoryIndexMap)/4; i++)
    {
        out += std::to_string(to->EquipItemToInventoryIndexMap[i]);
        out += " ";
    }
    out += "\n";

    out += print_ChrAsm(&to->chrasm);
    out += print_EquipMagicData(to->equipMagicData);
    out += print_EquipItemData(&to->equippedItemsInQuickbar);

    return out;
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

std::string print_EquipItemData(EquipItemData* to)
{
    std::string out = "EquipItemData\n";

    out += "quickbar:";
    for (size_t i = 0; i < sizeof(to->quickbar) / 4; i++)
    {
        out += std::to_string(to->quickbar[i]);
        out += " ";
    }
    out += "\n";

    out += "selectedQuickbarItem:" + std::to_string(to->selectedQuickbarItem) + "\n";

    return out;
}

void copy_EquipItemData(EquipItemData* to, const EquipItemData* from)
{
    memcpy(to->quickbar, from->quickbar, sizeof(to->quickbar));
    to->selectedQuickbarItem = from->selectedQuickbarItem;
}

std::string print_EquipMagicData(EquipMagicData* to)
{
    std::string out = "EquipMagicData\n";

    out += "equippedMagicList:";
    for (size_t i = 0; i < sizeof(to->equippedMagicList) / 8; i++)
    {
        out += std::to_string(to->equippedMagicList[i].magic_id);
        out += " ";
    }
    out += "\n";

    out += "curSelectedMagicSlot:" + std::to_string(to->curSelectedMagicSlot) + "\n";

    return out;
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

std::string print_PlayerGameData_AttributeInfo(PlayerGameData_AttributeInfo* to)
{
    std::string out = "PlayerGameData_AttributeInfo\n";

    out += "Unknown data 0:";
    for (size_t i = 0; i < sizeof(to->data_0); i++)
    {
        out += std::to_string(to->data_0[i]);
        out += " ";
    }
    out += "\n";

    return out;
}

void copy_PlayerGameData_AttributeInfo(PlayerGameData_AttributeInfo* to, const PlayerGameData_AttributeInfo* from)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
}

std::string print_ChrIns(ChrIns* to)
{
    std::string out = "ChrIns\n";
    out += print_PlayerCtrl(to->playerCtrl);
    out += "CharaInitParamID:" + std::to_string(to->CharaInitParamID) + "\n";

    out += "Unknown data 5:";
    for (size_t i = 0; i < sizeof(to->data_5); i++)
    {
        out += std::to_string(to->data_5[i]);
        out += " ";
    }
    out += "\n";

    out += "lowerThrowAnim:" + std::to_string(to->lowerThrowAnim.animationId) + "," + std::to_string(to->lowerThrowAnim.stateIndex) + "\n";
    out += "upperThrowAnim:" + std::to_string(to->upperThrowAnim.animationId) + "," + std::to_string(to->upperThrowAnim.stateIndex) + "\n";

    out += "player_handing_state:";
    for (size_t i = 0; i < 3; i++)
    {
        out += std::to_string(to->player_handing_state[i]);
        out += " ";
    }
    out += "\n";

    out += "curToughness:" + std::to_string(to->curToughness) + "\n";
    out += "maxToughness:" + std::to_string(to->maxToughness) + "\n";
    out += "toughnessUnk1:" + std::to_string(to->toughnessUnk1) + "\n";
    out += "toughnessRefillTimer:" + std::to_string(to->toughnessRefillTimer) + "\n";
    out += "toughnessUnk2:" + std::to_string(to->toughnessUnk2) + "\n";
    out += "curSelectedMagicId:" + std::to_string(to->curSelectedMagicId) + "\n";
    out += "curUsedItem:" + std::to_string(to->curUsedItem.amountUsed) + "," + std::to_string(to->curUsedItem.itemId) + "\n";
    out += print_SpecialEffect(to->specialEffects);
    out += print_QwcSpEffectEquipCtrl(to->qwcSpEffectEquipCtrl);

    out += "Unknown data 0:";
    for (size_t i = 0; i < sizeof(to->data_0); i++)
    {
        out += std::to_string(to->data_0[i]);
        out += " ";
    }
    out += "\n";

    out += print_ChrIns_field0x2c8(to->field0x2c8);

    out += "Unknown data 0a:";
    for (size_t i = 0; i < sizeof(to->data_0a); i++)
    {
        out += std::to_string(to->data_0a[i]);
        out += " ";
    }
    out += "\n";

    out += print_HitIns(to->hitins_1);
    out += print_HitIns(to->hitins_2);
    out += "data_0b:" + std::to_string(to->data_0b) + "\n";
    out += print_ChrAttachSys(&to->chrattachsys);
    out += "curHp:" + std::to_string(to->curHp) + "\n";
    out += "maxHp:" + std::to_string(to->maxHp) + "\n";
    out += "curSp:" + std::to_string(to->curSp) + "\n";
    out += "maxSp:" + std::to_string(to->maxSp) + "\n";
    out += "damage_taken_scalar:" + std::to_string(to->damage_taken_scalar) + "\n";
    out += "PoisonResist:" + std::to_string(to->PoisonResist) + "\n";
    out += "ToxicResist:" + std::to_string(to->ToxicResist) + "\n";
    out += "BleedResist:" + std::to_string(to->BleedResist) + "\n";
    out += "CurseResist:" + std::to_string(to->CurseResist) + "\n";
    out += "resistPoisonTotal:" + std::to_string(to->resistPoisonTotal) + "\n";
    out += "resistPlagueTotal:" + std::to_string(to->resistPlagueTotal) + "\n";
    out += "resistBleedingTotal:" + std::to_string(to->resistBleedingTotal) + "\n";
    out += "resistCurseTotal:" + std::to_string(to->resistCurseTotal) + "\n";
    out += print_EntityThrowAnimationStatus(to->throw_animation_info);

    out += "Unknown data 1:";
    for (size_t i = 0; i < sizeof(to->data_1); i++)
    {
        out += std::to_string(to->data_1[i]);
        out += " ";
    }
    out += "\n";

    out += "Unknown data 2:";
    for (size_t i = 0; i < sizeof(to->data_2); i++)
    {
        out += std::to_string(to->data_2[i]);
        out += " ";
    }
    out += "\n";

    out += "Unknown data 3:";
    for (size_t i = 0; i < sizeof(to->data_3)/8; i++)
    {
        out += std::to_string(to->data_3[i]);
        out += " ";
    }
    out += "\n";

    out += "Unknown data 4:";
    for (size_t i = 0; i < sizeof(to->data_4); i++)
    {
        out += std::to_string(to->data_4[i]);
        out += " ";
    }
    out += "\n";

    return out;
}

void copy_ChrIns(ChrIns* to, const ChrIns* from, bool to_game)
{
    //copy_ChrIns_field0x18(to->field0x18, from->field0x18);
    copy_PlayerCtrl(to->playerCtrl, from->playerCtrl, to_game);
    copy_PadManipulator(to->padManipulator, from->padManipulator);
    to->CharaInitParamID = from->CharaInitParamID;
    memcpy(to->data_5, from->data_5, sizeof(to->data_5));
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
    copy_SpecialEffect(to->specialEffects, from->specialEffects, to_game);
    copy_QwcSpEffectEquipCtrl(to->qwcSpEffectEquipCtrl, from->qwcSpEffectEquipCtrl);
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    copy_ChrIns_field0x2c8(to->field0x2c8, from->field0x2c8);
    memcpy(to->data_0a, from->data_0a, sizeof(to->data_0a));
    copy_HitIns(to->hitins_1, from->hitins_1);
    copy_HitIns(to->hitins_2, from->hitins_2);
    to->data_0b = from->data_0b;
    copy_ChrAttachSys(&to->chrattachsys, &from->chrattachsys, to_game);
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
    copy_EntityThrowAnimationStatus(to->throw_animation_info, from->throw_animation_info, to_game);
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
    //free_PadManipulator(to->padManipulator);
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

std::string print_ChrAttachSys(ChrAttachSys* to)
{
    std::string out = "ChrAttachSys\n";
    if (to->SysSlots != NULL)
    {
        out += print_AttachSysSlot(to->SysSlots);
    }
    else
    {
        out += "SysSlots: NULL\n";
    }
    return out;
}

void copy_ChrAttachSys(ChrAttachSys* to, const ChrAttachSys* from, bool to_game)
{
    if (to->SysSlots != NULL && from->SysSlots != NULL)
    {
        if (to_game)
        {
            Game::game_free_alt(to->SysSlots);
        }
        else
        {
            free(to->SysSlots);
        }
        to->SysSlots = NULL;
    }
    if (to->SysSlots == NULL && from->SysSlots != NULL)
    {
        to->SysSlots = init_AttachSysSlot((AttachSysSlotType)from->SysSlots->slotType, to_game);
    }
    if (to->SysSlots != NULL)
    {
        copy_AttachSysSlot(&(to->SysSlots), from->SysSlots, to_game);
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
    free_AttachSysSlot(to->SysSlots);

    if (freeself)
    {
        free(to);
    }
}

std::string print_ChrIns_field0x18(ChrIns_field0x18* to)
{
    std::string out = "ChrIns_field0x18\n";
    out += "Unknown data 0:";
    for (size_t i = 0; i < sizeof(to->data_0); i++)
    {
        out += std::to_string(to->data_0[i]);
        out += " ";
    }
    out += "\n";

    return out;
}

void copy_ChrIns_field0x18(ChrIns_field0x18* to, const ChrIns_field0x18* from)
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

std::string print_ChrIns_field0x2c8(ChrIns_field0x2c8* to)
{
    std::string out = "ChrIns_field0x2c8\n";

    out += "Unknown data 0:";
    for (size_t i = 0; i < sizeof(to->data_0); i++)
    {
        out += std::to_string(to->data_0[i]);
        out += " ";
    }
    out += "\n";

    out += "data_1:" + std::to_string(to->data_1) + "\n";

    out += "Unknown data 2:";
    for (size_t i = 0; i < sizeof(to->data_2); i++)
    {
        out += std::to_string(to->data_2[i]);
        out += " ";
    }
    out += "\n";

    out += "data_3:" + std::to_string(to->data_3) + "\n";

    return out;
}

void copy_ChrIns_field0x2c8(ChrIns_field0x2c8* to, const ChrIns_field0x2c8* from)
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

std::string print_EntityThrowAnimationStatus(EntityThrowAnimationStatus* to)
{
    std::string out = "EntityThrowAnimationStatus\n";

    out += "Unknown data 0:";
    for (size_t i = 0; i < sizeof(to->data_0); i++)
    {
        out += std::to_string(to->data_0[i]);
        out += " ";
    }
    out += "\n";

    if (to->throwSelfEsc != NULL)
    {
        out += print_ThrowSelfEsc(to->throwSelfEsc);
    }
    else
    {
        out += "throwSelfEsc: NULL\n";
    }

    out += "Unknown data 1:";
    for (size_t i = 0; i < sizeof(to->data_1); i++)
    {
        out += std::to_string(to->data_1[i]);
        out += " ";
    }
    out += "\n";

    return out;
}

void copy_EntityThrowAnimationStatus(EntityThrowAnimationStatus* to, const EntityThrowAnimationStatus* from, bool to_game)
{
    to->playerins_parent = from->playerins_parent;
    to->throw_paramdef = from->throw_paramdef;
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));

    if (to->throwSelfEsc == NULL && from->throwSelfEsc != NULL)
    {
        if (to_game)
        {
            to->throwSelfEsc = (ThrowSelfEsc*)Game::game_malloc(sizeof(ThrowSelfEsc), 8, *(uint64_t*)Game::internal_heap_2);
        }
        else
        {
            to->throwSelfEsc = init_ThrowSelfEsc();
        }
    }
    if (to->throwSelfEsc != NULL && from->throwSelfEsc == NULL)
    {
        if (to_game)
        {
            Game::game_free(to->throwSelfEsc);
        }
        else
        {
            free_ThrowSelfEsc(to->throwSelfEsc);
        }
        to->throwSelfEsc = NULL;
    }
    if (to->throwSelfEsc != NULL && from->throwSelfEsc != NULL)
    {
        copy_ThrowSelfEsc(to->throwSelfEsc, from->throwSelfEsc);
    }

    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
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

std::string print_ThrowSelfEsc(ThrowSelfEsc* to)
{
    std::string out = "ThrowSelfEsc\n";

    out += "Unknown data 0:";
    for (size_t i = 0; i < sizeof(to->data_0); i++)
    {
        out += std::to_string(to->data_0[i]);
        out += " ";
    }
    out += "\n";

    return out;
}

void copy_ThrowSelfEsc(ThrowSelfEsc* to, const ThrowSelfEsc* from)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
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

std::string print_QwcSpEffectEquipCtrl(QwcSpEffectEquipCtrl* to)
{
    std::string out = "QwcSpEffectEquipCtrl\n";

    out += "Array: ";
    for (size_t i = 0; i < to->arry_len; i++)
    {
        out += std::to_string(to->arry[i]) + " ";
    }
    out += "\n";

    out += "Unknown data 0:";
    for (size_t i = 0; i < sizeof(to->data_0)/4; i++)
    {
        out += std::to_string(to->data_0[i]);
        out += " ";
    }
    out += "\n";

    return out;
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

std::string print_SpecialEffect(SpecialEffect* to)
{
    std::string out = "SpecialEffect\n";

    out += print_SpecialEffect_Info(to->specialEffect_Info);

    out += "Unknown data 0:";
    for (size_t i = 0; i < sizeof(to->data_0); i++)
    {
        out += std::to_string(to->data_0[i]);
        out += " ";
    }
    out += "\n";

    out += "Unknown data 1:";
    for (size_t i = 0; i < sizeof(to->data_1); i++)
    {
        out += std::to_string(to->data_1[i]);
        out += " ";
    }
    out += "\n";

    out += "Unknown data 2:";
    for (size_t i = 0; i < sizeof(to->data_2); i++)
    {
        out += std::to_string(to->data_2[i]);
        out += " ";
    }
    out += "\n";

    return out;
}

void copy_SpecialEffect(SpecialEffect* to, const SpecialEffect* from, bool to_game)
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

std::string print_SpecialEffect_Info(SpecialEffect_Info* to)
{
    std::string out = "SpecialEffect_Info\n";

    out += "Life: " + std::to_string(to->data_0_struct.life) + "\n";
    out += "intervalLength: " + std::to_string(to->data_0_struct.intervalLength) + "\n";
    out += "intervalCountdownTimer: " + std::to_string(to->data_0_struct.intervalCountdownTimer) + "\n";
    out += "unk0: " + std::to_string(to->data_0_struct.unk0) + "\n";
    out += "unk1: " + std::to_string(to->data_0_struct.unk1) + "\n";
    out += "unk2: " + std::to_string(to->data_0_struct.unk2) + "\n";
    out += "unk3: " + std::to_string(to->data_0_struct.unk3) + "\n";
    out += "unk4: " + std::to_string(to->data_0_struct.unk4) + "\n";
    out += "unk5: " + std::to_string(to->data_0_struct.unk5) + "\n";
    out += "unk6: " + std::to_string(to->data_0_struct.unk6) + "\n";
    out += "unk7: " + std::to_string(to->data_0_struct.unk7) + "\n";
    out += "unk8: " + std::to_string(to->data_0_struct.unk8) + "\n";
    out += "unk9: " + std::to_string(to->data_0_struct.unk9) + "\n";
    out += "unk10: " + std::to_string(to->data_0_struct.unk10) + "\n";
    out += "target: " + std::to_string(to->data_0_struct.target) + "\n";
    out += "attacker: " + std::to_string(to->data_0_struct.attacker) + "\n";
    out += "unk11: " + std::to_string(to->data_0_struct.unk11) + "\n";
    out += "isDead: " + std::to_string(to->data_0_struct.isDead) + "\n";
    out += "unk12: " + std::to_string(to->data_0_struct.unk12) + "\n";
    out += "unk13: " + std::to_string(to->data_0_struct.unk13) + "\n";
    out += "unk14: " + std::to_string(to->data_0_struct.unk14) + "\n";
    out += "speffect_id: " + std::to_string(to->data_0_struct.speffect_id) + "\n";
    out += "unk15: " + std::to_string(to->data_0_struct.unk15) + "\n";

    if (to->next != NULL)
    {
        out += print_SpecialEffect_Info(to->next);
    }

    return out;
}

void copy_SpecialEffect_Info(SpecialEffect_Info* to, const SpecialEffect_Info* from, bool to_game)
{
    if (!to_game)
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

std::string print_PlayerCtrl(PlayerCtrl* to)
{
    std::string out = "PlayerCtrl\n";

    out += print_ChrCtrl(&to->chrCtrl);

    out += "Unknown data 0:";
    for (size_t i = 0; i < sizeof(to->data_0); i++)
    {
        out += std::to_string(to->data_0[i]);
        out += " ";
    }
    out += "\n";

    out += print_TurnAnim(to->turnAnim);
    out += print_ArrowTurnAnim(to->arrowTurnAnim);

    out += "Unknown data 1:";
    for (size_t i = 0; i < sizeof(to->data_1); i++)
    {
        out += std::to_string(to->data_1[i]);
        out += " ";
    }
    out += "\n";

    out += "Unknown data 2:";
    for (size_t i = 0; i < sizeof(to->data_2); i++)
    {
        out += std::to_string(to->data_2[i]);
        out += " ";
    }
    out += "\n";

    return out;
}

void copy_PlayerCtrl(PlayerCtrl* to, const PlayerCtrl* from, bool to_game)
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

std::string print_ArrowTurnAnim(ArrowTurnAnim* to)
{
    std::string out = "ArrowTurnAnim\n";

    out += print_TurnAnim(&to->turnAnim);
    out += print_SpinJoint(to->joint_spine_2);
    out += print_SpinJoint(to->joint_spine1_2);

    out += "Unknown data 0:";
    for (size_t i = 0; i < sizeof(to->data_0); i++)
    {
        out += std::to_string(to->data_0[i]);
        out += " ";
    }
    out += "\n";

    return out;
}

void copy_ArrowTurnAnim(ArrowTurnAnim* to, const ArrowTurnAnim* from)
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

std::string print_SpinJoint(SpinJoint* to)
{
    std::string out = "SpinJoint\n";

    out += "Unknown data 0:";
    for (size_t i = 0; i < sizeof(to->data_0); i++)
    {
        out += std::to_string(to->data_0[i]);
        out += " ";
    }
    out += "\n";

    out += "Unknown data_1:";
    for (size_t i = 0; i < sizeof(to->data_1); i++)
    {
        out += std::to_string(to->data_1[i]);
        out += " ";
    }
    out += "\n";

    out += "Unknown data_2:";
    for (size_t i = 0; i < sizeof(to->data_2); i++)
    {
        out += std::to_string(to->data_2[i]);
        out += " ";
    }
    out += "\n";

    return out;
}

void copy_SpinJoint(SpinJoint* to, const SpinJoint* from)
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

std::string print_TurnAnim(TurnAnim* to)
{
    std::string out = "TurnAnim\n";

    out += "Unknown data 0:";
    for (size_t i = 0; i < sizeof(to->data_0); i++)
    {
        out += std::to_string(to->data_0[i]);
        out += " ";
    }
    out += "\n";

    out += "Unknown data_1:";
    for (size_t i = 0; i < sizeof(to->data_1); i++)
    {
        out += std::to_string(to->data_1[i]);
        out += " ";
    }
    out += "\n";

    out += print_SpinJoint(to->joint_UpperRoot);
    out += print_SpinJoint(to->joint_LowerRoot);
    out += print_SpinJoint(to->joint_spine1_1);
    out += print_SpinJoint(to->joint_spine_1);
    out += print_SpinJoint(to->joint_master);

    out += "Unknown data_2:";
    for (size_t i = 0; i < sizeof(to->data_2); i++)
    {
        out += std::to_string(to->data_2[i]);
        out += " ";
    }
    out += "\n";

    out += "Unknown data_3:";
    for (size_t i = 0; i < sizeof(to->data_3); i++)
    {
        out += std::to_string(to->data_3[i]);
        out += " ";
    }
    out += "\n";

    return out;
}

void copy_TurnAnim(TurnAnim* to, const TurnAnim* from)
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

std::string print_ChrCtrl(ChrCtrl* to)
{
    std::string out = "TurnAnim\n";

    out += "Unknown data 0:";
    for (size_t i = 0; i < sizeof(to->data_0); i++)
    {
        out += std::to_string(to->data_0[i]);
        out += " ";
    }
    out += "\n";

    out += print_ChrCtrl_AnimationQueue(to->animationQueue);
    out += print_AnimationMediator(to->animationMediator);
    out += print_HavokChara(to->havokChara);
    out += print_ActionCtrl(to->actionctrl);

    out += "Unknown data_1:";
    for (size_t i = 0; i < sizeof(to->data_1); i++)
    {
        out += std::to_string(to->data_1[i]);
        out += " ";
    }
    out += "\n";

    out += "Unknown data_2:";
    for (size_t i = 0; i < sizeof(to->data_2); i++)
    {
        out += std::to_string(to->data_2[i]);
        out += " ";
    }
    out += "\n";

    out += print_WalkAnim_Twist(to->walkAnim_Twist);

    out += "Unknown data_3:";
    for (size_t i = 0; i < sizeof(to->data_3); i++)
    {
        out += std::to_string(to->data_3[i]);
        out += " ";
    }
    out += "\n";

    out += "Unknown data_4:";
    for (size_t i = 0; i < sizeof(to->data_4); i++)
    {
        out += std::to_string(to->data_4[i]);
        out += " ";
    }
    out += "\n";

    return out;
}

void copy_ChrCtrl(ChrCtrl* to, const ChrCtrl* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    copy_ChrCtrl_AnimationQueue(to->animationQueue, from->animationQueue, to_game);
    copy_AnimationMediator(to->animationMediator, from->animationMediator);
    copy_HavokChara(to->havokChara, from->havokChara, to_game);
    copy_ActionCtrl(to->actionctrl, from->actionctrl, to_game);
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
    memcpy(to->data_2, from->data_2, sizeof(to->data_2));
    copy_WalkAnim_Twist(to->walkAnim_Twist, from->walkAnim_Twist, to_game);
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

std::string print_WalkAnim_Twist(WalkAnim_Twist* to)
{
    std::string out = "WalkAnim_Twist\n";

    out += "Unknown data_0:";
    for (size_t i = 0; i < sizeof(to->data_0); i++)
    {
        out += std::to_string(to->data_0[i]);
        out += " ";
    }
    out += "\n";

    out += "Unknown data_1:";
    for (size_t i = 0; i < sizeof(to->data_1); i++)
    {
        out += std::to_string(to->data_1[i]);
        out += " ";
    }
    out += "\n";

    out += "Unknown data_2:";
    for (size_t i = 0; i < sizeof(to->data_2); i++)
    {
        out += std::to_string(to->data_2[i]);
        out += " ";
    }
    out += "\n";

    out += print_SpinJoint(to->Upper_Root_Joint);
    out += print_SpinJoint(to->master_joint);
    out += print_SpinJoint(to->neck_joint);

    out += "Unknown data_3:";
    for (size_t i = 0; i < sizeof(to->data_3); i++)
    {
        out += std::to_string(to->data_3[i]);
        out += " ";
    }
    out += "\n";

    out += print_WalkAnim_Twist_Field0x228Elem(&to->walkAnim_Twist_Field0x228Elem);

    out += "Unknown data_4:";
    for (size_t i = 0; i < sizeof(to->data_4); i++)
    {
        out += std::to_string(to->data_4[i]);
        out += " ";
    }
    out += "\n";

    return out;
}

void copy_WalkAnim_Twist(WalkAnim_Twist* to, const WalkAnim_Twist* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
    memcpy(to->data_2, from->data_2, sizeof(to->data_2));
    copy_SpinJoint(to->Upper_Root_Joint, from->Upper_Root_Joint);
    copy_SpinJoint(to->master_joint, from->master_joint);
    copy_SpinJoint(to->neck_joint, from->neck_joint);
    memcpy(to->data_3, from->data_3, sizeof(to->data_3));
    copy_WalkAnim_Twist_Field0x228Elem(&to->walkAnim_Twist_Field0x228Elem, &from->walkAnim_Twist_Field0x228Elem, to_game);
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

std::string print_WalkAnim_Twist_Field0x228Elem(WalkAnim_Twist_Field0x228Elem* to)
{
    std::string out = "WalkAnim_Twist_Field0x228Elem\n";

    for (size_t i = 0; i < to->field0x10_len; i++)
    {
        if (to->field0x10[i] != NULL)
        {
            out += print_WalkAnim_Twist_Field0x228Elem_field0x10elem(to->field0x10[i]);
        }
    }

    return out;
}

void copy_WalkAnim_Twist_Field0x228Elem(WalkAnim_Twist_Field0x228Elem* to, const WalkAnim_Twist_Field0x228Elem* from, bool to_game)
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
        if (to_game)
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
            if (!to_game)
            {
                free(to->field0x10[i]);
            }
            to->field0x10[i] = NULL;
        }
        if (to->field0x10[i] == NULL && from->field0x10[i] != NULL)
        {
            //need to manually alloc the entry for the game
            if (to_game)
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

std::string print_WalkAnim_Twist_Field0x228Elem_field0x10elem(WalkAnim_Twist_Field0x228Elem_field0x10elem* to)
{
    std::string out = "WalkAnim_Twist_Field0x228Elem_field0x10elem\n";

    out += "Unknown data_0:";
    for (size_t i = 0; i < sizeof(to->data_0)/4; i++)
    {
        out += std::to_string(to->data_0[i]);
        out += " ";
    }
    out += "\n";

    return out;
}

void copy_WalkAnim_Twist_Field0x228Elem_field0x10elem(WalkAnim_Twist_Field0x228Elem_field0x10elem* to, const WalkAnim_Twist_Field0x228Elem_field0x10elem* from)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
}

std::string print_ActionCtrl(ActionCtrl* to)
{
    std::string out = "ActionCtrl\n";

    out += "Unknown data_0:";
    for (size_t i = 0; i < sizeof(to->data_0); i++)
    {
        out += std::to_string(to->data_0[i]);
        out += " ";
    }
    out += "\n";

    out += print_ActionCtrl_0x30Substruct(&to->passive_state);
    out += print_ActionCtrl_0x30Substruct(&to->active_state);

    out += "Unknown data_1:";
    for (size_t i = 0; i < sizeof(to->data_1); i++)
    {
        out += std::to_string(to->data_1[i]);
        out += " ";
    }
    out += "\n";

    out += "Unknown data_2:";
    for (size_t i = 0; i < sizeof(to->data_2); i++)
    {
        out += std::to_string(to->data_2[i]);
        out += " ";
    }
    out += "\n";

    return out;
}

void copy_ActionCtrl(ActionCtrl* to, const ActionCtrl* from, bool to_game)
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

std::string print_ActionCtrl_0x30Substruct(ActionCtrl_0x30Substruct* to)
{
    std::string out = "ActionCtrl_0x30Substruct\n";

    out += print_EzState_detail_EzStateMachineImpl(to->EzStateMachineImpl);

    out += "Unknown data_0:";
    for (size_t i = 0; i < sizeof(to->data_0); i++)
    {
        out += std::to_string(to->data_0[i]);
        out += " ";
    }
    out += "\n";

    return out;
}

void copy_ActionCtrl_0x30Substruct(ActionCtrl_0x30Substruct* to, const ActionCtrl_0x30Substruct* from, bool to_game)
{
    copy_EzState_detail_EzStateMachineImpl(to->EzStateMachineImpl, from->EzStateMachineImpl, to_game);
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
}

std::string print_EzState_detail_EzStateMachineImpl(EzState_detail_EzStateMachineImpl* to)
{
    std::string out = "EzState_detail_EzStateMachineImpl\n";

    out += "Unknown data_0:";
    for (size_t i = 0; i < sizeof(to->data_0); i++)
    {
        out += std::to_string(to->data_0[i]);
        out += " ";
    }
    out += "\n";

    out += print_EzStateRegisterSet(&to->EzStateRegisterSet1);
    out += print_EzStateRegisterSet(&to->EzStateRegisterSet2);

    out += "Unknown data_1:";
    for (size_t i = 0; i < sizeof(to->data_1); i++)
    {
        out += std::to_string(to->data_1[i]);
        out += " ";
    }
    out += "\n";

    out += "Unknown data_2:";
    for (size_t i = 0; i < sizeof(to->data_2); i++)
    {
        out += std::to_string(to->data_2[i]);
        out += " ";
    }
    out += "\n";

    return out;
}

void copy_EzState_detail_EzStateMachineImpl(EzState_detail_EzStateMachineImpl* to, const EzState_detail_EzStateMachineImpl* from, bool to_game)
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

std::string print_EzStateRegisterSet(EzStateRegisterSet* to)
{
    std::string out = "EzStateRegisterSet\n";

    for (size_t i = 0; i < 8; i++)
    {
        for (size_t j = 0; j < sizeof(to->arry[i].data_0); j++)
        {
            out += std::to_string(to->arry[i].data_0[j]) + " ";
        }
        out += ";";
    }
    out += "\n";

    return out;
}

void copy_EzStateRegisterSet(EzStateRegisterSet* to, const EzStateRegisterSet* from, bool to_game)
{
    if (!to_game && from->arry_cur != NULL && from->arry != NULL)
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

std::string print_HavokChara(HavokChara* to)
{
    std::string out = "EzState_detail_EzStateMachineImpl\n";

    out += "Unknown data_0:";
    for (size_t i = 0; i < sizeof(to->data_0); i++)
    {
        out += std::to_string(to->data_0[i]);
        out += " ";
    }
    out += "\n";

    out += print_hkpCharacterProxy(to->char_proxy);

    out += "Unknown data_1:";
    for (size_t i = 0; i < sizeof(to->data_1); i++)
    {
        out += std::to_string(to->data_1[i]);
        out += " ";
    }
    out += "\n";

    out += "Unknown data_2:";
    for (size_t i = 0; i < sizeof(to->data_2); i++)
    {
        out += std::to_string(to->data_2[i]);
        out += " ";
    }
    out += "\n";

    out += "Unknown data_3:";
    for (size_t i = 0; i < sizeof(to->data_3); i++)
    {
        out += std::to_string(to->data_3[i]);
        out += " ";
    }
    out += "\n";

    out += "Unknown data_4:";
    for (size_t i = 0; i < sizeof(to->data_4); i++)
    {
        out += std::to_string(to->data_4[i]);
        out += " ";
    }
    out += "\n";

    return out;
}

void copy_HavokChara(HavokChara* to, const HavokChara* from, bool to_game)
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

std::string print_HitIns(HitIns* to)
{
    if (to == NULL)
    {
        return "HitIns: NULL\n";
    }

    std::string out = "HitIns\n";

    out += "data_0: " + std::to_string(to->data_0) + "\n";

    out += "Unknown data_1:";
    for (size_t i = 0; i < sizeof(to->data_1); i++)
    {
        out += std::to_string(to->data_1[i]);
        out += " ";
    }
    out += "\n";

    out += "data_2: " + std::to_string(to->data_2) + "\n";
    out += "data_3: " + std::to_string(to->data_3) + "\n";
    out += "data_4: " + std::to_string(to->data_4) + "\n";
    out += "data_5: " + std::to_string(to->data_5) + "\n";

    return out;
}

void copy_HitIns(HitIns* to, const HitIns* from)
{
    if (to == NULL || from == NULL)
    {
        return;
    }
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

std::string print_AnimationMediator(AnimationMediator* to)
{
    std::string out = "hkpSimpleShapePhantom_field0x30\n";

    for (int i = 0; i < 31; i++)
    {
        out += print_AnimationMediatorStateEntry(&to->states_list[i]);
    }
    out += print_AnimationQueue(to->animationQueue);

    out += "Unknown data_0:";
    for (size_t i = 0; i < sizeof(to->data_0); i++)
    {
        out += std::to_string(to->data_0[i]);
        out += " ";
    }
    out += "\n";

    return out;
}

void copy_AnimationMediator(AnimationMediator* to, const AnimationMediator* from)
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

std::string print_AnimationMediatorStateEntry(AnimationMediatorStateEntry* to)
{
    std::string out = "AnimationMediatorStateEntry\n";

    out += "Unknown data_0:";
    for (size_t i = 0; i < sizeof(to->data_0); i++)
    {
        out += std::to_string(to->data_0[i]);
        out += " ";
    }
    out += "\n";

    out += "Unknown data_1:";
    for (size_t i = 0; i < sizeof(to->data_1); i++)
    {
        out += std::to_string(to->data_1[i]);
        out += " ";
    }
    out += "\n";

    return out;
}

void copy_AnimationMediatorStateEntry(AnimationMediatorStateEntry* to, const AnimationMediatorStateEntry* from)
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

std::string print_ChrCtrl_AnimationQueue(ChrCtrl_AnimationQueue* to)
{
    std::string out = "ChrCtrl_AnimationQueue\n";

    out += "array_length: " + std::to_string(to->array_length) + "\n";
    out += "data_0: " + std::to_string(to->data_0) + "\n";

    for (size_t i = 0; i < to->array_length; i++)
    {
        out += print_ChrCtrl_AnimationQueueEntry(&to->arry[i]);
    }
    out += print_ChrCtrl_AnimationQueue_field0x10(to->field0x10);
    out += print_hkaAnimatedSkeleton(to->HkaAnimatedSkeleton);
    out += print_ChrCtrl_AnimationQueue_field0x20(to->field0x20);

    out += "Unknown data_1:";
    for (size_t i = 0; i < sizeof(to->data_1); i++)
    {
        out += std::to_string(to->data_1[i]);
        out += " ";
    }
    out += "\n";

    out += "Unknown data_2:";
    for (size_t i = 0; i < sizeof(to->data_2); i++)
    {
        out += std::to_string(to->data_2[i]);
        out += " ";
    }
    out += "\n";

    out += "data_3: " + std::to_string(to->data_3) + "\n";

    out += "Unknown data_4:";
    for (size_t i = 0; i < sizeof(to->data_4); i++)
    {
        out += std::to_string(to->data_4[i]);
        out += " ";
    }
    out += "\n";

    out += "Unknown data_5:";
    for (size_t i = 0; i < sizeof(to->data_5); i++)
    {
        out += std::to_string(to->data_5[i]);
        out += " ";
    }
    out += "\n";

    return out;
}

void copy_ChrCtrl_AnimationQueue(ChrCtrl_AnimationQueue* to, const ChrCtrl_AnimationQueue* from, bool to_game)
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

std::string print_ChrCtrl_AnimationQueue_field0x20(ChrCtrl_AnimationQueue_field0x20* to)
{
    std::string out = "ChrCtrl_AnimationQueue_field0x20\n";

    out += "Unknown field0x8:";
    for (size_t i = 0; i < (0x30 * 64) / 4; i++)
    {
        out += std::to_string( (int)( (((float*)to->field0x8)[i]) * 100) / 100.0f);
        out += " ";
    }
    out += "\n";

    out += "Unknown data_0:";
    for (size_t i = 0; i < sizeof(to->data_0)/4; i++)
    {
        out += std::to_string(to->data_0[i]);
        out += " ";
    }
    out += "\n";

    out += "Unknown field0x18:";
    for (size_t i = 0; i < (0x30 * 64) / 4; i++)
    {
        out += std::to_string( (int)( (((float*)to->field0x18)[i]) * 100) / 100.0f);
        out += " ";
    }
    out += "\n";

    out += "Unknown data_1:";
    for (size_t i = 0; i < sizeof(to->data_1) / 4; i++)
    {
        out += std::to_string(to->data_1[i]);
        out += " ";
    }
    out += "\n";

    out += "Unknown field0x28:";
    for (size_t i = 0; i < 64; i++)
    {
        out += std::to_string(to->field0x28[i]);
        out += " ";
    }
    out += "\n";

    out += "Unknown data_2:";
    for (size_t i = 0; i < sizeof(to->data_2) / 4; i++)
    {
        out += std::to_string(to->data_2[i]);
        out += " ";
    }
    out += "\n";

    out += "Unknown data_3:";
    for (size_t i = 0; i < sizeof(to->data_3) / 4; i++)
    {
        out += std::to_string(to->data_3[i]);
        out += " ";
    }
    out += "\n";

    return out;
}

void copy_ChrCtrl_AnimationQueue_field0x20(ChrCtrl_AnimationQueue_field0x20* to, const ChrCtrl_AnimationQueue_field0x20* from, bool to_game)
{
    if (!to_game && from->padding_0 != NULL)
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

std::string print_hkaAnimatedSkeleton(hkaAnimatedSkeleton* to)
{
    std::string out = "hkaAnimatedSkeleton\n";

    out += "Unknown data_0:" + std::to_string(to->data_0) + "\n";
    out += "Unknown animCtrl_list_len:" + std::to_string(to->animCtrl_list_len) + "\n";
    for (uint32_t i = 0; i < to->animCtrl_list_len; i++)
    {
        out += print_hkaDefaultAnimationControl(to->animCtrl_list[i]);
    }
    out += "Unknown data_1:" + std::to_string(to->data_1) + "\n";
    out += "Unknown data_2:" + std::to_string(to->data_2) + "\n";

    return out;
}

void copy_hkaAnimatedSkeleton(hkaAnimatedSkeleton* to, const hkaAnimatedSkeleton* from)
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

std::string print_hkaDefaultAnimationControl(hkaDefaultAnimationControl* to)
{
    std::string out = "hkaDefaultAnimationControl\n";

    out += print_hkaAnimationControl(&to->HkaAnimationControl);

    out += "Unknown data_0:";
    for (size_t i = 0; i < sizeof(to->data_0); i++)
    {
        out += std::to_string(to->data_0[i]);
        out += " ";
    }
    out += "\n";

    return out;
}

void copy_hkaDefaultAnimationControl(hkaDefaultAnimationControl* to, const hkaDefaultAnimationControl* from)
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

std::string print_hkaAnimationControl(hkaAnimationControl* to)
{
    std::string out = "hkaAnimationControl\n";

    out += "Unknown data_0:";
    for (size_t i = 0; i < sizeof(to->data_0); i++)
    {
        out += std::to_string(to->data_0[i]);
        out += " ";
    }
    out += "\n";

    out += "Unknown field0x18:";
    for (size_t i = 0; i < to->field0x18_len; i++)
    {
        out += std::to_string(to->field0x18[i]);
        out += " ";
    }
    out += "\n";

    out += "Unknown field0x28:";
    for (size_t i = 0; i < to->field0x28_len; i++)
    {
        out += std::to_string(to->field0x28[i]);
        out += " ";
    }
    out += "\n";

    out += "HkaAnimationBinding:" + std::to_string(to->HkaAnimationBinding) + "\n";

    out += "Unknown data_1:";
    for (size_t i = 0; i < sizeof(to->data_1)/4; i++)
    {
        out += std::to_string(to->data_1[i]);
        out += " ";
    }
    out += "\n";

    return out;
}

void copy_hkaAnimationControl(hkaAnimationControl* to, const hkaAnimationControl* from)
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

std::string print_ChrCtrl_AnimationQueue_field0x10(ChrCtrl_AnimationQueue_field0x10* to)
{
    std::string out = "ChrCtrl_AnimationQueue_field0x10\n";

    out += "array1_len:" + std::to_string(to->array1_len) + "\n";
    out += "array2_len:" + std::to_string(to->array2_len) + "\n";

    out += "Unknown arry2:";
    for (size_t i = 0; i < to->array2_len; i++)
    {
        for (size_t j = 0; j < sizeof(ChrCtrl_AnimationQueue_field0x10_field0x10arrayelem); j++)
        {
            out += std::to_string(to->arry2[i].data_0[j]);
            out += " ";
        }
        out += ";";
    }
    out += "\n";

    return out;
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

std::string print_ChrCtrl_AnimationQueueEntry(ChrCtrl_AnimationQueueEntry* to)
{
    std::string out = "ChrCtrl_AnimationQueueEntry\n";

    out += "Unknown data_0:";
    for (size_t i = 0; i < sizeof(to->data_0); i++)
    {
        out += std::to_string(to->data_0[i]);
        out += " ";
    }
    out += "\n";

    out += print_hkaDefaultAnimationControl(to->defaultAnimationControl);
    out += "HvkAnim_AnimInfoArrayElem:" + std::to_string(to->HvkAnim_AnimInfoArrayElem) + "\n";

    out += "Unknown data_1:";
    for (size_t i = 0; i < sizeof(to->data_1); i++)
    {
        out += std::to_string(to->data_1[i]);
        out += " ";
    }
    out += "\n";

    return out;
}

void copy_ChrCtrl_AnimationQueueEntry(ChrCtrl_AnimationQueueEntry* to, const ChrCtrl_AnimationQueueEntry* from, bool to_game)
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

std::string print_AnimationQueue(AnimationQueue* to)
{
    std::string out = "AnimationQueue\n";

    for (int i = 0; i < 6; i++)
    {
        out += print_AnimationQueue_Entry(to->AnimationQueue_Entries[i]);
    }

    return out;
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

std::string print_AnimationQueue_Entry(AnimationQueue_Entry* to)
{
    std::string out = "AnimationQueue_Entry\n";

    out += "Unknown data_0:";
    for (size_t i = 0; i < sizeof(to->data_0); i++)
    {
        out += std::to_string(to->data_0[i]);
        out += " ";
    }
    out += "\n";

    out += "Unknown data_1:";
    for (size_t i = 0; i < sizeof(to->data_1); i++)
    {
        out += std::to_string(to->data_1[i]);
        out += " ";
    }
    out += "\n";

    out += print_AnimationQueue_Entry_sub1(&to->sub1);

    out += "Unknown data_2:";
    for (size_t i = 0; i < sizeof(to->data_2)/8; i++)
    {
        out += std::to_string(to->data_2[i]);
        out += " ";
    }
    out += "\n";

    size_t len = ((uint64_t)to->chained_animations_array_end - (uint64_t)to->chained_animations_array_start) / 8;
    out += "arry len:" + std::to_string(len) + "\n";

    out += "chained_animations_array: ";
    for (size_t i = 0; i < len; i++)
    {
        out += std::to_string(to->chained_animations_array_start[i]) + " ";
    }
    out += "\n";

    out += "Unknown data_3:";
    for (size_t i = 0; i < sizeof(to->data_3); i++)
    {
        out += std::to_string(to->data_3[i]);
        out += " ";
    }
    out += "\n";

    return out;
}

void copy_AnimationQueue_Entry(AnimationQueue_Entry* to, const AnimationQueue_Entry* from)
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

std::string print_AnimationQueue_Entry_sub1(AnimationQueue_Entry_sub1* to)
{
    std::string out = "AnimationQueue_Entry_sub1\n";

    out += "field0x10_cap: " + std::to_string(to->field0x10_cap) + "\n";
    out += "unk: " + std::to_string(to->unk) + "\n";
    out += "field0x10_len: " + std::to_string(to->field0x10_len) + "\n";

    for (size_t i = 0; i < to->field0x10_len; i++)
    {
        if (to->field0x10[i] != NULL)
        {
            out += print_AnimationQueue_Entry_sub1_field0x10(to->field0x10[i]);
        }
    }

    return out;
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

std::string print_AnimationQueue_Entry_sub1_field0x10(AnimationQueue_Entry_sub1_field0x10* to)
{
    std::string out = "AnimationQueue_Entry\n";

    out += "Unknown data_0:";
    for (size_t i = 0; i < sizeof(to->data_0)/8; i++)
    {
        out += std::to_string(to->data_0[i]);
        out += " ";
    }
    out += "\n";

    return out;
}

void copy_AnimationQueue_Entry_sub1_field0x10(AnimationQueue_Entry_sub1_field0x10* to, const AnimationQueue_Entry_sub1_field0x10* from)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
}
