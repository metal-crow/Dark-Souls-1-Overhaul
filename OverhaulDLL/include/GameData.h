/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Sean Pesce    -  C++
        Metal-Crow    -  C++
        RavagerChris  -  Reverse engineering of game files, gesture cancelling technique
*/

#pragma once

#ifndef _DS1_OVERHAUL_GAME_DATA_H_
    #define _DS1_OVERHAUL_GAME_DATA_H_



#include <stdint.h>
#include <optional>
#include <unordered_set>

#include "DarkSoulsOverhaulMod.h"
#include "ModData.h"
#include "SP/main.h"
#include "SP/memory.h"
#include "SP/memory/pointer.h"
#include "SP/memory/aob_scan.h"
#include "Asset/Animation/Tae.h"
#include "PlayerInsStruct.h"


enum InventorySlots
{
    LeftHand1 = 0x0,
    RightHand1 = 0x1,
    LeftHand2 = 0x2,
    RightHand2 = 0x3,
    Arrow1 = 0x4,
    Bolt1 = 0x5,
    Arrow2 = 0x6,
    Bolt2 = 0x7,
    ArmorHead = 0x8,
    ArmorBody = 0x9,
    ArmorArms = 0xa,
    ArmorLegs = 0xb,
    Ring1 = 0xd,
    Ring2 = 0xe,
    Quickbar1 = 0xf,
    Quickbar2 = 0x10,
    Quickbar3 = 0x11,
    Quickbar4 = 0x12,
    Quickbar5 = 0x13,
    END
};

enum TAE_type0_param_values {
    cancel_by_atk = 4,
    lock_rotation = 7,
    activate_iframes = 8,
    cancel_by_moving = 11,
    cancel_by_left_atk = 16,
    cancel_by_left_block = 22,
    cancel_by_rolling_or_backstepping = 26,
    cancel_by_using_consumable = 31,
    cancel_by_two_handing = 32,
    allow_animation_cancel_events = 87,
    cap_movement_to_walk_speed = 90,
    disable_sprinting = 91,
    stop_all_movement = 89,
};

enum AnimationStateTypesEnum {
    Invalid = 31,
    Lower_Action = 24,
    Lower_Attack = 18,
    Lower_Damage_L = 26,
    Lower_Damage_S = 19,
    Lower_Event = 25,
    Lower_Event_HiPrio = 30,
    Lower_ExtraAnim = 27,
    Lower_Guard = 17,
    Lower_SpWait = 16,
    Lower_SpecialAttack = 23,
    Lower_TaeExtraAnim = 28,
    Lower_ThrowAnim = 29,
    Lower_Turn = 22,
    Lower_Wait = 15,
    Lower_WalkFB = 21,
    Lower_WalkLR = 20,
    None = 14,
    Upper_Action = 8,
    Upper_Attack = 6,
    Upper_Damage_L = 11,
    Upper_Damage_S = 9,
    Upper_Event = 10,
    Upper_Event_HiPrio = 13,
    Upper_Guard = 5,
    Upper_SpWait = 1,
    Upper_SpecialAttack = 7,
    Upper_ThrowAnim = 12,
    Upper_Turn = 4,
    Upper_Wait = 0,
    Upper_WalkFB = 3,
    Upper_WalkLR = 2
};

enum SessionActionResultEnum
{
    NoSession = 0,
    TryToCreateSession = 1,
    FailedToCreateSession = 2,
    CreateSessionSuccess = 3,
    TryToJoinSession = 4,
    FailedToJoinSession = 5,
    JoinSessionSuccess = 6
};

static const float FRAMETIME = 0.01666666666f;

class Game
{
public:

    ////////////////////////////////////////
    ////////////// GAME DATA ///////////////
    ////////////////////////////////////////

    // Base address of Dark Souls game process
    static uint64_t ds1_base;

    // Base address of fmod_event64 dll process
    static uint64_t fmod_event64_base;

    // Base address for world character data
    static uint64_t char_class_base;

    static uint64_t frpg_net_base;

    static uint64_t game_data_man;

    static uint64_t world_chr_man_imp;

    static uint64_t param_man;

    static uint64_t solo_param_man;

    static uint64_t file_man;

    static uint64_t session_man_imp;

    static uint64_t menu_man;

    static uint64_t bullet_man;

    static uint64_t unknown_global_struct_141d283a8;

    static uint64_t frpg_net_man;

    static uint64_t pad_man;

    static uint64_t frpg_system;

    static uint64_t damage_man;

    static uint64_t frpg_havok_man_imp;

    static uint64_t sfx_man;

    static uint64_t throw_man;

    static uint64_t delay_delete_man;

    static uint64_t QInputMgrWindowsFantasy;

    static uint64_t InputDirectionMovementMan;

    static uint64_t LockTgtManImp;

    static const uint64_t calculate_attack_damage_offset = 0x2f11bf2;

    static const uint64_t char_loaded_injection_offset = 0x24f1b0;
    static const uint64_t char_loading_injection_offset = 0x27a870;

    static const uint64_t gui_hpbar_max_injection_offset = 0x6797d7;

    static const uint64_t MoveMapStep_New_injection_offset = 0x24e600;

    static const uint64_t InitAndStart_DLThread_injection_offset = 0xcc5c9c;

    static const uint64_t Destruct_DLThread_injection_offset = 0xcc5d60;

    static const uint64_t LogoSkip_offset = 0x70f130;

    static const uint64_t Step_PadManipulator_GetInputs_offset = 0x396860;

    // Player character status (loading, human, co-op, invader, hollow)
    static sp::mem::pointer<int32_t> player_char_status;

    static bool first_character_loaded;

    // Address of lava brightness effect (used for dimming lava)
    static uint8_t *lava_luminosity;

    // Game saving on/off
    static sp::mem::pointer<uint8_t> saves_enabled;

    // Multiplayer node count
    static int node_count;

    //Const handle for the PC
    static const uint32_t PC_Handle = 0x10044000;

    // Pointers for the game malloc heap entries
    static const uint64_t internal_heap_2 = 0x141b68f10;
    static const uint64_t internal_heap_3 = 0x141b68f20;

    /////////////////////////////////////////
    /////////////// FUNCTIONS ///////////////
    /////////////////////////////////////////

    // Initializes pointers and base addresses required for most other functions
    static void init();

    // Any injections that would have been part of Game::Init()
	static void injections_init();

    // Runs tasks that were deferred until a character was loaded
    static bool on_character_load(void*);

    // Check if dim lava mod is currently active
    static bool dim_lava_enabled();

    // Dim lava effects or restore default lava visuals
    static void enable_dim_lava(bool dim);

    // Returns multiplayer node count as an int (or -1 if player is not online)
    static int get_node_count();

    // Enables/Disables automatic game disconnection when low framerate is detected
    static void disable_low_fps_disconnect(bool enable);

    // Returns current player character body animation ID (attacking, rolling, gestures, etc)
    static std::optional<int32_t> get_player_body_anim_id();

    static std::optional<int32_t> get_player_upper_body_anim_id();

    static std::optional<int32_t> get_player_lower_body_anim_id();

    static std::optional<void*> get_player_animationMediator();

    static void* get_PlayerIns_AnimationMediator(uint64_t playerIns);

    static int32_t get_animation_mediator_state_animation(void* animationMediator, AnimationStateTypesEnum state_id);

    static bool set_animation_currentProgress(void* animationMediator, AnimationStateTypesEnum state_id, float new_progressTime);

    // Return pointer to current game time in milliseconds since the game has started
    static std::optional<uint32_t*> get_game_time_ms();

	static uint32_t get_frame_count();

	static std::optional<uint64_t> get_pc_entity_pointer();

	static std::optional<float*> get_pc_position();


                    /////////////////////////////////////////
                    //////////////// PATCHES ////////////////
                    /////////////////////////////////////////

    // Set available pool of memory that Dark Souls allocates for itself
    static void set_memory_limit();

    static float current_hpbar_max;

    // Fix the bug where the player HP could be greater than the displayed GUI bar
    static void set_gui_hpbar_max();

    // Gets currently equipped L hand weapon
	static std::optional<uint32_t> left_hand_weapon();

    // Gets currently equipped R hand weapon
    static std::optional<uint32_t> right_hand_weapon();
    
	static bool playerchar_is_loaded();

    static void stopDurabilityDamage(bool enable);

	static std::optional<uint32_t> get_player_char_max_hp();

	static std::optional<float> get_entity_rotation(void * entity_ptr);

	static std::optional<uint8_t> get_area_number();

    static std::optional<uint8_t> get_world_number();

    static std::optional<int32_t*> get_online_area_id_ptr();

    static std::optional<int32_t*> get_area_id_ptr();

    static std::optional<int32_t*> get_saved_chars_menu_flag();

    static std::optional<int32_t*> get_main_menu_flag();

    static std::optional<uint8_t*> get_saved_chars_preview_data();

    static std::optional<uint32_t> get_pc_playernum();

    static std::optional<uint64_t> get_connected_player(uint32_t i);

    static std::optional<int32_t> convert_handle_to_playernum(uint32_t handle);

    static std::optional<uint32_t> convert_playernum_to_handle(uint32_t playernum);

	static std::optional<void*> get_pc_ActiveState_EzStateMachineImpl();

	static std::optional<uint64_t> get_EzStateMachineImpl_curstate_id(void * EzStateMachineImpl);

    static std::optional<SessionActionResultEnum> get_SessionManagerImp_session_action_result();

    static std::optional<void*> get_SessionManagerImp_SteamSessionLight();

    static std::optional<uint32_t> get_SessionManagerImp_Next_Player_Num();

    static std::optional<void*> get_PlayerIns();

	static uint32_t get_equipped_inventory(uint64_t playerins, InventorySlots index);

    static InventorySlots return_weaponslot_in_hand(uint64_t playerins, bool right_hand);

	static bool player_has_speffect(uint64_t playerins, std::unordered_set<uint32_t> speffects);

    static std::optional<SpecialEffect_Info*> player_get_speffect(uint64_t playerins, uint32_t speffectId);

    static void player_add_speffect(uint32_t speffectId);

	static std::optional<void*> get_host_player_gamedata();

	static bool set_display_name(bool useSteam);

	static uint64_t get_accurate_time();

	static float convert_time_to_offset(uint64_t time);

    static void show_popup_message(const wchar_t * msg);

    static void show_banner_message(const wchar_t * msg);

	static std::optional<void*> find_bullet(uint32_t owner_handle, uint32_t bullet_num);

    static std::optional<void*> find_unfired_bullet(uint32_t owner_handle, float x_pos, float y_pos, float z_pos, std::unordered_set<uint8_t> formidden_nums);

    static bool set_invasion_refresh_timer(float newtime);

    static bool set_blue_invasion_refresh_timer(float newtime);

    static uint32_t get_player_chr_type(uint64_t playerIns);

    static std::optional<int32_t*> get_MP_AreaID_ptr();

    static void* game_malloc(size_t size, size_t alignment, uint64_t heap);

    static void game_free(void* p);

    static void game_free_alt(void* p);

	static void* get_MoveMapStep();

	static void Step_GameSimulation(bool renderFrame = false);

    static void SuspendThreads();

    static void ResumeThreads();

    static int32_t get_SessionPlayerNumber_For_ConnectedPlayerData(uint64_t connectedplayerdata);

    static void set_ReadInputs_allowed(bool allow);

    static void invalidate_function_caches();

    /*
     * Help speedup some functions by, whenever we're loaded into an area,
     * preload/preresolve some pointers and values so they can be much more quickly read when we need them
     * This function should be called whenever an area is loaded (after player status changes from loading)
    */
	static void preload_function_caches();

    static void update_ChrAsmModelRes_model(uint64_t ChrAsmModelRes, uint64_t ChrAsmModelResElem, uint32_t newModelId, bool useProtector, bool useWeapon);



                    /////////////////////////////////////////
                    ////////////// HUD-RELATED //////////////
                    /////////////////////////////////////////

    class Hud {
    public:

        // Additional HUD elements
        static bool get_show_compass_radial();
        static void set_show_compass_radial(bool enable);
        static bool get_show_compass_bar();
        static void set_show_compass_bar(bool enable);
        static bool get_show_elevation_meter();
        static void set_show_elevation_meter(bool enable);
        static bool get_show_node_graph();
        static void set_show_node_graph(bool enable, bool game_flag_only = true);
    };



                    /////////////////////////////////////////
                    ////////////// FILE-RELATED /////////////
                    /////////////////////////////////////////
};
typedef Game::Hud Hud;

// Game functions that can be called directly

typedef uint32_t get_AnimationData_FUNC(ActionCtrl* actionctrl, uint32_t i);
static get_AnimationData_FUNC* get_AnimationData = (get_AnimationData_FUNC*)0x1403853c0;

typedef uint16_t compress_gamedata_flags_FUNC(uint64_t equipgamedata);
static compress_gamedata_flags_FUNC* compress_gamedata_flags = (compress_gamedata_flags_FUNC*)0x14074a5e0;

typedef uint32_t get_currently_selected_magic_id_FUNC(PlayerIns* playerins);
static get_currently_selected_magic_id_FUNC* get_currently_selected_magic_id = (get_currently_selected_magic_id_FUNC*)0x14035fd10;

typedef bool sendNetMessageToAllPlayers_FUNC(uint64_t sessionMan, uint32_t type, void* data, int size);
static sendNetMessageToAllPlayers_FUNC* sendNetMessageToAllPlayers = (sendNetMessageToAllPlayers_FUNC*)0x14050b880;

typedef uint32_t getNetMessage_FUNC(uint64_t session_man, uint64_t ConnectedPlayerData, uint32_t type, void* data_out, int max_size);
static getNetMessage_FUNC* getNetMessage = (getNetMessage_FUNC*)0x14050b540;

typedef uint16_t GetEntityNumForThrow_FUNC(void* WorldChrManImp, void* playerIns);
static GetEntityNumForThrow_FUNC* GetEntityNumForThrow = (GetEntityNumForThrow_FUNC*)0x142847c6a;

typedef bool sendNetMessage_FUNC(uint64_t sessionMan, uint64_t ConnectedPlayerData, uint32_t type, void* data, int size);
static sendNetMessage_FUNC* sendNetMessage = (sendNetMessage_FUNC*)0x14050b6b0;

typedef PlayerIns* getPlayerInsForConnectedPlayerData_FUNC(void* worldchrman, void* ConnectedPlayerData);
static getPlayerInsForConnectedPlayerData_FUNC* getPlayerInsForConnectedPlayerData = (getPlayerInsForConnectedPlayerData_FUNC*)0x140371d90;

typedef void PlayerIns_SetHp_FUNC(void* playerins, uint64_t curHp);
static PlayerIns_SetHp_FUNC* PlayerIns_SetHp = (PlayerIns_SetHp_FUNC*)0x140322910;

typedef void ChrAsm_Set_Equipped_Items_FromNetwork_FUNC(void* EquipGameData, uint32_t index, uint32_t given_item_id, int param_4, bool param_5);
static ChrAsm_Set_Equipped_Items_FromNetwork_FUNC* ChrAsm_Set_Equipped_Items_FromNetwork = (ChrAsm_Set_Equipped_Items_FromNetwork_FUNC*)0x140746840;

typedef void set_playergamedata_flags_FUNC(void* EquipGameData, uint16_t net_data);
static set_playergamedata_flags_FUNC* set_playergamedata_flags = (set_playergamedata_flags_FUNC*)0x14074a550;

typedef ChrIns* getEntity_FUNC(uint64_t WorldChrMan, uint32_t entityNum);
static getEntity_FUNC* getEntity = (getEntity_FUNC*)0x1428611e2;

typedef void* getThrowParamFromThrowId_FUNC(uint32_t throw_id);
static getThrowParamFromThrowId_FUNC* getThrowParamFromThrowId = (getThrowParamFromThrowId_FUNC*)0x140537210;

typedef void putAttackerIntoThrowAnimation_FUNC(uint64_t param_1);
static putAttackerIntoThrowAnimation_FUNC* putAttackerIntoThrowAnimation = (putAttackerIntoThrowAnimation_FUNC*)0x1403acc70;

typedef void putDefenderIntoThrowAnimation_FUNC(uint64_t param_1, byte param_2);
static putDefenderIntoThrowAnimation_FUNC* putDefenderIntoThrowAnimation = (putDefenderIntoThrowAnimation_FUNC*)0x1403acd70;

typedef void Apply_Speffect_FUNC(SpecialEffect*, uint32_t speffect_id, float const);
static Apply_Speffect_FUNC* Apply_Speffect = (Apply_Speffect_FUNC*)0x140402af0;

typedef SpecialEffect_Info* SpecialEffect_Remove_SpecialEffectInfo_FUNC(SpecialEffect*, SpecialEffect_Info*, uint8_t const);
static SpecialEffect_Remove_SpecialEffectInfo_FUNC* SpecialEffect_Remove_SpecialEffectInfo = (SpecialEffect_Remove_SpecialEffectInfo_FUNC*)0x140405ee0;

typedef void ActionCtrl_ApplyEzState_FUNC(ActionCtrl* actionctrl, uint32_t unk, uint32_t ezState);
static ActionCtrl_ApplyEzState_FUNC* ActionCtrl_ApplyEzState = (ActionCtrl_ApplyEzState_FUNC*)0x140385440;

typedef void ChrCtrl_Func_30_FUNC(ChrCtrl* param_1, float FrameTime_const);
static ChrCtrl_Func_30_FUNC* ChrCtrl_Func_30 = (ChrCtrl_Func_30_FUNC*)0x14037c250;

typedef void Set_Player_Sex_Specific_Attribs_FUNC(EquipGameData * EquipGameData, int playerSex, int chrType);
static Set_Player_Sex_Specific_Attribs_FUNC* Set_Player_Sex_Specific_Attribs = (Set_Player_Sex_Specific_Attribs_FUNC*)0x14074bf30;

typedef void PlayerIns_Update_curSelectedMagicId_FUNC(PlayerIns* param_1, uint32_t curSelectedMagicId);
static PlayerIns_Update_curSelectedMagicId_FUNC* PlayerIns_Update_curSelectedMagicId = (PlayerIns_Update_curSelectedMagicId_FUNC*)0x14035fd80;

typedef void* smallObject_internal_malloc_FUNC(uint64_t heap, uint64_t size, uint64_t align);
static smallObject_internal_malloc_FUNC* smallObject_internal_malloc = (smallObject_internal_malloc_FUNC*)0x140cc20d0;

typedef void* smallObject_internal_dealloc_FUNC(uint64_t heap, void* obj, uint64_t size, uint64_t align);
static smallObject_internal_dealloc_FUNC* smallObject_internal_dealloc = (smallObject_internal_dealloc_FUNC*)0x140cc2370;

typedef bool ok_to_enter_equipment_menu_FUNC(void* playerins);
static ok_to_enter_equipment_menu_FUNC* ok_to_enter_equipment_menu = (ok_to_enter_equipment_menu_FUNC*)0x140361140;

typedef void* Find_ResCap_FUNC(void* ResCapArray, const wchar_t* text);
static Find_ResCap_FUNC* Find_ResCap = (Find_ResCap_FUNC*)0x140518a10;

typedef void Unload_ResCap_FUNC(void* ResCapArray, void* ResCap);
static Unload_ResCap_FUNC* Unload_ResCap = (Unload_ResCap_FUNC*)0x140518c00;

typedef void* ParambndFileCap_Load_FUNC(const wchar_t* filename, void* param_2, void* taskItem, void* fileCap_next_functionPtrLoad);
static ParambndFileCap_Load_FUNC* ParambndFileCap_Load = (ParambndFileCap_Load_FUNC*)0x1405a08a0;

typedef void Force_PlayerReload_FUNC(void* world_chr_man_imp, const wchar_t* c0000);
static Force_PlayerReload_FUNC* Force_PlayerReload = (Force_PlayerReload_FUNC*)0x140370920;

typedef uint32_t Calculate_MaxHP_From_Vit_FUNC(uint32_t vit);
static Calculate_MaxHP_From_Vit_FUNC* Calculate_MaxHP_From_Vit = (Calculate_MaxHP_From_Vit_FUNC*)0x1402df9a0;

typedef uint32_t Calculate_MaxMP_From_Att_FUNC(uint32_t att);
static Calculate_MaxMP_From_Att_FUNC* Calculate_MaxMP_From_Att = (Calculate_MaxMP_From_Att_FUNC*)0x1402dfa60;

typedef uint32_t Calculate_MaxSP_From_End_FUNC(uint32_t end);
static Calculate_MaxSP_From_End_FUNC* Calculate_MaxSP_From_End = (Calculate_MaxSP_From_End_FUNC*)0x1402dfcd0;

typedef void Step_MapArea_MapAreaObjects_and_NearbyMapAreas_FUNC(void* FieldArea, float frame_time, uint32_t param_3, uint8_t param_4, uint8_t param_5);
static Step_MapArea_MapAreaObjects_and_NearbyMapAreas_FUNC* Step_MapArea_MapAreaObjects_and_NearbyMapAreas = (Step_MapArea_MapAreaObjects_and_NearbyMapAreas_FUNC*)0x1403cbb50;

typedef void Step_Chr_FUNC(void* movemapstep, float frame_time, byte param_3);
static Step_Chr_FUNC* Step_Chr = (Step_Chr_FUNC*)0x1402510d0;

typedef void Step_Bullet_FUNC(void* bulletman, float frame_time);
static Step_Bullet_FUNC* Step_Bullet = (Step_Bullet_FUNC*)0x140429940;

typedef void Step_DamageMan_FUNC(void* damageman, float frame_time);
static Step_DamageMan_FUNC* Step_DamageMan = (Step_DamageMan_FUNC*)0x1403c8dd0;

typedef void Step_Havok_FUNC(void* FrpgHavokManImp, float frame_time);
static Step_Havok_FUNC* Step_Havok = (Step_Havok_FUNC*)0x142f9d251;

typedef void FinishStep_Havok_FUNC(void* FrpgHavokManImp);
static FinishStep_Havok_FUNC* FinishStep_Havok = (FinishStep_Havok_FUNC*)0x1402a32d0;

typedef void MoveMapStep_Step_13_FUNC(void* movemapstep, float frame_time);
static MoveMapStep_Step_13_FUNC* MoveMapStep_Step_13 = (MoveMapStep_Step_13_FUNC*)0x14024f6b0;

typedef uint64_t* FUN_140cc29c0_FUNC(void* p);
static FUN_140cc29c0_FUNC* FUN_140cc29c0 = (FUN_140cc29c0_FUNC*)0x140cc29c0;

typedef void InGame_Free_FUNC(void* p);
static InGame_Free_FUNC* InGame_Free = (InGame_Free_FUNC*)0x1410e3f6c;

typedef void* InGame_Malloc_FUNC(size_t size, size_t alignment, void* heap);
static InGame_Malloc_FUNC* InGame_Malloc = (InGame_Malloc_FUNC*)0x140cc3e10;

typedef void ChrAsmModelRes_Load_PartsbndFileCap_Entry_FUNC(void* ChrAsmModelRes, void* ChrAsm, uint8_t param_3, uint8_t param_4, uint8_t param_5, uint8_t param_6, uint8_t param_7, uint8_t param_8);
static ChrAsmModelRes_Load_PartsbndFileCap_Entry_FUNC* ChrAsmModelRes_Load_PartsbndFileCap_Entry = (ChrAsmModelRes_Load_PartsbndFileCap_Entry_FUNC*)0x14020a280;

typedef void lua_SetEventSpecialEffect_2_FUNC(void* unused, uint32_t target, uint32_t speffectId);
static lua_SetEventSpecialEffect_2_FUNC* lua_SetEventSpecialEffect_2_function = (lua_SetEventSpecialEffect_2_FUNC*)0x140487d00;

typedef void Step_PadMan_FUNC(float frame_time);
static Step_PadMan_FUNC* Step_PadMan = (Step_PadMan_FUNC*)0x1401af3b0;

typedef bool Create_BannerMessage_Func_Typedef(uint64_t menuman_banner, uint32_t banner_type, const wchar_t* str);
static Create_BannerMessage_Func_Typedef* Create_BannerMessage_Func = (Create_BannerMessage_Func_Typedef*)0x14071fc60;

typedef void Step_PadManipulator_FUNC(PadManipulator* PadManipulator, float frameTime, PlayerCtrl* PlayerCtrl);
static Step_PadManipulator_FUNC* Step_PadManipulator = (Step_PadManipulator_FUNC*)0x140396860;

typedef bool PlayerIns_Is_NetworkedPlayer_FUNC(PlayerIns* pc);
static PlayerIns_Is_NetworkedPlayer_FUNC* PlayerIns_Is_NetworkedPlayer = (PlayerIns_Is_NetworkedPlayer_FUNC*)0x1403226e0;

typedef bool PadDevice_GetInputI_FUNC(void* PadDevice, uint32_t inputI);
static PadDevice_GetInputI_FUNC* PadDevice_GetInputI = (PadDevice_GetInputI_FUNC*)0x1401a5ca0;

typedef void* PadMan_GetPadDevice_FUNC(uint32_t DeviceNum);
static PadMan_GetPadDevice_FUNC* PadMan_GetPadDevice = (PadMan_GetPadDevice_FUNC*)0x1401af490;

typedef void* EquipParamWeaponInfo_From_ModelId_FUNC(void* EquipParamWeaponInfo, int newModelId);
static EquipParamWeaponInfo_From_ModelId_FUNC* EquipParamWeaponInfo_From_ModelId = (EquipParamWeaponInfo_From_ModelId_FUNC*)0x14020af60;

typedef bool EquipParamWeaponInfo_To_PartsIdString_FUNC(void* EquipParamWeaponInfo, void* strout, uint8_t param_3, uint64_t param_4);
static EquipParamWeaponInfo_To_PartsIdString_FUNC* EquipParamWeaponInfo_To_PartsIdString = (EquipParamWeaponInfo_To_PartsIdString_FUNC*)0x140532f40;

typedef void* Construct_PartsbndFileCap_FUNC(wchar_t* param_1, uint64_t param_2);
static Construct_PartsbndFileCap_FUNC* Construct_PartsbndFileCap = (Construct_PartsbndFileCap_FUNC*)0x1405a10f0;

typedef bool PartsbndFileCap_Free_FUNC(void* DelayDeleteManImp, uint32_t param_2, void* PartsbndFileCapOut, uint64_t param_4);
static PartsbndFileCap_Free_FUNC* PartsbndFileCap_Free = (PartsbndFileCap_Free_FUNC*)0x1401957a0;

typedef bool EquipParamProtectorInfo_To_PartsIdString_FUNC(void* EquipParamProtectorInfo, void* strout, uint8_t param_3, uint64_t param_4);
static EquipParamProtectorInfo_To_PartsIdString_FUNC* EquipParamProtectorInfo_To_PartsIdString = (EquipParamProtectorInfo_To_PartsIdString_FUNC*)0x14052eda0;

typedef void* Build_EquipParamProtectorInfo_FUNC(void* EquipParamProtectorInfo, int newModelId);
static Build_EquipParamProtectorInfo_FUNC* Build_EquipParamProtectorInfo = (Build_EquipParamProtectorInfo_FUNC*)0x14052eb90;

typedef void Vector_IncreaseSize_FUNC(void* vector, uint64_t spaceToAdd);
static Vector_IncreaseSize_FUNC* Vector_IncreaseSize = (Vector_IncreaseSize_FUNC*)0x1403ab910;

#endif // _DS1_OVERHAUL_GAME_DATA_H_
