/*
    CONTRIBUTORS:
        Sean Pesce

    References:
        https://msdn.microsoft.com/en-us/library/windows/desktop/bb174569(v=vs.85).aspx
*/
#ifdef _WIN32

#ifndef SP_DX_DXGI_IDXGI_SWAPCHAIN_WRAPPER_CLASS_H_
#error Invalid header inclusion "dx/dxgi/interface/swapchain/idxgiswapchain_impl.hpp" (Use "dx/dxgi/interface/swapchain/idxgiswapchain.h" instead)
#endif

#include "sp/sp.h"


__SP_NAMESPACE
namespace dx   {
namespace dxgi {



template <typename interface_t>
HRESULT STDMETHODCALLTYPE swapchain<interface_t>::Present(UINT SyncInterval, UINT Flags)
{
    HRESULT result = this->_interface->Present(SyncInterval, Flags);

    return result;
}

template <typename interface_t>
HRESULT STDMETHODCALLTYPE swapchain<interface_t>::GetBuffer(UINT Buffer, _In_  REFIID riid, _COM_Outptr_  void **ppSurface)
{
    HRESULT result = this->_interface->GetBuffer(Buffer, riid, ppSurface);

    return result;
}

template <typename interface_t>
HRESULT STDMETHODCALLTYPE swapchain<interface_t>::SetFullscreenState(BOOL Fullscreen, _In_opt_  IDXGIOutput *pTarget)
{
    HRESULT result = this->_interface->SetFullscreenState(Fullscreen, pTarget);

    return result;
}

template <typename interface_t>
HRESULT STDMETHODCALLTYPE swapchain<interface_t>::GetFullscreenState(_Out_opt_  BOOL *pFullscreen, _COM_Outptr_opt_result_maybenull_  IDXGIOutput **ppTarget)
{
    HRESULT result = this->_interface->GetFullscreenState(pFullscreen, ppTarget);

    return result;
}

template <typename interface_t>
HRESULT STDMETHODCALLTYPE swapchain<interface_t>::GetDesc(_Out_  DXGI_SWAP_CHAIN_DESC *pDesc)
{
    HRESULT result = this->_interface->GetDesc(pDesc);

    return result;
}

template <typename interface_t>
HRESULT STDMETHODCALLTYPE swapchain<interface_t>::ResizeBuffers(UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags)
{
    HRESULT result = this->_interface->ResizeBuffers(BufferCount, Width, Height, NewFormat, SwapChainFlags);

    return result;
}

template <typename interface_t>
HRESULT STDMETHODCALLTYPE swapchain<interface_t>::ResizeTarget(_In_  const DXGI_MODE_DESC *pNewTargetParameters)
{
    HRESULT result = this->_interface->ResizeTarget(pNewTargetParameters);

    return result;
}

template <typename interface_t>
HRESULT STDMETHODCALLTYPE swapchain<interface_t>::GetContainingOutput(_COM_Outptr_  IDXGIOutput **ppOutput)
{
    HRESULT result = this->_interface->GetContainingOutput(ppOutput);

    return result;
}

template <typename interface_t>
HRESULT STDMETHODCALLTYPE swapchain<interface_t>::GetFrameStatistics(_Out_  DXGI_FRAME_STATISTICS *pStats)
{
    HRESULT result = this->_interface->GetFrameStatistics(pStats);

    return result;
}

template <typename interface_t>
HRESULT STDMETHODCALLTYPE swapchain<interface_t>::GetLastPresentCount(_Out_  UINT *pLastPresentCount)
{
    HRESULT result = this->_interface->GetLastPresentCount(pLastPresentCount);

    return result;
}


} // namespace dxgi
} // namespace dx
__SP_NAMESPACE_CLOSE // namespace sp


#endif // _WIN32
