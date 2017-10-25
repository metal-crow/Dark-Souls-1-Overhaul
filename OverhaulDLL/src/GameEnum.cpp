/*
	DARK SOULS OVERHAUL
	
	Contributors to this file:
	    HotPocketRemix  -  Reversing and obtaining game enums/constants
		Meowmaritus     -  Reversing and obtaining game enums/constants
		Sean Pesce	    -  Converting the data to C++ formatting


	GameEnum.cpp

	Initializes constant static data that mirrors the data used by the Dark Souls game LUA scripts.


	Sources:
	    https://github.com/HotPocketRemix/DSEventScriptTools/blob/master/documentation/enums_doc.txt
		https://gist.github.com/Meowmaritus/2087819c36bdbcf7d160a6683a01beae
		https://gist.github.com/Meowmaritus/c3de22d05b694b227ae1f371cfb45242

*/


#include "GameEnum.h"

const float LuaEventConst::LadderAngle_A = 90;
const float LuaEventConst::LadderDist_A = 1.5f;
const float LuaEventConst::LadderTime_A = 0.2f;
const float LuaEventConst::TURNTIME = 0.05f;
const float LuaEventConst::SOUL_RATE_S = 0.1f;
const float LuaEventConst::SOUL_RATE_M = 0.25f;
const float LuaEventConst::SOUL_RATE_L = 0.5f;

