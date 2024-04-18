#pragma once
#ifndef SFXMAN_STRUCTFUNCTIONS_H
#define SFXMAN_STRUCTFUNCTIONS_H

#include <stdint.h>
#include "SfxManStruct.h"
#include <string>

std::string print_SfxMan(SfxMan* to);
void copy_SfxMan(SfxMan* to, SfxMan* from, bool to_game);
SfxMan* init_SfxMan();
void free_SfxMan(SfxMan* to);

std::string print_frpgFxManagerBase(frpgFxManagerBase* to);
void copy_frpgFxManagerBase(frpgFxManagerBase* to, frpgFxManagerBase* from, bool to_game);
frpgFxManagerBase* init_frpgFxManagerBase();
void free_frpgFxManagerBase(frpgFxManagerBase* to);

std::string print_FXManager(FXManager* to);
void copy_FXManager(FXManager* to, FXManager* from, bool to_game);
FXManager* init_FXManager();
void free_FXManager(FXManager* to);

std::string print_SFXEntryList(SFXEntry* to);
void copy_SFXEntryList(SFXEntry* to, SFXEntry* from, bool to_game, FXManager* to_parent, FXManager* from_parent);
void copy_SFXEntry(SFXEntry* to, SFXEntry* from, bool to_game);
SFXEntry* init_SFXEntryList();
void free_SFXEntryList(SFXEntry* to);

std::string print_FXEntry_Substruct(FXEntry_Substruct* to);
void copy_FXEntry_Substruct(FXEntry_Substruct* to, FXEntry_Substruct* from, bool to_game, SFXEntry* parent);
void copy_FXEntry_Substruct_Obj(FXEntry_Substruct* to, FXEntry_Substruct* from, bool to_game, SFXEntry* parent);
FXEntry_Substruct* init_FXEntry_Substruct();
void free_FXEntry_Substruct(FXEntry_Substruct* to);

#endif
