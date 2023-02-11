#pragma once
#ifndef QInputMgrWindows_STRUCT_H
#define QInputMgrWindows_STRUCT_H

//NOTE: Anything marked "data_x" is a bunch of non-pointer data that can be saved/loaded without really knowing what it is.
// See the ghidra repo for more specific info about what is in that blob, if it's even known

#include <stddef.h>
#include <stdint.h>

typedef struct QInputMgrWindows QInputMgrWindows;
typedef struct QInputMgr QInputMgr;
typedef struct QInputDevice QInputDevice;
typedef struct QMouse QMouse;
typedef struct QKeyboard QKeyboard;
typedef struct QXInputPad QXInputPad;
typedef struct QDirectInputPad QDirectInputPad;
typedef struct QInputStates_substruct_BD QInputStates_substruct_BD;
typedef struct QInputStates_substruct_BDQ QInputStates_substruct_BDQ;

struct QInputStates_substruct_BDQ
{
    uint8_t data_0[0x10];
    uint8_t* data_1;
};

static_assert(sizeof(QInputStates_substruct_BDQ) == 0x18);

struct QInputStates_substruct_BD
{
    uint8_t data_0[0x10];
};

static_assert(sizeof(QInputStates_substruct_BD) == 0x10);

struct QInputDevice
{
    uint64_t vtable;
    uint64_t padding_1;
};

static_assert(sizeof(QInputDevice) == 0x10);

const uint64_t QDirectInputPadVtable = 0x14149dae8;
struct QDirectInputPad
{
    QInputDevice base;
    uint64_t padding_0;
    uint64_t data_1;
    QInputStates_substruct_BD data_2[32];
    QInputStates_substruct_BDQ data_3[32];
    uint64_t padding_1[8];
    uint32_t data_4[32];
    uint64_t padding_2[2];
};

static_assert(offsetof(QDirectInputPad, data_1) == 0x10 + 0x8);
static_assert(offsetof(QDirectInputPad, data_2) == 0x10 + 0x10);
static_assert(offsetof(QDirectInputPad, data_3) == 0x10 + 0x210);
static_assert(offsetof(QDirectInputPad, data_4) == 0x10 + 0x550);
static_assert(sizeof(QDirectInputPad) == 0x5f0);

const uint64_t QXInputPadVtable = 0x14149dc60;
struct QXInputPad
{
    QInputDevice base;
    uint64_t padding_0;
    uint64_t data_1;
    QInputStates_substruct_BD data_2[24];
    QInputStates_substruct_BDQ data_3[24];
    uint64_t data_3a[6];
    uint32_t data_4[24];
    uint64_t padding_2[2];
};

static_assert(offsetof(QXInputPad, data_1) == 0x10 + 0x8);
static_assert(offsetof(QXInputPad, data_2) == 0x10 + 0x10);
static_assert(offsetof(QXInputPad, data_3) == 0x10 + 0x190);
static_assert(offsetof(QXInputPad, data_4) == 0x10 + 0x400);
static_assert(sizeof(QXInputPad) == 0x480);

const uint64_t QKeyboardVtable = 0x14149dd68;
#pragma pack(push, 1)
struct QKeyboard
{
    QInputDevice base;
    uint64_t padding_0;
    uint64_t data_1;
    QInputStates_substruct_BD data_2[256];
    QInputStates_substruct_BDQ data_3[256];
    uint8_t data_3a[0x200];
    uint32_t data_4[256];
    uint8_t padding_2[24];
};
#pragma pack(pop)

static_assert(offsetof(QKeyboard, data_1) == 0x10 + 0x8);
static_assert(offsetof(QKeyboard, data_2) == 0x10 + 0x10);
static_assert(offsetof(QKeyboard, data_3) == 0x10 + 0x1010);
static_assert(offsetof(QKeyboard, data_4) == 0x10 + 0x2a10);
static_assert(sizeof(QKeyboard) == 0x2e38);

const uint64_t QMouseVtable = 0x14149ddb8;
#pragma pack(push, 1)
struct QMouse
{
    QInputDevice base;
    uint64_t padding_0;
    uint64_t data_1;
    QInputStates_substruct_BD data_2[14];
    QInputStates_substruct_BDQ data_3[14];
    uint64_t padding_1;
    uint8_t data_4[6];
    uint64_t padding_2;
    uint8_t data_5[6];
    uint32_t data_6[15];
    uint64_t padding_3;
    uint64_t padding_4;
};
#pragma pack(pop)

static_assert(offsetof(QMouse, data_1) == 0x10 + 0x8);
static_assert(offsetof(QMouse, data_2) == 0x10 + 0x10);
static_assert(offsetof(QMouse, data_3) == 0x10 + 0xf0);
static_assert(offsetof(QMouse, data_4) == 0x10 + 0x248);
static_assert(offsetof(QMouse, data_5) == 0x10 + 0x256);
static_assert(offsetof(QMouse, data_6) == 0x10 + 0x25c);
static_assert(sizeof(QMouse) == 0x2b8);

struct QInputMgr
{
    uint64_t padding_0;
    QInputDevice** device_array_start; //this is a list of mixed QMouse, QKeyboard, QXInputPad, and QDirectInputPad struct ptrs. Identify via vtable
    QInputDevice** device_array_end;
    uint8_t padding_1[8];
    uint64_t data_0;
    uint64_t curDevice; //this is a pointer to a QInputDevice in the device list, but we can just treat it like a const since those pointers don't change
};

static_assert(offsetof(QInputMgr, device_array_start) == 0x8);
static_assert(offsetof(QInputMgr, curDevice) == 0x28);
static_assert(sizeof(QInputMgr) == 0x30);

struct QInputMgrWindows
{
    QInputMgr base;
};

static_assert(offsetof(QInputMgrWindows, base) == 0);
static_assert(sizeof(QInputMgrWindows) == 0x30);

#endif
