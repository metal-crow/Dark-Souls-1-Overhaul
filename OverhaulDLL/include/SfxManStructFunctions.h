#pragma once
#ifndef SFXMAN_STRUCTFUNCTIONS_H
#define SFXMAN_STRUCTFUNCTIONS_H

#include <stdint.h>
#include "SfxManStruct.h"

void copy_FXManager(FXManager* to, FXManager* from, bool to_game);
FXManager* init_FXManager();
void free_FXManager(FXManager* to);

void copy_SFXEntryList(SFXEntry* to, SFXEntry* from, bool to_game);
void copy_SFXEntry(SFXEntry* to, SFXEntry* from, bool to_game);
SFXEntry* init_SFXEntryList();
void free_SFXEntryList(SFXEntry* to);

void copy_class_14150b808_field0x48(class_14150b808_field0x48* to, class_14150b808_field0x48* from, bool to_game);
class_14150b808_field0x48* init_class_14150b808_field0x48();
void free_class_14150b808_field0x48(class_14150b808_field0x48* to);

#endif
