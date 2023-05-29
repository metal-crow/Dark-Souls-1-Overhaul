#pragma once
#ifndef THROWMAN_STRUCT_H
#define THROWMAN_STRUCT_H

//NOTE: Anything marked "data_x" is a bunch of non-pointer data that can be saved/loaded without really knowing what it is.
// See the ghidra repo for more specific info about what is in that blob, if it's even known

#include <stddef.h>
#include <stdint.h>

typedef struct ThrowMan ThrowMan;
typedef struct ThrowRequestedEntry ThrowRequestedEntry;

struct ThrowRequestedEntry
{
    void* attacker; //these both point to the playerins->throw_animation_info entries. Treat as consts
    void* defender;
    uint8_t data_0[16];
};
static_assert(sizeof(ThrowRequestedEntry) == 0x20);

struct ThrowMan
{
    uint8_t padding_0[0x28];
    ThrowRequestedEntry** throw_request_queue_start;
    ThrowRequestedEntry** throw_request_queue_cur;
    ThrowRequestedEntry** throw_request_queue_end;
    uint8_t padding_1[0x8d];
    uint8_t data_0;
};
static_assert(offsetof(ThrowMan, throw_request_queue_start) == 0x28);
static_assert(offsetof(ThrowMan, data_0) == 0xcd);
//static_assert(sizeof(ThrowMan) == 0x40);

#endif
