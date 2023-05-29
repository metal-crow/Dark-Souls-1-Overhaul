#pragma once
#ifndef THROWMAN_STRUCTFUNCTIONS_H
#define THROWMAN_STRUCTFUNCTIONS_H

#include <stdint.h>
#include "ThrowManStruct.h"

void copy_ThrowMan(ThrowMan* to, ThrowMan* from, bool to_game);
ThrowMan* init_ThrowMan();
void free_ThrowMan(ThrowMan* to);

void copy_ThrowRequestedEntry(ThrowRequestedEntry* to, ThrowRequestedEntry* from, bool to_game);
ThrowRequestedEntry* init_ThrowRequestedEntry();
void free_ThrowRequestedEntry(ThrowRequestedEntry* to);

#endif
