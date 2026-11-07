#pragma once
#ifndef SFXMAN_STRUCT_H
#define SFXMAN_STRUCT_H

#include <stddef.h>
#include <stdint.h>

/*
 * The parts of the FX system (FX4HG / FFX, owned by NS_FRPG::SfxMan) that rollback touches. See SfxManStructFunctions.cpp
 * for how it is rolled back. The effects themselves are not saved: only which effect each rolled-back FX handle
 * (SfxHandle) points to.
 *
 * An effect is a tree of FxBehaviorNodes. Each node is one SmallObjHeap block (Init_FxBehaviorNode 140fde7b0): a 0xE0
 * header, then the 0x50 body, then an optional 0x50 and an optional 0xE0 block. A root node's SFXEntry holds the roots
 * of one spawned effect.
 */

typedef struct FXEntry FXEntry;
typedef struct SFXEntry SFXEntry;
typedef struct FXManager FXManager;
typedef struct FxBehaviorNode FxBehaviorNode;
typedef struct FxBehaviorNodeBody FxBehaviorNodeBody;
typedef struct SfxManState SfxManState;
typedef struct SfxHandle SfxHandle;

//A handle on one effect, held by value by whatever drives the effect (bullets, attach slots). It points at the node it is
//attached to, and is linked into that node's body's handle list (FxBehaviorNodeBody::linked_sfxHandle). Once that node is
//killed it can move on to follow a later node of the same tree (SfxHandle_Detach).
struct SfxHandle
{
    uint64_t vtable;               // 0x0 (SfxHandle_Vtable)
    FXManager* fx_manager;         // 0x8
    FxBehaviorNode* node;          // 0x10 (usually the effect's root)
    SfxHandle* list_next;          // 0x18 (node's handle list)
    SfxHandle* list_prev;          // 0x20
    FxBehaviorNode* follow_node;   // 0x28 (set instead of node once node has died)
};
static_assert(offsetof(SfxHandle, fx_manager) == 0x8);
static_assert(offsetof(SfxHandle, node) == 0x10);
static_assert(offsetof(SfxHandle, list_next) == 0x18);
static_assert(offsetof(SfxHandle, list_prev) == 0x20);
static_assert(offsetof(SfxHandle, follow_node) == 0x28);
static_assert(sizeof(SfxHandle) == 0x30);

//header flags1: an effect draws while every bit except 29 is set. Each clear bit is one reason to hide it: the game uses bit
//0 (character/slot visibility, FUN_140fdd160) and bit 31 (instance limits, FUN_140f86d70); bit 30 is "alive" (FUN_140f0c2b0).
static const uint32_t FxBehaviorNode_flags1_alive = 1u << 30;

struct FxBehaviorNodeBody
{
    void* vtable;                  // 0x0
    FxBehaviorNode* self;          // 0x8
    void* host_a;                  // 0x10 (ghidra: FileCap*, the appearance host)
    void* host_b;                  // 0x18 (ghidra: FileCap*)
    //tail of the doubly linked list (through list_prev) of every handle watching this node: node == this node, or
    //node == NULL and follow_node == this node. Body teardown (FUN_140fe5fb0) detaches each of them.
    SfxHandle* linked_sfxHandle; // 0x20
    uint8_t unk_28[0x28];          // 0x28 (sound id, sound emitter, ...)
};
static_assert(offsetof(FxBehaviorNodeBody, self) == 0x8);
static_assert(offsetof(FxBehaviorNodeBody, linked_sfxHandle) == 0x20);
static_assert(sizeof(FxBehaviorNodeBody) == 0x50);

struct FxBehaviorNode
{
    float unk_0[8];                // 0x0
    float unk_20[12];              // 0x20 (transform)
    void* unk_50;                  // 0x50
    FxBehaviorNodeBody* body;      // 0x58 (always this node's own body, at +0xe0)
    uint32_t flags1;               // 0x60
    uint32_t flags2;               // 0x64
    uint8_t unk_68[0x20];          // 0x68
    FxBehaviorNode* parent_node;   // 0x88 (NULL for a root)
    FxBehaviorNode* next_node;     // 0x90
    FxBehaviorNode* child_node;    // 0x98
    void* effect_descriptor;       // 0xa0
    uint8_t unk_a8[0x28];          // 0xa8 (per-instance param evaluators)
    SFXEntry* parent;              // 0xd0
    FxBehaviorNode* destruction_queue_next; // 0xd8
};
static_assert(offsetof(FxBehaviorNode, body) == 0x58);
static_assert(offsetof(FxBehaviorNode, flags1) == 0x60);
static_assert(offsetof(FxBehaviorNode, parent_node) == 0x88);
static_assert(offsetof(FxBehaviorNode, next_node) == 0x90);
static_assert(offsetof(FxBehaviorNode, child_node) == 0x98);
static_assert(offsetof(FxBehaviorNode, parent) == 0xd0);
static_assert(sizeof(FxBehaviorNode) == 0xe0);

// ---- FXEntry (base for SFXEntry) ----
struct FXEntry
{
    void* vtable;                  // 0x0
    uint64_t unk_8;                // 0x8
    uint32_t unk_10;               // 0x10 (reference count)
    uint32_t sfxId;                // 0x14
    uint8_t unk_18[0x10];          // 0x18
    FXManager* fxParent;           // 0x28 (the FXManager that spawned it)
    void* vec;                     // 0x30
    void* list;                    // 0x38
    SFXEntry* next;                // 0x40 (FXManager's SFXEntryList)
    FxBehaviorNode* behaviour_list;     // 0x48 (root nodes)
    FxBehaviorNode* behaviour_list_end; // 0x50
    uint8_t unk_58[8];             // 0x58
};
static_assert(offsetof(FXEntry, sfxId) == 0x14);
static_assert(offsetof(FXEntry, fxParent) == 0x28);
static_assert(offsetof(FXEntry, next) == 0x40);
static_assert(offsetof(FXEntry, behaviour_list) == 0x48);
static_assert(sizeof(FXEntry) == 0x60);

// ---- SFXEntry (extends FXEntry), 0x100 from SmallObjHeap ----
struct SFXEntry
{
    FXEntry base;                  // 0x0
    uint8_t unk_60[0xa0];          // 0x60
};
static_assert(sizeof(SFXEntry) == 0x100);

// ---- Local rollback state (not a game struct) ----

//The SfxMan part of a RollbackState. The handles themselves are saved inside their owners (bullets, attach slots).
struct SfxManState
{
    //the GGPO frame this state was saved for, or SfxManState_frame_current for a capture outside the GGPO save callback,
    //whose load discards nothing (RollbackStartSync)
    int32_t frame;
};
static const int32_t SfxManState_frame_current = INT32_MIN;

#endif
