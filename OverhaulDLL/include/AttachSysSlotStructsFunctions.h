#pragma once
#ifndef SYSSLOTSTRUCTFUNCTIONS_H
#define SYSSLOTSTRUCTFUNCTIONS_H

#include <stdint.h>
#include <string>
#include "AttachSysSlotStructs.h"
#include "Rollback.h"

std::string print_AttachSysSlot(AttachSysSlotBaseImpl* to);
void copy_AttachSysSlot(AttachSysSlotBaseImpl** to, AttachSysSlotBaseImpl* from, StateTarget target);
AttachSysSlotBaseImpl* init_AttachSysSlot(AttachSysSlotType type, StateTarget target);
void free_AttachSysSlot(AttachSysSlotBaseImpl* to);

std::string print_ChrShineTreasureSlot(ChrShineTreasureSlot* to);
void copy_ChrShineTreasureSlot(ChrShineTreasureSlot* to, ChrShineTreasureSlot* from, StateTarget target);

std::string print_ChrSingleSeSlot(ChrSingleSeSlot* to);
void copy_ChrSingleSeSlot(ChrSingleSeSlot* to, ChrSingleSeSlot* from, StateTarget target);

std::string print_ChrSingleSfxSlot(ChrSingleSfxSlot* to);
void copy_ChrSingleSfxSlot(ChrSingleSfxSlot* to, ChrSingleSfxSlot* from, StateTarget target);

std::string print_ChrMultiSfxSlot(ChrMultiSfxSlot* to);
void copy_ChrMultiSfxSlot(ChrMultiSfxSlot* to, ChrMultiSfxSlot* from, StateTarget target);

std::string print_ChrBurnSlot(ChrBurnSlot* to);
void copy_ChrBurnSlot(ChrBurnSlot* to, ChrBurnSlot* from, StateTarget target);

std::string print_ChrGasmanSlot(ChrGasmanSlot* to);
void copy_ChrGasmanSlot(ChrGasmanSlot* to, ChrGasmanSlot* from, StateTarget target);

std::string print_ChrGrassSlot(ChrGrassSlot* to);
void copy_ChrGrassSlot(ChrGrassSlot* to, ChrGrassSlot* from, StateTarget target);

std::string print_ChrFootEffectSlot(ChrFootEffectSlot* to);
void copy_ChrFootEffectSlot(ChrFootEffectSlot* to, ChrFootEffectSlot* from, StateTarget target);

std::string print_ChrRigidOffsetSfxSlot(ChrRigidOffsetSfxSlot* to);
void copy_ChrRigidOffsetSfxSlot(ChrRigidOffsetSfxSlot* to, ChrRigidOffsetSfxSlot* from, StateTarget target);

std::string print_ChrRigidOffsetChrSlot(ChrRigidOffsetChrSlot* to);
void copy_ChrRigidOffsetChrSlot(ChrRigidOffsetChrSlot* to, ChrRigidOffsetChrSlot* from, StateTarget target);

std::string print_ChrSoulEatSlot(ChrSoulEatSlot* to);
void copy_ChrSoulEatSlot(ChrSoulEatSlot* to, ChrSoulEatSlot* from, StateTarget target);

std::string print_ChrSorceryWepSlot(ChrSorceryWepSlot* to);
void copy_ChrSorceryWepSlot(ChrSorceryWepSlot* to, ChrSorceryWepSlot* from, StateTarget target);

std::string print_ChrLanternSlot(ChrLanternSlot* to);
void copy_ChrLanternSlot(ChrLanternSlot* to, ChrLanternSlot* from, StateTarget target);

std::string print_ChrSingleOneshotSfxSlot(ChrSingleOneshotSfxSlot* to);
void copy_ChrSingleOneshotSfxSlot(ChrSingleOneshotSfxSlot* to, ChrSingleOneshotSfxSlot* from, StateTarget target);

std::string print_ChrSingleTraceSfxSlot(ChrSingleTraceSfxSlot* to);
void copy_ChrSingleTraceSfxSlot(ChrSingleTraceSfxSlot* to, ChrSingleTraceSfxSlot* from, StateTarget target);

std::string print_ChrMagicGoodsUseSfxSlot(ChrMagicGoodsUseSfxSlot* to);
void copy_ChrMagicGoodsUseSfxSlot(ChrMagicGoodsUseSfxSlot* to, ChrMagicGoodsUseSfxSlot* from, StateTarget target);

std::string print_ChrActPntSlot_ArrayElem(ChrActPntSlot_ArrayElem* to);
void copy_ChrActPntSlot_ArrayElem(ChrActPntSlot_ArrayElem* to, ChrActPntSlot_ArrayElem* from, StateTarget target);

std::string print_ChrActPntSlot(ChrActPntSlot* to);
void copy_ChrActPntSlot(ChrActPntSlot** to, ChrActPntSlot* from, StateTarget target);

std::string print_ChrPointLightSlot(ChrPointLightSlot* to);
void copy_ChrPointLightSlot(ChrPointLightSlot* to, ChrPointLightSlot* from, StateTarget target);

std::string print_ChrWepEnchantSlot(ChrWepEnchantSlot* to);
void copy_ChrWepEnchantSlot(ChrWepEnchantSlot* to, ChrWepEnchantSlot* from, StateTarget target);

std::string print_ChrFallingControlSlot(ChrFallingControlSlot* to);
void copy_ChrFallingControlSlot(ChrFallingControlSlot* to, ChrFallingControlSlot* from, StateTarget target);

std::string print_ChrConditionSfxSeSlot(ChrConditionSfxSeSlot* to);
void copy_ChrConditionSfxSeSlot(ChrConditionSfxSeSlot* to, ChrConditionSfxSeSlot* from, StateTarget target);

std::string print_ChrCamouflageSlot(ChrCamouflageSlot* to);
void copy_ChrCamouflageSlot(ChrCamouflageSlot* to, ChrCamouflageSlot* from, StateTarget target);

std::string print_ChrSoulDeadSlot(ChrSoulDeadSlot* to);
void copy_ChrSoulDeadSlot(ChrSoulDeadSlot* to, ChrSoulDeadSlot* from, StateTarget target);

std::string print_ChrShinpanshaHaraSlot(ChrShinpanshaHaraSlot* to);
void copy_ChrShinpanshaHaraSlot(ChrShinpanshaHaraSlot* to, ChrShinpanshaHaraSlot* from, StateTarget target);

std::string print_ChrLimitInvincibleSlot(ChrLimitInvincibleSlot* to);
void copy_ChrLimitInvincibleSlot(ChrLimitInvincibleSlot* to, ChrLimitInvincibleSlot* from, StateTarget target);

std::string print_ChrTravelItemSlot(ChrTravelItemSlot* to);
void copy_ChrTravelItemSlot(ChrTravelItemSlot* to, ChrTravelItemSlot* from, StateTarget target);

std::string print_ChrStatueDeadSlot(ChrStatueDeadSlot* to);
void copy_ChrStatueDeadSlot(ChrStatueDeadSlot* to, ChrStatueDeadSlot* from, StateTarget target);

std::string print_ChrResonanceMagicSlot(ChrResonanceMagicSlot* to);
void copy_ChrResonanceMagicSlot(ChrResonanceMagicSlot* to, ChrResonanceMagicSlot* from, StateTarget target);

std::string print_ChrRetributionMagicSlot(ChrRetributionMagicSlot* to);
void copy_ChrRetributionMagicSlot(ChrRetributionMagicSlot* to, ChrRetributionMagicSlot* from, StateTarget target);

std::string print_ChrPlayerResidentSlot(ChrPlayerResidentSlot* to);
void copy_ChrPlayerResidentSlot(ChrPlayerResidentSlot* to, ChrPlayerResidentSlot* from, StateTarget target);

std::string print_ChrFollowSfxSlot(ChrFollowSfxSlot* to);
void copy_ChrFollowSfxSlot(ChrFollowSfxSlot* to, ChrFollowSfxSlot* from, StateTarget target);

#endif
