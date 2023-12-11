#pragma once
#ifndef SYSSLOTSTRUCTFUNCTIONS_H
#define SYSSLOTSTRUCTFUNCTIONS_H

#include <stdint.h>
#include "AttachSysSlotStructs.h"

void copy_AttachSysSlot(AttachSysSlotBaseImpl* to, AttachSysSlotBaseImpl* from, bool to_game);

AttachSysSlotBaseImpl* init_AttachSysSlot(AttachSysSlotType type, bool to_game);

void free_AttachSysSlot(AttachSysSlotBaseImpl* to);

void copy_ChrShineTreasureSlot(ChrShineTreasureSlot* to, ChrShineTreasureSlot* from, bool to_game);

void copy_ChrSingleSeSlot(ChrSingleSeSlot* to, ChrSingleSeSlot* from, bool to_game);

void copy_ChrSingleSfxSlot(ChrSingleSfxSlot* to, ChrSingleSfxSlot* from, bool to_game);

void copy_ChrMultiSfxSlot(ChrMultiSfxSlot* to, ChrMultiSfxSlot* from, bool to_game);

void copy_ChrBurnSlot(ChrBurnSlot* to, ChrBurnSlot* from, bool to_game);

void copy_ChrGasmanSlot(ChrGasmanSlot* to, ChrGasmanSlot* from, bool to_game);

void copy_ChrGrassSlot(ChrGrassSlot* to, ChrGrassSlot* from, bool to_game);

void copy_ChrFootEffectSlot(ChrFootEffectSlot* to, ChrFootEffectSlot* from, bool to_game);

void copy_ChrRigidOffsetSfxSlot(ChrRigidOffsetSfxSlot* to, ChrRigidOffsetSfxSlot* from, bool to_game);

void copy_ChrRigidOffsetChrSlot(ChrRigidOffsetChrSlot* to, ChrRigidOffsetChrSlot* from, bool to_game);

void copy_ChrSoulEatSlot(ChrSoulEatSlot* to, ChrSoulEatSlot* from, bool to_game);

void copy_ChrSorceryWepSlot(ChrSorceryWepSlot* to, ChrSorceryWepSlot* from, bool to_game);

void copy_ChrLanternSlot(ChrLanternSlot* to, ChrLanternSlot* from, bool to_game);

void copy_ChrSingleOneshotSfxSlot(ChrSingleOneshotSfxSlot* to, ChrSingleOneshotSfxSlot* from, bool to_game);

void copy_ChrSingleTraceSfxSlot(ChrSingleTraceSfxSlot* to, ChrSingleTraceSfxSlot* from, bool to_game);

void copy_ChrMagicGoodsUseSfxSlot(ChrMagicGoodsUseSfxSlot* to, ChrMagicGoodsUseSfxSlot* from, bool to_game);

void copy_ChrActPntSlot(ChrActPntSlot* to, ChrActPntSlot* from, bool to_game);

void copy_ChrPointLightSlot(ChrPointLightSlot* to, ChrPointLightSlot* from, bool to_game);

void copy_ChrWepEnchantSlot(ChrWepEnchantSlot* to, ChrWepEnchantSlot* from, bool to_game);

void copy_ChrFallingControlSlot(ChrFallingControlSlot* to, ChrFallingControlSlot* from, bool to_game);

void copy_ChrConditionSfxSeSlot(ChrConditionSfxSeSlot* to, ChrConditionSfxSeSlot* from, bool to_game);

void copy_ChrCamouflageSlot(ChrCamouflageSlot* to, ChrCamouflageSlot* from, bool to_game);

void copy_ChrSoulDeadSlot(ChrSoulDeadSlot* to, ChrSoulDeadSlot* from, bool to_game);

void copy_ChrShinpanshaHaraSlot(ChrShinpanshaHaraSlot* to, ChrShinpanshaHaraSlot* from, bool to_game);

void copy_ChrLimitInvincibleSlot(ChrLimitInvincibleSlot* to, ChrLimitInvincibleSlot* from, bool to_game);

void copy_ChrTravelItemSlot(ChrTravelItemSlot* to, ChrTravelItemSlot* from, bool to_game);

void copy_ChrStatueDeadSlot(ChrStatueDeadSlot* to, ChrStatueDeadSlot* from, bool to_game);

void copy_ChrResonanceMagicSlot(ChrResonanceMagicSlot* to, ChrResonanceMagicSlot* from, bool to_game);

void copy_ChrRetributionMagicSlot(ChrRetributionMagicSlot* to, ChrRetributionMagicSlot* from, bool to_game);

void copy_ChrPlayerResidentSlot(ChrPlayerResidentSlot* to, ChrPlayerResidentSlot* from, bool to_game);

void copy_ChrFollowSfxSlot(ChrFollowSfxSlot* to, ChrFollowSfxSlot* from, bool to_game);

#endif
