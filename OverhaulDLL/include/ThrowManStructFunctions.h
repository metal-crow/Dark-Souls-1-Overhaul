#pragma once
#ifndef THROWMAN_STRUCTFUNCTIONS_H
#define THROWMAN_STRUCTFUNCTIONS_H

#include <stdint.h>
#include <string>
#include "ThrowManStruct.h"
#include "Rollback.h"

class StateVisitor;

void copy_ThrowMan(ThrowMan* to, ThrowMan* from, StateTarget target);
ThrowMan* init_ThrowMan();
void free_ThrowMan(ThrowMan* to);
void serialize_ThrowMan(StateVisitor& v, ThrowMan* t);
std::string print_ThrowMan(ThrowMan* t);
uint64_t hash_ThrowMan(ThrowMan* t);

void copy_ThrowRequestedEntry(ThrowRequestedEntry* to, ThrowRequestedEntry* from, StateTarget target);
ThrowRequestedEntry* init_ThrowRequestedEntry();
void free_ThrowRequestedEntry(ThrowRequestedEntry* to);
void serialize_ThrowRequestedEntry(StateVisitor& v, ThrowRequestedEntry* e);

#endif
