#pragma once
#ifndef SFXMAN_STRUCT_H
#define SFXMAN_STRUCT_H

//NOTE: Anything marked "data_x" is a bunch of non-pointer data that can be saved/loaded without really knowing what it is.
// See the ghidra repo for more specific info about what is in that blob, if it's even known

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
    uint8_t data_0[0x50];
    void* unk_1; //some sort of static pointer
    void* body; //TODO
    uint8_t data_1[16];
    void* unk_2; //TODO
    uint8_t data_2[16];
    FxBehaviorNode* parent_node;
    FxBehaviorNode* next_node;
    FxBehaviorNode* child_node;
    void* unk_3; //TODO
    void* unk_4; //TODO
    uint8_t _0[24+4+4]; //TODO
    SFXEntry* parent; //already saved, just a static pointer
    void* destruction_queue_next;
};
static_assert(sizeof(FxBehaviorNode) == 0xe0);
static_assert(offsetof(FxBehaviorNode, unk_1) == 0x50);
static_assert(offsetof(FxBehaviorNode, body) == 0x58);
static_assert(offsetof(FxBehaviorNode, unk_2) == 0x70);
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
    uint8_t data_0[0x28];
    void* fxParent; //this is static, rollback won't affect the FxManager pointer
    vectorFXEntry* vec;
    doublelinkedlistFxEntry* list; //the pointers here are static, not tied to this FXEntry lifetime
    SFXEntry* next;
    FxBehaviorNode* behaviour_list;
    FxBehaviorNode* behaviour_list_end;
    uint8_t data_1[8];
};
static_assert(sizeof(FXEntry) == 0x60);
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
    FXEntry base;
    uint8_t data_0[0x80];
    uint8_t* field_0xe0;
    uint64_t field_0xe0_size;
    SFXEntry_field0xf0* field_0xf0;
    uint8_t data_1[8];
};
static_assert(sizeof(SFXEntry) == 0x100);
static_assert(offsetof(SFXEntry, base) == 0);
static_assert(offsetof(SFXEntry, field_0xe0) == 0xe0);
static_assert(offsetof(SFXEntry, field_0xf0) == 0xf0);


// ---- FXManager ----
// Manages the SFXEntry linked list and multiple internal vectors.
// Accessed via frpgFxManagerBase->base.fXManager.
struct FXManager
{
    uint8_t data_0[0x18];
    SFXEntry* SFXEntryList; // the active SFXEntry linked list
    SFXEntry* SFXEntryList_tail;
    void* FxBehaviorNode_destructlist_head; //keep both of these null, the should only be set in the middle of a frame anyway
    void* FxBehaviorNode_destructlist_tail;
    uint8_t data_1[8];
    uint8_t FXDrawEntityHandler_list[0x20]; //this list is static and it's contents (and the contents of it's element) don't change during runtime. Safe to memcpy
    uint8_t FxBehaviorNode_staging_list[0x20]; //this list is cleared at the start of every frame and only holds pointers to stuff saved elsewhere. Safe to memcpy
    void* FxBehaviorNode_destruction_queue; //this should be set to 1 (empty queue) since any substruct destruction is handled by us anyway
    uint8_t data_2[0x118]; //this contains arrays which are constants during runtime (or are settings related and don't change during a game)
    void* FrpgFxAdapterBase; //this is static and not important for rollback
    uint64_t substructs_visable[4]; //unsure how important this is. Don't save for now
    uint64_t loaded_assets[8]; //safe to not save
    uint64_t unk;
    // Local-only fields (not part of the game struct, only used in our local copies)
    std::vector<SavedSFXEntry> saved_entries;
};
//static_assert(sizeof(FXManager) == 0x210);
static_assert(offsetof(FXManager, SFXEntryList) == 0x18);
static_assert(offsetof(FXManager, SFXEntryList_tail) == 0x20);
static_assert(offsetof(FXManager, FxBehaviorNode_destructlist_head) == 0x28);
static_assert(offsetof(FXManager, FXDrawEntityHandler_list) == 0x40);
static_assert(offsetof(FXManager, FxBehaviorNode_staging_list) == 0x60);
static_assert(offsetof(FXManager, FxBehaviorNode_destruction_queue) == 0x80);
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
