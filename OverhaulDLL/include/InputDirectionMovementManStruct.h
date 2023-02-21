#pragma once
#ifndef InputDirectionMovementMan_STRUCT_H
#define InputDirectionMovementMan_STRUCT_H

//NOTE: Anything marked "data_x" is a bunch of non-pointer data that can be saved/loaded without really knowing what it is.
// See the ghidra repo for more specific info about what is in that blob, if it's even known

#include <stddef.h>
#include <stdint.h>

typedef struct InputDirectionMovementMan InputDirectionMovementMan;
typedef struct FlattenedInputDirectionMovementManObj FlattenedInputDirectionMovementManObj;

#pragma pack(push, 1)

struct FlattenedInputDirectionMovementManObj
{
    uint64_t data_0[44];
    float data_1[44];
};

#pragma pack(pop)

struct InputDirectionMovementMan
{
    uint64_t data_0[44];
    float* data_1[44];
};

static_assert(offsetof(InputDirectionMovementMan, data_1) == 0x160);
static_assert(sizeof(InputDirectionMovementMan) == 0x2c0);

#endif
