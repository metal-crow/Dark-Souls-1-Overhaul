/*
	DARK SOULS OVERHAUL

	Contributors to this file:
		Sean Pesce	-	C++


	ParamDef.h

	ParamDef namespace header, used to reference ParamDef classes
	more intuitively.

*/

#pragma once

#ifndef _DS1_OVERHAUL_PARAM_DEF_NAMESPACE_H_
	#define _DS1_OVERHAUL_PARAM_DEF_NAMESPACE_H_



#include "ParamDef/ArmorParamDef.h"
#include "ParamDef/BehaviorNPCParamDef.h"
#include "ParamDef/BulletParamDef.h"
#include "ParamDef/CharInitParamDef.h"
#include "ParamDef/ItemParamDef.h"
#include "ParamDef/LockCamParamDef.h"
#include "ParamDef/MagicParamDef.h"
#include "ParamDef/NPCParamDef.h"
#include "ParamDef/ShopLineupParamDef.h"
#include "ParamDef/SpecialEffectParamDef.h"
#include "ParamDef/ThrowParamDef.h"



namespace ParamDef {

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

	// Armor parameters
	static ArmorParamDef& Armor() { return ArmorParamDef::get_instance(); }
	//static ArmorParamDef& Protector() { return ArmorParamDef::get_instance(); } // Alias for armor params
	//static ArmorParamDef& EquipProtector() { return ArmorParamDef::get_instance(); } // Alias for Armor params

	// NPC Behavior parameters
	static BehaviorNpcParamDef& BehaviorNpc() { return BehaviorNpcParamDef::get_instance(); }

	// Bullet (projectile) parameters
	static BulletParamDef& Bullet() { return BulletParamDef::get_instance(); }
	//static BulletParamDef& Projectile() { return BulletParamDef::get_instance(); } // Alias for Bullet

	// Camera lock paramters
	static LockCamParamDef& CamLock() { return LockCamParamDef::get_instance(); }

	// Character initializer parameters (Human NPCs)
	static CharInitParamDef& CharInit() { return CharInitParamDef::get_instance(); }
	//static CharInitParamDef& NpcChar() { return CharInitParamDef::get_instance(); } // Alias for CharInit

	// Item parameters
	static ItemParamDef& Item() { return ItemParamDef::get_instance(); }
	static ItemParamDef& Goods() { return ItemParamDef::get_instance(); } // Alias for Item params

	// Magic parameters
	static MagicParamDef& Magic() { return MagicParamDef::get_instance(); }

	// NPC parameters
	static NpcParamDef& Npc() { return NpcParamDef::get_instance(); }
	//static NpcParamDef& Enemy() { return NpcParamDef::get_instance(); } // Alias for Npc params

	// Shop lineup parameters
	static ShopLineupParamDef& ShopLineup() { return ShopLineupParamDef::get_instance(); }

	// Special effect parameters
	static SpEffectParamDef& SpEffect() { return SpEffectParamDef::get_instance(); }

	// Throw parameters
	static ThrowParamDef& Throw() { return ThrowParamDef::get_instance(); }

}



#endif _DS1_OVERHAUL_PARAM_DEF_NAMESPACE_H_