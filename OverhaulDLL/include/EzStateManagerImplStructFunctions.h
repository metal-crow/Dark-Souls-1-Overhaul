#pragma once
#ifndef EZSTATEMAN_STRUCTFUNCTIONS_H
#define EZSTATEMAN_STRUCTFUNCTIONS_H

#include <stdint.h>
#include <string>
#include "EzStateManagerImplStruct.h"

void copy_EzStateManagerImpl_field0x20(EzStateManagerImpl_field0x20* to, EzStateManagerImpl_field0x20* from, bool to_game);
EzStateManagerImpl_field0x20* init_EzStateManagerImpl_field0x20();
void free_EzStateManagerImpl_field0x20(EzStateManagerImpl_field0x20* to);

void copy_EzStateManagerImpl(EzStateManagerImpl* to, EzStateManagerImpl* from, bool to_game);
EzStateManagerImpl* init_EzStateManagerImpl();
void free_EzStateManagerImpl(EzStateManagerImpl* to);

#endif
