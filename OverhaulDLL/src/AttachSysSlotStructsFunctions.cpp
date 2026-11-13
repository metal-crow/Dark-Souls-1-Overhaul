#include "AttachSysSlotStructsFunctions.h"
#include "BulletManStructFunctions.h"
#include "SfxManStructFunctions.h"
#include "Rollback.h"

//Need to to be a ** since ChrActPntSlot may need to resize it
//If any called func it does resize it, it must ensure that the AttachSysSlotBaseImpl base obj is correctly init'd
//For now only copy_ChrActPntSlot uses this
void copy_AttachSysSlot(AttachSysSlotBaseImpl** to, AttachSysSlotBaseImpl* from, StateTarget target)
{
    switch (from->slotType)
    {
    case TypeChrShineTreasureSlot:
        copy_ChrShineTreasureSlot((ChrShineTreasureSlot*)*to, (ChrShineTreasureSlot*)from, target);
        break;
    case TypeChrSingleSeSlot:
        copy_ChrSingleSeSlot((ChrSingleSeSlot*)*to, (ChrSingleSeSlot*)from, target);
        break;
    case TypeChrSingleSfxSlot:
        copy_ChrSingleSfxSlot((ChrSingleSfxSlot*)*to, (ChrSingleSfxSlot*)from, target);
        break;
    case TypeChrMultiSfxSlot:
        copy_ChrMultiSfxSlot((ChrMultiSfxSlot*)*to, (ChrMultiSfxSlot*)from, target);
        break;
    case TypeChrBurnSlot:
        copy_ChrBurnSlot((ChrBurnSlot*)*to, (ChrBurnSlot*)from, target);
        break;
    case TypeChrGasmanSlot:
        copy_ChrGasmanSlot((ChrGasmanSlot*)*to, (ChrGasmanSlot*)from, target);
        break;
    //case TypeChrGrassSlot:
    //    copy_ChrGrassSlot((ChrGrassSlot*)*to, (ChrGrassSlot*)from, target);
    //    break;
    case TypeChrFootEffectSlot:
        copy_ChrFootEffectSlot((ChrFootEffectSlot*)*to, (ChrFootEffectSlot*)from, target);
        break;
    case TypeChrRigidOffsetSfxSlot:
        copy_ChrRigidOffsetSfxSlot((ChrRigidOffsetSfxSlot*)*to, (ChrRigidOffsetSfxSlot*)from, target);
        break;
    case TypeChrRigidOffsetChrSlot:
        copy_ChrRigidOffsetChrSlot((ChrRigidOffsetChrSlot*)*to, (ChrRigidOffsetChrSlot*)from, target);
        break;
    case TypeChrSoulEatSlot:
        copy_ChrSoulEatSlot((ChrSoulEatSlot*)*to, (ChrSoulEatSlot*)from, target);
        break;
    case TypeChrSorceryWepSlot:
        copy_ChrSorceryWepSlot((ChrSorceryWepSlot*)*to, (ChrSorceryWepSlot*)from, target);
        break;
    case TypeChrLanternSlot:
        copy_ChrLanternSlot((ChrLanternSlot*)*to, (ChrLanternSlot*)from, target);
        break;
    case TypeChrSingleOneshotSfxSlot:
        copy_ChrSingleOneshotSfxSlot((ChrSingleOneshotSfxSlot*)*to, (ChrSingleOneshotSfxSlot*)from, target);
        break;
    case TypeChrSingleTraceSfxSlot:
        copy_ChrSingleTraceSfxSlot((ChrSingleTraceSfxSlot*)*to, (ChrSingleTraceSfxSlot*)from, target);
        break;
    case TypeChrMagicGoodsUseSfxSlot:
        copy_ChrMagicGoodsUseSfxSlot((ChrMagicGoodsUseSfxSlot*)*to, (ChrMagicGoodsUseSfxSlot*)from, target);
        break;
    case TypeChrActPntSlot:
        copy_ChrActPntSlot((ChrActPntSlot**)to, (ChrActPntSlot*)from, target);
        break;
    case TypeChrPointLightSlot:
        copy_ChrPointLightSlot((ChrPointLightSlot*)*to, (ChrPointLightSlot*)from, target);
        break;
    case TypeChrWepEnchantSlot:
        copy_ChrWepEnchantSlot((ChrWepEnchantSlot*)*to, (ChrWepEnchantSlot*)from, target);
        break;
    case TypeChrFallingControlSlot:
        copy_ChrFallingControlSlot((ChrFallingControlSlot*)*to, (ChrFallingControlSlot*)from, target);
        break;
    case TypeChrConditionSfxSeSlot:
        copy_ChrConditionSfxSeSlot((ChrConditionSfxSeSlot*)*to, (ChrConditionSfxSeSlot*)from, target);
        break;
    //case TypeChrCamouflageSlot:
    //    copy_ChrCamouflageSlot((ChrCamouflageSlot*)*to, (ChrCamouflageSlot*)from, target);
    //    break;
    case TypeChrSoulDeadSlot:
        copy_ChrSoulDeadSlot((ChrSoulDeadSlot*)*to, (ChrSoulDeadSlot*)from, target);
        break;
    case TypeChrShinpanshaHaraSlot:
        copy_ChrShinpanshaHaraSlot((ChrShinpanshaHaraSlot*)*to, (ChrShinpanshaHaraSlot*)from, target);
        break;
    case TypeChrLimitInvincibleSlot:
        copy_ChrLimitInvincibleSlot((ChrLimitInvincibleSlot*)*to, (ChrLimitInvincibleSlot*)from, target);
        break;
    case TypeChrTravelItemSlot:
        copy_ChrTravelItemSlot((ChrTravelItemSlot*)*to, (ChrTravelItemSlot*)from, target);
        break;
    case TypeChrStatueDeadSlot:
        copy_ChrStatueDeadSlot((ChrStatueDeadSlot*)*to, (ChrStatueDeadSlot*)from, target);
        break;
    case TypeChrResonanceMagicSlot:
        copy_ChrResonanceMagicSlot((ChrResonanceMagicSlot*)*to, (ChrResonanceMagicSlot*)from, target);
        break;
    case TypeChrRetributionMagicSlot:
        copy_ChrRetributionMagicSlot((ChrRetributionMagicSlot*)*to, (ChrRetributionMagicSlot*)from, target);
        break;
    case TypeChrPlayerResidentSlot:
        copy_ChrPlayerResidentSlot((ChrPlayerResidentSlot*)*to, (ChrPlayerResidentSlot*)from, target);
        break;
    case TypeChrFollowSfxSlot:
        copy_ChrFollowSfxSlot((ChrFollowSfxSlot*)*to, (ChrFollowSfxSlot*)from, target);
        break;
    default: FATALERROR("Attempted to copy AttachSysSlot of type %d", from->slotType);
    }

    //Copy these after the parent class since the parent class may need to be resized, and thus change
    (*to)->vtable = from->vtable;
    (*to)->timerId = from->timerId;
    (*to)->slotType = from->slotType;
    (*to)->slotIsUsable = from->slotIsUsable;
    (*to)->unk_c = from->unk_c;

    if (from->next != NULL)
    {
        //need to free and realloc the slot since the type will probably differ and thus the struct size will differ
        if ((*to)->next != NULL)
        {
            //free_AttachSysSlot frees the whole chain + any embedded dynamic arrays
            free_AttachSysSlot((*to)->next, target);
            (*to)->next = NULL;
        }
        (*to)->next = init_AttachSysSlot((AttachSysSlotType)(from->next->slotType), target);
        copy_AttachSysSlot(&((*to)->next), from->next, target);
    }
    else if (from->next == NULL)
    {
        if ((*to)->next != NULL)
        {
            free_AttachSysSlot((*to)->next, target);
            (*to)->next = NULL;
        }
    }
}

AttachSysSlotBaseImpl* init_AttachSysSlot(AttachSysSlotType type, StateTarget target)
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
    if (target == StateTarget::ToGame)
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

//Frees any dynamic arrays embedded in an AttachSysSlot, dispatching on slotType.
//Must be called before freeing the slot itself so embedded allocations don't leak.
//target tells us whether the slot (and thus its embedded arrays) live in game memory or MSVC memory.
void free_AttachSysSlot_Fields(AttachSysSlotBaseImpl* to, StateTarget target)
{
    if (to == NULL)
    {
        return;
    }
    switch (to->slotType)
    {
    case TypeChrMultiSfxSlot:
    {
        ChrMultiSfxSlot* slot = (ChrMultiSfxSlot*)to;
        free_SfxHandle_List(&slot->sfx_list, &slot->sfx_list_len, target);
        break;
    }
    case TypeChrWepEnchantSlot:
    {
        ChrWepEnchantSlot* slot = (ChrWepEnchantSlot*)to;
        free_SfxHandle_List(&slot->sfx_list, &slot->sfx_list_len, target);
        break;
    }
    case TypeChrStatueDeadSlot:
    {
        ChrStatueDeadSlot* slot = (ChrStatueDeadSlot*)to;
        int16_t len = (int16_t)slot->sfx_list_len;
        free_SfxHandle_List(&slot->sfx_list, &len, target);
        slot->sfx_list_len = len;
        break;
    }
    case TypeChrConditionSfxSeSlot:
    {
        ChrConditionSfxSeSlot* slot = (ChrConditionSfxSeSlot*)to;
        free_SfxHandle_List(&slot->sfx_list, &slot->sfx_list_len, target);
        break;
    }
    case TypeChrBurnSlot:
    {
        ChrBurnSlot* slot = (ChrBurnSlot*)to;
        if (slot->floatlist != NULL)
        {
            if (target == StateTarget::ToGame) { Game::game_free(slot->floatlist); }
            else { free(slot->floatlist); }
            slot->floatlist = NULL;
        }
        slot->floatlist_len = 0;
        if (slot->list != NULL)
        {
            if (target == StateTarget::ToGame) { Game::game_free(slot->list); }
            else { free(slot->list); }
            slot->list = NULL;
        }
        slot->list_len = 0;
        break;
    }
    case TypeChrGasmanSlot:
    {
        ChrGasmanSlot* slot = (ChrGasmanSlot*)to;
        if (slot->list != NULL)
        {
            if (target == StateTarget::ToGame) { Game::game_free(slot->list); }
            else { free(slot->list); }
            slot->list = NULL;
        }
        slot->list_len = 0;
        break;
    }
    default:
        //slot types without dynamic allocations: nothing to free
        break;
    }
}

//Every FX handle a game slot holds leaves its effect's handle list before the slot's memory goes: to the FX system, freeing a
//game slot is destructing it
static void release_AttachSysSlot_SfxHandles(AttachSysSlotBaseImpl* to)
{
    switch (to->slotType)
    {
    case TypeChrShineTreasureSlot: release_SfxHandle(&((ChrShineTreasureSlot*)to)->sfx); break;
    case TypeChrSingleSfxSlot: release_SfxHandle(&((ChrSingleSfxSlot*)to)->sfx); break;
    case TypeChrFootEffectSlot: release_SfxHandle(&((ChrFootEffectSlot*)to)->sfx); break;
    case TypeChrRigidOffsetSfxSlot: release_SfxHandle(&((ChrRigidOffsetSfxSlot*)to)->sfx); break;
    case TypeChrSoulEatSlot: release_SfxHandle(&((ChrSoulEatSlot*)to)->sfx); break;
    case TypeChrSingleOneshotSfxSlot: release_SfxHandle(&((ChrSingleOneshotSfxSlot*)to)->sfx); break;
    case TypeChrSingleTraceSfxSlot: release_SfxHandle(&((ChrSingleTraceSfxSlot*)to)->sfx); break;
    case TypeChrMagicGoodsUseSfxSlot: release_SfxHandle(&((ChrMagicGoodsUseSfxSlot*)to)->sfx); break;
    case TypeChrShinpanshaHaraSlot: release_SfxHandle(&((ChrShinpanshaHaraSlot*)to)->sfx); break;
    case TypeChrFollowSfxSlot: release_SfxHandle(&((ChrFollowSfxSlot*)to)->sfx); break;
    case TypeChrSorceryWepSlot:
        release_SfxHandle(&((ChrSorceryWepSlot*)to)->sfx1);
        release_SfxHandle(&((ChrSorceryWepSlot*)to)->sfx2);
        break;
    case TypeChrLanternSlot:
        release_SfxHandle(&((ChrLanternSlot*)to)->sfx1);
        release_SfxHandle(&((ChrLanternSlot*)to)->sfx2);
        release_SfxHandle(&((ChrLanternSlot*)to)->sfx3);
        break;
    case TypeChrBurnSlot:
    {
        ChrBurnSlot* slot = (ChrBurnSlot*)to;
        for (int32_t i = 0; slot->list != NULL && i < slot->list_len; i++)
        {
            release_SfxHandle(&slot->list[i].sfx);
        }
        break;
    }
    case TypeChrGasmanSlot:
    {
        ChrGasmanSlot* slot = (ChrGasmanSlot*)to;
        for (int16_t i = 0; slot->list != NULL && i < slot->list_len; i++)
        {
            release_SfxHandle(&slot->list[i].sfx_a);
            release_SfxHandle(&slot->list[i].sfx_b);
        }
        break;
    }
    default:
        //the handle lists (ChrMultiSfxSlot, ChrWepEnchantSlot, ChrStatueDeadSlot, ChrConditionSfxSeSlot) are released by
        //free_SfxHandle_List
        break;
    }
}

void free_AttachSysSlot(AttachSysSlotBaseImpl* to, StateTarget target)
{
    if (to == NULL)
    {
        return;
    }
    if (to->next != NULL)
    {
        free_AttachSysSlot(to->next, target);
        to->next = NULL;
    }
    if (target == StateTarget::ToGame)
    {
        release_AttachSysSlot_SfxHandles(to);
    }
    free_AttachSysSlot_Fields(to, target);
    if (target == StateTarget::ToGame)
    {
        Game::game_free_alt(to);
    }
    else
    {
        free(to);
    }
}


void copy_ChrShineTreasureSlot(ChrShineTreasureSlot* to, ChrShineTreasureSlot* from, StateTarget target)
{
    to->unk_20 = from->unk_20;
    to->unk_22 = from->unk_22;
    to->unk_24 = from->unk_24;
    copy_SfxHandle(&to->sfx, &from->sfx, target);
}

void copy_ChrSingleSeSlot(ChrSingleSeSlot* to, ChrSingleSeSlot* from, StateTarget target)
{
    to->unk_20 = from->unk_20;
    to->unk_24 = from->unk_24;
    to->unk_28 = from->unk_28;
    to->unk_2c = from->unk_2c;
    to->unk_30 = from->unk_30;
    to->unk_32 = from->unk_32;
    to->unk_34 = from->unk_34;
    to->unk_38 = from->unk_38;
    to->unk_3c = from->unk_3c;
}

void copy_ChrSingleSfxSlot(ChrSingleSfxSlot* to, ChrSingleSfxSlot* from, StateTarget target)
{
    to->unk_20 = from->unk_20;
    to->ffxid = from->ffxid;
    to->dummyPolyID_B = from->dummyPolyID_B;
    to->dummyPolyID_A = from->dummyPolyID_A;
    to->unk_2c = from->unk_2c;
    to->unk_2e = from->unk_2e;
    to->unk_30 = from->unk_30;
    memcpy(to->unk_32, from->unk_32, sizeof(to->unk_32));
    copy_SfxHandle(&to->sfx, &from->sfx, target);
}

void copy_ChrMultiSfxSlot(ChrMultiSfxSlot* to, ChrMultiSfxSlot* from, StateTarget target)
{
    to->unk_20 = from->unk_20;
    to->ffxid = from->ffxid;
    to->dummyPolyID_B = from->dummyPolyID_B;
    to->dummyPolyID_A = from->dummyPolyID_A;
    to->unk_2c = from->unk_2c;
    copy_SfxHandle_List(&to->sfx_list, &to->sfx_list_len, &from->sfx_list, &from->sfx_list_len, target);
}

void copy_ChrBurnSlot(ChrBurnSlot* to, ChrBurnSlot* from, StateTarget target)
{
    to->unk_20 = from->unk_20;
    to->unk_24 = from->unk_24;
    to->unk_28 = from->unk_28;
    to->unk_2c = from->unk_2c;

    //float list
    if (from->floatlist == NULL)
    {
        if (to->floatlist != NULL)
        {
            if (target == StateTarget::ToGame)
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
            if (target == StateTarget::ToGame)
            {
                auto new_floatlist = (float*)Game::game_malloc(sizeof(float) * from_list_size, 0x10, *(uint64_t*)Game::internal_heap_3);
                if (to->floatlist != NULL)
                {
                    Game::game_free(to->floatlist);
                }
                to->floatlist = new_floatlist;
            }
            else
            {
                auto new_floatlist = (float*)malloc_(sizeof(float) * from_list_size);
                if (to->floatlist != NULL)
                {
                    free(to->floatlist);
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
            if (target == StateTarget::ToGame)
            {
                for (int32_t i = 0; i < to->list_len; i++)
                {
                    release_SfxHandle(&to->list[i].sfx);
                }
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
            if (target == StateTarget::ToGame)
            {
                auto new_list = (ChrBurnSlot_ListElem*)Game::game_malloc(sizeof(ChrBurnSlot_ListElem) * from_list_size, 0x10, *(uint64_t*)Game::internal_heap_3);
                if (to->list != NULL)
                {
                    for (int32_t i = 0; i < to->list_len; i++)
                    {
                        release_SfxHandle(&to->list[i].sfx);
                    }
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

            to_elem->unk_0 = from_elem->unk_0;
            copy_SfxHandle(&to_elem->sfx, &from_elem->sfx, target);
        }
    }
    to->list_len = from->list_len;

    to->unk_3c = from->unk_3c;
    to->unk_48 = from->unk_48;
    memcpy(to->unk_49, from->unk_49, sizeof(to->unk_49));
    to->unk_4c = from->unk_4c;
    to->unk_50 = from->unk_50;
    to->unk_54 = from->unk_54;
}

void copy_ChrGasmanSlot(ChrGasmanSlot* to, ChrGasmanSlot* from, StateTarget target)
{
    to->unk_20 = from->unk_20;
    to->unk_24 = from->unk_24;

    if (from->list == NULL)
    {
        if (to->list != NULL)
        {
            if (target == StateTarget::ToGame)
            {
                for (int16_t i = 0; i < to->list_len; i++)
                {
                    release_SfxHandle(&to->list[i].sfx_a);
                    release_SfxHandle(&to->list[i].sfx_b);
                }
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
            if (target == StateTarget::ToGame)
            {
                auto new_ChrGasmanSlot_ListElem_list = (ChrGasmanSlot_ListElem*)Game::game_malloc(sizeof(ChrGasmanSlot_ListElem) * from_list_size, 0x10, *(uint64_t*)Game::internal_heap_3);
                if (to->list != NULL)
                {
                    for (int16_t i = 0; i < to->list_len; i++)
                    {
                        release_SfxHandle(&to->list[i].sfx_a);
                        release_SfxHandle(&to->list[i].sfx_b);
                    }
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

            to_elem->unk_0 = from_elem->unk_0;
            memcpy(to_elem->unk_1, from_elem->unk_1, sizeof(to_elem->unk_1));
            memcpy(to_elem->vector, from_elem->vector, sizeof(to_elem->vector));
            memcpy(to_elem->unk_1c, from_elem->unk_1c, sizeof(to_elem->unk_1c));

            copy_SfxHandle(&to_elem->sfx_a, &from_elem->sfx_a, target);
            copy_SfxHandle(&to_elem->sfx_b, &from_elem->sfx_b, target);
        }
    }
    to->list_len = from->list_len;
}

void copy_ChrGrassSlot(ChrGrassSlot* to, ChrGrassSlot* from, StateTarget target)
{

}

void copy_ChrFootEffectSlot(ChrFootEffectSlot* to, ChrFootEffectSlot* from, StateTarget target)
{
    to->unk_20 = from->unk_20;
    to->unk_24 = from->unk_24;
    to->unk_26 = from->unk_26;
    to->unk_28 = from->unk_28;
    to->unk_2c = from->unk_2c;
    copy_SfxHandle(&to->sfx, &from->sfx, target);
}

void copy_ChrRigidOffsetSfxSlot(ChrRigidOffsetSfxSlot* to, ChrRigidOffsetSfxSlot* from, StateTarget target)
{
    to->unk_20 = from->unk_20;
    to->unk_24 = from->unk_24;
    to->unk_26 = from->unk_26;
    memcpy(to->unk_28, from->unk_28, sizeof(to->unk_28));
    memcpy(to->fieldE0, from->fieldE0, sizeof(to->fieldE0));
    copy_SfxHandle(&to->sfx, &from->sfx, target);
}

void copy_ChrRigidOffsetChrSlot(ChrRigidOffsetChrSlot* to, ChrRigidOffsetChrSlot* from, StateTarget target)
{
    to->unk_20 = from->unk_20;
    to->unk_24 = from->unk_24;
    to->unk_26 = from->unk_26;
    memcpy(to->unk_28, from->unk_28, sizeof(to->unk_28));
    memcpy(to->fieldE0, from->fieldE0, sizeof(to->fieldE0));
}

void copy_ChrSoulEatSlot(ChrSoulEatSlot* to, ChrSoulEatSlot* from, StateTarget target)
{
    to->unk_20 = from->unk_20;
    memcpy(to->unk_22, from->unk_22, sizeof(to->unk_22));
    to->unk_30 = from->unk_30;
    to->unk_34 = from->unk_34;
    to->unk_38 = from->unk_38;
    to->unk_3c = from->unk_3c;
    copy_SfxHandle(&to->sfx, &from->sfx, target);
}

void copy_ChrSorceryWepSlot(ChrSorceryWepSlot* to, ChrSorceryWepSlot* from, StateTarget target)
{
    to->unk_20 = from->unk_20;
    to->unk_21 = from->unk_21;
    memcpy(to->unk_22, from->unk_22, sizeof(to->unk_22));
    to->unk_28 = from->unk_28;
    to->unk_2c = from->unk_2c;
    copy_SfxHandle(&to->sfx1, &from->sfx1, target);
    to->unk_60 = from->unk_60;
    to->unk_64 = from->unk_64;
    copy_SfxHandle(&to->sfx2, &from->sfx2, target);
}

void copy_ChrLanternSlot(ChrLanternSlot* to, ChrLanternSlot* from, StateTarget target)
{
    to->unk_20 = from->unk_20;
    to->unk_22 = from->unk_22;
    to->unk_24 = from->unk_24;
    copy_SfxHandle(&to->sfx1, &from->sfx1, target);
    copy_SfxHandle(&to->sfx2, &from->sfx2, target);
    copy_SfxHandle(&to->sfx3, &from->sfx3, target);
}

void copy_ChrSingleOneshotSfxSlot(ChrSingleOneshotSfxSlot* to, ChrSingleOneshotSfxSlot* from, StateTarget target)
{
    to->unk_20 = from->unk_20;
    to->unk_24 = from->unk_24;
    to->unk_26 = from->unk_26;
    copy_SfxHandle(&to->sfx, &from->sfx, target);
}

void copy_ChrSingleTraceSfxSlot(ChrSingleTraceSfxSlot* to, ChrSingleTraceSfxSlot* from, StateTarget target)
{
    to->ffxid = from->ffxid;
    to->dummyPolyA = from->dummyPolyA;
    to->dummyPolyB = from->dummyPolyB;
    to->dummyPolySourceInternal = from->dummyPolySourceInternal;
    to->unk_29 = from->unk_29;
    to->unk_2a = from->unk_2a;
    memcpy(to->unk_2c, from->unk_2c, sizeof(to->unk_2c));
    copy_SfxHandle(&to->sfx, &from->sfx, target);
}

void copy_ChrMagicGoodsUseSfxSlot(ChrMagicGoodsUseSfxSlot* to, ChrMagicGoodsUseSfxSlot* from, StateTarget target)
{
    to->ffxid = from->ffxid;
    to->dummyPolyID = from->dummyPolyID;
    to->dummyPolySourceInternal = from->dummyPolySourceInternal;
    to->unk_27 = from->unk_27;
    to->unk_28 = from->unk_28;
    memcpy(to->unk_2a, from->unk_2a, sizeof(to->unk_2a));
    copy_SfxHandle(&to->sfx, &from->sfx, target);
}

void copy_ChrActPntSlot_ArrayElem(ChrActPntSlot_ArrayElem* to, ChrActPntSlot_ArrayElem* from, StateTarget target)
{
    to->unk_0 = from->unk_0;
    to->unk_4 = from->unk_4;
    to->ActPntManImp_Entry = from->ActPntManImp_Entry;
}

void copy_ChrActPntSlot(ChrActPntSlot** to, ChrActPntSlot* from, StateTarget target)
{
    if (target != StateTarget::ToGame && from->array_len > sizeof(from->arry)/sizeof(from->arry[0]))
    {
        FATALERROR("ChrActPntSlot arry larger then preallocated max. Size is %d", (*to)->array_len);
    }
    if (target == StateTarget::ToGame && from->array_len > (*to)->array_len)
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
        copy_ChrActPntSlot_ArrayElem(&((*to)->arry[i]), &from->arry[i], target);
    }

    (*to)->unk_20 = from->unk_20;
    (*to)->unk_24 = from->unk_24;
    (*to)->array_len = from->array_len;
}

void copy_ChrPointLightSlot(ChrPointLightSlot* to, ChrPointLightSlot* from, StateTarget target)
{
    to->unk_20 = from->unk_20;
    to->unk_22 = from->unk_22;
    to->unk_24 = from->unk_24;
    to->unk_26 = from->unk_26;
    to->unk_28 = from->unk_28;
    to->unk_2c = from->unk_2c;
}

void copy_ChrWepEnchantSlot(ChrWepEnchantSlot* to, ChrWepEnchantSlot* from, StateTarget target)
{
    to->unk_20 = from->unk_20;
    to->sfxId = from->sfxId;
    to->unk_28 = from->unk_28;
    to->seId = from->seId;
    to->time_a = from->time_a;
    to->time_b = from->time_b;
    to->unk_38 = from->unk_38;
    to->effectType = from->effectType;
    to->unk_3a = from->unk_3a;
    to->invisible = from->invisible;
    to->unk_3c = from->unk_3c;
    to->unk_3d = from->unk_3d;

    copy_SfxHandle_List(&to->sfx_list, &to->sfx_list_len, &from->sfx_list, &from->sfx_list_len, target);

    to->unk_48 = from->unk_48;
    to->unk_4c = from->unk_4c;
}

void copy_ChrFallingControlSlot(ChrFallingControlSlot* to, ChrFallingControlSlot* from, StateTarget target)
{
    to->unk_20 = from->unk_20;
    memcpy(to->unk_21, from->unk_21, sizeof(to->unk_21));
}

void copy_ChrConditionSfxSeSlot(ChrConditionSfxSeSlot* to, ChrConditionSfxSeSlot* from, StateTarget target)
{
    to->sfxId1 = from->sfxId1;
    to->seId1 = from->seId1;
    to->dummyId1 = from->dummyId1;
    to->unkBitfield = from->unkBitfield;
    to->sfxId2 = from->sfxId2;
    to->seId2 = from->seId2;
    to->dummyId2 = from->dummyId2;

    copy_SfxHandle_List(&to->sfx_list, &to->sfx_list_len, &from->sfx_list, &from->sfx_list_len, target);

    to->unk_40 = from->unk_40;
    to->unk_44 = from->unk_44;
}

void copy_ChrCamouflageSlot(ChrCamouflageSlot* to, ChrCamouflageSlot* from, StateTarget target)
{

}

void copy_ChrSoulDeadSlot(ChrSoulDeadSlot* to, ChrSoulDeadSlot* from, StateTarget target)
{
    to->unk_20 = from->unk_20;
    to->unk_24 = from->unk_24;
}

void copy_ChrShinpanshaHaraSlot(ChrShinpanshaHaraSlot* to, ChrShinpanshaHaraSlot* from, StateTarget target)
{
    to->unk_20 = from->unk_20;
    memcpy(to->unk_21, from->unk_21, sizeof(to->unk_21));
    copy_SfxHandle(&to->sfx, &from->sfx, target);
}

void copy_ChrLimitInvincibleSlot(ChrLimitInvincibleSlot* to, ChrLimitInvincibleSlot* from, StateTarget target)
{
    to->unk_20 = from->unk_20;
    to->unk_24 = from->unk_24;
    to->chr = from->chr;
}

void copy_ChrTravelItemSlot(ChrTravelItemSlot* to, ChrTravelItemSlot* from, StateTarget target)
{
    to->unk_20 = from->unk_20;
    memcpy(to->unk_21, from->unk_21, sizeof(to->unk_21));
}

void copy_ChrStatueDeadSlot(ChrStatueDeadSlot* to, ChrStatueDeadSlot* from, StateTarget target)
{
    to->unk_20 = from->unk_20;
    memcpy(to->unk_21, from->unk_21, sizeof(to->unk_21));
    int16_t to_sfx_list_len = to->sfx_list_len;
    int16_t from_sfx_list_len = from->sfx_list_len;
    copy_SfxHandle_List(&to->sfx_list, &to_sfx_list_len, &from->sfx_list, &from_sfx_list_len, target);
    to->sfx_list_len = to_sfx_list_len;
    from->sfx_list_len = from_sfx_list_len;
}

void copy_ChrResonanceMagicSlot(ChrResonanceMagicSlot* to, ChrResonanceMagicSlot* from, StateTarget target)
{
    to->unk_20 = from->unk_20;
    to->unk_24 = from->unk_24;
    to->unk_28 = from->unk_28;
    memcpy(to->unk_29, from->unk_29, sizeof(to->unk_29));
}

void copy_ChrRetributionMagicSlot(ChrRetributionMagicSlot* to, ChrRetributionMagicSlot* from, StateTarget target)
{
    to->unk_20 = from->unk_20;
    to->unk_24 = from->unk_24;
    to->unk_28 = from->unk_28;
    to->unk_2c = from->unk_2c;
}

void copy_ChrPlayerResidentSlot(ChrPlayerResidentSlot* to, ChrPlayerResidentSlot* from, StateTarget target)
{
    to->playerIns = from->playerIns;
    to->unk_28 = from->unk_28;
    memcpy(to->unk_29, from->unk_29, sizeof(to->unk_29));
    memcpy(to->PlayerGameData_Vec, from->PlayerGameData_Vec, sizeof(to->PlayerGameData_Vec));
}

void copy_ChrFollowSfxSlot(ChrFollowSfxSlot* to, ChrFollowSfxSlot* from, StateTarget target)
{
    to->unk_20 = from->unk_20;
    to->unk_24 = from->unk_24;
    copy_SfxHandle(&to->sfx, &from->sfx, target);
    to->unk_58 = from->unk_58;
    to->unk_5c = from->unk_5c;
    to->unk_60 = from->unk_60;
    to->unk_64 = from->unk_64;
}
