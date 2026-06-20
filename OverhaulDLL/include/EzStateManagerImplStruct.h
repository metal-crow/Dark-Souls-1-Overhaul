#pragma once
#ifndef EZSTATEMAN_STRUCT_H
#define EZSTATEMAN_STRUCT_H

#include <stddef.h>
#include <stdint.h>

typedef struct EzStateManagerImpl EzStateManagerImpl;
typedef struct EzStateManagerImpl_field0x20 EzStateManagerImpl_field0x20;
typedef struct EzStateEnvironmentQueryImpl_Elem EzStateEnvironmentQueryImpl_Elem;

struct EzStateEnvironmentQueryImpl_Elem
{
    uint64_t data;   // 0x0 (ghidra: EzStateSharedArg, opaque 8-byte type - possibly a pointer/union)
    uint32_t type;   // 0x8 (ghidra: EzStateCommandVarTypes enum)
    uint32_t unk_c;  // 0xc (undefined in ghidra; alignment)
};
static_assert(sizeof(EzStateEnvironmentQueryImpl_Elem) == 0x10);

struct EzStateManagerImpl_field0x20
{
    EzStateEnvironmentQueryImpl_Elem queries[64];   // 0x0
    EzStateEnvironmentQueryImpl_Elem registers[8];  // 0x400
    void* current;   // 0x480 (ghidra: EzStateEnvironmentQueryImpl_Elem* - points into the arrays above)
    uint64_t heap;   // 0x488 (ghidra: pointer)
};
static_assert(offsetof(EzStateManagerImpl_field0x20, registers) == 0x400);
static_assert(offsetof(EzStateManagerImpl_field0x20, current) == 0x480);
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
