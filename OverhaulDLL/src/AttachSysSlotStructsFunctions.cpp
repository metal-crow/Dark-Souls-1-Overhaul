#include "AttachSysSlotStructsFunctions.h"
#include "Rollback.h"

void copy_AttachSysSlot(AttachSysSlotBaseImpl* to, AttachSysSlotBaseImpl* from, bool to_game)
{
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
    case TypeChrGrassSlot:
        copy_ChrGrassSlot((ChrGrassSlot*)to, (ChrGrassSlot*)from, to_game);
        break;
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
    case TypeChrCamouflageSlot:
        copy_ChrCamouflageSlot((ChrCamouflageSlot*)to, (ChrCamouflageSlot*)from, to_game);
        break;
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

//TODO need to handle the bullets in these structs

void copy_ChrShineTreasureSlot(ChrShineTreasureSlot* to, ChrShineTreasureSlot* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
}

void copy_ChrSingleSeSlot(ChrSingleSeSlot* to, ChrSingleSeSlot* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
}

void copy_ChrSingleSfxSlot(ChrSingleSfxSlot* to, ChrSingleSfxSlot* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
}

void copy_ChrMultiSfxSlot(ChrMultiSfxSlot* to, ChrMultiSfxSlot* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    to->bullet = NULL;
}

void copy_ChrBurnSlot(ChrBurnSlot* to, ChrBurnSlot* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    to->unk1 = NULL;
    to->data_1 = from->data_1;
    to->unk2 = NULL;
    memcpy(to->data_2, from->data_2, sizeof(to->data_2));
}

void copy_ChrGasmanSlot(ChrGasmanSlot* to, ChrGasmanSlot* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
}

void copy_ChrGrassSlot(ChrGrassSlot* to, ChrGrassSlot* from, bool to_game)
{

}

void copy_ChrFootEffectSlot(ChrFootEffectSlot* to, ChrFootEffectSlot* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    to->bullet = NULL;
}

void copy_ChrRigidOffsetSfxSlot(ChrRigidOffsetSfxSlot* to, ChrRigidOffsetSfxSlot* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
}

void copy_ChrRigidOffsetChrSlot(ChrRigidOffsetChrSlot* to, ChrRigidOffsetChrSlot* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
}

void copy_ChrSoulEatSlot(ChrSoulEatSlot* to, ChrSoulEatSlot* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
}

void copy_ChrSorceryWepSlot(ChrSorceryWepSlot* to, ChrSorceryWepSlot* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    to->data_1 = from->data_1;
    to->data_2 = from->data_2;
}

void copy_ChrLanternSlot(ChrLanternSlot* to, ChrLanternSlot* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
}

void copy_ChrSingleOneshotSfxSlot(ChrSingleOneshotSfxSlot* to, ChrSingleOneshotSfxSlot* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
}

void copy_ChrSingleTraceSfxSlot(ChrSingleTraceSfxSlot* to, ChrSingleTraceSfxSlot* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
}

void copy_ChrMagicGoodsUseSfxSlot(ChrMagicGoodsUseSfxSlot* to, ChrMagicGoodsUseSfxSlot* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
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

}

void copy_ChrFallingControlSlot(ChrFallingControlSlot* to, ChrFallingControlSlot* from, bool to_game)
{
    to->data_0 = from->data_0;
}

void copy_ChrConditionSfxSeSlot(ChrConditionSfxSeSlot* to, ChrConditionSfxSeSlot* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
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
}

void copy_ChrLimitInvincibleSlot(ChrLimitInvincibleSlot* to, ChrLimitInvincibleSlot* from, bool to_game)
{
    to->data_0 = from->data_0;
}

void copy_ChrTravelItemSlot(ChrTravelItemSlot* to, ChrTravelItemSlot* from, bool to_game)
{
    to->data_0 = from->data_0;
}

void copy_ChrStatueDeadSlot(ChrStatueDeadSlot* to, ChrStatueDeadSlot* from, bool to_game)
{
    to->data_0 = from->data_0;
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
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
}
