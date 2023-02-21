#include "QInputMgrWindowsStructFunctions.h"
#include "Rollback.h"

void FlattenedQInputMgrWindowsObj_to_QInputMgrWindows(QInputMgrWindows* to, FlattenedQInputMgrWindowsObj* from)
{
    {
        QMouse* to_mouse = (QMouse*)to->base.device_array_start[0];
        to_mouse->data_1 = from->mouse.data_1;
        for (size_t i = 0; i < 14; i++)
        {
            memcpy(to_mouse->data_2[i].data_0, from->mouse.data_2[i].data_0, 0x10);
        }
        for (size_t i = 0; i < 14; i++)
        {
            memcpy(to_mouse->data_3[i].data_0, from->mouse.data_3[i].data_0, 0x10);
            *to_mouse->data_3[i].data_1 = from->mouse.data_3[i].data_1;
        }
        memcpy(to_mouse->data_4, from->mouse.data_4, sizeof(to_mouse->data_4));
        memcpy(to_mouse->data_5, from->mouse.data_5, sizeof(to_mouse->data_5));
        memcpy(to_mouse->data_6, from->mouse.data_6, sizeof(to_mouse->data_6));
    }

    {
        QKeyboard* to_kybd = (QKeyboard*)to->base.device_array_start[1];
        to_kybd->data_1 = from->keyboard.data_1;
        for (size_t i = 0; i < 256; i++)
        {
            memcpy(to_kybd->data_2[i].data_0, from->keyboard.data_2[i].data_0, 0x10);
        }
        for (size_t i = 0; i < 256; i++)
        {
            memcpy(to_kybd->data_3[i].data_0, from->keyboard.data_3[i].data_0, 0x10);
            *to_kybd->data_3[i].data_1 = from->keyboard.data_3[i].data_1;
        }
        memcpy(to_kybd->data_3a, from->keyboard.data_3a, sizeof(to_kybd->data_3a));
        memcpy(to_kybd->data_4, from->keyboard.data_4, sizeof(to_kybd->data_4));
    }

    for (size_t j = 0; j < 4; j++)
    {
        QXInputPad* to_pad = (QXInputPad*)to->base.device_array_start[2 + j];
        FlattenedQXInputPad* from_pad = &from->xpads[j];

        to_pad->data_1 = from_pad->data_1;
        for (size_t i = 0; i < 24; i++)
        {
            memcpy(to_pad->data_2[i].data_0, from_pad->data_2[i].data_0, 0x10);
        }
        for (size_t i = 0; i < 24; i++)
        {
            memcpy(to_pad->data_3[i].data_0, from_pad->data_3[i].data_0, 0x10);
            *to_pad->data_3[i].data_1 = from_pad->data_3[i].data_1;
        }
        memcpy(to_pad->data_3a, from_pad->data_3a, sizeof(to_pad->data_3a));
        memcpy(to_pad->data_4, from_pad->data_4, sizeof(to_pad->data_4));
    }

    for (size_t j = 0; j < 4; j++)
    {
        QDirectInputPad* to_pad = (QDirectInputPad*)to->base.device_array_start[6 + j];
        FlattenedQDirectInputPad* from_pad = &from->dpads[j];

        to_pad->data_1 = from_pad->data_1;
        for (size_t i = 0; i < 32; i++)
        {
            memcpy(to_pad->data_2[i].data_0, from_pad->data_2[i].data_0, 0x10);
        }
        for (size_t i = 0; i < 32; i++)
        {
            memcpy(to_pad->data_3[i].data_0, from_pad->data_3[i].data_0, 0x10);
            *to_pad->data_3[i].data_1 = from_pad->data_3[i].data_1;
        }
        memcpy(to_pad->data_4, from_pad->data_4, sizeof(to_pad->data_4));
    }

    to->base.data_0 = from->data_0;
    to->base.curDevice = from->curDevice;
}

void QInputMgrWindows_to_FlattenedQInputMgrWindowsObj(FlattenedQInputMgrWindowsObj* to, QInputMgrWindows* from)
{
    {
        QMouse* from_mouse = (QMouse*)from->base.device_array_start[0];
        to->mouse.data_1 = from_mouse->data_1;
        for (size_t i = 0; i < 14; i++)
        {
            memcpy(to->mouse.data_2[i].data_0, from_mouse->data_2[i].data_0, 0x10);
        }
        for (size_t i = 0; i < 14; i++)
        {
            memcpy(to->mouse.data_3[i].data_0, from_mouse->data_3[i].data_0, 0x10);
            to->mouse.data_3[i].data_1 = *from_mouse->data_3[i].data_1;
        }
        memcpy(to->mouse.data_4, from_mouse->data_4, sizeof(to->mouse.data_4));
        memcpy(to->mouse.data_5, from_mouse->data_5, sizeof(to->mouse.data_5));
        memcpy(to->mouse.data_6, from_mouse->data_6, sizeof(to->mouse.data_6));
    }

    {
        QKeyboard* from_kybd = (QKeyboard*)from->base.device_array_start[1];
        to->keyboard.data_1 = from_kybd->data_1;
        for (size_t i = 0; i < 256; i++)
        {
            memcpy(to->keyboard.data_2[i].data_0, from_kybd->data_2[i].data_0, 0x10);
        }
        for (size_t i = 0; i < 256; i++)
        {
            memcpy(to->keyboard.data_3[i].data_0, from_kybd->data_3[i].data_0, 0x10);
            to->keyboard.data_3[i].data_1 = *from_kybd->data_3[i].data_1;
        }
        memcpy(to->keyboard.data_3a, from_kybd->data_3a, sizeof(to->keyboard.data_3a));
        memcpy(to->keyboard.data_4, from_kybd->data_4, sizeof(to->keyboard.data_4));
    }

    for (size_t j = 0; j < 4; j++)
    {
        QXInputPad* from_pad = (QXInputPad*)from->base.device_array_start[2+j];
        FlattenedQXInputPad* to_pad = &to->xpads[j];

        to_pad->data_1 = from_pad->data_1;
        for (size_t i = 0; i < 24; i++)
        {
            memcpy(to_pad->data_2[i].data_0, from_pad->data_2[i].data_0, 0x10);
        }
        for (size_t i = 0; i < 24; i++)
        {
            memcpy(to_pad->data_3[i].data_0, from_pad->data_3[i].data_0, 0x10);
            to_pad->data_3[i].data_1 = *from_pad->data_3[i].data_1;
        }
        memcpy(to_pad->data_3a, from_pad->data_3a, sizeof(to_pad->data_3a));
        memcpy(to_pad->data_4, from_pad->data_4, sizeof(to_pad->data_4));
    }

    for (size_t j = 0; j < 4; j++)
    {
        QDirectInputPad* from_pad = (QDirectInputPad*)from->base.device_array_start[6 + j];
        FlattenedQDirectInputPad* to_pad = &to->dpads[j];

        to_pad->data_1 = from_pad->data_1;
        for (size_t i = 0; i < 32; i++)
        {
            memcpy(to_pad->data_2[i].data_0, from_pad->data_2[i].data_0, 0x10);
        }
        for (size_t i = 0; i < 32; i++)
        {
            memcpy(to_pad->data_3[i].data_0, from_pad->data_3[i].data_0, 0x10);
            to_pad->data_3[i].data_1 = *from_pad->data_3[i].data_1;
        }
        memcpy(to_pad->data_4, from_pad->data_4, sizeof(to_pad->data_4));
    }

    to->data_0 = from->base.data_0;
    to->curDevice = from->base.curDevice;
}

void copy_QInputMgrWindows(QInputMgrWindows* to, QInputMgrWindows* from)
{
    copy_QInputMgr(&to->base, &from->base);
}

QInputMgrWindows* init_QInputMgrWindows()
{
    QInputMgrWindows* local_QInputMgrWindows = (QInputMgrWindows*)malloc_(sizeof(QInputMgrWindows));

    QInputMgr* local_QInputMgr = init_QInputMgr();
    local_QInputMgrWindows->base = *local_QInputMgr;
    free(local_QInputMgr);

    return local_QInputMgrWindows;
}

void free_QInputMgrWindows(QInputMgrWindows* to)
{
    free_QInputMgr(&to->base, false);

    free(to);
}

static const size_t QInputMgrDeviceArraySize = 10;

void copy_QInputMgr(QInputMgr* to, QInputMgr* from)
{
    if (from->device_array_start == NULL)
    {
        ConsoleWrite("Error: copy_QInputMgr got NULL for from->device_array_start");
        return;
    }
    if (to->device_array_start == NULL)
    {
        ConsoleWrite("Error: copy_QInputMgr got NULL for to->device_array_start");
        return;
    }

    size_t fromDeviceArrayLen = ((uint64_t)from->device_array_end - (uint64_t)from->device_array_start) / 8;
    size_t toDeviceArrayLen = ((uint64_t)to->device_array_end - (uint64_t)to->device_array_start) / 8;
    if (toDeviceArrayLen != QInputMgrDeviceArraySize || fromDeviceArrayLen != QInputMgrDeviceArraySize)
    {
        ConsoleWrite("Error: copy_QInputMgr got device array not of expected size: %d %d", toDeviceArrayLen, fromDeviceArrayLen);
        return;
    }

    for (size_t i = 0; i < QInputMgrDeviceArraySize; i++)
    {
        QInputDevice* from_device = from->device_array_start[i];
        QInputDevice* to_device = to->device_array_start[i];
        uint64_t from_vtable = from_device->vtable;
        uint64_t to_vtable = to_device->vtable;
        if (from_vtable != to_vtable)
        {
            ConsoleWrite("ERROR: Vtable mismatch in copy_QInputMgr at index %d. Skipping.", i);
            continue;
        }

        switch (from_vtable)
        {
        case QMouseVtable:
            copy_QMouse((QMouse*)(to_device), (QMouse*)(from_device));
            break;
        case QKeyboardVtable:
            copy_QKeyboard((QKeyboard*)(to_device), (QKeyboard*)(from_device));
            break;
        case QXInputPadVtable:
            copy_QXInputPad((QXInputPad*)(to_device), (QXInputPad*)(from_device));
            break;
        case QDirectInputPadVtable:
            copy_QDirectInputPad((QDirectInputPad*)(to_device), (QDirectInputPad*)(from_device));
            break;
        default:
            ConsoleWrite("ERROR: copy_QInputMgr got vtable %llx in the device_array at %p (index %d). Skipping entry.", from_vtable, from_device, i);
            break;
        }
    }

    to->data_0 = from->data_0;
    to->curDevice = from->curDevice;
}

QInputMgr* init_QInputMgr()
{
    QInputMgr* local_QInputMgr = (QInputMgr*)malloc_(sizeof(QInputMgr));

    local_QInputMgr->device_array_start = (QInputDevice**)malloc_(sizeof(void*) * QInputMgrDeviceArraySize);
    local_QInputMgr->device_array_end = &local_QInputMgr->device_array_start[QInputMgrDeviceArraySize];

    //the game always constructs these in the same order
    local_QInputMgr->device_array_start[0] = (QInputDevice*)init_QMouse();
    local_QInputMgr->device_array_start[1] = (QInputDevice*)init_QKeyboard();

    local_QInputMgr->device_array_start[2] = (QInputDevice*)init_QXInputPad();
    local_QInputMgr->device_array_start[3] = (QInputDevice*)init_QXInputPad();
    local_QInputMgr->device_array_start[4] = (QInputDevice*)init_QXInputPad();
    local_QInputMgr->device_array_start[5] = (QInputDevice*)init_QXInputPad();

    local_QInputMgr->device_array_start[6] = (QInputDevice*)init_QDirectInputPad();
    local_QInputMgr->device_array_start[7] = (QInputDevice*)init_QDirectInputPad();
    local_QInputMgr->device_array_start[8] = (QInputDevice*)init_QDirectInputPad();
    local_QInputMgr->device_array_start[9] = (QInputDevice*)init_QDirectInputPad();

    return local_QInputMgr;
}

void free_QInputMgr(QInputMgr* to, bool free_self)
{
    for (size_t i = 0; i < QInputMgrDeviceArraySize; i++)
    {
        QInputDevice* device = to->device_array_start[i];
        uint64_t vtable = device->vtable;
        switch (vtable)
        {
        case QMouseVtable:
            free_QMouse((QMouse*)(device));
            break;
        case QKeyboardVtable:
            free_QKeyboard((QKeyboard*)(device));
            break;
        case QXInputPadVtable:
            free_QXInputPad((QXInputPad*)(device));
            break;
        case QDirectInputPadVtable:
            free_QDirectInputPad((QDirectInputPad*)(device));
            break;
        }
    }

    free(to->device_array_start);

    if (free_self)
    {
        free(to);
    }
}

void copy_QMouse(QMouse* to, QMouse* from)
{
    to->data_1 = from->data_1;
    memcpy(to->data_2, from->data_2, sizeof(to->data_2));
    for (size_t i = 0; i < sizeof(to->data_3) / sizeof(to->data_3[0]); i++)
    {
        copy_QInputStates_substruct_BDQ(&to->data_3[i], &from->data_3[i]);
    }
    memcpy(to->data_4, from->data_4, sizeof(to->data_4));
    memcpy(to->data_5, from->data_5, sizeof(to->data_5));
    memcpy(to->data_6, from->data_6, sizeof(to->data_6));
}

QMouse* init_QMouse()
{
    QMouse* local = (QMouse*)malloc_(sizeof(QMouse));

    local->base.vtable = QMouseVtable;
    for (size_t i = 0; i < sizeof(local->data_3) / sizeof(local->data_3[0]); i++)
    {
        QInputStates_substruct_BDQ* elem = init_QInputStates_substruct_BDQ();
        local->data_3[i] = *elem;
        free(elem);
    }

    return local;
}

void free_QMouse(QMouse* to)
{
    for (size_t i = 0; i < sizeof(to->data_3) / sizeof(to->data_3[0]); i++)
    {
        free_QInputStates_substruct_BDQ(&to->data_3[i], false);
    }
    free(to);
}

void copy_QKeyboard(QKeyboard* to, QKeyboard* from)
{
    to->data_1 = from->data_1;
    memcpy(to->data_2, from->data_2, sizeof(to->data_2));
    for (size_t i = 0; i < sizeof(to->data_3) / sizeof(to->data_3[0]); i++)
    {
        copy_QInputStates_substruct_BDQ(&to->data_3[i], &from->data_3[i]);
    }
    memcpy(to->data_3a, from->data_3a, sizeof(to->data_3a));
    memcpy(to->data_4, from->data_4, sizeof(to->data_4));
}

QKeyboard* init_QKeyboard()
{
    QKeyboard* local = (QKeyboard*)malloc_(sizeof(QKeyboard));

    local->base.vtable = QKeyboardVtable;
    for (size_t i = 0; i < sizeof(local->data_3) / sizeof(local->data_3[0]); i++)
    {
        QInputStates_substruct_BDQ* elem = init_QInputStates_substruct_BDQ();
        local->data_3[i] = *elem;
        free(elem);
    }

    return local;
}

void free_QKeyboard(QKeyboard* to)
{
    for (size_t i = 0; i < sizeof(to->data_3) / sizeof(to->data_3[0]); i++)
    {
        free_QInputStates_substruct_BDQ(&to->data_3[i], false);
    }
    free(to);
}

void copy_QXInputPad(QXInputPad* to, QXInputPad* from)
{
    to->data_1 = from->data_1;
    memcpy(to->data_2, from->data_2, sizeof(to->data_2));
    for (size_t i = 0; i < sizeof(to->data_3) / sizeof(to->data_3[0]); i++)
    {
        copy_QInputStates_substruct_BDQ(&to->data_3[i], &from->data_3[i]);
    }
    memcpy(to->data_3a, from->data_3a, sizeof(to->data_3a));
    memcpy(to->data_4, from->data_4, sizeof(to->data_4));
}

QXInputPad* init_QXInputPad()
{
    QXInputPad* local = (QXInputPad*)malloc_(sizeof(QXInputPad));

    local->base.vtable = QXInputPadVtable;
    for (size_t i = 0; i < sizeof(local->data_3) / sizeof(local->data_3[0]); i++)
    {
        QInputStates_substruct_BDQ* elem = init_QInputStates_substruct_BDQ();
        local->data_3[i] = *elem;
        free(elem);
    }

    return local;
}

void free_QXInputPad(QXInputPad* to)
{
    for (size_t i = 0; i < sizeof(to->data_3) / sizeof(to->data_3[0]); i++)
    {
        free_QInputStates_substruct_BDQ(&to->data_3[i], false);
    }
    free(to);
}

void copy_QDirectInputPad(QDirectInputPad* to, QDirectInputPad* from)
{
    to->data_1 = from->data_1;
    memcpy(to->data_2, from->data_2, sizeof(to->data_2));
    for (size_t i = 0; i < sizeof(to->data_3) / sizeof(to->data_3[0]); i++)
    {
        copy_QInputStates_substruct_BDQ(&to->data_3[i], &from->data_3[i]);
    }
    memcpy(to->data_4, from->data_4, sizeof(to->data_4));
}

QDirectInputPad* init_QDirectInputPad()
{
    QDirectInputPad* local = (QDirectInputPad*)malloc_(sizeof(QDirectInputPad));

    local->base.vtable = QDirectInputPadVtable;
    for (size_t i = 0; i < sizeof(local->data_3) / sizeof(local->data_3[0]); i++)
    {
        QInputStates_substruct_BDQ* elem = init_QInputStates_substruct_BDQ();
        local->data_3[i] = *elem;
        free(elem);
    }

    return local;
}

void free_QDirectInputPad(QDirectInputPad* to)
{
    for (size_t i = 0; i < sizeof(to->data_3) / sizeof(to->data_3[0]); i++)
    {
        free_QInputStates_substruct_BDQ(&to->data_3[i], false);
    }
    free(to);
}

void copy_QInputStates_substruct_BDQ(QInputStates_substruct_BDQ* to, QInputStates_substruct_BDQ* from)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    if (to->data_1 == NULL && from->data_1 == NULL)
    {
        ConsoleWrite("ERROR: got data1 ptr as null for QInputStates_substruct_BDQ. %p %p", to, from);
    }
    else
    {
        *to->data_1 = *from->data_1;
    }
}

QInputStates_substruct_BDQ* init_QInputStates_substruct_BDQ()
{
    QInputStates_substruct_BDQ* local = (QInputStates_substruct_BDQ*)malloc_(sizeof(QInputStates_substruct_BDQ));

    local->data_1 = (uint8_t*)malloc_(sizeof(uint8_t));

    return local;
}

void free_QInputStates_substruct_BDQ(QInputStates_substruct_BDQ* to, bool free_self)
{
    free(to->data_1);

    if (free_self)
    {
        free(to);
    }
}
