#pragma once
#ifndef DMGHITRECORD_STRUCT_H
#define DMGHITRECORD_STRUCT_H

//NOTE: Anything marked "data_x" is a bunch of non-pointer data that can be saved/loaded without really knowing what it is.
// See the ghidra repo for more specific info about what is in that blob, if it's even known

#include <stddef.h>
#include <stdint.h>
#include "BulletManStruct.h"

typedef struct DmgHitRecordManImp_field0x28Elem DmgHitRecordManImp_field0x28Elem;
typedef struct DmgHitRecordManImp_field0x10Elem DmgHitRecordManImp_field0x10Elem;
typedef struct DmgHitRecordManImp DmgHitRecordManImp;

struct DmgHitRecordManImp_field0x28Elem
{
    uint32_t targetHandle;  // 0x0
    uint32_t unk_4;         // 0x4
    DmgHitRecordManImp_field0x28Elem* next; //offset in this array (0x8)
    uint32_t unk_10;        // 0x10 (ghidra: uint)
    uint32_t unk_14;        // 0x14 (undefined in ghidra; trailing alignment)
};
static_assert(offsetof(DmgHitRecordManImp_field0x28Elem, targetHandle) == 0);
static_assert(offsetof(DmgHitRecordManImp_field0x28Elem, next) == 0x8);
static_assert(offsetof(DmgHitRecordManImp_field0x28Elem, unk_10) == 0x10);
static_assert(sizeof(DmgHitRecordManImp_field0x28Elem) == 0x18);

struct DmgHitRecordManImp_field0x10Elem
{
    DmgHitRecordManImp_field0x28Elem* attached; //treat this as an offset, since it's an element in the other DmgHitRecordManImp array (0x0)
    float unk_8;            // 0x8 (ghidra: float)
    uint32_t unk_c;         // 0xc (undefined in ghidra; alignment)
    DmgHitRecordManImp_field0x10Elem* next; //offset in this array (0x10)
    uint32_t unk_18;        // 0x18 (ghidra: uint)
    uint32_t unk_1c;        // 0x1c (undefined in ghidra; trailing alignment)
};
static_assert(offsetof(DmgHitRecordManImp_field0x10Elem, attached) == 0);
static_assert(offsetof(DmgHitRecordManImp_field0x10Elem, unk_8) == 8);
static_assert(offsetof(DmgHitRecordManImp_field0x10Elem, next) == 0x10);
static_assert(offsetof(DmgHitRecordManImp_field0x10Elem, unk_18) == 0x18);
static_assert(sizeof(DmgHitRecordManImp_field0x10Elem) == 0x20);

struct DmgHitRecordManImp
{
    uint64_t vtable;
    DmgHitRecordManImp_field0x10Elem* field0x10_array; //offset from array start
    DmgHitRecordManImp_field0x10Elem* field0x10_array_start;
    uint64_t padding_0;
    DmgHitRecordManImp_field0x28Elem* field0x28_array; //offset from array start
    DmgHitRecordManImp_field0x28Elem* field0x28_array_start;
    uint32_t field0x10_array_len;
    uint32_t data_0;
    uint32_t field0x28_array_len;
    uint32_t data_1;
};
static_assert(offsetof(DmgHitRecordManImp, vtable) == 0);
static_assert(offsetof(DmgHitRecordManImp, field0x10_array) == 0x8);
static_assert(offsetof(DmgHitRecordManImp, field0x10_array_start) == 0x10);
static_assert(offsetof(DmgHitRecordManImp, field0x28_array) == 0x20);
static_assert(offsetof(DmgHitRecordManImp, field0x28_array_start) == 0x28);
static_assert(offsetof(DmgHitRecordManImp, field0x10_array_len) == 0x30);
static_assert(offsetof(DmgHitRecordManImp, field0x28_array_len) == 0x38);
static_assert(sizeof(DmgHitRecordManImp) == 0x40);

#endif
