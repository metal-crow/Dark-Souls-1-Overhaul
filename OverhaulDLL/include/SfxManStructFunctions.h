#pragma once
#ifndef SFXMAN_STRUCTFUNCTIONS_H
#define SFXMAN_STRUCTFUNCTIONS_H

#include <stdint.h>
#include <string>

#include "SfxManStruct.h"
#include "Rollback.h"

class StateVisitor;

void copy_SfxMan(SfxMan* to, SfxMan* from, StateTarget target);
SfxMan* init_SfxMan();
void free_SfxMan(SfxMan* to);
void serialize_SfxMan(StateVisitor& v, SfxMan* s);
std::string print_SfxMan(SfxMan* s);
uint64_t hash_SfxMan(SfxMan* s);

void copy_frpgFxManagerBase(frpgFxManagerBase* to, frpgFxManagerBase* from, StateTarget target);
frpgFxManagerBase* init_frpgFxManagerBase();
void free_frpgFxManagerBase(frpgFxManagerBase* to);

void copy_FXManager(FXManager* to, FXManager* from, StateTarget target);
FXManager* init_FXManager();
void free_FXManager(FXManager* to);

void Save_SFXEntryList(std::vector<SavedSFXEntry>* to, FXManager* from);
void Restore_SFXEntryList(FXManager* to, std::vector<SavedSFXEntry>* from);
void Copy_SFXEntryList(std::vector<SavedSFXEntry>* to, std::vector<SavedSFXEntry>* from);
void Clear_SFXEntryList(std::vector<SavedSFXEntry>* to);

//don't handle the behaviour_list in this, that's a seperate function
void copy_SFXEntry(SFXEntry* to, SFXEntry* from, StateTarget target);
void Free_SFXEntry(SFXEntry* to, bool freeself);

//these functions operate on the Node graph, but it's actually a tree with no duplicates, not strictly a graph
void Save_SavedFxBehaviorNodeGraph(SavedFxBehaviorNode* to, FxBehaviorNode* from);
void Restore_SavedFxBehaviorNodeGraph(FxBehaviorNode** to, SavedFxBehaviorNode* from);
void Copy_SavedFxBehaviorNodeGraph(SavedFxBehaviorNode* to, SavedFxBehaviorNode* from);
void Clear_SavedFxBehaviorNodeGraph(SavedFxBehaviorNode* to); //the caller should delete the to

void copy_FxBehaviorNode(FxBehaviorNode* to, FxBehaviorNode* from, StateTarget target);
void Free_FxBehaviorNode(FxBehaviorNode* to, bool freeself);

// SFX graveyard: prevents SFXEntries and FxBehaviorNodes
// internal allocations from being freed during the rollback window.
// Hook callbacks invoked from RollbackASM.asm — return true to skip
// the original dealloc.
extern "C" bool OnSfxEntryDestruct(void* SFXEntry);
extern "C" bool OnFxBehaviorNodeDealloc(void* unused, void* FxBehaviorNode);

#endif
