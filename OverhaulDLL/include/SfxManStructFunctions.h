#pragma once
#ifndef SFXMAN_STRUCTFUNCTIONS_H
#define SFXMAN_STRUCTFUNCTIONS_H

#include <stdint.h>
#include "SfxManStruct.h"

void copy_FXManager(FXManager* to, FXManager* from, bool to_game);
FXManager* init_FXManager();
void free_FXManager(FXManager* to);

void copy_SFXEntryList(SFXEntry* to, SFXEntry* from, bool to_game, FXManager* to_parent, FXManager* from_parent);
void copy_SFXEntry(SFXEntry* to, SFXEntry* from, bool to_game);
SFXEntry* init_SFXEntryList();
void free_SFXEntryList(SFXEntry* to);

void copy_FXEntry_Substruct(FXEntry_Substruct* to, FXEntry_Substruct* from, bool to_game);
FXEntry_Substruct* init_FXEntry_Substruct();
void free_FXEntry_Substruct(FXEntry_Substruct* to);

#endif
