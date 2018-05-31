/*
    CONTRIBUTORS:
        Sean Pesce

    Functions generated with a modified version of Lin "MavenLin" Min's DLL Wrapper Generator:
        https://github.com/mavenlin/Dll_Wrapper_Gen
*/
#ifdef _WIN32
#pragma once

#ifndef SP_DX_DXGI_HOOKS_H_
#define SP_DX_DXGI_HOOKS_H_

#include "sp/sp.h"
#include "sp/dx/dxgi/dxgi_main.h"


__SP_NAMESPACE
namespace dx   {
namespace dxgi {

extern const char* const export_names[SP_DXGI_EXPORT_COUNT_];

void hook_exports();

} // namespace dxgi
} // namespace dx
__SP_NAMESPACE_CLOSE // namespace sp


#ifdef _WIN64
/*  Trampolines to original exported functions from dxgi.dll */
extern "C" void ApplyCompatResolutionQuirking_jmp();
extern "C" void CompatString_jmp();
extern "C" void CompatValue_jmp();
extern "C" void CreateDXGIFactory_jmp();
extern "C" void CreateDXGIFactory1_jmp();
extern "C" void CreateDXGIFactory2_jmp();
extern "C" void DXGID3D10CreateDevice_jmp();
extern "C" void DXGID3D10CreateLayeredDevice_jmp();
extern "C" void DXGID3D10ETWRundown_jmp();
extern "C" void DXGID3D10GetLayeredDeviceSize_jmp();
extern "C" void DXGID3D10RegisterLayers_jmp();
extern "C" void DXGIDumpJournal_jmp();
extern "C" void DXGIGetDebugInterface1_jmp();
extern "C" void DXGIReportAdapterConfiguration_jmp();
extern "C" void DXGIRevertToSxS_jmp();
extern "C" void PIXBeginCapture_jmp();
extern "C" void PIXEndCapture_jmp();
extern "C" void PIXGetCaptureState_jmp();
extern "C" void SetAppCompatStringPointer_jmp();
extern "C" void UpdateHMDEmulationStatus_jmp();
#else // !_WIN64
extern "C" void __stdcall ApplyCompatResolutionQuirking_jmp();
extern "C" void __stdcall CompatString_jmp();
extern "C" void __stdcall CompatValue_jmp();
extern "C" void __stdcall CreateDXGIFactory_jmp();
extern "C" void __stdcall CreateDXGIFactory1_jmp();
extern "C" void __stdcall CreateDXGIFactory2_jmp();
extern "C" void __stdcall DXGID3D10CreateDevice_jmp();
extern "C" void __stdcall DXGID3D10CreateLayeredDevice_jmp();
extern "C" void __stdcall DXGID3D10ETWRundown_jmp();
extern "C" void __stdcall DXGID3D10GetLayeredDeviceSize_jmp();
extern "C" void __stdcall DXGID3D10RegisterLayers_jmp();
extern "C" void __stdcall DXGIDumpJournal_jmp();
extern "C" void __stdcall DXGIGetDebugInterface1_jmp();
extern "C" void __stdcall DXGIReportAdapterConfiguration_jmp();
extern "C" void __stdcall DXGIRevertToSxS_jmp();
extern "C" void __stdcall PIXBeginCapture_jmp();
extern "C" void __stdcall PIXEndCapture_jmp();
extern "C" void __stdcall PIXGetCaptureState_jmp();
extern "C" void __stdcall SetAppCompatStringPointer_jmp();
extern "C" void __stdcall UpdateHMDEmulationStatus_jmp();
#endif // _WIN64

#endif // SP_DX_DXGI_HOOKS_H_


#endif // _WIN32
