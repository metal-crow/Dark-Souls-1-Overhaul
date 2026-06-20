#pragma once
#ifndef SYSSLOTSTRUCTFUNCTIONS_H
#define SYSSLOTSTRUCTFUNCTIONS_H

#include <stdint.h>
#include <string>
#include "AttachSysSlotStructs.h"
#include "Rollback.h"

void copy_AttachSysSlot(AttachSysSlotBaseImpl** to, AttachSysSlotBaseImpl* from, StateTarget target);
AttachSysSlotBaseImpl* init_AttachSysSlot(AttachSysSlotType type, StateTarget target);
void free_AttachSysSlot(AttachSysSlotBaseImpl* to, StateTarget target);
void free_AttachSysSlot_Fields(AttachSysSlotBaseImpl* to, StateTarget target);

void copy_ChrShineTreasureSlot(ChrShineTreasureSlot* to, ChrShineTreasureSlot* from, StateTarget target);
void copy_ChrSingleSeSlot(ChrSingleSeSlot* to, ChrSingleSeSlot* from, StateTarget target);
void copy_ChrSingleSfxSlot(ChrSingleSfxSlot* to, ChrSingleSfxSlot* from, StateTarget target);
void copy_ChrMultiSfxSlot(ChrMultiSfxSlot* to, ChrMultiSfxSlot* from, StateTarget target);
void copy_ChrBurnSlot(ChrBurnSlot* to, ChrBurnSlot* from, StateTarget target);
void copy_ChrGasmanSlot(ChrGasmanSlot* to, ChrGasmanSlot* from, StateTarget target);
void copy_ChrGrassSlot(ChrGrassSlot* to, ChrGrassSlot* from, StateTarget target);
void copy_ChrFootEffectSlot(ChrFootEffectSlot* to, ChrFootEffectSlot* from, StateTarget target);
void copy_ChrRigidOffsetSfxSlot(ChrRigidOffsetSfxSlot* to, ChrRigidOffsetSfxSlot* from, StateTarget target);
void copy_ChrRigidOffsetChrSlot(ChrRigidOffsetChrSlot* to, ChrRigidOffsetChrSlot* from, StateTarget target);
void copy_ChrSoulEatSlot(ChrSoulEatSlot* to, ChrSoulEatSlot* from, StateTarget target);
void copy_ChrSorceryWepSlot(ChrSorceryWepSlot* to, ChrSorceryWepSlot* from, StateTarget target);
void copy_ChrLanternSlot(ChrLanternSlot* to, ChrLanternSlot* from, StateTarget target);
void copy_ChrSingleOneshotSfxSlot(ChrSingleOneshotSfxSlot* to, ChrSingleOneshotSfxSlot* from, StateTarget target);
void copy_ChrSingleTraceSfxSlot(ChrSingleTraceSfxSlot* to, ChrSingleTraceSfxSlot* from, StateTarget target);
void copy_ChrMagicGoodsUseSfxSlot(ChrMagicGoodsUseSfxSlot* to, ChrMagicGoodsUseSfxSlot* from, StateTarget target);
void copy_ChrActPntSlot_ArrayElem(ChrActPntSlot_ArrayElem* to, ChrActPntSlot_ArrayElem* from, StateTarget target);
void copy_ChrActPntSlot(ChrActPntSlot** to, ChrActPntSlot* from, StateTarget target);
void copy_ChrPointLightSlot(ChrPointLightSlot* to, ChrPointLightSlot* from, StateTarget target);
void copy_ChrWepEnchantSlot(ChrWepEnchantSlot* to, ChrWepEnchantSlot* from, StateTarget target);
void copy_ChrFallingControlSlot(ChrFallingControlSlot* to, ChrFallingControlSlot* from, StateTarget target);
void copy_ChrConditionSfxSeSlot(ChrConditionSfxSeSlot* to, ChrConditionSfxSeSlot* from, StateTarget target);
void copy_ChrCamouflageSlot(ChrCamouflageSlot* to, ChrCamouflageSlot* from, StateTarget target);
void copy_ChrSoulDeadSlot(ChrSoulDeadSlot* to, ChrSoulDeadSlot* from, StateTarget target);
void copy_ChrShinpanshaHaraSlot(ChrShinpanshaHaraSlot* to, ChrShinpanshaHaraSlot* from, StateTarget target);
void copy_ChrLimitInvincibleSlot(ChrLimitInvincibleSlot* to, ChrLimitInvincibleSlot* from, StateTarget target);
void copy_ChrTravelItemSlot(ChrTravelItemSlot* to, ChrTravelItemSlot* from, StateTarget target);
void copy_ChrStatueDeadSlot(ChrStatueDeadSlot* to, ChrStatueDeadSlot* from, StateTarget target);
void copy_ChrResonanceMagicSlot(ChrResonanceMagicSlot* to, ChrResonanceMagicSlot* from, StateTarget target);
void copy_ChrRetributionMagicSlot(ChrRetributionMagicSlot* to, ChrRetributionMagicSlot* from, StateTarget target);
void copy_ChrPlayerResidentSlot(ChrPlayerResidentSlot* to, ChrPlayerResidentSlot* from, StateTarget target);
void copy_ChrFollowSfxSlot(ChrFollowSfxSlot* to, ChrFollowSfxSlot* from, StateTarget target);

#endif
