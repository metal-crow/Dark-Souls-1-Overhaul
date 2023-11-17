#pragma once
#ifndef DAMAGEMANSTRUCTFUNCTIONS_H
#define DAMAGEMANSTRUCTFUNCTIONS_H

#include <stdint.h>
#include "DamageManStruct.h"

void copy_DamageMan(DamageMan* to, DamageMan* from, bool to_game);
DamageMan* init_DamageMan();
void free_DamageMan(DamageMan* to);

void copy_DamageEntry(DamageEntry* to, DamageEntry* from, bool to_game);
DamageEntry* init_DamageEntry();
void free_DamageEntry(DamageEntry* to, bool freeself);

void copy_DamageEntryField0x118(DamageEntryField0x118* to, DamageEntryField0x118* from, bool to_game);
DamageEntryField0x118* init_DamageEntryField0x118();
void free_DamageEntryField0x118(DamageEntryField0x118* to);

#endif
