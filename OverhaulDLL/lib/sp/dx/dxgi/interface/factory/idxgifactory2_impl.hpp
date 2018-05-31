/*
    CONTRIBUTORS:
        Sean Pesce

    References:
        https://msdn.microsoft.com/en-us/library/windows/desktop/hh404556(v=vs.85).aspx
*/
#ifdef _WIN32

#ifndef SP_DX_DXGI_IDXGIFACTORY2_WRAPPER_CLASS_H_
#error Invalid header inclusion "dx/dxgi/interface/factory/idxgifactory2_impl.hpp" (Use "dx/dxgi/interface/factory/idxgifactory2.h" instead)
#endif

#include "sp/sp.h"


__SP_NAMESPACE
namespace dx   {
namespace dxgi {


template <typename interface_t>
HRESULT factory2<interface_t>::CreateSwapChainForComposition(IUnknown *pDevice, const DXGI_SWAP_CHAIN_DESC1 *pDesc, IDXGIOutput *pRestrictToOutput, IDXGISwapChain1 **ppSwapChain)
{
    return this->_interface->CreateSwapChainForComposition(pDevice, pDesc, pRestrictToOutput, ppSwapChain);
}


template <typename interface_t>
HRESULT factory2<interface_t>::CreateSwapChainForCoreWindow(IUnknown *pDevice, IUnknown *pWindow, const DXGI_SWAP_CHAIN_DESC1 *pDesc, IDXGIOutput *pRestrictToOutput, IDXGISwapChain1 **ppSwapChain)
{
    return this->_interface->CreateSwapChainForCoreWindow(pDevice, pWindow, pDesc, pRestrictToOutput, ppSwapChain);
}


template <typename interface_t>
HRESULT factory2<interface_t>::CreateSwapChainForHwnd(IUnknown *pDevice, HWND hWnd, const DXGI_SWAP_CHAIN_DESC1 *pDesc, const DXGI_SWAP_CHAIN_FULLSCREEN_DESC *pFullscreenDesc, IDXGIOutput *pRestrictToOutput, IDXGISwapChain1 **ppSwapChain)
{
    return this->_interface->CreateSwapChainForHwnd(pDevice, hWnd, pDesc, pFullscreenDesc, pRestrictToOutput, ppSwapChain);
}


template <typename interface_t>
HRESULT factory2<interface_t>::GetSharedResourceAdapterLuid(HANDLE hResource, LUID *pLuid)
{
    return this->_interface->GetSharedResourceAdapterLuid(hResource, pLuid);
}


template <typename interface_t>
BOOL factory2<interface_t>::IsWindowedStereoEnabled()
{
    return this->_interface->IsWindowedStereoEnabled();
}


template <typename interface_t>
HRESULT factory2<interface_t>::RegisterOcclusionStatusEvent(HANDLE hEvent, DWORD *pdwCookie)
{
    return this->_interface->RegisterOcclusionStatusEvent(hEvent, pdwCookie);
}


template <typename interface_t>
HRESULT factory2<interface_t>::RegisterOcclusionStatusWindow(HWND WindowHandle, UINT wMsg, DWORD *pdwCookie)
{
    return this->_interface->RegisterOcclusionStatusWindow(WindowHandle, wMsg, pdwCookie);
}


template <typename interface_t>
HRESULT factory2<interface_t>::RegisterStereoStatusEvent(HANDLE hEvent, DWORD *pdwCookie)
{
    return this->_interface->RegisterStereoStatusEvent(hEvent, pdwCookie);
}


template <typename interface_t>
HRESULT factory2<interface_t>::RegisterStereoStatusWindow(HWND WindowHandle, UINT wMsg, DWORD *pdwCookie)
{
    return this->_interface->RegisterStereoStatusWindow(WindowHandle, wMsg, pdwCookie);
}


template <typename interface_t>
void factory2<interface_t>::UnregisterOcclusionStatus(DWORD dwCookie)
{
    return this->_interface->UnregisterOcclusionStatus(dwCookie);
}


template <typename interface_t>
void factory2<interface_t>::UnregisterStereoStatus(DWORD dwCookie)
{
    return this->_interface->UnregisterStereoStatus(dwCookie);
}




} // namespace dxgi
} // namespace dx
__SP_NAMESPACE_CLOSE // namespace sp


#endif // _WIN32
