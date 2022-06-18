#pragma once
#ifndef BULLETMAN_STRUCT_H
#define BULLETMAN_STRUCT_H

//NOTE: Anything marked "data_x" is a bunch of non-pointer data that can be saved/loaded without really knowing what it is.
// See the ghidra repo for more specific info about what is in that blob, if it's even known

#include <stddef.h>
#include <stdint.h>

typedef struct BulletMan BulletMan;
typedef struct BulletIns BulletIns;

struct Bulletman
{
    BulletIns** bulletins_arry;
    uint64_t data_0[2]; //just pointers to offsets in bulletins_arry

};
static_assert(offsetof(Bulletman, bulletins_arry) == 0);
static_assert(offsetof(Bulletman, data_0) == 8);
static_assert(sizeof(Bulletman) == 0xb8);

#endif
