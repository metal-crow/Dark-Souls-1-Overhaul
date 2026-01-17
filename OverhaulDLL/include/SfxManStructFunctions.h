#pragma once
#ifndef SFXMAN_STRUCTFUNCTIONS_H
#define SFXMAN_STRUCTFUNCTIONS_H

#include <stdint.h>
#include "SfxManStruct.h"
#include <string>
#include "Rollback.h"

std::string print_SfxMan(SfxMan* to);
void copy_SfxMan(SfxMan* to, SfxMan* from, StateTarget target);
SfxMan* init_SfxMan();
void free_SfxMan(SfxMan* to);

std::string print_frpgFxManagerBase(frpgFxManagerBase* to);
void copy_frpgFxManagerBase(frpgFxManagerBase* to, frpgFxManagerBase* from, StateTarget target);
frpgFxManagerBase* init_frpgFxManagerBase();
void free_frpgFxManagerBase(frpgFxManagerBase* to);

std::string print_FXManager(FXManager* to);
void copy_FXManager(FXManager* to, FXManager* from, StateTarget target);
FXManager* init_FXManager();
void free_FXManager(FXManager* to);

std::string print_SFXEntryList(SFXEntry* to);
void copy_SFXEntryList(SFXEntry* to, SFXEntry* from, StateTarget target, FXManager* to_parent, FXManager* from_parent);
void copy_SFXEntry(SFXEntry* to, SFXEntry* from, StateTarget target);
SFXEntry* init_SFXEntryList();
void free_SFXEntryList(SFXEntry* to);

std::string print_FXEntry_Substruct(FXEntry_Substruct* to);
void copy_FXEntry_Substruct(FXEntry_Substruct* to, FXEntry_Substruct* from, StateTarget target, SFXEntry* parent);
void copy_FXEntry_Substruct_Obj(FXEntry_Substruct* to, FXEntry_Substruct* from, StateTarget target, SFXEntry* parent);
FXEntry_Substruct* init_FXEntry_Substruct();
void free_FXEntry_Substruct(FXEntry_Substruct* to);

#endif
