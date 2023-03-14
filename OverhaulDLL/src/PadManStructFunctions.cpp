#include "PadManStructFunctions.h"
#include "Rollback.h"

void DLUserInputDeviceImpl_to_FlattenedPadManObj(FlattenedDLUserInputDeviceImplObj* to, DLUserInputDeviceImpl* from)
{
    memcpy(to->a, from->base.VirtInputData.VirAnalogKeyInfo.analogSticksAndPad, sizeof(float) * 0x80);
    memcpy(to->b, from->base.VirtInputData.keys.inputKeys, sizeof(uint32_t) * 32);
    memcpy(to->c, from->VirtAnalogKeyInfo_1.analogSticksAndPad, sizeof(float) * 0x80);
    memcpy(to->d, from->VirtAnalogKeyInfo_2.analogSticksAndPad, sizeof(float) * 0x80);
    memcpy(to->e, from->VirtInputData.VirAnalogKeyInfo.analogSticksAndPad, sizeof(float) * 0x80);
    memcpy(to->f, from->VirtInputData.keys.inputKeys, sizeof(uint32_t) * 32);
}

void PadMan_to_FlattenedPadManObj(FlattenedPadManObj* to, PadMan* from)
{
    DLUserInputDeviceImpl_to_FlattenedPadManObj(&to->VirtMultiDevice.base, &from->PadDevice_0->VirtMultiDevice->base);

    DLUserInputDeviceImpl_to_FlattenedPadManObj(&to->padDevice_UserInput.base, &from->PadDevice_0->padDevice_UserInput->base);
    memcpy(to->padDevice_UserInput.data_0, from->PadDevice_0->padDevice_UserInput->data_0, sizeof(to->padDevice_UserInput.data_0));
    memcpy(to->padDevice_UserInput.data_1, from->PadDevice_0->padDevice_UserInput->data_1, sizeof(to->padDevice_UserInput.data_1));
    to->padDevice_UserInput.data_2 = from->PadDevice_0->padDevice_UserInput->data_2;

    DLUserInputDeviceImpl_to_FlattenedPadManObj(&to->mouseDevice.base, &from->PadDevice_0->mouseDevice->base);
    memcpy(to->mouseDevice.data_0, from->PadDevice_0->mouseDevice->data_0, sizeof(to->mouseDevice.data_0));
    memcpy(to->mouseDevice.data_1, from->PadDevice_0->mouseDevice->data_1, sizeof(to->mouseDevice.data_1));

    DLUserInputDeviceImpl_to_FlattenedPadManObj(&to->keyboardDevice.base, &from->PadDevice_0->keyboardDevice->base);
    memcpy(to->keyboardDevice.data_0, from->PadDevice_0->keyboardDevice->data_0, sizeof(to->keyboardDevice.data_0));
}

void FlattenedPadManObj_to_DLUserInputDeviceImpl(DLUserInputDeviceImpl* to, FlattenedDLUserInputDeviceImplObj* from)
{
    memcpy(to->base.VirtInputData.VirAnalogKeyInfo.analogSticksAndPad, from->a, sizeof(float) * 0x80);
    memcpy(to->base.VirtInputData.keys.inputKeys, from->b, sizeof(uint32_t) * 32);
    memcpy(to->VirtAnalogKeyInfo_1.analogSticksAndPad, from->c, sizeof(float) * 0x80);
    memcpy(to->VirtAnalogKeyInfo_2.analogSticksAndPad, from->d, sizeof(float) * 0x80);
    memcpy(to->VirtInputData.VirAnalogKeyInfo.analogSticksAndPad, from->e, sizeof(float) * 0x80);
    memcpy(to->VirtInputData.keys.inputKeys, from->f, sizeof(uint32_t) * 32);
}

void FlattenedPadManObj_to_PadMan(PadMan* to, FlattenedPadManObj* from)
{
    FlattenedPadManObj_to_DLUserInputDeviceImpl(&to->PadDevice_0->VirtMultiDevice->base, &from->VirtMultiDevice.base);

    FlattenedPadManObj_to_DLUserInputDeviceImpl(&to->PadDevice_0->padDevice_UserInput->base, &from->padDevice_UserInput.base);
    memcpy(to->PadDevice_0->padDevice_UserInput->data_0, from->padDevice_UserInput.data_0, sizeof(to->PadDevice_0->padDevice_UserInput->data_0));
    memcpy(to->PadDevice_0->padDevice_UserInput->data_1, from->padDevice_UserInput.data_1, sizeof(to->PadDevice_0->padDevice_UserInput->data_1));
    to->PadDevice_0->padDevice_UserInput->data_2 = from->padDevice_UserInput.data_2;

    FlattenedPadManObj_to_DLUserInputDeviceImpl(&to->PadDevice_0->mouseDevice->base, &from->mouseDevice.base);
    memcpy(to->PadDevice_0->mouseDevice->data_0, from->mouseDevice.data_0, sizeof(to->PadDevice_0->mouseDevice->data_0));
    memcpy(to->PadDevice_0->mouseDevice->data_1, from->mouseDevice.data_1, sizeof(to->PadDevice_0->mouseDevice->data_1));

    FlattenedPadManObj_to_DLUserInputDeviceImpl(&to->PadDevice_0->keyboardDevice->base, &from->keyboardDevice.base);
    memcpy(to->PadDevice_0->keyboardDevice->data_0, from->keyboardDevice.data_0, sizeof(to->PadDevice_0->keyboardDevice->data_0));
}

void copy_PadMan(PadMan* to, PadMan* from)
{
    Game::SuspendThreads();

    copy_PadDevice(to->PadDevice_0, from->PadDevice_0);

    Game::ResumeThreads();
}

PadMan* init_PadMan()
{
    PadMan* local_PadMan = (PadMan*)malloc_(sizeof(PadMan));

    local_PadMan->PadDevice_0 = init_PadDevice();

    return local_PadMan;
}

void free_PadMan(PadMan* to)
{
    free_PadDevice(to->PadDevice_0);

    free(to);
}

void copy_PadDevice(PadDevice* to, PadDevice* from)
{
    copy_VirtualMultiDevice(to->VirtMultiDevice, from->VirtMultiDevice);
    copy_PadDevice_UserInput(to->padDevice_UserInput, from->padDevice_UserInput);
    copy_MouseDevice(to->mouseDevice, from->mouseDevice);
    copy_KeyboardDevice(to->keyboardDevice, from->keyboardDevice);
}

PadDevice* init_PadDevice()
{
    PadDevice* local_PadDevice = (PadDevice*)malloc_(sizeof(PadDevice));

    local_PadDevice->VirtMultiDevice = init_VirtualMultiDevice();
    local_PadDevice->padDevice_UserInput = init_PadDevice_UserInput();
    local_PadDevice->mouseDevice = init_MouseDevice();
    local_PadDevice->keyboardDevice = init_KeyboardDevice();

    return local_PadDevice;
}
void free_PadDevice(PadDevice* to)
{
    free_VirtualMultiDevice(to->VirtMultiDevice);
    free_PadDevice_UserInput(to->padDevice_UserInput);
    free_MouseDevice(to->mouseDevice);
    free_KeyboardDevice(to->keyboardDevice);

    free(to);
}

void copy_KeyboardDevice(KeyboardDevice* to, KeyboardDevice* from)
{
    copy_DLUserInputDeviceImpl(&to->base, &from->base);
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
}

KeyboardDevice* init_KeyboardDevice()
{
    KeyboardDevice* local_KeyboardDevice = (KeyboardDevice*)malloc_(sizeof(KeyboardDevice));

    DLUserInputDeviceImpl* local_DLUserInputDeviceImpl = init_DLUserInputDeviceImpl();
    local_KeyboardDevice->base = *local_DLUserInputDeviceImpl;
    free(local_DLUserInputDeviceImpl);

    return local_KeyboardDevice;
}

void free_KeyboardDevice(KeyboardDevice* to)
{
    free_DLUserInputDeviceImpl(&to->base, false);

    free(to);
}

void copy_MouseDevice(MouseDevice* to, MouseDevice* from)
{
    copy_DLUserInputDeviceImpl(&to->base, &from->base);
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
}

MouseDevice* init_MouseDevice()
{
    MouseDevice* local_MouseDevice = (MouseDevice*)malloc_(sizeof(MouseDevice));

    DLUserInputDeviceImpl* local_DLUserInputDeviceImpl = init_DLUserInputDeviceImpl();
    local_MouseDevice->base = *local_DLUserInputDeviceImpl;
    free(local_DLUserInputDeviceImpl);

    return local_MouseDevice;
}

void free_MouseDevice(MouseDevice* to)
{
    free_DLUserInputDeviceImpl(&to->base, false);

    free(to);
}

void copy_PadDevice_UserInput(PadDevice_UserInput* to, PadDevice_UserInput* from)
{
    copy_DLUserInputDeviceImpl(&to->base, &from->base);
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
    to->data_2 = from->data_2;
}

PadDevice_UserInput* init_PadDevice_UserInput()
{
    PadDevice_UserInput* local_PadDevice_UserInput = (PadDevice_UserInput*)malloc_(sizeof(PadDevice_UserInput));

    DLUserInputDeviceImpl* local_DLUserInputDeviceImpl = init_DLUserInputDeviceImpl();
    local_PadDevice_UserInput->base = *local_DLUserInputDeviceImpl;
    free(local_DLUserInputDeviceImpl);

    return local_PadDevice_UserInput;
}

void free_PadDevice_UserInput(PadDevice_UserInput* to)
{
    free_DLUserInputDeviceImpl(&to->base, false);

    free(to);
}

void copy_VirtualMultiDevice(VirtualMultiDevice* to, VirtualMultiDevice* from)
{
    copy_DLUserInputDeviceImpl(&to->base, &from->base);
}

VirtualMultiDevice* init_VirtualMultiDevice()
{
    VirtualMultiDevice* local_VirtualMultiDevice = (VirtualMultiDevice*)malloc_(sizeof(VirtualMultiDevice));

    DLUserInputDeviceImpl* local_DLUserInputDeviceImpl = init_DLUserInputDeviceImpl();
    local_VirtualMultiDevice->base = *local_DLUserInputDeviceImpl;
    free(local_DLUserInputDeviceImpl);

    return local_VirtualMultiDevice;
}

void free_VirtualMultiDevice(VirtualMultiDevice* to)
{
    free_DLUserInputDeviceImpl(&to->base, false);

    free(to);
}

void copy_DLUserInputDeviceImpl(DLUserInputDeviceImpl* to, DLUserInputDeviceImpl* from)
{
    copy_DLUserInputDevice(&to->base, &from->base);
    copy_VirtualAnalogKeyInfo_float(&to->VirtAnalogKeyInfo_1, &from->VirtAnalogKeyInfo_1);
    copy_VirtualAnalogKeyInfo_float(&to->VirtAnalogKeyInfo_2, &from->VirtAnalogKeyInfo_2);
    copy_VirtualInputData(&to->VirtInputData, &from->VirtInputData);
}

DLUserInputDeviceImpl* init_DLUserInputDeviceImpl()
{
    DLUserInputDeviceImpl* local_DLUserInputDeviceImpl = (DLUserInputDeviceImpl*)malloc_(sizeof(DLUserInputDeviceImpl));

    DLUserInputDevice* local_DLUserInputDevice = init_DLUserInputDevice();
    local_DLUserInputDeviceImpl->base = *local_DLUserInputDevice;
    free(local_DLUserInputDevice);

    VirtualAnalogKeyInfo_float* local_VirtualAnalogKeyInfo_float1 = init_VirtualAnalogKeyInfo_float();
    local_DLUserInputDeviceImpl->VirtAnalogKeyInfo_1 = *local_VirtualAnalogKeyInfo_float1;
    free(local_VirtualAnalogKeyInfo_float1);

    VirtualAnalogKeyInfo_float* local_VirtualAnalogKeyInfo_float2 = init_VirtualAnalogKeyInfo_float();
    local_DLUserInputDeviceImpl->VirtAnalogKeyInfo_2 = *local_VirtualAnalogKeyInfo_float2;
    free(local_VirtualAnalogKeyInfo_float2);

    VirtualInputData* local_VirtualInputData = init_VirtualInputData();
    local_DLUserInputDeviceImpl->VirtInputData = *local_VirtualInputData;
    free(local_VirtualInputData);

    return local_DLUserInputDeviceImpl;

}
void free_DLUserInputDeviceImpl(DLUserInputDeviceImpl* to, bool freeself)
{
    free_DLUserInputDevice(&to->base, false);
    free_VirtualAnalogKeyInfo_float(&to->VirtAnalogKeyInfo_1, false);
    free_VirtualAnalogKeyInfo_float(&to->VirtAnalogKeyInfo_2, false);
    free_VirtualInputData(&to->VirtInputData, false);

    if (freeself)
    {
        free(to);
    }
}

void copy_DLUserInputDevice(DLUserInputDevice* to, DLUserInputDevice* from)
{
    copy_VirtualInputData(&to->VirtInputData, &from->VirtInputData);
}

DLUserInputDevice* init_DLUserInputDevice()
{
    DLUserInputDevice* local_DLUserInputDevice = (DLUserInputDevice*)malloc_(sizeof(DLUserInputDevice));

    VirtualInputData* local_VirtualInputData = init_VirtualInputData();
    local_DLUserInputDevice->VirtInputData = *local_VirtualInputData;
    free(local_VirtualInputData);

    return local_DLUserInputDevice;
}

void free_DLUserInputDevice(DLUserInputDevice* to, bool freeself)
{
    free_VirtualInputData(&to->VirtInputData, false);

    if (freeself)
    {
        free(to);
    }
}

void copy_VirtualInputData(VirtualInputData* to, VirtualInputData* from)
{
    copy_VirtualAnalogKeyInfo_float(&to->VirAnalogKeyInfo, &from->VirAnalogKeyInfo);
    copy_DynamicBitset(&to->keys, &from->keys);
}

VirtualInputData* init_VirtualInputData()
{
    VirtualInputData* local_VirtualInputData = (VirtualInputData*)malloc_(sizeof(VirtualInputData));

    VirtualAnalogKeyInfo_float* local_VirtualAnalogKeyInfo_float = init_VirtualAnalogKeyInfo_float();
    local_VirtualInputData->VirAnalogKeyInfo = *local_VirtualAnalogKeyInfo_float;
    free(local_VirtualAnalogKeyInfo_float);

    DynamicBitset* local_DynamicBitset = init_DynamicBitset();
    local_VirtualInputData->keys = *local_DynamicBitset;
    free(local_DynamicBitset);

    return local_VirtualInputData;

}
void free_VirtualInputData(VirtualInputData* to, bool freeself)
{
    free_VirtualAnalogKeyInfo_float(&to->VirAnalogKeyInfo, false);
    free_DynamicBitset(&to->keys, false);

    if (freeself)
    {
        free(to);
    }
}

void copy_VirtualAnalogKeyInfo_float(VirtualAnalogKeyInfo_float* to, VirtualAnalogKeyInfo_float* from)
{
    memcpy(to->analogSticksAndPad, from->analogSticksAndPad, sizeof(float) * 0x80);
}

VirtualAnalogKeyInfo_float* init_VirtualAnalogKeyInfo_float()
{
    VirtualAnalogKeyInfo_float* local_VirtualAnalogKeyInfo_float = (VirtualAnalogKeyInfo_float*)malloc_(sizeof(VirtualAnalogKeyInfo_float));

    local_VirtualAnalogKeyInfo_float->analogSticksAndPad = (float*)malloc_(sizeof(float) * 0x80);

    return local_VirtualAnalogKeyInfo_float;
}

void free_VirtualAnalogKeyInfo_float(VirtualAnalogKeyInfo_float* to, bool freeself)
{
    free(to->analogSticksAndPad);

    if (freeself)
    {
        free(to);
    }
}


void copy_DynamicBitset(DynamicBitset* to, DynamicBitset* from)
{
    memcpy(to->inputKeys, from->inputKeys, sizeof(uint32_t) * 32);
}

DynamicBitset* init_DynamicBitset()
{
    DynamicBitset* local_DynamicBitset = (DynamicBitset*)malloc_(sizeof(DynamicBitset));

    local_DynamicBitset->inputKeys = (uint32_t*)malloc_(sizeof(uint32_t) * 32);

    return local_DynamicBitset;
}

void free_DynamicBitset(DynamicBitset* to, bool freeself)
{
    free(to->inputKeys);

    if (freeself)
    {
        free(to);
    }
}
