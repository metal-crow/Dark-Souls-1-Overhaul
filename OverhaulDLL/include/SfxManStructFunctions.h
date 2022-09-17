#pragma once
#ifndef SFXMAN_STRUCTFUNCTIONS_H
#define SFXMAN_STRUCTFUNCTIONS_H

#include <stdint.h>
#include "SfxManStruct.h"

void copy_SfxMan(SfxMan* to, SfxMan* from, bool to_game);
SfxMan* init_SfxMan();
void free_SfxMan(SfxMan* to);

void copy_frpgFxManagerBase(frpgFxManagerBase* to, frpgFxManagerBase* from, bool to_game);
frpgFxManagerBase* init_frpgFxManagerBase();
void free_frpgFxManagerBase(frpgFxManagerBase* to);

void copy_FXHGManagerBase(FXHGManagerBase* to, FXHGManagerBase* from, bool to_game);
FXHGManagerBase* init_FXHGManagerBase();
void free_FXHGManagerBase(FXHGManagerBase* to, bool freeself);

void copy_class_1415002c8(class_1415002c8* to, class_1415002c8* from, bool to_game);
class_1415002c8* init_class_1415002c8();
void free_class_1415002c8(class_1415002c8* to);

void copy_class_14152d360(class_14152d360* to, class_14152d360* from, bool to_game);
class_14152d360* init_class_14152d360();
void free_class_14152d360(class_14152d360* to);

#endif
