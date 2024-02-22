#include "AttachSysSlotStructsFunctions.h"
#include "BulletManStructFunctions.h"
#include "Rollback.h"

std::string print_AttachSysSlot(AttachSysSlotBaseImpl* to)
{
    std::string out = "AttachSysSlotBaseImpl\n";

    switch (to->slotType)
    {
    case TypeChrShineTreasureSlot:
        out += print_ChrShineTreasureSlot((ChrShineTreasureSlot*)to);
        break;
    case TypeChrSingleSeSlot:
        out += print_ChrSingleSeSlot((ChrSingleSeSlot*)to);
        break;
    case TypeChrSingleSfxSlot:
        out += print_ChrSingleSfxSlot((ChrSingleSfxSlot*)to);
        break;
    case TypeChrMultiSfxSlot:
        out += print_ChrMultiSfxSlot((ChrMultiSfxSlot*)to);
        break;
    case TypeChrBurnSlot:
        out += print_ChrBurnSlot((ChrBurnSlot*)to);
        break;
    case TypeChrGasmanSlot:
        out += print_ChrGasmanSlot((ChrGasmanSlot*)to);
        break;
    case TypeChrGrassSlot:
        out += print_ChrGrassSlot((ChrGrassSlot*)to);
        break;
    case TypeChrFootEffectSlot:
        out += print_ChrFootEffectSlot((ChrFootEffectSlot*)to);
        break;
    case TypeChrRigidOffsetSfxSlot:
        out += print_ChrRigidOffsetSfxSlot((ChrRigidOffsetSfxSlot*)to);
        break;
    case TypeChrRigidOffsetChrSlot:
        out += print_ChrRigidOffsetChrSlot((ChrRigidOffsetChrSlot*)to);
        break;
    case TypeChrSoulEatSlot:
        out += print_ChrSoulEatSlot((ChrSoulEatSlot*)to);
        break;
    case TypeChrSorceryWepSlot:
        out += print_ChrSorceryWepSlot((ChrSorceryWepSlot*)to);
        break;
    case TypeChrLanternSlot:
        out += print_ChrLanternSlot((ChrLanternSlot*)to);
        break;
    case TypeChrSingleOneshotSfxSlot:
        out += print_ChrSingleOneshotSfxSlot((ChrSingleOneshotSfxSlot*)to);
        break;
    case TypeChrSingleTraceSfxSlot:
        out += print_ChrSingleTraceSfxSlot((ChrSingleTraceSfxSlot*)to);
        break;
    case TypeChrMagicGoodsUseSfxSlot:
        out += print_ChrMagicGoodsUseSfxSlot((ChrMagicGoodsUseSfxSlot*)to);
        break;
    case TypeChrActPntSlot:
        out += print_ChrActPntSlot((ChrActPntSlot*)to);
        break;
    case TypeChrPointLightSlot:
        out += print_ChrPointLightSlot((ChrPointLightSlot*)to);
        break;
    case TypeChrWepEnchantSlot:
        out += print_ChrWepEnchantSlot((ChrWepEnchantSlot*)to);
        break;
    case TypeChrFallingControlSlot:
        out += print_ChrFallingControlSlot((ChrFallingControlSlot*)to);
        break;
    case TypeChrConditionSfxSeSlot:
        out += print_ChrConditionSfxSeSlot((ChrConditionSfxSeSlot*)to);
        break;
    case TypeChrCamouflageSlot:
        out += print_ChrCamouflageSlot((ChrCamouflageSlot*)to);
        break;
    case TypeChrSoulDeadSlot:
        out += print_ChrSoulDeadSlot((ChrSoulDeadSlot*)to);
        break;
    case TypeChrShinpanshaHaraSlot:
        out += print_ChrShinpanshaHaraSlot((ChrShinpanshaHaraSlot*)to);
        break;
    case TypeChrLimitInvincibleSlot:
        out += print_ChrLimitInvincibleSlot((ChrLimitInvincibleSlot*)to);
        break;
    case TypeChrDetectSignSlot:
        out = "ChrDetectSignSlot\n";
        break;
    case TypeChrRescueSignSlot:
        out = "ChrRescueSignSlot\n";
        break;
    case TypeChrTravelItemSlot:
        out += print_ChrTravelItemSlot((ChrTravelItemSlot*)to);
        break;
    case TypeChrStatueDeadSlot:
        out += print_ChrStatueDeadSlot((ChrStatueDeadSlot*)to);
        break;
    case TypeChrResonanceMagicSlot:
        out += print_ChrResonanceMagicSlot((ChrResonanceMagicSlot*)to);
        break;
    case TypeChrRetributionMagicSlot:
        out += print_ChrRetributionMagicSlot((ChrRetributionMagicSlot*)to);
        break;
    case TypeChrHellkiteBreathSlot:
        out = "ChrHellkiteBreathSlot\n";
        break;
    case TypeChrColiseumEntrantSlot:
        out = "ChrColiseumEntrantSlot\n";
        break;
    case TypeChrPlayerResidentSlot:
        out += print_ChrPlayerResidentSlot((ChrPlayerResidentSlot*)to);
        break;
    case TypeChrFollowSfxSlot:
        out += print_ChrFollowSfxSlot((ChrFollowSfxSlot*)to);
        break;
    default: FATALERROR("Attempted to print AttachSysSlot of type %d", to->slotType);
    }

    out += "vtable:" + std::to_string(to->vtable) + "\n";
    out += "timerId:" + std::to_string(to->timerId) + "\n";
    out += "slotIsUsable:" + std::to_string(to->slotIsUsable) + "\n";
    out += "data_0:" + std::to_string(to->data_0) + "\n";

    if (to->next != NULL)
    {
        out += print_AttachSysSlot(to->next);
    }

    return out;
}

//Need to to be a ** since ChrActPntSlot may need to resize it
//If any called func it does resize it, it must ensure that the AttachSysSlotBaseImpl base obj is correctly init'd
//For now only copy_ChrActPntSlot uses this
void copy_AttachSysSlot(AttachSysSlotBaseImpl** to, AttachSysSlotBaseImpl* from, bool to_game)
{
    switch (from->slotType)
    {
    case TypeChrShineTreasureSlot:
        copy_ChrShineTreasureSlot((ChrShineTreasureSlot*)*to, (ChrShineTreasureSlot*)from, to_game);
        break;
    case TypeChrSingleSeSlot:
        copy_ChrSingleSeSlot((ChrSingleSeSlot*)*to, (ChrSingleSeSlot*)from, to_game);
        break;
    case TypeChrSingleSfxSlot:
        copy_ChrSingleSfxSlot((ChrSingleSfxSlot*)*to, (ChrSingleSfxSlot*)from, to_game);
        break;
    case TypeChrMultiSfxSlot:
        copy_ChrMultiSfxSlot((ChrMultiSfxSlot*)*to, (ChrMultiSfxSlot*)from, to_game);
        break;
    case TypeChrBurnSlot:
        copy_ChrBurnSlot((ChrBurnSlot*)*to, (ChrBurnSlot*)from, to_game);
        break;
    case TypeChrGasmanSlot:
        copy_ChrGasmanSlot((ChrGasmanSlot*)*to, (ChrGasmanSlot*)from, to_game);
        break;
    //case TypeChrGrassSlot:
    //    copy_ChrGrassSlot((ChrGrassSlot*)*to, (ChrGrassSlot*)from, to_game);
    //    break;
    case TypeChrFootEffectSlot:
        copy_ChrFootEffectSlot((ChrFootEffectSlot*)*to, (ChrFootEffectSlot*)from, to_game);
        break;
    case TypeChrRigidOffsetSfxSlot:
        copy_ChrRigidOffsetSfxSlot((ChrRigidOffsetSfxSlot*)*to, (ChrRigidOffsetSfxSlot*)from, to_game);
        break;
    case TypeChrRigidOffsetChrSlot:
        copy_ChrRigidOffsetChrSlot((ChrRigidOffsetChrSlot*)*to, (ChrRigidOffsetChrSlot*)from, to_game);
        break;
    case TypeChrSoulEatSlot:
        copy_ChrSoulEatSlot((ChrSoulEatSlot*)*to, (ChrSoulEatSlot*)from, to_game);
        break;
    case TypeChrSorceryWepSlot:
        copy_ChrSorceryWepSlot((ChrSorceryWepSlot*)*to, (ChrSorceryWepSlot*)from, to_game);
        break;
    case TypeChrLanternSlot:
        copy_ChrLanternSlot((ChrLanternSlot*)*to, (ChrLanternSlot*)from, to_game);
        break;
    case TypeChrSingleOneshotSfxSlot:
        copy_ChrSingleOneshotSfxSlot((ChrSingleOneshotSfxSlot*)*to, (ChrSingleOneshotSfxSlot*)from, to_game);
        break;
    case TypeChrSingleTraceSfxSlot:
        copy_ChrSingleTraceSfxSlot((ChrSingleTraceSfxSlot*)*to, (ChrSingleTraceSfxSlot*)from, to_game);
        break;
    case TypeChrMagicGoodsUseSfxSlot:
        copy_ChrMagicGoodsUseSfxSlot((ChrMagicGoodsUseSfxSlot*)*to, (ChrMagicGoodsUseSfxSlot*)from, to_game);
        break;
    case TypeChrActPntSlot:
        copy_ChrActPntSlot((ChrActPntSlot**)to, (ChrActPntSlot*)from, to_game);
        break;
    case TypeChrPointLightSlot:
        copy_ChrPointLightSlot((ChrPointLightSlot*)*to, (ChrPointLightSlot*)from, to_game);
        break;
    case TypeChrWepEnchantSlot:
        copy_ChrWepEnchantSlot((ChrWepEnchantSlot*)*to, (ChrWepEnchantSlot*)from, to_game);
        break;
    case TypeChrFallingControlSlot:
        copy_ChrFallingControlSlot((ChrFallingControlSlot*)*to, (ChrFallingControlSlot*)from, to_game);
        break;
    case TypeChrConditionSfxSeSlot:
        copy_ChrConditionSfxSeSlot((ChrConditionSfxSeSlot*)*to, (ChrConditionSfxSeSlot*)from, to_game);
        break;
    //case TypeChrCamouflageSlot:
    //    copy_ChrCamouflageSlot((ChrCamouflageSlot*)*to, (ChrCamouflageSlot*)from, to_game);
    //    break;
    case TypeChrSoulDeadSlot:
        copy_ChrSoulDeadSlot((ChrSoulDeadSlot*)*to, (ChrSoulDeadSlot*)from, to_game);
        break;
    case TypeChrShinpanshaHaraSlot:
        copy_ChrShinpanshaHaraSlot((ChrShinpanshaHaraSlot*)*to, (ChrShinpanshaHaraSlot*)from, to_game);
        break;
    case TypeChrLimitInvincibleSlot:
        copy_ChrLimitInvincibleSlot((ChrLimitInvincibleSlot*)*to, (ChrLimitInvincibleSlot*)from, to_game);
        break;
    case TypeChrTravelItemSlot:
        copy_ChrTravelItemSlot((ChrTravelItemSlot*)*to, (ChrTravelItemSlot*)from, to_game);
        break;
    case TypeChrStatueDeadSlot:
        copy_ChrStatueDeadSlot((ChrStatueDeadSlot*)*to, (ChrStatueDeadSlot*)from, to_game);
        break;
    case TypeChrResonanceMagicSlot:
        copy_ChrResonanceMagicSlot((ChrResonanceMagicSlot*)*to, (ChrResonanceMagicSlot*)from, to_game);
        break;
    case TypeChrRetributionMagicSlot:
        copy_ChrRetributionMagicSlot((ChrRetributionMagicSlot*)*to, (ChrRetributionMagicSlot*)from, to_game);
        break;
    case TypeChrPlayerResidentSlot:
        copy_ChrPlayerResidentSlot((ChrPlayerResidentSlot*)*to, (ChrPlayerResidentSlot*)from, to_game);
        break;
    case TypeChrFollowSfxSlot:
        copy_ChrFollowSfxSlot((ChrFollowSfxSlot*)*to, (ChrFollowSfxSlot*)from, to_game);
        break;
    default: FATALERROR("Attempted to copy AttachSysSlot of type %d", from->slotType);
    }

    //Copy these after the parent class since the parent class may need to be resized, and thus change
    (*to)->vtable = from->vtable;
    (*to)->timerId = from->timerId;
    (*to)->slotType = from->slotType;
    (*to)->slotIsUsable = from->slotIsUsable;
    (*to)->data_0 = from->data_0;

    if (from->next != NULL)
    {
        //need to free and realloc the slot since the type will probably differ and thus the struct size will differ
        if ((*to)->next != NULL)
        {
            if (to_game)
            {
                Game::game_free_alt((*to)->next);
            }
            else
            {
                free((*to)->next);
            }
            (*to)->next = NULL;
        }
        (*to)->next = init_AttachSysSlot((AttachSysSlotType)(from->next->slotType), to_game);
        copy_AttachSysSlot(&((*to)->next), from->next, to_game);
    }
    else if (from->next == NULL)
    {
        if ((*to)->next != NULL)
        {
            if (to_game)
            {
                Game::game_free_alt((*to)->next);
            }
            else
            {
                free((*to)->next);
            }
            (*to)->next = NULL;
        }
    }
}

AttachSysSlotBaseImpl* init_AttachSysSlot(AttachSysSlotType type, bool to_game)
{
    size_t struct_size = 0;

    switch (type) {
    case TypeChrShineTreasureSlot:
        struct_size = sizeof(ChrShineTreasureSlot);
        break;
    case TypeChrSingleSeSlot:
        struct_size = sizeof(ChrSingleSeSlot);
        break;
    case TypeChrSingleSfxSlot:
        struct_size = sizeof(ChrSingleSfxSlot);
        break;
    case TypeChrMultiSfxSlot:
        struct_size = sizeof(ChrMultiSfxSlot);
        break;
    case TypeChrBurnSlot:
        struct_size = sizeof(ChrBurnSlot);
        break;
    case TypeChrGasmanSlot:
        struct_size = sizeof(ChrGasmanSlot);
        break;
    case TypeChrGrassSlot:
        struct_size = sizeof(ChrGrassSlot);
        break;
    case TypeChrFootEffectSlot:
        struct_size = sizeof(ChrFootEffectSlot);
        break;
    case TypeChrRigidOffsetSfxSlot:
        struct_size = sizeof(ChrRigidOffsetSfxSlot);
        break;
    case TypeChrRigidOffsetChrSlot:
        struct_size = sizeof(ChrRigidOffsetChrSlot);
        break;
    case TypeChrSoulEatSlot:
        struct_size = sizeof(ChrSoulEatSlot);
        break;
    case TypeChrSorceryWepSlot:
        struct_size = sizeof(ChrSorceryWepSlot);
        break;
    case TypeChrLanternSlot:
        struct_size = sizeof(ChrLanternSlot);
        break;
    case TypeChrSingleOneshotSfxSlot:
        struct_size = sizeof(ChrSingleOneshotSfxSlot);
        break;
    case TypeChrSingleTraceSfxSlot:
        struct_size = sizeof(ChrSingleTraceSfxSlot);
        break;
    case TypeChrMagicGoodsUseSfxSlot:
        struct_size = sizeof(ChrMagicGoodsUseSfxSlot);
        break;
    case TypeChrActPntSlot:
        struct_size = sizeof(ChrActPntSlot);
        break;
    case TypeChrPointLightSlot:
        struct_size = sizeof(ChrPointLightSlot);
        break;
    case TypeChrWepEnchantSlot:
        struct_size = sizeof(ChrWepEnchantSlot);
        break;
    case TypeChrFallingControlSlot:
        struct_size = sizeof(ChrFallingControlSlot);
        break;
    case TypeChrConditionSfxSeSlot:
        struct_size = sizeof(ChrConditionSfxSeSlot);
        break;
    case TypeChrCamouflageSlot:
        struct_size = sizeof(ChrCamouflageSlot);
        break;
    case TypeChrSoulDeadSlot:
        struct_size = sizeof(ChrSoulDeadSlot);
        break;
    case TypeChrShinpanshaHaraSlot:
        struct_size = sizeof(ChrShinpanshaHaraSlot);
        break;
    case TypeChrLimitInvincibleSlot:
        struct_size = sizeof(ChrLimitInvincibleSlot);
        break;
    case TypeChrTravelItemSlot:
        struct_size = sizeof(ChrTravelItemSlot);
        break;
    case TypeChrStatueDeadSlot:
        struct_size = sizeof(ChrStatueDeadSlot);
        break;
    case TypeChrResonanceMagicSlot:
        struct_size = sizeof(ChrResonanceMagicSlot);
        break;
    case TypeChrRetributionMagicSlot:
        struct_size = sizeof(ChrRetributionMagicSlot);
        break;
    case TypeChrPlayerResidentSlot:
        struct_size = sizeof(ChrPlayerResidentSlot);
        break;
    case TypeChrFollowSfxSlot:
        struct_size = sizeof(ChrFollowSfxSlot);
        break;
    default:
        FATALERROR("Attempted to init AttachSysSlot of type %d", type);
    }

    AttachSysSlotBaseImpl* local;
    if (to_game)
    {
        local = (AttachSysSlotBaseImpl*)Game::game_malloc(struct_size, 8, *(uint64_t*)Game::internal_heap_3);
    }
    else
    {
        local = (AttachSysSlotBaseImpl*)malloc_(struct_size);
    }

    local->slotType = type;
    local->next = NULL;

    return local;
}

void free_AttachSysSlot(AttachSysSlotBaseImpl* to)
{
    if (to->next != NULL)
    {
        free_AttachSysSlot(to->next);
    }
    free(to);
}


std::string print_ChrShineTreasureSlot(ChrShineTreasureSlot* to)
{
    std::string out = "ChrShineTreasureSlot\n";
    if (to == NULL)
    {
        return out;
    }

    out += "Unknown data 0:";
    for (size_t i = 0; i < sizeof(to->data_0); i++)
    {
        out += std::to_string(to->data_0[i]);
        out += " ";
    }
    out += "\n";
    out += print_BulletIns_FollowupBullet(&to->bullet);

    return out;
}

void copy_ChrShineTreasureSlot(ChrShineTreasureSlot* to, ChrShineTreasureSlot* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    copy_BulletIns_FollowupBullet(&to->bullet, &from->bullet, to_game);
}

std::string print_ChrSingleSeSlot(ChrSingleSeSlot* to)
{
    std::string out = "ChrSingleSeSlot\n";
    if (to == NULL)
    {
        return out;
    }

    out += "Unknown data 0:";
    for (size_t i = 0; i < sizeof(to->data_0); i++)
    {
        out += std::to_string(to->data_0[i]);
        out += " ";
    }
    out += "\n";

    return out;
}

void copy_ChrSingleSeSlot(ChrSingleSeSlot* to, ChrSingleSeSlot* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
}

std::string print_ChrSingleSfxSlot(ChrSingleSfxSlot* to)
{
    std::string out = "ChrSingleSfxSlot\n";
    if (to == NULL)
    {
        return out;
    }

    out += "Unknown data 0:";
    for (size_t i = 0; i < sizeof(to->data_0); i++)
    {
        out += std::to_string(to->data_0[i]);
        out += " ";
    }
    out += "\n";
    out += print_BulletIns_FollowupBullet(&to->bullet);

    return out;
}

void copy_ChrSingleSfxSlot(ChrSingleSfxSlot* to, ChrSingleSfxSlot* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    copy_BulletIns_FollowupBullet(&to->bullet, &from->bullet, to_game);
}

std::string print_ChrMultiSfxSlot(ChrMultiSfxSlot* to)
{
    std::string out = "ChrMultiSfxSlot\n";
    if (to == NULL)
    {
        return out;
    }

    out += "Unknown data 0:";
    for (size_t i = 0; i < sizeof(to->data_0); i++)
    {
        out += std::to_string(to->data_0[i]);
        out += " ";
    }
    out += "\n";
    out += print_BulletIns_FollowupBullet(to->bullet);

    return out;
}

void copy_ChrMultiSfxSlot(ChrMultiSfxSlot* to, ChrMultiSfxSlot* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    copy_BulletIns_FollowupBullet(to->bullet, from->bullet, to_game);
}

std::string print_ChrBurnSlot(ChrBurnSlot* to)
{
    std::string out = "ChrBurnSlot\n";
    if (to == NULL)
    {
        return out;
    }

    out += "Unknown data 0:";
    for (size_t i = 0; i < sizeof(to->data_0); i++)
    {
        out += std::to_string(to->data_0[i]);
        out += " ";
    }
    out += "\n";

    out += "Unknown data 1:" + std::to_string(to->data_1) + "\n";

    out += "Unknown float list [" + std::to_string(to->floatlist_len) + "]:";
    for (size_t i = 0; i < to->floatlist_len; i++)
    {
        out += std::to_string(to->floatlist[i]);
        out += " ";
    }
    out += "\n";

    out += "Unknown list [" + std::to_string(to->list_len) + "]:";
    for (size_t i = 0; i < to->list_len; i++)
    {
        out += std::to_string(to->list[i].data_0);
        out += ",";
        out += print_BulletIns_FollowupBullet(&to->list[i].bullet);
        out += " ";
    }
    out += "\n";

    out += "Unknown data 2:" + std::to_string(to->data_2) + "\n";

    out += "Unknown data 3:";
    for (size_t i = 0; i < sizeof(to->data_3); i++)
    {
        out += std::to_string(to->data_3[i]);
        out += " ";
    }
    out += "\n";

    return out;
}

void copy_ChrBurnSlot(ChrBurnSlot* to, ChrBurnSlot* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    to->data_1 = from->data_1;

    //float list
    if (from->floatlist == NULL)
    {
        if (to->floatlist != NULL)
        {
            if (to_game)
            {
                Game::game_free(to->floatlist);
            }
            else
            {
                free(to->floatlist);
            }
        }
        to->floatlist = NULL;
    }
    else
    {
        //Resize the list
        size_t from_list_size = from->floatlist_len;
        size_t to_list_size = to->floatlist_len;
        if (to_list_size != from_list_size)
        {
            //allocate all the entities as a block
            if (to_game)
            {
                auto new_floatlist = (float*)Game::game_malloc(sizeof(float) * from_list_size, 0x10, *(uint64_t*)Game::internal_heap_3);
                if (to->floatlist != NULL)
                {
                    Game::game_free(to->list);
                }
                to->floatlist = new_floatlist;
            }
            else
            {
                auto new_floatlist = (float*)malloc_(sizeof(float) * from_list_size);
                if (to->floatlist != NULL)
                {
                    free(to->list);
                }
                to->floatlist = new_floatlist;
            }
        }
        //Copy the entries
        for (size_t list_i = 0; list_i < from->floatlist_len; list_i++)
        {
            to->floatlist[list_i] = from->floatlist[list_i];
        }
    }
    to->floatlist_len = from->floatlist_len;

    //list
    if (from->list == NULL)
    {
        if (to->list != NULL)
        {
            if (to_game)
            {
                Game::game_free(to->list);
            }
            else
            {
                free(to->list);
            }
        }
        to->list = NULL;
    }
    else
    {
        //Resize the list
        size_t from_list_size = from->list_len;
        size_t to_list_size = to->list_len;
        if (to_list_size != from_list_size)
        {
            //allocate all the entities as a block
            if (to_game)
            {
                auto new_list = (ChrBurnSlot_ListElem*)Game::game_malloc(sizeof(ChrBurnSlot_ListElem) * from_list_size, 0x10, *(uint64_t*)Game::internal_heap_3);
                if (to->list != NULL)
                {
                    Game::game_free(to->list);
                }
                to->list = new_list;
            }
            else
            {
                auto new_list = (ChrBurnSlot_ListElem*)malloc_(sizeof(ChrBurnSlot_ListElem) * from_list_size);
                if (to->list != NULL)
                {
                    //This warning is invalid since on first init this if won't get hit, and on later init's the above mallocs it
                    #pragma warning(push)
                    #pragma warning(disable:6001)
                    free(to->list);
                    #pragma warning(pop)
                }
                to->list = new_list;
            }
        }
        //Copy the entries
        for (size_t list_i = 0; list_i < from->list_len; list_i++)
        {
            ChrBurnSlot_ListElem* to_elem = &to->list[list_i];
            ChrBurnSlot_ListElem* from_elem = &from->list[list_i];

            to_elem->data_0 = from_elem->data_0;

            copy_BulletIns_FollowupBullet_Data(&to_elem->bullet, &from_elem->bullet, to_game);
            //set up the next ptr. We can probably ignore prev
            if (from_elem->bullet.next != NULL)
            {
                size_t from_next_offset = ((uint64_t)from_elem->bullet.next) - ((uint64_t)(from->list));
                to_elem->bullet.next = (BulletIns_FollowupBullet*)(((uint64_t)(to->list)) + from_next_offset);
            }
            else
            {
                to_elem->bullet.next = NULL;
            }
            to_elem->bullet.prev = NULL;
        }
    }
    to->list_len = from->list_len;

    to->data_2 = from->data_2;
    memcpy(to->data_3, from->data_3, sizeof(to->data_3));
}

std::string print_ChrGasmanSlot(ChrGasmanSlot* to)
{
    std::string out = "ChrGasmanSlot\n";
    if (to == NULL)
    {
        return out;
    }

    out += "Unknown data 0:";
    for (size_t i = 0; i < sizeof(to->data_0); i++)
    {
        out += std::to_string(to->data_0[i]);
        out += " ";
    }
    out += "\n";

    out += "Unknown list [" + std::to_string(to->list_len) + "]:";
    for (size_t i = 0; i < to->list_len; i++)
    {
        for (size_t j = 0; j < sizeof(to->list[i].data_0); j++)
        {
            out += std::to_string(to->list[i].data_0[j]);
            out += " ";
        }
        out += ",";
        out += print_BulletIns_FollowupBullet(&to->list[i].bullet_a);
        out += ",";
        out += print_BulletIns_FollowupBullet(&to->list[i].bullet_b);
        out += "|";
    }
    out += "\n";

    return out;
}

void copy_ChrGasmanSlot(ChrGasmanSlot* to, ChrGasmanSlot* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));

    if (from->list == NULL)
    {
        if (to->list != NULL)
        {
            if (to_game)
            {
                Game::game_free(to->list);
            }
            else
            {
                free(to->list);
            }
        }
        to->list = NULL;
    }
    else
    {
        //Resize the list
        size_t from_list_size = from->list_len;
        size_t to_list_size = to->list_len;
        if (to_list_size != from_list_size)
        {
            //allocate all the entities as a block
            if (to_game)
            {
                auto new_ChrGasmanSlot_ListElem_list = (ChrGasmanSlot_ListElem*)Game::game_malloc(sizeof(ChrGasmanSlot_ListElem) * from_list_size, 0x10, *(uint64_t*)Game::internal_heap_3);
                if (to->list != NULL)
                {
                    Game::game_free(to->list);
                }
                to->list = new_ChrGasmanSlot_ListElem_list;
            }
            else
            {
                auto new_ChrGasmanSlot_ListElem_list = (ChrGasmanSlot_ListElem*)malloc_(sizeof(ChrGasmanSlot_ListElem) * from_list_size);
                if (to->list != NULL)
                {
                    free(to->list);
                }
                to->list = new_ChrGasmanSlot_ListElem_list;
            }
        }

        //Copy the entries
        for (size_t list_i = 0; list_i < from->list_len; list_i++)
        {
            ChrGasmanSlot_ListElem* to_elem = &to->list[list_i];
            ChrGasmanSlot_ListElem* from_elem = &from->list[list_i];

            memcpy(to_elem->data_0, from_elem->data_0, sizeof(to_elem->data_0));

            copy_BulletIns_FollowupBullet_Data(&to_elem->bullet_a, &from_elem->bullet_a, to_game);
            //set up the next ptr. We can probably ignore prev
            if (from_elem->bullet_a.next != NULL)
            {
                size_t from_next_offset = ((uint64_t)from_elem->bullet_a.next) - ((uint64_t)(from->list));
                to_elem->bullet_a.next = (BulletIns_FollowupBullet*)(((uint64_t)(to->list)) + from_next_offset);
            }
            else
            {
                to_elem->bullet_a.next = NULL;
            }
            to_elem->bullet_a.prev = NULL;

            copy_BulletIns_FollowupBullet_Data(&to_elem->bullet_b, &from_elem->bullet_b, to_game);
            if (from_elem->bullet_b.next != NULL)
            {
                size_t from_next_offset = ((uint64_t)from_elem->bullet_b.next) - ((uint64_t)(from->list));
                to_elem->bullet_b.next = (BulletIns_FollowupBullet*)(((uint64_t)(to->list)) + from_next_offset);
            }
            else
            {
                to_elem->bullet_b.next = NULL;
            }
            to_elem->bullet_b.prev = NULL;
        }
    }
    to->list_len = from->list_len;
}

std::string print_ChrGrassSlot(ChrGrassSlot* to)
{
    std::string out = "ChrGrassSlot\n";
    if (to == NULL)
    {
        return out;
    }

    return out;
}

void copy_ChrGrassSlot(ChrGrassSlot* to, ChrGrassSlot* from, bool to_game)
{

}

std::string print_ChrFootEffectSlot(ChrFootEffectSlot* to)
{
    std::string out = "ChrFootEffectSlot\n";
    if (to == NULL)
    {
        return out;
    }

    out += "Unknown data 0:";
    for (size_t i = 0; i < sizeof(to->data_0); i++)
    {
        out += std::to_string(to->data_0[i]);
        out += " ";
    }
    out += "\n";
    out += print_BulletIns_FollowupBullet(to->bullet);

    return out;
}

void copy_ChrFootEffectSlot(ChrFootEffectSlot* to, ChrFootEffectSlot* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    copy_BulletIns_FollowupBullet(to->bullet, from->bullet, to_game);
}

std::string print_ChrRigidOffsetSfxSlot(ChrRigidOffsetSfxSlot* to)
{
    std::string out = "ChrRigidOffsetSfxSlot\n";
    if (to == NULL)
    {
        return out;
    }

    out += "Unknown data 0:";
    for (size_t i = 0; i < sizeof(to->data_0); i++)
    {
        out += std::to_string(to->data_0[i]);
        out += " ";
    }
    out += "\n";
    out += print_BulletIns_FollowupBullet(&to->bullet);

    return out;
}

void copy_ChrRigidOffsetSfxSlot(ChrRigidOffsetSfxSlot* to, ChrRigidOffsetSfxSlot* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    copy_BulletIns_FollowupBullet(&to->bullet, &from->bullet, to_game);
}

std::string print_ChrRigidOffsetChrSlot(ChrRigidOffsetChrSlot* to)
{
    std::string out = "ChrRigidOffsetChrSlot\n";
    if (to == NULL)
    {
        return out;
    }

    out += "Unknown data 0:";
    for (size_t i = 0; i < sizeof(to->data_0); i++)
    {
        out += std::to_string(to->data_0[i]);
        out += " ";
    }
    out += "\n";

    return out;
}

void copy_ChrRigidOffsetChrSlot(ChrRigidOffsetChrSlot* to, ChrRigidOffsetChrSlot* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
}

std::string print_ChrSoulEatSlot(ChrSoulEatSlot* to)
{
    std::string out = "ChrSoulEatSlot\n";
    if (to == NULL)
    {
        return out;
    }

    out += "Unknown data 0:";
    for (size_t i = 0; i < sizeof(to->data_0); i++)
    {
        out += std::to_string(to->data_0[i]);
        out += " ";
    }
    out += "\n";
    out += print_BulletIns_FollowupBullet(&to->bullet);

    return out;
}

void copy_ChrSoulEatSlot(ChrSoulEatSlot* to, ChrSoulEatSlot* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    copy_BulletIns_FollowupBullet(&to->bullet, &from->bullet, to_game);
}

std::string print_ChrSorceryWepSlot(ChrSorceryWepSlot* to)
{
    std::string out = "ChrSorceryWepSlot\n";
    if (to == NULL)
    {
        return out;
    }

    out += "Unknown data 0:";
    for (size_t i = 0; i < sizeof(to->data_0); i++)
    {
        out += std::to_string(to->data_0[i]);
        out += " ";
    }
    out += "\n";

    out += print_BulletIns_FollowupBullet(&to->bullet1);

    out += "Unknown data 1:" + std::to_string(to->data_1) + "\n";

    out += print_BulletIns_FollowupBullet(&to->bullet2);

    return out;
}

void copy_ChrSorceryWepSlot(ChrSorceryWepSlot* to, ChrSorceryWepSlot* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    copy_BulletIns_FollowupBullet(&to->bullet1, &from->bullet1, to_game);
    to->data_1 = from->data_1;
    copy_BulletIns_FollowupBullet(&to->bullet2, &from->bullet2, to_game);
}

std::string print_ChrLanternSlot(ChrLanternSlot* to)
{
    std::string out = "ChrLanternSlot\n";
    if (to == NULL)
    {
        return out;
    }

    out += "Unknown data 0:";
    for (size_t i = 0; i < sizeof(to->data_0); i++)
    {
        out += std::to_string(to->data_0[i]);
        out += " ";
    }
    out += "\n";

    out += print_BulletIns_FollowupBullet(&to->bullet1);
    out += print_BulletIns_FollowupBullet(&to->bullet2);
    out += print_BulletIns_FollowupBullet(&to->bullet3);

    return out;
}

void copy_ChrLanternSlot(ChrLanternSlot* to, ChrLanternSlot* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    copy_BulletIns_FollowupBullet(&to->bullet1, &from->bullet1, to_game);
    copy_BulletIns_FollowupBullet(&to->bullet2, &from->bullet2, to_game);
    copy_BulletIns_FollowupBullet(&to->bullet3, &from->bullet3, to_game);
}

std::string print_ChrSingleOneshotSfxSlot(ChrSingleOneshotSfxSlot* to)
{
    std::string out = "ChrSingleOneshotSfxSlot\n";
    if (to == NULL)
    {
        return out;
    }

    out += "Unknown data 0:";
    for (size_t i = 0; i < sizeof(to->data_0); i++)
    {
        out += std::to_string(to->data_0[i]);
        out += " ";
    }
    out += "\n";

    out += print_BulletIns_FollowupBullet(&to->bullet);

    return out;
}

void copy_ChrSingleOneshotSfxSlot(ChrSingleOneshotSfxSlot* to, ChrSingleOneshotSfxSlot* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    copy_BulletIns_FollowupBullet(&to->bullet, &from->bullet, to_game);
}

std::string print_ChrSingleTraceSfxSlot(ChrSingleTraceSfxSlot* to)
{
    std::string out = "ChrSingleTraceSfxSlot\n";
    if (to == NULL)
    {
        return out;
    }

    out += "Unknown data 0:";
    for (size_t i = 0; i < sizeof(to->data_0); i++)
    {
        out += std::to_string(to->data_0[i]);
        out += " ";
    }
    out += "\n";

    out += print_BulletIns_FollowupBullet(&to->bullet);

    return out;
}

void copy_ChrSingleTraceSfxSlot(ChrSingleTraceSfxSlot* to, ChrSingleTraceSfxSlot* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    copy_BulletIns_FollowupBullet(&to->bullet, &from->bullet, to_game);
}

std::string print_ChrMagicGoodsUseSfxSlot(ChrMagicGoodsUseSfxSlot* to)
{
    std::string out = "ChrMagicGoodsUseSfxSlot\n";
    if (to == NULL)
    {
        return out;
    }

    out += "Unknown data 0:";
    for (size_t i = 0; i < sizeof(to->data_0); i++)
    {
        out += std::to_string(to->data_0[i]);
        out += " ";
    }
    out += "\n";

    out += print_BulletIns_FollowupBullet(&to->bullet);

    return out;
}

void copy_ChrMagicGoodsUseSfxSlot(ChrMagicGoodsUseSfxSlot* to, ChrMagicGoodsUseSfxSlot* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    copy_BulletIns_FollowupBullet(&to->bullet, &from->bullet, to_game);
}

std::string print_ChrActPntSlot_ArrayElem(ChrActPntSlot_ArrayElem* to)
{
    std::string out = "ChrActPntSlot\n";
    if (to == NULL)
    {
        return out;
    }

    out += "Unknown data 0:" + std::to_string(to->data_0) + "\n";
    out += "ActPntManImp_Entry:" + std::to_string((uint64_t)to->ActPntManImp_Entry) + "\n";

    return out;
}

void copy_ChrActPntSlot_ArrayElem(ChrActPntSlot_ArrayElem* to, ChrActPntSlot_ArrayElem* from, bool to_game)
{
    to->data_0 = from->data_0;
    to->ActPntManImp_Entry = from->ActPntManImp_Entry;
}

std::string print_ChrActPntSlot(ChrActPntSlot* to)
{
    std::string out = "ChrActPntSlot\n";
    if (to == NULL)
    {
        return out;
    }

    out += "Unknown data 0:" + std::to_string(to->data_0) + "\n";
    out += "Unknown data 1:" + std::to_string(to->data_1) + "\n";

    out += "Array [" + std::to_string(to->array_len) + "]:\n";
    for (int16_t i = 0; i < to->array_len; i++)
    {
        out += print_ChrActPntSlot_ArrayElem(&to->arry[i]);
    }

    return out;
}

void copy_ChrActPntSlot(ChrActPntSlot** to, ChrActPntSlot* from, bool to_game)
{
    if (!to_game && from->array_len > sizeof(from->arry)/sizeof(from->arry[0]))
    {
        FATALERROR("ChrActPntSlot arry larger then preallocated max. Size is %d", (*to)->array_len);
    }
    if (to_game && from->array_len > (*to)->array_len)
    {
        //increase game alloc size
        ChrActPntSlot* new_arry = (ChrActPntSlot*)Game::game_malloc(0x28 + sizeof(ChrActPntSlot_ArrayElem) * from->array_len, 0x10, *(uint64_t*)Game::internal_heap_3);
        Game::game_free_alt(*to);
        //set the required base object values
        new_arry->base.slotType = TypeChrActPntSlot;
        new_arry->base.next = NULL;

        *to = new_arry;
    }
    for (int16_t i = 0; i < from->array_len; i++)
    {
        copy_ChrActPntSlot_ArrayElem(&((*to)->arry[i]), &from->arry[i], to_game);
    }

    (*to)->data_0 = from->data_0;
    (*to)->data_1 = from->data_1;
    (*to)->array_len = from->array_len;
}

std::string print_ChrPointLightSlot(ChrPointLightSlot* to)
{
    std::string out = "ChrPointLightSlot\n";
    if (to == NULL)
    {
        return out;
    }

    out += "Unknown data 0:";
    for (size_t i = 0; i < sizeof(to->data_0); i++)
    {
        out += std::to_string(to->data_0[i]);
        out += " ";
    }
    out += "\n";

    return out;
}

void copy_ChrPointLightSlot(ChrPointLightSlot* to, ChrPointLightSlot* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
}

std::string print_ChrWepEnchantSlot(ChrWepEnchantSlot* to)
{
    std::string out = "ChrWepEnchantSlot\n";
    if (to == NULL)
    {
        return out;
    }

    out += "Unknown data 0:";
    for (size_t i = 0; i < sizeof(to->data_0); i++)
    {
        out += std::to_string(to->data_0[i]);
        out += " ";
    }
    out += "\n";

    out += "followup_bullet_list [" + std::to_string(to->followup_bullet_list_len) + "]:";
    for (size_t i = 0; i < to->followup_bullet_list_len; i++)
    {
        out += print_BulletIns_FollowupBullet(&to->followup_bullet_list[i]);
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

    return out;
}

void copy_ChrWepEnchantSlot(ChrWepEnchantSlot* to, ChrWepEnchantSlot* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));

    copy_BulletIns_FollowupBullet_List(&to->followup_bullet_list, &to->followup_bullet_list_len, &from->followup_bullet_list, &from->followup_bullet_list_len, to_game);

    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
}

std::string print_ChrFallingControlSlot(ChrFallingControlSlot* to)
{
    std::string out = "ChrFallingControlSlot\n";
    if (to == NULL)
    {
        return out;
    }

    out += "Unknown data 0:" + std::to_string(to->data_0) + "\n";

    return out;
}

void copy_ChrFallingControlSlot(ChrFallingControlSlot* to, ChrFallingControlSlot* from, bool to_game)
{
    to->data_0 = from->data_0;
}

std::string print_ChrConditionSfxSeSlot(ChrConditionSfxSeSlot* to)
{
    std::string out = "ChrConditionSfxSeSlot\n";
    if (to == NULL)
    {
        return out;
    }

    out += "Unknown data 0:";
    for (size_t i = 0; i < sizeof(to->data_0); i++)
    {
        out += std::to_string(to->data_0[i]);
        out += " ";
    }
    out += "\n";

    out += "followup_bullet_list [" + std::to_string(to->followupbullet_list_len) + "]:";
    for (size_t i = 0; i < to->followupbullet_list_len; i++)
    {
        out += print_BulletIns_FollowupBullet(&to->followupbullet_list[i]);
        out += " ";
    }
    out += "\n";

    out += "Unknown data 1:" + std::to_string(to->data_1) + "\n";

    return out;
}

void copy_ChrConditionSfxSeSlot(ChrConditionSfxSeSlot* to, ChrConditionSfxSeSlot* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));

    copy_BulletIns_FollowupBullet_List(&to->followupbullet_list, &to->followupbullet_list_len, &from->followupbullet_list, &from->followupbullet_list_len, to_game);

    to->data_1 = from->data_1;
}

std::string print_ChrCamouflageSlot(ChrCamouflageSlot* to)
{
    std::string out = "ChrCamouflageSlot\n";
    if (to == NULL)
    {
        return out;
    }

    return out;
}

void copy_ChrCamouflageSlot(ChrCamouflageSlot* to, ChrCamouflageSlot* from, bool to_game)
{

}

std::string print_ChrSoulDeadSlot(ChrSoulDeadSlot* to)
{
    std::string out = "ChrSoulDeadSlot\n";
    if (to == NULL)
    {
        return out;
    }

    out += "Unknown data 0:" + std::to_string(to->data_0) + "\n";

    return out;
}

void copy_ChrSoulDeadSlot(ChrSoulDeadSlot* to, ChrSoulDeadSlot* from, bool to_game)
{
    to->data_0 = from->data_0;
}

std::string print_ChrShinpanshaHaraSlot(ChrShinpanshaHaraSlot* to)
{
    std::string out = "ChrShinpanshaHaraSlot\n";
    if (to == NULL)
    {
        return out;
    }

    out += "Unknown data 0:";
    for (size_t i = 0; i < sizeof(to->data_0); i++)
    {
        out += std::to_string(to->data_0[i]);
        out += " ";
    }
    out += "\n";

    out += print_BulletIns_FollowupBullet(&to->bullet);

    return out;
}

void copy_ChrShinpanshaHaraSlot(ChrShinpanshaHaraSlot* to, ChrShinpanshaHaraSlot* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    copy_BulletIns_FollowupBullet(&to->bullet, &from->bullet, to_game);
}

std::string print_ChrLimitInvincibleSlot(ChrLimitInvincibleSlot* to)
{
    std::string out = "ChrLimitInvincibleSlot\n";
    if (to == NULL)
    {
        return out;
    }

    out += "Unknown data 0:" + std::to_string(to->data_0) + "\n";
    out += "ChrIns:" + std::to_string((uint64_t)(to->chr)) + "\n";

    return out;
}

void copy_ChrLimitInvincibleSlot(ChrLimitInvincibleSlot* to, ChrLimitInvincibleSlot* from, bool to_game)
{
    to->data_0 = from->data_0;
    to->chr = from->chr;
}

std::string print_ChrTravelItemSlot(ChrTravelItemSlot* to)
{
    std::string out = "ChrTravelItemSlot\n";
    if (to == NULL)
    {
        return out;
    }

    out += "Unknown data 0:" + std::to_string(to->data_0) + "\n";

    return out;
}

void copy_ChrTravelItemSlot(ChrTravelItemSlot* to, ChrTravelItemSlot* from, bool to_game)
{
    to->data_0 = from->data_0;
}

std::string print_ChrStatueDeadSlot(ChrStatueDeadSlot* to)
{
    std::string out = "ChrStatueDeadSlot\n";
    if (to == NULL)
    {
        return out;
    }

    out += "Unknown data 0:" + std::to_string(to->data_0) + "\n";

    out += "followup_bullet_list [" + std::to_string(to->followup_bullet_list_len) + "]:";
    for (size_t i = 0; i < to->followup_bullet_list_len; i++)
    {
        out += print_BulletIns_FollowupBullet(&to->followup_bullet_list[i]);
        out += " ";
    }
    out += "\n";

    return out;
}

void copy_ChrStatueDeadSlot(ChrStatueDeadSlot* to, ChrStatueDeadSlot* from, bool to_game)
{
    to->data_0 = from->data_0;
    int16_t to_followup_bullet_list_len = to->followup_bullet_list_len;
    int16_t from_followup_bullet_list_len = from->followup_bullet_list_len;
    copy_BulletIns_FollowupBullet_List(&to->followup_bullet_list, &to_followup_bullet_list_len, &from->followup_bullet_list, &from_followup_bullet_list_len, to_game);
    to->followup_bullet_list_len = to_followup_bullet_list_len;
    from->followup_bullet_list_len = from_followup_bullet_list_len;
}

std::string print_ChrResonanceMagicSlot(ChrResonanceMagicSlot* to)
{
    std::string out = "ChrResonanceMagicSlot\n";
    if (to == NULL)
    {
        return out;
    }

    out += "Unknown data 0:";
    for (size_t i = 0; i < sizeof(to->data_0); i++)
    {
        out += std::to_string(to->data_0[i]);
        out += " ";
    }
    out += "\n";

    return out;
}

void copy_ChrResonanceMagicSlot(ChrResonanceMagicSlot* to, ChrResonanceMagicSlot* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
}

std::string print_ChrRetributionMagicSlot(ChrRetributionMagicSlot* to)
{
    std::string out = "ChrRetributionMagicSlot\n";
    if (to == NULL)
    {
        return out;
    }

    out += "Unknown data 0:";
    for (size_t i = 0; i < sizeof(to->data_0); i++)
    {
        out += std::to_string(to->data_0[i]);
        out += " ";
    }
    out += "\n";

    return out;
}

void copy_ChrRetributionMagicSlot(ChrRetributionMagicSlot* to, ChrRetributionMagicSlot* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
}

std::string print_ChrPlayerResidentSlot(ChrPlayerResidentSlot* to)
{
    std::string out = "ChrPlayerResidentSlot\n";
    if (to == NULL)
    {
        return out;
    }

    out += "Unknown data 0:" + std::to_string(to->data_0) + "\n";
    out += "Unknown data 1:" + std::to_string(to->data_1) + "\n";
    out += "Unknown data 2:" + std::to_string(to->data_2[0]) + "\n";
    out += "Unknown data 2:" + std::to_string(to->data_2[1]) + "\n";
    out += "Unknown data 2:" + std::to_string(to->data_2[2]) + "\n";
    out += "Unknown data 2:" + std::to_string(to->data_2[3]) + "\n";

    return out;
}

void copy_ChrPlayerResidentSlot(ChrPlayerResidentSlot* to, ChrPlayerResidentSlot* from, bool to_game)
{
    to->data_0 = from->data_0;
    to->data_1 = from->data_1;
    memcpy(to->data_2, from->data_2, sizeof(to->data_2));
}

std::string print_ChrFollowSfxSlot(ChrFollowSfxSlot* to)
{
    std::string out = "ChrFollowSfxSlot\n";
    if (to == NULL)
    {
        return out;
    }

    out += "Unknown data 0:" + std::to_string(to->data_0) + "\n";

    out += print_BulletIns_FollowupBullet(&to->bullet);

    out += "Unknown data 1:";
    for (size_t i = 0; i < sizeof(to->data_1); i++)
    {
        out += std::to_string(to->data_1[i]);
        out += " ";
    }
    out += "\n";

    return out;
}

void copy_ChrFollowSfxSlot(ChrFollowSfxSlot* to, ChrFollowSfxSlot* from, bool to_game)
{
    to->data_0 = from->data_0;
    copy_BulletIns_FollowupBullet(&to->bullet, &from->bullet, to_game);
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
}
