#pragma once
#ifndef SFXMAN_STRUCT_H
#define SFXMAN_STRUCT_H

//NOTE: Anything marked "data_x" is a bunch of non-pointer data that can be saved/loaded without really knowing what it is.
// See the ghidra repo for more specific info about what is in that blob, if it's even known

#include <stddef.h>
#include <stdint.h>

typedef struct SfxMan SfxMan;
typedef struct frpgFxManagerBase frpgFxManagerBase;
typedef struct FXHGManagerBase FXHGManagerBase;
typedef struct FXManager FXManager;
typedef struct SFXEntry SFXEntry;
typedef struct FXEntry_Substruct FXEntry_Substruct;
typedef struct class_14152d360_field0xe0 class_14152d360_field0xe0;
typedef struct class_14150b808_field0xf0 class_14150b808_field0xf0;

struct FXEntry_Substruct
{
    uint8_t data_0[0x58];
    uint64_t self_substruct2; //&self + 0xe0
    uint8_t data_1[16];
    void* unk1; //needed, otherwise game crashes
    uint8_t data_2[16];
    void* unk2; //can be nulled without error
    FXEntry_Substruct* next; //this is p much always null, probably safe to ignore
    void* unk4; //needed, otherwise the sfx freezes in place
    void* unk5; //needed, otherwise game crashes
    uint8_t padding_0[0x28]; //wstring, set to null
    SFXEntry* parent;
    uint64_t unk6; //can be nulled without error
    uint64_t vtable;
    void* unk7; //needed, otherwise game crashes
    void* unk8; //can be nulled without error
    void* unk9; //can be nulled without error
    void* unk10; //can be nulled without error
    uint8_t data_3[28+8];
};

static_assert(offsetof(FXEntry_Substruct, self_substruct2) == 0x58);
static_assert(offsetof(FXEntry_Substruct, data_1) == 0x60);
static_assert(offsetof(FXEntry_Substruct, unk1) == 0x70);
static_assert(offsetof(FXEntry_Substruct, data_2) == 0x78);
static_assert(offsetof(FXEntry_Substruct, unk2) == 0x88);
static_assert(offsetof(FXEntry_Substruct, next) == 0x90);
static_assert(offsetof(FXEntry_Substruct, unk4) == 0x98);
static_assert(offsetof(FXEntry_Substruct, unk5) == 0xa0);
static_assert(offsetof(FXEntry_Substruct, parent) == 0xd0);
static_assert(offsetof(FXEntry_Substruct, unk6) == 0xd8);
static_assert(offsetof(FXEntry_Substruct, unk7) == 0xe0+8);
static_assert(offsetof(FXEntry_Substruct, data_3) == 0xe0+0x28);
static_assert(sizeof(FXEntry_Substruct) == 0x130);

struct SFXEntry
{
    //inline FXEntry here since this class is a linked list array and we can't handle inheritance in our basic copy functions
        uint64_t vtable;
        void* field0x8; //can be nulled without error
        uint8_t data_0[24];
        FXManager* parent;
        uint64_t unk1; //always null
        uint64_t unk2; //can be nulled without error
        SFXEntry* next;
        FXEntry_Substruct* field0x48_head;
        FXEntry_Substruct* field0x48_tail;
        uint64_t unk4; //always null
    //end inline
    uint8_t data_1[0x80];
    class_14152d360_field0xe0* field0xe0;
    uint64_t data_2;
    class_14150b808_field0xf0* field0xf0;
    uint64_t data_3;
};
static_assert(offsetof(SFXEntry, field0x8) == 0x8);
static_assert(offsetof(SFXEntry, data_0) == 0x10);
static_assert(offsetof(SFXEntry, unk1) == 0x30);
static_assert(offsetof(SFXEntry, unk2) == 0x38);
static_assert(offsetof(SFXEntry, next) == 0x40);
static_assert(offsetof(SFXEntry, field0x48_head) == 0x48);
static_assert(offsetof(SFXEntry, field0x48_tail) == 0x50);
static_assert(offsetof(SFXEntry, unk4) == 0x58);
static_assert(offsetof(SFXEntry, data_1) == 0x60);
static_assert(offsetof(SFXEntry, field0xe0) == 0xe0);
static_assert(offsetof(SFXEntry, data_2) == 0xe8);
static_assert(offsetof(SFXEntry, field0xf0) == 0xf0);
static_assert(sizeof(SFXEntry) == 0x100);

struct FXManager
{
    uint64_t padding_0[3];
    SFXEntry* SFXEntryList;
    SFXEntry* SFXEntryList_tail; //can ignore since it's unchanging after alloc. Need to check to ensure we don't go past it, since it represents the last "valid" entry.
};
static_assert(offsetof(FXManager, SFXEntryList) == 0x18);

struct FXHGManagerBase
{
    uint64_t padding_0;
    FXManager* fXManager;
};
static_assert(offsetof(FXHGManagerBase, fXManager) == 8);

struct frpgFxManagerBase
{
    FXHGManagerBase base;
};

struct SfxMan
{
    uint8_t padding_0[16];
    frpgFxManagerBase* FrpgFxManagerBase;
};
static_assert(offsetof(SfxMan, FrpgFxManagerBase) == 0x10);

#endif
