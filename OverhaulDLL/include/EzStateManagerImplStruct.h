#pragma once
#ifndef EZSTATEMAN_STRUCT_H
#define EZSTATEMAN_STRUCT_H

//NOTE: Anything marked "data_x" is a bunch of non-pointer data that can be saved/loaded without really knowing what it is.
// See the ghidra repo for more specific info about what is in that blob, if it's even known

#include <stddef.h>
#include <stdint.h>

typedef struct EzStateManagerImpl EzStateManagerImpl;
typedef struct EzStateManagerImpl_field0x20 EzStateManagerImpl_field0x20;

struct EzStateManagerImpl_field0x20
{
    uint8_t data_0[0x490];
};
static_assert(sizeof(EzStateManagerImpl_field0x20) == 0x490);

struct EzStateManagerImpl
{
    uint64_t vtable;
    uint64_t heap;
    void* field0x10;
    void* DLSerializeLightArchiver;
    EzStateManagerImpl_field0x20* field0x20;
};
static_assert(sizeof(EzStateManagerImpl) == 0x28);


#endif
