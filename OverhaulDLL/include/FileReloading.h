#pragma once

#ifndef FILE_RELOADING_H_
#define FILE_RELOADING_H_

#include <cstdint>
#include <vector>

enum IndividualParams {
    LIGHT_BANK = 0x0,
    FOG_BANK = 0x1,
    LIGHT_SCATTERING_BANK = 0x2,
    POINT_LIGHT_BANK = 0x3,
    LENS_FLARE_BANK = 0x4,
    DOF_BANK = 0x5,
    TONE_MAP_BANK = 0x6,
    TONE_CORRECT_BANK = 0x7,
    SHADOW_BANK = 0x8,
    LENS_FLARE_EX_BANK = 0x9,
    ENV_LIGHT_TEX_BANK = 0xa,
    LOD_BANK = 0xb,
    ENEMY_STANDARD_INFO_BANK = 0xc,
    AI_STANDARD_INFO_BANK = 0xd,
    THROW_INFO_BANK = 0xe,
    MISSILE_PARAM_ST = 0xf,
    SoloParamMan_Loaded = 0x10,
    SoloParamMan = 0x11
};

enum IndividualSoloParams {
    EquipParamWeapon = 0x0,
    EquipParamProtector = 0x1,
    EquipParamAccessory = 0x2,
    EquipParamGoods = 0x3,
    ReinforceParamWeapon = 0x4,
    ReinforceParamProtector = 0x5,
    NpcParam = 0x6,
    AtkParam_Npc = 0x7,
    AtkParam_Pc = 0x8,
    NpcThinkParam = 0x9,
    ObjectParam = 0xa,
    Bullet = 0xb,
    BehaviorParam = 0xc,
    BehaviorParam_PC = 0xd,
    Magic = 0xe,
    SpEffectParam = 0xf,
    SpEffectVfxParam = 0x10,
    TalkParam = 0x11,
    MenuColorTableParam = 0x12,
    ItemLotParam = 0x13,
    MoveParam = 0x14,
    CharaInitParam = 0x15,
    EquipMtrlSetParam = 0x16,
    FaceGenParam = 0x17,
    RagdollParam = 0x18,
    ShopLineupParam = 0x19,
    QwcChange = 0x1a,
    QwcJudge = 0x1b,
    GameAreaParam = 0x1c,
    SkeletonParam = 0x1d,
    CalcCorrectGraph = 0x1e,
    LockCamParam = 0x1f,
    ObjActParam = 0x20,
    HitMtrlParam = 0x21,
    KnockBackParam = 0x22,
    LevelSyncParam = 0x23,
    CoolTimeParam = 0x24,
    WhiteCoolTimeParam = 0x25,
    All_Other_Params = 0x26
};

enum ParamBNDs
{
    GameParam,
    a10_DrawParam,
    a11_DrawParam,
    a12_DrawParam,
    a13_DrawParam,
    a14_DrawParam,
    a15_DrawParam,
    a16_DrawParam,
    a17_DrawParam,
    a18_DrawParam,
    a99_DrawParam,
    default_DrawParam,
};

class FileReloading
{
public:
    static void start();
    static void ReloadGameParam();
    static void ReloadPlayer();
    static void RefreshPlayerStats();
private:
    static void UnloadIndividualParams(std::vector<IndividualParams> params);
    static void UnloadIndividualSoloParams(std::vector<IndividualSoloParams> params);
    static void ReloadParamFile(ParamBNDs paramfile);

    static const uint64_t CalcCorrectGraph_injection_offset = 0x2de08d;
};

#endif
