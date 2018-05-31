/*
    CONTRIBUTORS:
        Sean Pesce

    Functions generated with a modified version of Lin "MavenLin" Min's DLL Wrapper Generator:
        https://github.com/mavenlin/Dll_Wrapper_Gen
*/
#ifdef _WIN32
#pragma once

#ifndef SP_DX_D3D9_HOOKS_H_
#define SP_DX_D3D9_HOOKS_H_

#include "sp/sp.h"
#include "sp/dx/d3d9/d3d9_main.h"


__SP_NAMESPACE
namespace dx   {
namespace d3d9 {

extern const char* const export_names[SP_D3D9_EXPORT_COUNT_];

void hook_exports();

} // namespace d3d9
} // namespace dx
__SP_NAMESPACE_CLOSE // namespace sp


#ifdef _WIN64
/*  Trampolines to original exported functions from d3d9.dll */
extern "C" void D3DPERF_BeginEvent_jmp();
extern "C" void D3DPERF_EndEvent_jmp();
extern "C" void D3DPERF_GetStatus_jmp();
extern "C" void D3DPERF_QueryRepeatFrame_jmp();
extern "C" void D3DPERF_SetMarker_jmp();
extern "C" void D3DPERF_SetOptions_jmp();
extern "C" void D3DPERF_SetRegion_jmp();
extern "C" void DebugSetLevel_jmp();
extern "C" void DebugSetMute_jmp();
extern "C" void Direct3D9EnableMaximizedWindowedModeShim_jmp();
extern "C" void Direct3DCreate9_jmp();
extern "C" void Direct3DCreate9Ex_jmp();
extern "C" void Direct3DShaderValidatorCreate9_jmp();
extern "C" void PSGPError_jmp();
extern "C" void PSGPSampleTexture_jmp();
#else // !_WIN64
extern "C" void __stdcall D3DPERF_BeginEvent_jmp();
extern "C" void __stdcall D3DPERF_EndEvent_jmp();
extern "C" void __stdcall D3DPERF_GetStatus_jmp();
extern "C" void __stdcall D3DPERF_QueryRepeatFrame_jmp();
extern "C" void __stdcall D3DPERF_SetMarker_jmp();
extern "C" void __stdcall D3DPERF_SetOptions_jmp();
extern "C" void __stdcall D3DPERF_SetRegion_jmp();
extern "C" void __stdcall DebugSetLevel_jmp();
extern "C" void __stdcall DebugSetMute_jmp();
extern "C" void __stdcall Direct3D9EnableMaximizedWindowedModeShim_jmp();
extern "C" void __stdcall Direct3DCreate9_jmp();
extern "C" void __stdcall Direct3DCreate9Ex_jmp();
extern "C" void __stdcall Direct3DShaderValidatorCreate9_jmp();
extern "C" void __stdcall PSGPError_jmp();
extern "C" void __stdcall PSGPSampleTexture_jmp();
#endif // _WIN64

#endif // SP_DX_D3D9_HOOKS_H_


#endif // _WIN32
