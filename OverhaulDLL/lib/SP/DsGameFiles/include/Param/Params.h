/*
    DARK SOULS GAME FILE C++ LIBRARY

    Contributors to this file:
        Sean Pesce	-	C++


    Param/Param.h



*/

#pragma once

#ifndef DS1_FILE_LIB_GENERIC_GET_PARAMS_DEF_FILE_H_
    #define DS1_FILE_LIB_GENERIC_GET_PARAMS_DEF_FILE_H_


#include "Param/AIStandardInfo.h"
#include "Param/Accessory.h"
#include "Param/Armor.h"
#include "Param/ArmorReinforcement.h"
#include "Param/AttackNpc.h"
#include "Param/AttackPc.h"
#include "Param/BehaviorNpc.h"
#include "Param/BehaviorPc.h"
#include "Param/Bullet.h"
#include "Param/CalcCorrect.h"
#include "Param/CharInit.h"
#include "Param/EnemyStandardInfo.h"
#include "Param/FaceGeneration.h"
#include "Param/GameArea.h"
#include "Param/HitMaterial.h"
#include "Param/Item.h"
#include "Param/ItemLot.h"
#include "Param/KnockBack.h"
#include "Param/LockCam.h"
#include "Param/Magic.h"
#include "Param/MaterialSet.h"
#include "Param/MenuColorTable.h"
#include "Param/Move.h"
#include "Param/NPC.h"
#include "Param/NpcThink.h"
#include "Param/Object.h"
#include "Param/ObjectAction.h"
#include "Param/QwcChange.h"
#include "Param/QwcJudge.h"
#include "Param/Ragdoll.h"
#include "Param/ShopLineup.h"
#include "Param/Skeleton.h"
#include "Param/SpecialEffect.h"
#include "Param/SpecialEffectVfx.h"
#include "Param/Talk.h"
#include "Param/Throw.h"
#include "Param/Weapon.h"
#include "Param/WeaponReinforcement.h"

#include "Param/Draw/DofBank.h"
#include "Param/Draw/EnvLightTexBank.h"
#include "Param/Draw/FogBank.h"
#include "Param/Draw/LightBank.h"
#include "Param/Draw/PointLightBank.h"
#include "Param/Draw/ToneCorrectBank.h"
#include "Param/Draw/ToneMapBank.h"

#include "SP_AsmInject_x86.hpp"


#define TOTAL_PARAM_COUNT 170


// Draw types for human NPCs (doesn't affect players)
enum DS1_NPC_PLAYER_DRAW_TYPE {
    NPC_PLAYER_DRAW_TYPE_HUMAN = 0,
    NPC_PLAYER_DRAW_TYPE_WHITE_PHANTOM = 1,
    NPC_PLAYER_DRAW_TYPE_RED_PHANTOM = 2,
    NPC_PLAYER_DRAW_TYPE_RED_PHANTOM_2 = 3,
    NPC_PLAYER_DRAW_TYPE_HOLLOW = 4
};

// Draw types for enemy NPCs
enum DS1_NPC_DRAW_TYPE {
    NPC_DRAW_TYPE_NORMAL = 0,
    NPC_DRAW_TYPE_WHITE_PHANTOM = 1,
    NPC_DRAW_TYPE_RED_PHANTOM = 2,
    NPC_DRAW_TYPE_RED_PHANTOM_2 = 3,
    NPC_DRAW_TYPE_HOLLOW = 4
};


// Params Namespace for easy referencing of parameter data
namespace Params {

// Index of each param file corresponding to load order
enum FILE_INDEX {
    DOF_BANK_DEFAULT = 0,
    ENV_LIGHT_TEX_BANK_DEFAULT = 1,
    FOG_BANK_DEFAULT = 2,
    LENS_FLARE_BANK_DEFAULT = 3,
    LENS_FLARE_EX_BANK_DEFAULT = 4,
    LIGHT_BANK_DEFAULT = 5,
    LIGHT_SCATTERING_BANK_DEFAULT = 6,
    LOD_BANK_DEFAULT = 7,
    POINT_LIGHT_BANK_DEFAULT = 8,
    SHADOW_BANK_DEFAULT = 9,
    TONE_CORRECT_BANK_DEFAULT = 10,
    TONE_MAP_BANK_DEFAULT = 11,
    EQUIP_PARAM_WEAPON_ST = 12,
    EQUIP_PARAM_PROTECTOR_ST = 13,
    EQUIP_PARAM_ACCESSORY_ST = 14,
    EQUIP_PARAM_GOODS_ST = 15,
    REINFORCE_PARAM_WEAPON_ST = 16,
    REINFORCE_PARAM_PROTECTOR_ST = 17,
    EQUIP_MTRL_SET_PARAM_ST = 18,
    ENEMY_STANDARD_INFO_BANK = 19,
    AI_STANDARD_INFO_BANK = 20,
    THROW_INFO_BANK = 21,
    BEHAVIOR_PARAM_NPC_ST = 22,
    BEHAVIOR_PARAM_PC_ST = 23,
    NPC_PARAM_ST = 24,
    ATK_PARAM_PC_ST = 25,
    ATK_PARAM_NPC_ST = 26,
    MAGIC_PARAM_ST = 27,
    NPC_THINK_PARAM_ST = 28,
    OBJECT_PARAM_ST = 29,
    BULLET_PARAM_ST = 30,
    SP_EFFECT_PARAM_ST = 31,
    SP_EFFECT_VFX_PARAM_ST = 32,
    TALK_PARAM_ST = 33,
    MENU_PARAM_COLOR_TABLE_ST = 34,
    ITEMLOT_PARAM_ST = 35,
    MOVE_PARAM_ST = 36,
    CHARACTER_INIT_PARAM = 37,
    FACE_PARAM_ST = 38,
    RAGDOLL_PARAM_ST = 39,
    SHOP_LINEUP_PARAM = 40,
    QWC_CHANGE_PARAM_ST = 41,
    QWC_JUDGE_PARAM_ST = 42,
    GAME_AREA_PARAM_ST = 43,
    SKELETON_PARAM_ST = 44,
    CALC_CORRECT_GRAPH_ST = 45, // "CACL_CORRECT_GRAPH_ST" in game
    LOCK_CAM_PARAM_ST = 46,
    OBJ_ACT_PARAM_ST = 47,
    HIT_MTRL_PARAM_ST = 48,
    KNOCKBACK_PARAM_ST = 49,
    DOF_BANK_M10 = 50,
    ENV_LIGHT_TEX_BANK_M10 = 51,
    FOG_BANK_M10 = 52,
    LENS_FLARE_BANK_M10 = 53,
    LENS_FLARE_EX_BANK_M10 = 54,
    LIGHT_BANK_M10 = 55,
    LIGHT_SCATTERING_BANK_M10 = 56,
    POINT_LIGHT_BANK_M10 = 57,
    SHADOW_BANK_M10 = 58,
    TONE_CORRECT_BANK_M10 = 59,
    TONE_MAP_BANK_M10 = 60,
    LIGHT_BANK_S10 = 61,
    DOF_BANK_M11 = 62,
    ENV_LIGHT_TEX_BANK_M11 = 63,
    FOG_BANK_M11 = 64,
    LENS_FLARE_BANK_M11 = 65,
    LENS_FLARE_EX_BANK_M11 = 66,
    LIGHT_BANK_M11 = 67,
    LIGHT_SCATTERING_BANK_M11 = 68,
    POINT_LIGHT_BANK_M11 = 69,
    SHADOW_BANK_M11 = 70,
    TONE_CORRECT_BANK_M11 = 71,
    TONE_MAP_BANK_M11 = 72,
    LIGHT_BANK_S11 = 73,
    DOF_BANK_M12 = 74,
    ENV_LIGHT_TEX_BANK_M12 = 75,
    FOG_BANK_M12 = 76,
    LENS_FLARE_BANK_M12 = 77,
    LENS_FLARE_EX_BANK_M12 = 78,
    LIGHT_BANK_M12 = 79,
    LIGHT_SCATTERING_BANK_M12 = 80,
    POINT_LIGHT_BANK_M12 = 81,
    SHADOW_BANK_M12 = 82,
    TONE_CORRECT_BANK_M12 = 83,
    TONE_MAP_BANK_M12 = 84,
    LIGHT_BANK_S12 = 85,
    DOF_BANK_M13 = 86,
    ENV_LIGHT_TEX_BANK_M13 = 87,
    FOG_BANK_M13 = 88,
    LENS_FLARE_BANK_M13 = 89,
    LENS_FLARE_EX_BANK_M13 = 90,
    LIGHT_BANK_M13 = 91,
    LIGHT_SCATTERING_BANK_M13 = 92,
    POINT_LIGHT_BANK_M13 = 93,
    SHADOW_BANK_M13 = 94,
    TONE_CORRECT_BANK_M13 = 95,
    TONE_MAP_BANK_M13 = 96,
    LIGHT_BANK_S13 = 97,
    DOF_BANK_M14 = 98,
    ENV_LIGHT_TEX_BANK_M14 = 99,
    FOG_BANK_M14 = 100,
    LENS_FLARE_BANK_M14 = 101,
    LENS_FLARE_EX_BANK_M14 = 102,
    LIGHT_BANK_M14 = 103,
    LIGHT_SCATTERING_BANK_M14 = 104,
    POINT_LIGHT_BANK_M14 = 105,
    SHADOW_BANK_M14 = 106,
    TONE_CORRECT_BANK_M14 = 107,
    TONE_MAP_BANK_M14 = 108,
    LIGHT_BANK_S14 = 109,
    DOF_BANK_M15_1 = 110,
    ENV_LIGHT_TEX_BANK_M15_1 = 111,
    FOG_BANK_M15_1 = 112,
    LENS_FLARE_BANK_M15_1 = 113,
    LENS_FLARE_EX_BANK_M15_1 = 114,
    LIGHT_BANK_M15_1 = 115,
    LIGHT_SCATTERING_BANK_M15_1 = 116,
    POINT_LIGHT_BANK_M15_1 = 117,
    SHADOW_BANK_M15_1 = 118,
    TONE_CORRECT_BANK_M15_1 = 119,
    TONE_MAP_BANK_M15_1 = 120,
    DOF_BANK_M15 = 121,
    ENV_LIGHT_TEX_BANK_M15 = 122,
    FOG_BANK_M15 = 123,
    LENS_FLARE_BANK_M15 = 124,
    LENS_FLARE_EX_BANK_M15 = 125,
    LIGHT_BANK_M15 = 126,
    LIGHT_SCATTERING_BANK_M15 = 127,
    POINT_LIGHT_BANK_M15 = 128,
    SHADOW_BANK_M15 = 129,
    TONE_CORRECT_BANK_M15 = 130,
    TONE_MAP_BANK_M15 = 131,
    LIGHT_BANK_S15_1 = 132,
    LIGHT_BANK_S15 = 133,
    DOF_BANK_M16 = 134,
    ENV_LIGHT_TEX_BANK_M16 = 135,
    FOG_BANK_M16 = 136,
    LENS_FLARE_BANK_M16 = 137,
    LENS_FLARE_EX_BANK_M16 = 138,
    LIGHT_BANK_M16 = 139,
    LIGHT_SCATTERING_BANK_M16 = 140,
    POINT_LIGHT_BANK_M16 = 141,
    SHADOW_BANK_M16 = 142,
    TONE_CORRECT_BANK_M16 = 143,
    TONE_MAP_BANK_M16 = 144,
    LIGHT_BANK_S16 = 145,
    DOF_BANK_M17 = 146,
    ENV_LIGHT_TEX_BANK_M17 = 147,
    FOG_BANK_M17 = 148,
    LENS_FLARE_BANK_M17 = 149,
    LENS_FLARE_EX_BANK_M17 = 150,
    LIGHT_BANK_M17 = 151,
    LIGHT_SCATTERING_BANK_M17 = 152,
    POINT_LIGHT_BANK_M17 = 153,
    SHADOW_BANK_M17 = 154,
    TONE_CORRECT_BANK_M17 = 155,
    TONE_MAP_BANK_M17 = 156,
    LIGHT_BANK_S17 = 157,
    DOF_BANK_M18 = 158,
    ENV_LIGHT_TEX_BANK_M18 = 159,
    FOG_BANK_M18 = 160,
    LENS_FLARE_BANK_M18 = 161,
    LENS_FLARE_EX_BANK_M18 = 162,
    LIGHT_BANK_M18 = 163,
    LIGHT_SCATTERING_BANK_M18 = 164,
    POINT_LIGHT_BANK_M18 = 165,
    SHADOW_BANK_M18 = 166,
    TONE_CORRECT_BANK_M18 = 167,
    TONE_MAP_BANK_M18 = 168,
    LIGHT_BANK_S18 = 169,
    DOF_BANK_M99 = 170,
    ENV_LIGHT_TEX_BANK_M99 = 171,
    FOG_BANK_M99 = 172,
    LENS_FLARE_BANK_M99 = 173,
    LENS_FLARE_EX_BANK_M99 = 174,
    LIGHT_BANK_M99 = 175,
    LIGHT_SCATTERING_BANK_M99 = 176,
    POINT_LIGHT_BANK_M99 = 177,
    SHADOW_BANK_M99 = 178,
    TONE_CORRECT_BANK_M99 = 179,
    TONE_MAP_BANK_M99 = 180,
    LIGHT_BANK_S99 = 181
};

/////////////////////////////////////
////////// GAME PARAMETERS //////////
/////////////////////////////////////

// Accessory parameters
static AccessoryParamDef& Accessory() { return AccessoryParamDef::get_instance(); }

// Default AI parameters
static AiStandardInfoParamDef& AiStandardInfo() { return AiStandardInfoParamDef::get_instance(); }

// Armor Reinforcement parameters
static ArmorReinforceParamDef& ArmorReinforce() { return ArmorReinforceParamDef::get_instance(); }
//static ArmorParamDef& Protector() { return ArmorParamDef::get_instance(); } // Alias for armor params
//static ArmorParamDef& EquipProtector() { return ArmorParamDef::get_instance(); } // Alias for Armor params

// Armor parameters
static ArmorParamDef& Armor() { return ArmorParamDef::get_instance(); }

// Player character attack parameters
static AttackPcParamDef& AttackPc() { return AttackPcParamDef::get_instance(); }

// NPC attack parameters
static AttackNpcParamDef& AttackNpc() { return AttackNpcParamDef::get_instance(); }

// NPC Behavior parameters
static BehaviorNpcParamDef& BehaviorNpc() { return BehaviorNpcParamDef::get_instance(); }

// PC (Player character) Behavior parameters
static BehaviorPcParamDef& BehaviorPc() { return BehaviorPcParamDef::get_instance(); }

// Bullet (projectile) parameters
static BulletParamDef& Bullet() { return BulletParamDef::get_instance(); }
//static BulletParamDef& Projectile() { return BulletParamDef::get_instance(); } // Alias for Bullet

// Calculation Correction parameters
static CalcCorrectParamDef& CalcCorrect() { return CalcCorrectParamDef::get_instance(); }

// Character initializer parameters (Human NPCs)
static CharInitParamDef& CharInit() { return CharInitParamDef::get_instance(); }
//static CharInitParamDef& NpcChar() { return CharInitParamDef::get_instance(); } // Alias for CharInit

// Default Enemy Behavior Parameters
static EnemyStandardInfoParamDef& EnemyStandardInfo() { return EnemyStandardInfoParamDef::get_instance(); }

// Game Area parameters
static FaceGenParamDef& FaceGen() { return FaceGenParamDef::get_instance(); }

// Game Area parameters
static GameAreaParamDef& GameArea() { return GameAreaParamDef::get_instance(); }

// Hit material parameters
static HitMaterialParamDef& HitMaterial() { return HitMaterialParamDef::get_instance(); }

// Item parameters
static ItemParamDef& Item() { return ItemParamDef::get_instance(); }
static ItemParamDef& Goods() { return ItemParamDef::get_instance(); } // Alias for Item params

// Item Lot parameters
static ItemLotParamDef& ItemLot() { return ItemLotParamDef::get_instance(); }

// Knock-back parameters
static KnockBackParamDef& KnockBack() { return KnockBackParamDef::get_instance(); }

// Camera lock paramters
static LockCamParamDef& LockCam() { return LockCamParamDef::get_instance(); }
static LockCamParamDef& CamLock() { return LockCamParamDef::get_instance(); } // Alias for LockCam

// Magic parameters
static MagicParamDef& Magic() { return MagicParamDef::get_instance(); }

// Material set parameters
static MaterialSetParamDef& MaterialSet() { return MaterialSetParamDef::get_instance(); }

// Menu color parameters
static MenuColorParamDef& MenuColor() { return MenuColorParamDef::get_instance(); }

// Movement parameters
static MoveParamDef& Move() { return MoveParamDef::get_instance(); }

// NPC parameters
static NpcParamDef& Npc() { return NpcParamDef::get_instance(); }

// NPC Thought parameters
static NpcThinkParamDef& NpcThink() { return NpcThinkParamDef::get_instance(); }

// Object parameters
static ObjectParamDef& Object() { return ObjectParamDef::get_instance(); }

// Object action parameters
static ObjectActionParamDef& ObjectAction() { return ObjectActionParamDef::get_instance(); }

// World tendency change parameters
static QwcChangeParamDef& QwcChange() { return QwcChangeParamDef::get_instance(); }

// World tendency judge parameters
static QwcJudgeParamDef& QwcJudge() { return QwcJudgeParamDef::get_instance(); }

// Ragdoll parameters
static RagdollParamDef& Ragdoll() { return RagdollParamDef::get_instance(); }

// Skeleton parameters
static SkeletonParamDef& Skeleton() { return SkeletonParamDef::get_instance(); }

// Shop lineup parameters
static ShopLineupParamDef& ShopLineup() { return ShopLineupParamDef::get_instance(); }

// Special effect parameters
static SpEffectParamDef& SpEffect() { return SpEffectParamDef::get_instance(); }

// Special effect visual effects parameters
static SpEffectVfxParamDef& SpEffectVfx() { return SpEffectVfxParamDef::get_instance(); }

// Talk parameters
static TalkParamDef& Talk() { return TalkParamDef::get_instance(); }

// Throw parameters
static ThrowParamDef& Throw() { return ThrowParamDef::get_instance(); }

// Weapon parameters
static WeaponParamDef& Weapon() { return WeaponParamDef::get_instance(); }

// Weapon Reinforcement parameters
static WeaponReinforceParamDef& WeaponReinforce() { return WeaponReinforceParamDef::get_instance(); }




/////////////////////////////////////
////////// DRAW PARAMETERS //////////
/////////////////////////////////////

// Depth of Field Bank Parameters
static DofBankParamDef& DofBank() { return DofBankParamDef::get_instance(); }

// Fog Bank Parameters
static FogBankParamDef& FogBank() { return FogBankParamDef::get_instance(); }

// Environment Lights Texture Bank Parameters
static EnvLightTexBankParamDef& EnvLightTexBank() { return EnvLightTexBankParamDef::get_instance(); }

// Light Bank Parameters
static LightBankParamDef& LightBank() { return LightBankParamDef::get_instance(); }

// Point Light Bank Parameters
static PointLightBankParamDef& PointLightBank() { return PointLightBankParamDef::get_instance(); }

// Tone Correction Bank Parameters
static ToneCorrectBankParamDef& ToneCorrectBank() { return ToneCorrectBankParamDef::get_instance(); }

// Tone Map Bank Parameters
static ToneMapBankParamDef& ToneMapBank() { return ToneMapBankParamDef::get_instance(); }




// Checks if all param files have been loaded into memory
bool all_loaded();

// Injects code to capture starting address of each parameter file as it is loaded by the game
void patch();

// Restores original code that was patched to capture starting addresses of all param files.
//   (Call this after all params have been loaded)
void unpatch();

// Initializes all param file instances
void init(bool print_output = false);

} // namespace Params

#endif // DS1_FILE_LIB_GENERIC_GET_PARAMS_DEF_FILE_H_
