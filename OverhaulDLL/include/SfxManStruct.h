#pragma once
#ifndef SFXMAN_STRUCT_H
#define SFXMAN_STRUCT_H

#include <stddef.h>
#include <stdint.h>
#include <vector>

typedef struct SfxMan SfxMan;
typedef struct frpgFxManagerBase frpgFxManagerBase;
typedef struct FXHGManagerBase FXHGManagerBase;
typedef struct FXEntry FXEntry;
typedef struct SFXEntry SFXEntry;
typedef struct FXManager FXManager;
typedef struct FxBehaviorNode FxBehaviorNode;
typedef struct SavedSFXEntry SavedSFXEntry;
typedef struct SavedFxBehaviorNode SavedFxBehaviorNode;
typedef struct vectorFXEntry vectorFXEntry;
typedef struct doublelinkedlistFxEntry doublelinkedlistFxEntry;
typedef struct SFXEntry_field0xf0 SFXEntry_field0xf0;

struct FxBehaviorNode
{
    float unk_0[8];        // 0x0
    float unk_20[12];      // 0x20
    void* unk_1;           // 0x50 (some sort of static pointer)
    void* body;            // 0x58 (ghidra: FxBehaviorNodeBody*)
    uint32_t flags1;       // 0x60
    uint32_t flags2;       // 0x64
    uint64_t unk_68;       // 0x68
    void* unk_2;           // 0x70 (TODO)
    uint32_t unk_78;       // 0x78
    uint32_t unk_7c;       // 0x7c
    uint8_t parent_node_updated; // 0x80 (ghidra: bool)
    uint8_t unk_81[3];     // 0x81 (gap)
    uint8_t unk_84;        // 0x84
    uint8_t unk_85;        // 0x85
    uint8_t unk_86[2];     // 0x86 (gap)
    FxBehaviorNode* parent_node;  // 0x88
    FxBehaviorNode* next_node;    // 0x90
    FxBehaviorNode* child_node;   // 0x98
    void* unk_3;           // 0xa0 (ghidra: effect_descriptor)
    void* unk_4;           // 0xa8 (ghidra: array_0xa8)
    uint8_t _0[24+4+4];    // 0xb0 (ghidra: gap + array_0xa8_len@0xc8 + gap; not copied)
    SFXEntry* parent;      // 0xd0 (already saved, just a static pointer)
    void* destruction_queue_next; // 0xd8
};
static_assert(sizeof(FxBehaviorNode) == 0xe0);
static_assert(offsetof(FxBehaviorNode, unk_1) == 0x50);
static_assert(offsetof(FxBehaviorNode, body) == 0x58);
static_assert(offsetof(FxBehaviorNode, flags1) == 0x60);
static_assert(offsetof(FxBehaviorNode, unk_2) == 0x70);
static_assert(offsetof(FxBehaviorNode, parent_node) == 0x88);
static_assert(offsetof(FxBehaviorNode, next_node) == 0x90);
static_assert(offsetof(FxBehaviorNode, child_node) == 0x98);
static_assert(offsetof(FxBehaviorNode, unk_3) == 0xa0);
static_assert(offsetof(FxBehaviorNode, unk_4) == 0xa8);
static_assert(offsetof(FxBehaviorNode, parent) == 0xd0);

struct doublelinkedlistFxEntry
{
    doublelinkedlistFxEntry* prev;
    doublelinkedlistFxEntry* next;
    FxBehaviorNode* data;
};

struct vectorFXEntry
{
    uint64_t heap;
    uint64_t* vec; //actually a {uint32, float}
    uint64_t vec_cur;
    uint64_t vec_end;
};

// ---- FXEntry (base for SFXEntry) ----
struct FXEntry
{
    void* vtable;          // 0x0
    uint64_t unk_8;        // 0x8
    uint32_t unk_10;       // 0x10
    uint32_t sfxId;        // 0x14
    uint32_t unk_18;       // 0x18
    float unk_1c;          // 0x1c
    uint64_t unk_20;       // 0x20
    void* fxParent; //this is static, rollback won't affect the FxManager pointer (0x28)
    vectorFXEntry* vec;    // 0x30
    doublelinkedlistFxEntry* list; //the pointers here are static, not tied to this FXEntry lifetime (0x38)
    SFXEntry* next;        // 0x40
    FxBehaviorNode* behaviour_list;     // 0x48
    FxBehaviorNode* behaviour_list_end; // 0x50
    uint8_t data_1[8];     // 0x58 (ghidra: undefined8)
};
static_assert(sizeof(FXEntry) == 0x60);
static_assert(offsetof(FXEntry, sfxId) == 0x14);
static_assert(offsetof(FXEntry, fxParent) == 0x28);
static_assert(offsetof(FXEntry, vec) == 0x30);
static_assert(offsetof(FXEntry, next) == 0x40);
static_assert(offsetof(FXEntry, behaviour_list) == 0x48);
static_assert(offsetof(FXEntry, behaviour_list_end) == 0x50);

struct SFXEntry_field0xf0
{
    uint8_t* data;
    size_t data_size;
    uint64_t data_0;
    uint8_t* secondary_data; //0x10 bytes
};
static_assert(sizeof(SFXEntry_field0xf0) == 0x20);

// ---- SFXEntry (extends FXEntry) ----
// Allocated from SmallObjHeap. Each represents an active SFX instance.
struct SFXEntry
{
    FXEntry base;          // 0x0
    uint64_t unk_60;       // 0x60
    float unk_68;          // 0x68
    uint32_t unk_6c;       // 0x6c
    float unk_70;          // 0x70
    float unk_74;          // 0x74
    float unk_78;          // 0x78
    uint32_t unk_7c;       // 0x7c
    uint8_t field0x80[48]; // 0x80 (ghidra: NS_FRPG_BulletIns_Field0x90_Field0xE0)
    uint8_t field0xb0[48]; // 0xb0 (ghidra: NS_FRPG_BulletIns_Field0x90_Field0xE0)
    uint8_t* field_0xe0;   // 0xe0
    uint64_t field_0xe0_size; // 0xe8
    SFXEntry_field0xf0* field_0xf0; // 0xf0
    uint32_t unk_f8;       // 0xf8
    uint8_t unk_fc;        // 0xfc
    uint8_t unk_fd[3];     // 0xfd (gap)
};
static_assert(sizeof(SFXEntry) == 0x100);
static_assert(offsetof(SFXEntry, base) == 0);
static_assert(offsetof(SFXEntry, unk_60) == 0x60);
static_assert(offsetof(SFXEntry, field0x80) == 0x80);
static_assert(offsetof(SFXEntry, field_0xe0) == 0xe0);
static_assert(offsetof(SFXEntry, field_0xf0) == 0xf0);


// ---- FXManager ----
// Manages the SFXEntry linked list and multiple internal vectors.
// Accessed via frpgFxManagerBase->base.fXManager.
struct FXManager
{
    void* vtable;          // 0x0
    void* parent;          // 0x8 (ghidra: FXManager*)
    uint64_t heap;         // 0x10
    SFXEntry* SFXEntryList; // the active SFXEntry linked list (0x18)
    SFXEntry* SFXEntryList_tail; // 0x20
    void* FxBehaviorNode_destructlist_head; //keep both of these null, the should only be set in the middle of a frame anyway (0x28)
    void* FxBehaviorNode_destructlist_tail; // 0x30
    uint32_t unk_38;       // 0x38
    uint32_t unk_3c;       // 0x3c
    uint8_t FXDrawEntityHandler_list[0x20]; //this list is static and it's contents (and the contents of it's element) don't change during runtime. Safe to memcpy (0x40)
    uint8_t FxBehaviorNode_staging_list[0x20]; //this list is cleared at the start of every frame and only holds pointers to stuff saved elsewhere. Safe to memcpy (0x60)
    void* FxBehaviorNode_destruction_queue; //this should be set to 1 (empty queue) since any substruct destruction is handled by us anyway (0x80)
    uint8_t unk_88[16];    // 0x88 (ghidra: undefined[16])
    uint8_t unk_98[8];     // 0x98 (gap)
    float unk_a0[4];       // 0xa0 (ghidra: float[4])
    uint8_t unk_b0;        // 0xb0 (ghidra: bool)
    uint8_t unk_b1[3];     // 0xb1 (gap)
    uint32_t FXSettingOptions[50]; // 0xb4 (ghidra: uint32_t[50])
    uint8_t unk_17c[4];    // 0x17c (gap)
    uint8_t FXSettingScales[0x20]; // 0x180 (ghidra: vector<t>)
    void* FrpgFxAdapterBase; //this is static and not important for rollback (0x1a0)
    uint64_t substructs_visable[4]; //unsure how important this is. Don't save for now (0x1a8; ghidra: vector)
    uint64_t loaded_assets[8]; //safe to not save (0x1c8; ghidra: 2 vectors)
    uint64_t unk;          // 0x208
    // Local-only fields (not part of the game struct, only used in our local copies)
    std::vector<SavedSFXEntry> saved_entries;
};
//static_assert(sizeof(FXManager) == 0x210); //has trailing std::vector
static_assert(offsetof(FXManager, SFXEntryList) == 0x18);
static_assert(offsetof(FXManager, SFXEntryList_tail) == 0x20);
static_assert(offsetof(FXManager, FxBehaviorNode_destructlist_head) == 0x28);
static_assert(offsetof(FXManager, unk_38) == 0x38);
static_assert(offsetof(FXManager, FXDrawEntityHandler_list) == 0x40);
static_assert(offsetof(FXManager, FxBehaviorNode_staging_list) == 0x60);
static_assert(offsetof(FXManager, FxBehaviorNode_destruction_queue) == 0x80);
static_assert(offsetof(FXManager, FXSettingOptions) == 0xb4);
static_assert(offsetof(FXManager, FXSettingScales) == 0x180);
static_assert(offsetof(FXManager, FrpgFxAdapterBase) == 0x1a0);
static_assert(offsetof(FXManager, loaded_assets) == 0x1c8);

// ---- Saved FxBehaviorNode for rollback ----
// Pairs a game-memory address with a snapshot of the FxBehaviorNode data
struct SavedFxBehaviorNode
{
    FxBehaviorNode* game_addr;
    FxBehaviorNode data;
    SavedFxBehaviorNode* next_node;
    SavedFxBehaviorNode* child_node;
};

// ---- Saved SFXEntry for rollback ----
// Pairs a game-memory address with a snapshot of the SFXEntry data and
// snapshots of every node hanging off the entry.
struct SavedSFXEntry
{
    SFXEntry* game_addr;
    SFXEntry data;
    SavedFxBehaviorNode* behaviour_list_head;
};

// ---- FXHGManagerBase ----
struct FXHGManagerBase
{
    uint64_t padding_0;
    FXManager* fXManager;
};
static_assert(offsetof(FXHGManagerBase, fXManager) == 8);


// ---- frpgFxManagerBase ----
struct frpgFxManagerBase
{
    FXHGManagerBase base;
};


// ---- SfxMan ----
struct SfxMan
{
    uint8_t padding_0[16];
    frpgFxManagerBase* FrpgFxManagerBase;
};
static_assert(offsetof(SfxMan, FrpgFxManagerBase) == 0x10);

#endif
