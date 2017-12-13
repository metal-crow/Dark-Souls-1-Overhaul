/*
    DARK SOULS GAME FILE C++ LIBRARY

    Contributors to this file:
        HotPocketRemix  -  Reversing and obtaining game enums
        Meowmaritus     -  Reversing and obtaining game constants
        Sean Pesce	    -  Converting other contributers' data to C++ formatting, and obtaining other constant/enum values


    GameEnum.h

    Defines constants and enums that are equivalent to those used by the Dark Souls game LUA scripts.


    Sources:
        https://github.com/HotPocketRemix/DSEventScriptTools/blob/master/documentation/enums_doc.txt
        https://gist.github.com/Meowmaritus/2087819c36bdbcf7d160a6683a01beae
        https://gist.github.com/Meowmaritus/c3de22d05b694b227ae1f371cfb45242

*/




#ifndef _DS1_FILE_LIB_GAME_ENUMS_H_
#define _DS1_FILE_LIB_GAME_ENUMS_H_

#include <stdint.h>



//////////////////// Published by SeanP ////////////////////

enum class ATTACK_TYPE : uint8_t {
    MELEE = 0,  // Physical melee attack
    RANGED = 1,  // Physical ranged attacks such as arrows, darts, throwing knives, etc
    SPECIAL = 2   // Elemental attacks such as lightning, fire, etc
};
//typedef ATTACK_TYPE BEHAVIOR_REF_TYPE;


enum class SPECIAL_EFFECT_CATEGORY : uint8_t {
    NONE = 0,
    MAGIC = 3, // Fire, magic, lightning
    MIRACLE = 4,
    BELT_ITEM = 5,
    PHYSICAL = 6,
    PHYISCAL2 = 7
};
//typedef SPECIAL_EFFECT_CATEGORY BEHAVIOR_CATEGORY;


//////////////////// Published by HotPocketRemix ////////////////////

enum class ENUM_AI_STATUS_TYPE {
    Normal = 0,
    Recognition = 1,
    Alert = 2,
    Battle = 3
};


enum class ENUM_BITOP {
    Add = 0,
    Delete = 1,
    Invert = 2
};


enum class ENUM_BOOL {
    _FALSE = 0,
    _TRUE = 1
};


enum class ENUM_BUTTON_NUMBER {
    Button1 = 1,
    Button2 = 2,
    NoButton = 6
};


enum class ENUM_BUTTON_TYPE {
    YES_NO = 0,
    OK_CANCEL = 1
};


enum class ENUM_CATEGORY {
    Object = 0,
    Area = 1,
    Character = 2
};



enum class ENUM_CHARACTER_TYPE {
    Survival = 0,
    WhiteGhost = 1,
    BlackGhost = 2,
    GrayGhost = 8,
    Intruder = 12
};


enum class ENUM_CHARACTER_UPDATE_RATE {
    Never = -1,
    Always = 0,
    Every2frames = 2,
    Every5frames = 5
};



// Note: The translations are not accurate. Instead, they are based on what classes the game actually has corresponding to each value.

enum class ENUM_CLASS_TYPE {
    Warrior = 0,
    Knight = 1,
    Wanderer = 2,
    Thief = 3,
    Bandit = 4,
    Hunter = 5,
    Sorcerer = 6,
    Pyromancer = 7,
    Cleric = 8,
    Deprived = 9,
    Temp_Warrior = 20,
    Temp_Knight = 21,
    Temp_Sorcerer = 22,
    Temp_Pyromancer = 23,
    Chi_Warrior = 24,
    Chi_Knight = 25,
    Chi_Sorcerer = 26,
    Chi_Pyromancer = 27
};


enum class ENUM_COMPARISON_TYPE {
    EQUALS = 0,
    NOT_EQUALS = 1,
    GREATER_THAN = 2,
    LESS_THAN = 3,
    GREATER_THAN_OR_EQUALS = 4,
    LESS_THAN_OR_EQUALS = 5
};


enum class ENUM_CONDITION_STATE {
    _FALSE = 0,
    _TRUE = 1
};


enum class ENUM_CONTAINED {
    Outside = 0,
    Inside = 1
};


enum class ENUM_CUTSCENE_TYPE {
    Skippable = 0,
    Unskippable = 2,
    SkippablewithFadeOut = 8,
    UnskippablewithFadeOut = 10
};


enum class ENUM_DAMAGE_TARGET_TYPE {
    Character = 1,
    Map = 2,
    CharacterAndMap = 3
};


enum class ENUM_DEATH_STATUS {
    Alive = 0,
    Dead = 1
};


enum class ENUM_ENABLE_STATE {
    DISABLE = 0,
    ENABLE = 1
};


enum class ENUM_EVENT_END_TYPE {
    END = 0,
    RESTART = 1
};


enum class ENUM_FLAG_TYPE {
    EventFlagID = 0,
    EventID = 1,
    EventIDwithSlotNumber = 2
};


enum class ENUM_INTERPOLATION_STATE {
    Interpolated = 0,
    NotInterpolated = 1
};


enum class ENUM_ITEM_TYPE : int32_t {
    Weapon = 0,
    Armor = 1,
    Ring = 2,
    Item = 3
};
typedef ENUM_ITEM_TYPE ITEMLOT_ITEMCATEGORY;


enum class ENUM_LOGICAL_OPERATION_TYPE {
    allON = 0,
    allOFF = 1,
    notAllOFF = 2,
    notAllON = 3
};


enum class ENUM_MULTIPLAYER_STATE {
    Host = 0,
    Client = 1,
    Multiplayer = 2,
    Singleplayer = 3
};


enum class ENUM_NAVIMESH_TYPE {
    Solid = 1,
    Exit = 2,
    Obstacle = 4,
    Wall = 8,
    WallTouchingFloor = 32,
    LandingPoint = 64,
    Event = 128,
    Cliff = 256,
    Wide = 512,
    Ladder = 1024,
    Hole = 2048,
    Door = 4096,
    ClosedDoor = 8192
};


enum class ENUM_ON_OFF {
    OFF = 0,
    ON = 1
};


enum class ENUM_ON_OFF_CHANGE {
    OFF = 0,
    ON = 1,
    CHANGE = 2
};


enum class ENUM_OWN_STATE {
    Doesnotown = 0,
    Owns = 1
};


enum class ENUM_REACTION_ATTRIBUTE {
    SurvivalAndGray = 48,
    All = 255
};


enum class ENUM_REGISTER {
    OR07 = -7,
    OR06 = -6,
    OR05 = -5,
    OR04 = -4,
    OR03 = -3,
    OR02 = -2,
    OR01 = -1,
    MAIN = 0,
    AND01 = 1,
    AND02 = 2,
    AND03 = 3,
    AND04 = 4,
    AND05 = 5,
    AND06 = 6,
    AND07 = 7
};


enum class ENUM_SIGN_TYPE {
    BlueEyeSign = 0,
    BlackEyeSign = 1,
    RedEyeSign = 2,
    DetectionSign = 3,
    WhiteHelpSign = 4,
    BlackHelpSign = 5
};


enum class ENUM_SITE_TYPE {
    Part1 = 1,
    Part2 = 2,
    Part3 = 3,
    Part4 = 4,
    Part5 = 5,
    Part6 = 6,
    WeakPoint = 7,
    Part7 = 8,
    Part8 = 9
};


enum class ENUM_SOUND_TYPE {
    a_EvironmentalSound = 0,
    c_CharacterMotion = 1,
    f_MenuSE = 2,
    o_Object = 3,
    p_PolyDrama = 4,
    s_SFX = 5,
    m_BGM = 6,
    v_Voice = 7,
    x_FloorMaterialDependence = 8,
    b_ArmorMaterialDependence = 9,
    g_Ghost = 10
};


enum class ENUM_STATUE_TYPE {
    Petrified = 0,
    Crystallized = 1
};


enum class ENUM_TEAM_TYPE {
    Default = 255,
    Invalid = 0,
    Survival = 1,
    WhiteGhost = 2,
    BlackGhost = 3,
    GrayGhost = 4,
    WanderingGhost = 5,
    Enemy = 6,
    StrongEnemy = 7,
    Ally = 8,
    HostileAlly = 9,
    DecoyEnemy = 10,
    RedChild = 11,
    FightingAlly = 12,
    Intruder = 13
};


enum class ENUM_TENDENCY_TYPE {
    WhiteTendency = 0,
    BlackTendency = 1
};


enum class ENUM_TEXT_BANNER_TYPE {
    DemonKilled = 1,
    Death = 2,
    Revival = 3,
    SoulAcquisition = 4,
    TargetKilled = 5,
    GhostDeath = 6,
    BlackGhostDeath = 7,
    MapName = 8,
    Congratulations = 12,
    StadiumVictory = 15,
    StadiumDefeat = 16,
    StadiumDraw = 17
};


enum class ENUM_UPDATE_AUTH {
    Normal = 0,
    Forced = 4095
};



// This is not a real enum class in the file, but it's convenient to record these values somewhere.
// It's normally just encoded as integer value between 0 and 20.

enum class ENUM_COVENANT_TYPE {
    None = 0,
    WayofWhite = 1,
    PrincessGuard = 2,
    WarriorofSunlight = 3,
    Darkwraith = 4,
    PathoftheDragon = 5,
    GravelordServant = 6,
    ForestHunter = 7,
    DarkmoonBlade = 8,
    ChaosServant = 9
};




//////////////////// Published by Meowmaritus ////////////////////

class AI_Define_LUA_Constants {
    // Source:
    // https://gist.github.com/Meowmaritus/2087819c36bdbcf7d160a6683a01beae

    enum class GOAL_RESULT {
        Failed = -1,
        Continue = 0,
        Success = 1
    };

    enum class AI_DIR_TYPE {
        CENTER = 0,
        F = 1,
        B = 2,
        L = 3,
        R = 4,
        ToF = 5,
        ToB = 6,
        ToL = 7,
        ToR = 8,
        Top = 9
    };

    enum class DIST {
        Near = -1,
        Middle = -2,
        Far = -3,
        Out = -4,
        None = -5
    };

    enum class TARGET {
        NONE = -2,
        SELF = -1,
        ENE_0 = 0,
        FRI_0 = 10,
        EVENT = 20,
        LOCALPLAYER = 21,
        LowHp_Friend = 22
    };

    enum class POINT {
        INITIAL = 100,
        SNIPE = 101,
        EVENT = 102,
        MOVE_POINT = 103,
        NEAR_NAVIMESH = 104,
        FAR_NAVIGATE = 105,
        NEAR_NAVIGATE = 106,
        AI_FIXED_POS = 107,
        FAR_LANDING = 108,
        NEAR_LANDING = 109,
        SECOND_NEAR_LANDING = 110,
        INIT_POSE = 111,
        HitObstacle = 112,
        CurrRequestPosition = 114,
        NearMovePoint = 115,
        NEAR_OBJ_ACT_POINT = 116,
        SECOND_NEAR_OBJ_ACT_POINT = 117,
        LastSightPosition = 118,
        NearCorpsePosition = 119,
        AutoWalkAroundTest = 120
    };

    enum class EVENT_TARGET {
        _0 = 1000,
        _1 = 1001,
        _2 = 1002,
        _3 = 1003,
        _4 = 1004,
        _5 = 1005,
        _6 = 1006,
        _7 = 1007,
        _8 = 1008,
        _9 = 1009,
        _10 = 1010
    };

    enum class INTERRUPT {
        FindEnemy = 0,
        FindAttack = 1,
        Damaged = 2,
        Damaged_Stranger = 3,
        FindMissile = 4,
        SuccessGuard = 5,
        MissSwing = 6,
        GuardBegin = 7,
        GuardFinish = 8,
        GuardBreak = 9,
        Shoot = 10,
        ShootReady = 11,
        UseItem = 12,
        EnterBattleArea = 13,
        LeaveBattleArea = 14,
        CANNOT_MOVE = 15,
        Inside_ObserveArea = 16,
        ReboundByOpponentGuard = 17,
        ForgetTarget = 18,
        FriendRequestSupport = 19,
        TargetIsGuard = 20,
        HitEnemyWall = 21,
        SuccessParry = 22,
        CANNOT_MOVE_DisableInterupt = 23,
        ParryTiming = 24,
        RideNode_LadderBottom = 25,
        FLAG_RideNode_Door = 26,
        StraightByPath = 27,
        ChangedAnimIdOffset = 28,
        SuccessThrow = 29,
        LookedTarget = 30,
        LoseSightTarget = 31,
        RideNode_InsideWall = 32,
        MissSwingSelf = 33,
        GuardBreakBlow = 34
    };

    enum class PLATOON_STATE {
        None = 0,
        Caution = 1,
        Find = 2,
        ReplyHelp = 3,
        Battle = 4
    };

    enum class COORDINATE_TYPE {
        None = -1,
        Attack = 0,
        SideWalk_L = 1,
        SideWalk_R = 2,
        AttackOrder = 3,
        Support = 4,
        KIMERAbite = 100,
        UROKOIwaSupport = 110
    };

    enum class ORDER_TYPE {
        Role = 0,
        CallHelp = 1
    };

    enum class ROLE_TYPE {
        None = -1,
        Attack = 0,
        Torimaki = 1,
        Kankyaku = 2
    };

    enum class NPC_ATK {
        NormalR = 0,
        LargeR = 1,
        PushR = 2,
        NormalL = 3,
        GuardL = 4,
        Parry = 5,
        Magic = 6,
        MagicL = Magic,
        Item = 7,
        SwitchWep = 8,
        StepF = 9,
        StepB = 10,
        StepL = 11,
        StepR = 12,
        ChangeWep_R1 = 13,
        ChangeWep_R2 = 14,
        ChangeWep_L1 = 15,
        ChangeWep_L2 = 16,
        BackstepF = 17,
        BackstepB = 18,
        BackstepL = 19,
        BackstepR = 20,
        MagicR = 21,
        Ladder_10 = 22,
        Ladder_11 = 23,
        Ladder_12 = 24,
        Ladder_13 = 25,
        Ladder_20 = 26,
        Ladder_21 = 27,
        Ladder_22 = 28,
        Ladder_23 = 29
    };

    enum class AI_EXCEL_THINK_PARAM_TYPE {
        NONE = 0,
        maxBackhomeDist = 1,
        backhomeDist = 2,
        backhomeBattleDist = 3,
        nonBattleActLife = 4,
        BattleStartDist = 5,
        bMoveOnHearSound = 6,
        CannotMoveAction = 7,
        battleGoalID = 8,
        BackHome_LookTargetTime = 9,
        BackHome_LookTargetDist = 10,
        BackHomeLife_OnHitEnemyWall = 11,
        callHelp_IsCall = 12,
        callHelp_IsReply = 13,
        callHelp_MyPeerId = 14,
        callHelp_CallPeerId = 15,
        callHelp_DelayTime = 16,
        callHelp_CallActionId = 17,
        callHelp_ReplyBehaviorType = 18,
        callHelp_ForgetTimeByArrival = 19,
        callHelp_MinWaitTime = 20,
        callHelp_MaxWaitTime = 21,
        callHelp_ReplyActionId = 22,
        thinkAttr_doAdmirer = 23
    };

    enum class MOVE_TYPE {
        Patrol = 0,
        RoundTrip = 1,
        Randum = 2,
        Gargoyle_Air_Patrol = 3,
        Gargoyle_Landing = 4
    };

    enum class ARM {
        L = 0,
        R = 1
    };

    enum class WEP { // WEP = WEAP = WEAPON
        Primary = 0,
        Secondary = 1
    };

    enum class PARTS_DMG {
        NONE = 0,
        LV1 = 1,
        LV2 = 2,
        LV3 = 3,
        FINAL = 20
    };

    enum class SP_EFFECT_TYPE {
        NONE = 0,
        FIRE = 1,
        POIZON = 2, // POISON*
        LEECH = 3,
        SPOIL = 4,
        ILLNESS = 5,
        BLOOD = 6,
        CAMOUFLAGE = 7,
        HIDDEN = 8,
        MASOCHIST = 9,
        RECOVER_POZON = 10,
        RECOVER_ILLNESS = 11,
        RECOVER_BLOOD = 12,
        RECOVER_ALL = 13,
        SOUL_STEAL = 14,
        ZOOM = 15,
        WARP = 16,
        DEMONS_SOUL = 17,
        BLACK_DISPERSE = 18,
        TOUGH_GHOST = 19,
        WHITE_REQUEST = 20,
        BLACK_REQUEST = 21,
        CHANGE_BLACK = 22,
        REVIVE = 23,
        FORBID_USEMAGIC = 24,
        MIRACLE_DIRAY = 25,
        WHETSTONE = 26,
        SUSPENDED_REVIVE = 27,
        ENCHANT_WEAPON = 28,
        ENCHANT_ARMOR = 29,
        WIRE_WRAPED = 30,
        GHOST_PARAM_CHANGE = 31,
        PARALYSIS = 32,
        FLY_CROWD = 33,
        FIREMAN_STAGE_1 = 34,
        FIREMAN_STAGE_2 = 35,
        FIREMAN_STAGE_3 = 36,
        FIREMAN_STAGE_4 = 37,
        HALLUCINATION = 38,
        SOULCOIN = 39,
        TOUGH_SHIELD = 40,
        ANTIFIRE_SHIELD = 41,
        HP_RECOVERY = 42,
        FORCE_GHOST_STAGE1 = 43,
        FORCE_GHOST_STAGE2 = 44,
        FORCE_GHOST_STAGE3 = 45,
        PHEROMONE = 46,
        CAT_LANDING = 47,
        PINCH_ATTACKPOWER_UP = 48,
        PINCH_DEFENSIBILITY_UP = 49,
        REGENERATE = 50,
        TORCH = 51,
        WEAK_REGENERATE = 52,
        WEAK_CAMOUFLAGE = 53,
        WEAK_HIDDEN = 54,
        HINT_BLOOD_SIGN = 55,
        LEECH_FOOT = 56,
        YELLOW_CLOAK = 57,
        POINT_LIGHT = 58,
        BLOOD_SIGN_ESTIMATION = 59,
        ENCHANT_WEAPON_REGULAR = 60,
        ENCHANT_WEAPON_LARGE = 61,
        ENCHANT_WEAPON_FIRE = 62,
        ENCHANT_WEAPON_FIRE_LARGE = 63,
        ENCHANT_WEAPON_MAGIC = 64,
        CHIMERA_POWER_UP = 65,
        ITEM_DROP_RATE = 66
    };

    enum class OBJ_ACT_TYPE {
        LEVER = 0,
        DOOR = 1
    };

    enum class TEAM_TYPE {
        None = 0,
        Live = 1,
        WhiteGhost = 2,
        BlackGhost = 3,
        GlayGhost = 4,
        WanderGhost = 5,
        Enemy = 6,
        Boss = 7,
        Friend = 8,
        AngryFriend = 9,
        Decoy = 10,
        DecoyLike = 11,
        BattleFriend = 12,
        Intruder = 13,
        Neutral = 14,
        Charm = 15
    };

    enum class GUARD_GOAL_DESIRE_RET {
        Success = 1,
        Continue = 2,
        Failed = 3
    };

    enum class WEP_CATE {
        None = 0,
        Shield = 1,
        Bow = 2,
        Crossbow = 3,
        Wand = 4
    };

};
typedef AI_Define_LUA_Constants LuaAiConst;


class Event_Define_LUA_Constants {
    // Source:
    // https://gist.github.com/Meowmaritus/c3de22d05b694b227ae1f371cfb45242
    static const float LadderAngle_A;
    static const float LadderDist_A;
    static const float LadderTime_A;
    static const uint8_t once = 1;
    static const uint8_t everytime = 0;
    static const uint16_t LOCAL_PLAYER = 10000;
    static const uint16_t NET_PLAYER = 10001;
    static const uint16_t SYNCHROANIM = 4013;
    static const float TURNTIME;
    static const uint8_t TURNSKIP_ANGLE = 10;

    enum class ITEM_TYPE : uint32_t {
        WEAPON = 0,
        PROTECTER = 268435456,
        ACCESSORY = 536870912,
        GOODS = 1073741824,
        MASK = 4026531840
    };

    enum class HELPID {
        UP = 10010300,
        DOWN = 10010301
    };

    enum class ANIMEID {
        WALK = 7410,
        PICK = 7520,
        COFFER_PICK = 7522,
        KICK = 7400,
        OPEN = 7401,
        CLOSE = 7402,
        PULL_LEVER = 8000,
        PUSH_LEVER = 8000,
        TURN_HANDLE = 8010
    };

    enum class SOUND_TYPE {
        A = 0,
        C = 1,
        F = 2,
        O = 3,
        P = 4,
        S = 5,
        M = 6,
        V = 7,
        X = 8,
        B = 9,
        G = 10
    };

    enum class CHR_TYPE {
        LivePlayer = 0,
        GreyGhost = 8,
        WhiteGhost = 1,
        BlackGhost = 2
    };

    enum class TEAM_TYPE {
        None = 0,
        Live = 1,
        WhiteGhost = 2,
        BlackGhost = 3,
        GlayGhost = 4,
        WanderGhost = 5,
        Enemy = 6,
        Boss = 7,
        Friend = 8,
        AngryFriend = 9,
        DecoyEnemy = 10,
        BloodChild = 11,
        BattleFriend = 12
    };

    enum class MSG_CATEGORY {
        SAMPLE = 0,
        TALK = 1,
        BLOOD = 2,
        ITEM = 10,
        WEP = 11,
        PRO = 12,
        ACC = 13,
        ITEM_EXP = 20,
        WEP_EXP = 21,
        PRO_EXP = 22,
        ACC_EXP = 23,
        EVENT = 30,
        DIALOG = 78
    };

    static const uint16_t SYSTEM_ID_IGNORE = 4048;
    static const uint8_t IGNORE_TIME = 3;
    static const uint16_t SYSTEM_WARP = 4052;
    static const uint16_t SYSTEM_CHECK_POINT = 4054;
    static const uint16_t SYSTEM_GENDIALOG = 4061;

    enum class INFOMENU_TYPE {
        LIST = 0,
        SIMPLE = 1
    };

    enum class EV_HIT {
        CHR = 1,
        HIT = 2,
        ALL = 3
    };

    enum class MP_ATTR {
        SESSION = 1,
        NOSESSION = 2,
        HOST = 4,
        CLIENT = 8,
        LIVE = 16,
        GREY = 32,
        WHITE = 64,
        BLACK = 128,
        ALL = 255
    };

    enum class TALK_ATTR {
        REPEAT = 1,
        PAD = 2,
        DRAW = 4,
        VOICE = 8,
        ALL = 255
    };

    enum class TAG_IDX {
        liveChara = 0,
        joinChara = 1,
        deadChara = 2,
        leaveChara = 3
    };

    enum class TEXT_TYPE {
        KillDemon = 1,
        Dead = 2,
        Revival = 3,
        SoulGet = 4,
        TargetClear = 5,
        GhostDead = 6,
        BlackClear = 7,
        MapName = 8,
        MagicResurrection = 9,
        RingNormalResurrection = 10,
        RingCurseResurrection = 11,
        Congratulations = 12,
        Bonfire = 13
    };

    enum class USER_ID {
        X_Unknown = -1,
        Network_Error = 0,
        Network_Notice = 1,
        Network_MultiPlay = 2,
        Network_Login = 3,
        Event_SosLost_White = 4,
        Event_SosLost_Red = 5,
        Event_SosLost_Black = 6,
        Event_GetItem = 7
    };

    enum class DEATH_STATE {
        Normal = 0,
        MagicResurrection = 1,
        RingNormalResurrection = 2,
        RingCurseResurrection = 3
    };

    enum class CLEAR_DELAYTIME {
        GETSOUL = 3,
        LEAVE = 5
    };

    enum class SUMMONPARAM_TYPE {
        None = 0,
        White = -1,
        Black = -2,
        FroceJoinBlack = -3,
        DetectBlack = -4,
        InvadeNito = -7,
        Dragonewt = -10,
        InvadeBounty = -11,
        Coliseum = -12
    };

    static const float SOUL_RATE_S;
    static const float SOUL_RATE_M;
    static const float SOUL_RATE_L;

    enum class INVADE_TYPE {
        None = 0,
        NormalWhite = 1,
        NormalBlack = 2,
        ForceJoinBlack = 3,
        DetectBlack = 4,
        WhiteRescue = 5,
        BlackRescue = 6,
        Nito = 7,
        ThievesGuild = 8,
        OtoutoUmbasa = 9,
        Dragonewt = 10,
        InvadeBounty = 11,
        ColiseumOneB = 12,
        ColiseumTwoA2 = 13,
        ColiseumTwoB1 = 14,
        ColiseumTwoB2 = 15,
        ColiseumBrB = 16,
        ColiseumBrC = 17,
        ColiseumBrD = 18
    };

};
typedef Event_Define_LUA_Constants LuaEventConst;


#endif // _DS1_FILE_LIB_GAME_ENUMS_H_
