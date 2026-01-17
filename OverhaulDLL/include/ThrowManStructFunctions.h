#pragma once
#ifndef THROWMAN_STRUCTFUNCTIONS_H
#define THROWMAN_STRUCTFUNCTIONS_H

#include <stdint.h>
#include "ThrowManStruct.h"
#include "Rollback.h"

void copy_ThrowMan(ThrowMan* to, ThrowMan* from, StateTarget target);
ThrowMan* init_ThrowMan();
void free_ThrowMan(ThrowMan* to);

void copy_ThrowRequestedEntry(ThrowRequestedEntry* to, ThrowRequestedEntry* from, StateTarget target);
ThrowRequestedEntry* init_ThrowRequestedEntry();
void free_ThrowRequestedEntry(ThrowRequestedEntry* to);

#endif
