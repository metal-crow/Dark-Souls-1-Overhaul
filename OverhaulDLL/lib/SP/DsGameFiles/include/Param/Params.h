/*
    DARK SOULS GAME FILE C++ LIBRARY

    Contributors to this file:
        Sean Pesce	-	C++


    Param/Param.h



*/

#pragma once

#ifndef _DS1_FILE_LIB_GENERIC_GET_PARAMS_DEF_FILE_H_
    #define _DS1_FILE_LIB_GENERIC_GET_PARAMS_DEF_FILE_H_


#include "Param/AIStandardInfo.h"
#include "Param/Accessory.h"
#include "Param/Armor.h"
#include "Param/ArmorReinforcement.h"
#include "Param/Attack.h"
#include "Param/BehaviorNpc.h"
#include "Param/BehaviorPc.h"
#include "Param/Bullet.h"
#include "Param/CalcCorrect.h"
#include "Param/CharInit.h"
#include "Param/EnemyStandardInfo.h"
#include "Param/FaceGeneration.h"
#include "Param/GameArea.h"
#include "Param/Item.h"
#include "Param/ItemLot.h"
#include "Param/LockCam.h"
#include "Param/Magic.h"
#include "Param/MaterialSet.h"
#include "Param/MenuColorTable.h"
#include "Param/Move.h"
#include "Param/NPC.h"
#include "Param/NpcThink.h"
#include "Param/Object.h"
#include "Param/Ragdoll.h"
#include "Param/ShopLineup.h"
#include "Param/Skeleton.h"
#include "Param/SpecialEffect.h"
#include "Param/SpecialEffectVfx.h"
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

    // Attack parameters
    static AttackParamDef& Attack() { return AttackParamDef::get_instance(); }

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

    // Item parameters
    static ItemParamDef& Item() { return ItemParamDef::get_instance(); }
    static ItemParamDef& Goods() { return ItemParamDef::get_instance(); } // Alias for Item params

    // Item Lot parameters
    static ItemLotParamDef& ItemLot() { return ItemLotParamDef::get_instance(); }

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
}

#endif // _DS1_FILE_LIB_GENERIC_GET_PARAMS_DEF_FILE_H_
