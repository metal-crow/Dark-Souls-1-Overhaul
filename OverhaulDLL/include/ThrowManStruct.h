#pragma once
#ifndef THROWMAN_STRUCT_H
#define THROWMAN_STRUCT_H

//NOTE: Anything marked "data_x" is a bunch of non-pointer data that can be saved/loaded without really knowing what it is.
// See the ghidra repo for more specific info about what is in that blob, if it's even known

#include <stddef.h>
#include <stdint.h>

typedef struct ThrowMan ThrowMan;
typedef struct ThrowRequestedEntry ThrowRequestedEntry;

//ghidra equivalent: ThrowEntryInfo
struct ThrowRequestedEntry
{
    void* attacker; //these both point to the playerins->throw_animation_info entries. Treat as consts (ghidra: EntityThrowAnimationStatus*)
    void* defender;
    uint32_t throwId;      // 0x10
    float throwTimeout;    // 0x14
    uint8_t unk_18;        // 0x18 (undefined in ghidra)
    uint8_t unk1;          // 0x19
    uint8_t unk_1a[6];     // 0x1a (undefined in ghidra; trailing bytes)
};
static_assert(offsetof(ThrowRequestedEntry, throwId) == 0x10);
static_assert(sizeof(ThrowRequestedEntry) == 0x20);

struct ThrowMan
{
    uint8_t padding_0[0x28];   // heap1 + entity_throw_status vector + heap2 (not copied)
    ThrowRequestedEntry** throw_request_queue_start;
    ThrowRequestedEntry** throw_request_queue_cur;
    ThrowRequestedEntry** throw_request_queue_end;
    uint8_t padding_1[8*8];    // heap3 + unused vector + throwList vector (not copied)
    bool unk_80;               // 0x80
    bool unk_81;               // 0x81
    uint8_t unk_82[6];         // 0x82 (alignment)
    uint8_t padding_2[16];     // 0x88: two byte* pointers (not copied)
    float timeoutVal;                                  // 0x98
    float throwEscape_remainingTypeToDecayWeighting;   // 0x9c
    float throwEscape_IncreaseValueOfGoalWeighting;    // 0xa0
    float throwEscape_NowIncreaseWeighting;            // 0xa4
    float throwEscape_goalWeightLossVals;              // 0xa8
    float throwEscape_curWeightLossVals;               // 0xac
    uint8_t padding_3[8];      // 0xb0: dbgMenu pointer (not copied)
    uint32_t attacking_chr;    // 0xb8
    uint32_t defending_chr;    // 0xbc
    uint8_t allDrawing;        // 0xc0
    uint8_t unk_c1[3];         // 0xc1 (alignment)
    float unk_c4;              // 0xc4
    float animPlaySpeed;       // 0xc8
    uint8_t unk_cc;            // 0xcc
    uint8_t unk_cd[3];         // 0xcd (alignment)
    uint32_t unk_d0;           // 0xd0
    uint8_t unk_d4[12];        // 0xd4 (undefined in ghidra)
    float vec_e0[8];           // 0xe0 (ghidra: Vector4[2])
    float vec_100[8];          // 0x100 (ghidra: Vector4[2])
    uint32_t unk_120[2];       // 0x120
    uint8_t unk_128[2];        // 0x128
    uint8_t unk_12a[2];        // 0x12a (alignment)
    uint32_t unk_12c;          // 0x12c
};
static_assert(offsetof(ThrowMan, throw_request_queue_start) == 0x28);
static_assert(offsetof(ThrowMan, unk_80) == 0x80);
static_assert(offsetof(ThrowMan, timeoutVal) == 0x98);
static_assert(offsetof(ThrowMan, attacking_chr) == 0xb8);
static_assert(offsetof(ThrowMan, vec_e0) == 0xe0);
static_assert(offsetof(ThrowMan, unk_12c) == 0x12c);
static_assert(sizeof(ThrowMan) == 0x130);

#endif
