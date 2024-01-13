#include "AttachSysSlotStructsFunctions.h"
#include "Rollback.h"

std::string print_AttachSysSlot(AttachSysSlotBaseImpl* to)
{
    std::string out = "AttachSysSlotBaseImpl\n";

    switch (to->slotType)
    {
    case TypeChrShineTreasureSlot:
        out = "ChrShineTreasureSlot\n";
        break;
    case TypeChrSingleSeSlot:
        out = "ChrSingleSeSlot\n";
        break;
    case TypeChrSingleSfxSlot:
        out = "ChrSingleSfxSlot\n";
        break;
    case TypeChrMultiSfxSlot:
        out = "ChrMultiSfxSlot\n";
        break;
    case TypeChrBurnSlot:
        out = "ChrBurnSlot\n";
        break;
    case TypeChrGasmanSlot:
        out = "ChrGasmanSlot\n";
        break;
    case TypeChrGrassSlot:
        out = "ChrGrassSlot\n";
        break;
    case TypeChrFootEffectSlot:
        out = "ChrFootEffectSlot\n";
        break;
    case TypeChrRigidOffsetSfxSlot:
        out = "ChrRigidOffsetSfxSlot\n";
        break;
    case TypeChrRigidOffsetChrSlot:
        out = "ChrRigidOffsetChrSlot\n";
        break;
    case TypeChrSoulEatSlot:
        out = "ChrSoulEatSlot\n";
        break;
    case TypeChrSorceryWepSlot:
        out = "ChrSorceryWepSlot\n";
        break;
    case TypeChrLanternSlot:
        out = "ChrLanternSlot\n";
        break;
    case TypeChrSingleOneshotSfxSlot:
        out = "ChrSingleOneshotSfxSlot\n";
        break;
    case TypeChrSingleTraceSfxSlot:
        out = "ChrSingleTraceSfxSlot\n";
        break;
    case TypeChrMagicGoodsUseSfxSlot:
        out = "ChrMagicGoodsUseSfxSlot\n";
        break;
    case TypeChrActPntSlot:
        out = "ChrActPntSlot\n";
        break;
    case TypeChrPointLightSlot:
        out = "ChrPointLightSlot\n";
        break;
    case TypeChrWepEnchantSlot:
        out = "ChrWepEnchantSlot\n";
        break;
    case TypeChrFallingControlSlot:
        out = "ChrFallingControlSlot\n";
        break;
    case TypeChrConditionSfxSeSlot:
        out = "ChrConditionSfxSeSlot\n";
        break;
    case TypeChrCamouflageSlot:
        out = "ChrCamouflageSlot\n";
        break;
    case TypeChrSoulDeadSlot:
        out = "ChrSoulDeadSlot\n";
        break;
    case TypeChrShinpanshaHaraSlot:
        out = "ChrShinpanshaHaraSlot\n";
        break;
    case TypeChrLimitInvincibleSlot:
        out = "ChrLimitInvincibleSlot\n";
        break;
    case TypeChrDetectSignSlot:
        out = "ChrDetectSignSlot\n";
        break;
    case TypeChrRescueSignSlot:
        out = "ChrRescueSignSlot\n";
        break;
    case TypeChrTravelItemSlot:
        out = "ChrTravelItemSlot\n";
        break;
    case TypeChrStatueDeadSlot:
        out = "ChrStatueDeadSlot\n";
        break;
    case TypeChrResonanceMagicSlot:
        out = "ChrResonanceMagicSlot\n";
        break;
    case TypeChrRetributionMagicSlot:
        out = "ChrRetributionMagicSlot\n";
        break;
    case TypeChrHellkiteBreathSlot:
        out = "ChrHellkiteBreathSlot\n";
        break;
    case TypeChrColiseumEntrantSlot:
        out = "ChrColiseumEntrantSlot\n";
        break;
    case TypeChrPlayerResidentSlot:
        out = "ChrPlayerResidentSlot\n";
        break;
    case TypeChrFollowSfxSlot:
        out = "ChrFollowSfxSlot\n";
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

void copy_AttachSysSlot(AttachSysSlotBaseImpl* to, AttachSysSlotBaseImpl* from, bool to_game)
{
    to->vtable = from->vtable;
    to->timerId = from->timerId;
    to->slotType = from->slotType;
    to->slotIsUsable = from->slotIsUsable;
    to->data_0 = from->data_0;

    switch (from->slotType)
    {
    case TypeChrShineTreasureSlot:
        copy_ChrShineTreasureSlot((ChrShineTreasureSlot*)to, (ChrShineTreasureSlot*)from, to_game);
        break;
    case TypeChrSingleSeSlot:
        copy_ChrSingleSeSlot((ChrSingleSeSlot*)to, (ChrSingleSeSlot*)from, to_game);
        break;
    case TypeChrSingleSfxSlot:
        copy_ChrSingleSfxSlot((ChrSingleSfxSlot*)to, (ChrSingleSfxSlot*)from, to_game);
        break;
    case TypeChrMultiSfxSlot:
        copy_ChrMultiSfxSlot((ChrMultiSfxSlot*)to, (ChrMultiSfxSlot*)from, to_game);
        break;
    case TypeChrBurnSlot:
        copy_ChrBurnSlot((ChrBurnSlot*)to, (ChrBurnSlot*)from, to_game);
        break;
    case TypeChrGasmanSlot:
        copy_ChrGasmanSlot((ChrGasmanSlot*)to, (ChrGasmanSlot*)from, to_game);
        break;
    //case TypeChrGrassSlot:
    //    copy_ChrGrassSlot((ChrGrassSlot*)to, (ChrGrassSlot*)from, to_game);
    //    break;
    case TypeChrFootEffectSlot:
        copy_ChrFootEffectSlot((ChrFootEffectSlot*)to, (ChrFootEffectSlot*)from, to_game);
        break;
    case TypeChrRigidOffsetSfxSlot:
        copy_ChrRigidOffsetSfxSlot((ChrRigidOffsetSfxSlot*)to, (ChrRigidOffsetSfxSlot*)from, to_game);
        break;
    case TypeChrRigidOffsetChrSlot:
        copy_ChrRigidOffsetChrSlot((ChrRigidOffsetChrSlot*)to, (ChrRigidOffsetChrSlot*)from, to_game);
        break;
    case TypeChrSoulEatSlot:
        copy_ChrSoulEatSlot((ChrSoulEatSlot*)to, (ChrSoulEatSlot*)from, to_game);
        break;
    case TypeChrSorceryWepSlot:
        copy_ChrSorceryWepSlot((ChrSorceryWepSlot*)to, (ChrSorceryWepSlot*)from, to_game);
        break;
    case TypeChrLanternSlot:
        copy_ChrLanternSlot((ChrLanternSlot*)to, (ChrLanternSlot*)from, to_game);
        break;
    case TypeChrSingleOneshotSfxSlot:
        copy_ChrSingleOneshotSfxSlot((ChrSingleOneshotSfxSlot*)to, (ChrSingleOneshotSfxSlot*)from, to_game);
        break;
    case TypeChrSingleTraceSfxSlot:
        copy_ChrSingleTraceSfxSlot((ChrSingleTraceSfxSlot*)to, (ChrSingleTraceSfxSlot*)from, to_game);
        break;
    case TypeChrMagicGoodsUseSfxSlot:
        copy_ChrMagicGoodsUseSfxSlot((ChrMagicGoodsUseSfxSlot*)to, (ChrMagicGoodsUseSfxSlot*)from, to_game);
        break;
    case TypeChrActPntSlot:
        copy_ChrActPntSlot((ChrActPntSlot*)to, (ChrActPntSlot*)from, to_game);
        break;
    case TypeChrPointLightSlot:
        copy_ChrPointLightSlot((ChrPointLightSlot*)to, (ChrPointLightSlot*)from, to_game);
        break;
    case TypeChrWepEnchantSlot:
        copy_ChrWepEnchantSlot((ChrWepEnchantSlot*)to, (ChrWepEnchantSlot*)from, to_game);
        break;
    case TypeChrFallingControlSlot:
        copy_ChrFallingControlSlot((ChrFallingControlSlot*)to, (ChrFallingControlSlot*)from, to_game);
        break;
    case TypeChrConditionSfxSeSlot:
        copy_ChrConditionSfxSeSlot((ChrConditionSfxSeSlot*)to, (ChrConditionSfxSeSlot*)from, to_game);
        break;
    //case TypeChrCamouflageSlot:
    //    copy_ChrCamouflageSlot((ChrCamouflageSlot*)to, (ChrCamouflageSlot*)from, to_game);
    //    break;
    case TypeChrSoulDeadSlot:
        copy_ChrSoulDeadSlot((ChrSoulDeadSlot*)to, (ChrSoulDeadSlot*)from, to_game);
        break;
    case TypeChrShinpanshaHaraSlot:
        copy_ChrShinpanshaHaraSlot((ChrShinpanshaHaraSlot*)to, (ChrShinpanshaHaraSlot*)from, to_game);
        break;
    case TypeChrLimitInvincibleSlot:
        copy_ChrLimitInvincibleSlot((ChrLimitInvincibleSlot*)to, (ChrLimitInvincibleSlot*)from, to_game);
        break;
    case TypeChrTravelItemSlot:
        copy_ChrTravelItemSlot((ChrTravelItemSlot*)to, (ChrTravelItemSlot*)from, to_game);
        break;
    case TypeChrStatueDeadSlot:
        copy_ChrStatueDeadSlot((ChrStatueDeadSlot*)to, (ChrStatueDeadSlot*)from, to_game);
        break;
    case TypeChrResonanceMagicSlot:
        copy_ChrResonanceMagicSlot((ChrResonanceMagicSlot*)to, (ChrResonanceMagicSlot*)from, to_game);
        break;
    case TypeChrRetributionMagicSlot:
        copy_ChrRetributionMagicSlot((ChrRetributionMagicSlot*)to, (ChrRetributionMagicSlot*)from, to_game);
        break;
    case TypeChrPlayerResidentSlot:
        copy_ChrPlayerResidentSlot((ChrPlayerResidentSlot*)to, (ChrPlayerResidentSlot*)from, to_game);
        break;
    case TypeChrFollowSfxSlot:
        copy_ChrFollowSfxSlot((ChrFollowSfxSlot*)to, (ChrFollowSfxSlot*)from, to_game);
        break;
    default: FATALERROR("Attempted to copy AttachSysSlot of type %d", from->slotType);
    }

    if (from->next != NULL)
    {
        to->next = init_AttachSysSlot((AttachSysSlotType)(from->next->slotType), to_game);
        copy_AttachSysSlot(to->next, from->next, to_game);
    }
    else
    {
        to->next = NULL;
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

static void helper_copy_followup_bullet_list(
    BulletIns_FollowupBullet** to_followup_bullet_list_ptr, int16_t* to_followup_bullet_list_len_ptr,
    BulletIns_FollowupBullet** from_followup_bullet_list_ptr, int16_t* from_followup_bullet_list_len_ptr,
    bool to_game)
{
    if (*from_followup_bullet_list_ptr == NULL)
    {
        if (*to_followup_bullet_list_ptr != NULL)
        {
            if (to_game)
            {
                Game::game_free(*to_followup_bullet_list_ptr, sizeof(BulletIns_FollowupBullet) * *(to_followup_bullet_list_len_ptr));
            }
            else
            {
                free(*to_followup_bullet_list_ptr);
            }
        }
        *to_followup_bullet_list_ptr = NULL;
    }
    else
    {
        //Resize the list
        size_t from_list_size = *from_followup_bullet_list_len_ptr;
        size_t to_list_size = *to_followup_bullet_list_len_ptr;
        if (to_list_size != from_list_size)
        {
            //allocate all the entities as a block
            if (to_game)
            {
                auto new_followup_bullet_list = (BulletIns_FollowupBullet*)Game::game_malloc(sizeof(BulletIns_FollowupBullet) * from_list_size, 0x10, *(uint64_t*)Game::internal_heap_3);
                if (*to_followup_bullet_list_ptr != NULL)
                {
                    Game::game_free(*to_followup_bullet_list_ptr, sizeof(BulletIns_FollowupBullet) * to_list_size);
                }
                *to_followup_bullet_list_ptr = new_followup_bullet_list;
            }
            else
            {
                auto new_followup_bullet_list = (BulletIns_FollowupBullet*)malloc_(sizeof(BulletIns_FollowupBullet) * from_list_size);
                if (*to_followup_bullet_list_ptr != NULL)
                {
                    free(*to_followup_bullet_list_ptr);
                }
                *to_followup_bullet_list_ptr = new_followup_bullet_list;
            }
        }

        //Copy the bullet entries
        for (size_t list_i = 0; list_i < *from_followup_bullet_list_len_ptr; list_i++)
        {
            BulletIns_FollowupBullet* to_bullet = &(*to_followup_bullet_list_ptr)[list_i];
            BulletIns_FollowupBullet* from_bullet = &(*from_followup_bullet_list_ptr)[list_i];

            copy_BulletIns_FollowupBullet(to_bullet, from_bullet, to_game);
            //set up the next ptr. We can probably ignore prev
            if (from_bullet->next != NULL)
            {
                size_t from_next_offset = ((uint64_t)from_bullet->next) - ((uint64_t)(*from_followup_bullet_list_ptr));
                to_bullet->next = (BulletIns_FollowupBullet*)(((uint64_t)(*to_followup_bullet_list_ptr)) + from_next_offset);
            }
            else
            {
                to_bullet->next = NULL;
            }
            to_bullet->prev = NULL;
        }
    }
    *to_followup_bullet_list_len_ptr = *from_followup_bullet_list_len_ptr;
}

void copy_ChrShineTreasureSlot(ChrShineTreasureSlot* to, ChrShineTreasureSlot* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    copy_BulletIns_FollowupBullet(&to->bullet, &from->bullet, to_game);
}

void copy_ChrSingleSeSlot(ChrSingleSeSlot* to, ChrSingleSeSlot* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
}

void copy_ChrSingleSfxSlot(ChrSingleSfxSlot* to, ChrSingleSfxSlot* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    copy_BulletIns_FollowupBullet(&to->bullet, &from->bullet, to_game);
}

void copy_ChrMultiSfxSlot(ChrMultiSfxSlot* to, ChrMultiSfxSlot* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    copy_BulletIns_FollowupBullet(to->bullet, from->bullet, to_game);
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
                Game::game_free(to->floatlist, sizeof(float) * to->floatlist_len);
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
                    Game::game_free(to->list, sizeof(float) * to_list_size);
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
                Game::game_free(to->list, sizeof(ChrBurnSlot_ListElem) * to->list_len);
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
                    Game::game_free(to->list, sizeof(ChrBurnSlot_ListElem) * to_list_size);
                }
                to->list = new_list;
            }
            else
            {
                auto new_list = (ChrBurnSlot_ListElem*)malloc_(sizeof(ChrBurnSlot_ListElem) * from_list_size);
                if (to->list != NULL)
                {
                    free(to->list);
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

            copy_BulletIns_FollowupBullet(&to_elem->bullet, &from_elem->bullet, to_game);
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

void copy_ChrGasmanSlot(ChrGasmanSlot* to, ChrGasmanSlot* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));

    if (from->list == NULL)
    {
        if (to->list != NULL)
        {
            if (to_game)
            {
                Game::game_free(to->list, sizeof(ChrGasmanSlot_ListElem) * to->list_len);
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
                    Game::game_free(to->list, sizeof(ChrGasmanSlot_ListElem) * to_list_size);
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

            copy_BulletIns_FollowupBullet(&to_elem->bullet_a, &from_elem->bullet_a, to_game);
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

            copy_BulletIns_FollowupBullet(&to_elem->bullet_b, &from_elem->bullet_b, to_game);
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

void copy_ChrGrassSlot(ChrGrassSlot* to, ChrGrassSlot* from, bool to_game)
{

}

void copy_ChrFootEffectSlot(ChrFootEffectSlot* to, ChrFootEffectSlot* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    copy_BulletIns_FollowupBullet(to->bullet, from->bullet, to_game);
}

void copy_ChrRigidOffsetSfxSlot(ChrRigidOffsetSfxSlot* to, ChrRigidOffsetSfxSlot* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    copy_BulletIns_FollowupBullet(&to->bullet, &from->bullet, to_game);
}

void copy_ChrRigidOffsetChrSlot(ChrRigidOffsetChrSlot* to, ChrRigidOffsetChrSlot* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
}

void copy_ChrSoulEatSlot(ChrSoulEatSlot* to, ChrSoulEatSlot* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    copy_BulletIns_FollowupBullet(&to->bullet, &from->bullet, to_game);
}

void copy_ChrSorceryWepSlot(ChrSorceryWepSlot* to, ChrSorceryWepSlot* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    to->data_1 = from->data_1;
    copy_BulletIns_FollowupBullet(&to->bullet1, &from->bullet1, to_game);
    to->data_2 = from->data_2;
    copy_BulletIns_FollowupBullet(&to->bullet2, &from->bullet2, to_game);
}

void copy_ChrLanternSlot(ChrLanternSlot* to, ChrLanternSlot* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    copy_BulletIns_FollowupBullet(&to->bullet1, &from->bullet1, to_game);
    copy_BulletIns_FollowupBullet(&to->bullet2, &from->bullet2, to_game);
    copy_BulletIns_FollowupBullet(&to->bullet3, &from->bullet3, to_game);
}

void copy_ChrSingleOneshotSfxSlot(ChrSingleOneshotSfxSlot* to, ChrSingleOneshotSfxSlot* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    copy_BulletIns_FollowupBullet(&to->bullet, &from->bullet, to_game);
}

void copy_ChrSingleTraceSfxSlot(ChrSingleTraceSfxSlot* to, ChrSingleTraceSfxSlot* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    copy_BulletIns_FollowupBullet(&to->bullet, &from->bullet, to_game);
}

void copy_ChrMagicGoodsUseSfxSlot(ChrMagicGoodsUseSfxSlot* to, ChrMagicGoodsUseSfxSlot* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    copy_BulletIns_FollowupBullet(&to->bullet, &from->bullet, to_game);
}

void copy_ChrActPntSlot(ChrActPntSlot* to, ChrActPntSlot* from, bool to_game)
{
    to->data_0 = from->data_0;
}

void copy_ChrPointLightSlot(ChrPointLightSlot* to, ChrPointLightSlot* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
}

void copy_ChrWepEnchantSlot(ChrWepEnchantSlot* to, ChrWepEnchantSlot* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));

    helper_copy_followup_bullet_list(&to->followup_bullet_list, &to->followup_bullet_list_len, &from->followup_bullet_list, &from->followup_bullet_list_len, to_game);

    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
}

void copy_ChrFallingControlSlot(ChrFallingControlSlot* to, ChrFallingControlSlot* from, bool to_game)
{
    to->data_0 = from->data_0;
}

void copy_ChrConditionSfxSeSlot(ChrConditionSfxSeSlot* to, ChrConditionSfxSeSlot* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));

    helper_copy_followup_bullet_list(&to->followupbullet_list, &to->followupbullet_list_len, &from->followupbullet_list, &from->followupbullet_list_len, to_game);

    to->data_1 = from->data_1;
}

void copy_ChrCamouflageSlot(ChrCamouflageSlot* to, ChrCamouflageSlot* from, bool to_game)
{

}

void copy_ChrSoulDeadSlot(ChrSoulDeadSlot* to, ChrSoulDeadSlot* from, bool to_game)
{
    to->data_0 = from->data_0;
}

void copy_ChrShinpanshaHaraSlot(ChrShinpanshaHaraSlot* to, ChrShinpanshaHaraSlot* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    copy_BulletIns_FollowupBullet(&to->bullet, &from->bullet, to_game);
}

void copy_ChrLimitInvincibleSlot(ChrLimitInvincibleSlot* to, ChrLimitInvincibleSlot* from, bool to_game)
{
    to->data_0 = from->data_0;
    to->chr = from->chr;
}

void copy_ChrTravelItemSlot(ChrTravelItemSlot* to, ChrTravelItemSlot* from, bool to_game)
{
    to->data_0 = from->data_0;
}

void copy_ChrStatueDeadSlot(ChrStatueDeadSlot* to, ChrStatueDeadSlot* from, bool to_game)
{
    to->data_0 = from->data_0;
    int16_t to_followup_bullet_list_len = to->followup_bullet_list_len;
    int16_t from_followup_bullet_list_len = from->followup_bullet_list_len;
    helper_copy_followup_bullet_list(&to->followup_bullet_list, &to_followup_bullet_list_len, &from->followup_bullet_list, &from_followup_bullet_list_len, to_game);
    to->followup_bullet_list_len = to_followup_bullet_list_len;
    from->followup_bullet_list_len = from_followup_bullet_list_len;
}

void copy_ChrResonanceMagicSlot(ChrResonanceMagicSlot* to, ChrResonanceMagicSlot* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
}

void copy_ChrRetributionMagicSlot(ChrRetributionMagicSlot* to, ChrRetributionMagicSlot* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
}

void copy_ChrPlayerResidentSlot(ChrPlayerResidentSlot* to, ChrPlayerResidentSlot* from, bool to_game)
{
    to->data_0 = from->data_0;
    to->data_1 = from->data_1;
    memcpy(to->data_2, from->data_2, sizeof(to->data_2));
}

void copy_ChrFollowSfxSlot(ChrFollowSfxSlot* to, ChrFollowSfxSlot* from, bool to_game)
{
    to->data_0 = from->data_0;
    copy_BulletIns_FollowupBullet(&to->bullet, &from->bullet, to_game);
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
}
