/*
    CONTRIBUTORS:
        Sean Pesce

    References:
        https://msdn.microsoft.com/en-us/library/windows/desktop/hh404631%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396
*/
#ifdef _WIN32

#ifndef SP_DX_DXGI_IDXGI_SWAPCHAIN1_WRAPPER_CLASS_H_
#error Invalid header inclusion "dx/dxgi/interface/swapchain/idxgiswapchain1_impl.hpp" (Use "dx/dxgi/interface/swapchain/idxgiswapchain1.h" instead)
#endif

#include "sp/sp.h"


__SP_NAMESPACE
namespace dx   {
namespace dxgi {


template <typename interface_t>
HRESULT STDMETHODCALLTYPE swapchain1<interface_t>::GetDesc1(_Out_  DXGI_SWAP_CHAIN_DESC1 *pDesc)
{
    HRESULT result = this->_interface->GetDesc1(pDesc);

    return result;
}


template <typename interface_t>
HRESULT STDMETHODCALLTYPE swapchain1<interface_t>::GetFullscreenDesc(_Out_  DXGI_SWAP_CHAIN_FULLSCREEN_DESC *pDesc)
{
    HRESULT result = this->_interface->GetFullscreenDesc(pDesc);

    return result;
}


template <typename interface_t>
HRESULT STDMETHODCALLTYPE swapchain1<interface_t>::GetHwnd(_Out_ HWND *pHwnd)
{
    HRESULT result = this->_interface->GetHwnd(pHwnd);

    return result;
}


template <typename interface_t>
HRESULT STDMETHODCALLTYPE swapchain1<interface_t>::GetCoreWindow(_In_  REFIID refiid, _COM_Outptr_  void **ppUnk)
{
    HRESULT result = this->_interface->GetCoreWindow(refiid, ppUnk);

    return result;
}


template <typename interface_t>
HRESULT STDMETHODCALLTYPE swapchain1<interface_t>::Present1(UINT SyncInterval, UINT PresentFlags, _In_  const DXGI_PRESENT_PARAMETERS *pPresentParameters)
{
    HRESULT result = this->_interface->Present1(SyncInterval, PresentFlags, pPresentParameters);

    return result;
}


template <typename interface_t>
BOOL STDMETHODCALLTYPE swapchain1<interface_t>::IsTemporaryMonoSupported()
{
    BOOL result = this->_interface->IsTemporaryMonoSupported();

    return result;
}


template <typename interface_t>
HRESULT STDMETHODCALLTYPE swapchain1<interface_t>::GetRestrictToOutput(_Out_  IDXGIOutput **ppRestrictToOutput)
{
    HRESULT result = this->_interface->GetRestrictToOutput(ppRestrictToOutput);

    return result;
}


template <typename interface_t>
HRESULT STDMETHODCALLTYPE swapchain1<interface_t>::SetBackgroundColor(_In_  const DXGI_RGBA *pColor)
{
    HRESULT result = this->_interface->SetBackgroundColor(pColor);

    return result;
}


template <typename interface_t>
HRESULT STDMETHODCALLTYPE swapchain1<interface_t>::GetBackgroundColor(_Out_  DXGI_RGBA *pColor)
{
    HRESULT result = this->_interface->GetBackgroundColor(pColor);

    return result;
}


template <typename interface_t>
HRESULT STDMETHODCALLTYPE swapchain1<interface_t>::SetRotation(_In_  DXGI_MODE_ROTATION Rotation)
{
    HRESULT result = this->_interface->SetRotation(Rotation);

    return result;
}


template <typename interface_t>
HRESULT STDMETHODCALLTYPE swapchain1<interface_t>::GetRotation(_Out_  DXGI_MODE_ROTATION *pRotation)
{
    HRESULT result = this->_interface->GetRotation(pRotation);

    return result;
}



} // namespace dxgi
} // namespace dx
__SP_NAMESPACE_CLOSE // namespace sp


#endif // _WIN32
