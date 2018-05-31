/*
    CONTRIBUTORS:
        Sean Pesce

    References:
        https://msdn.microsoft.com/en-us/library/windows/desktop/dn903673(v=vs.85).aspx
*/
#ifdef _WIN32

#ifndef SP_DX_DXGI_IDXGI_SWAPCHAIN3_WRAPPER_CLASS_H_
#error Invalid header inclusion "dx/dxgi/interface/swapchain/idxgiswapchain3_impl.hpp" (Use "dx/dxgi/interface/swapchain/idxgiswapchain3.h" instead)
#endif

#include "sp/sp.h"


__SP_NAMESPACE
namespace dx   {
namespace dxgi {


template <typename interface_t>
UINT STDMETHODCALLTYPE swapchain3<interface_t>::GetCurrentBackBufferIndex()
{
    UINT result = this->_interface->GetCurrentBackBufferIndex();

    return result;
}

template <typename interface_t>
HRESULT STDMETHODCALLTYPE swapchain3<interface_t>::CheckColorSpaceSupport(_In_  DXGI_COLOR_SPACE_TYPE ColorSpace, _Out_  UINT *pColorSpaceSupport)
{
    HRESULT result = this->_interface->CheckColorSpaceSupport(ColorSpace, pColorSpaceSupport);

    return result;
}

template <typename interface_t>
HRESULT STDMETHODCALLTYPE swapchain3<interface_t>::SetColorSpace1(_In_  DXGI_COLOR_SPACE_TYPE ColorSpace)
{
    HRESULT result = this->_interface->SetColorSpace1(ColorSpace);

    return result;
}

template <typename interface_t>
HRESULT STDMETHODCALLTYPE swapchain3<interface_t>::ResizeBuffers1(_In_  UINT BufferCount, _In_  UINT Width, _In_  UINT Height, _In_  DXGI_FORMAT Format, _In_  UINT SwapChainFlags, _In_reads_(BufferCount)  const UINT *pCreationNodeMask, _In_reads_(BufferCount)  IUnknown *const *ppPresentQueue)
{
    HRESULT result = this->_interface->ResizeBuffers1(BufferCount, Width, Height, Format, SwapChainFlags, pCreationNodeMask, ppPresentQueue);

    return result;
}



} // namespace dxgi
} // namespace dx
__SP_NAMESPACE_CLOSE // namespace sp


#endif // _WIN32
