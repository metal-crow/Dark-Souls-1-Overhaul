/*
    CONTRIBUTORS:
        Sean Pesce

    References:
        https://msdn.microsoft.com/en-us/library/windows/desktop/dn280420(v=vs.85).aspx
*/
#ifdef _WIN32

#ifndef SP_DX_DXGI_IDXGI_SWAPCHAIN2_WRAPPER_CLASS_H_
#error Invalid header inclusion "dx/dxgi/interface/swapchain/idxgiswapchain2_impl.hpp" (Use "dx/dxgi/interface/swapchain/idxgiswapchain2.h" instead)
#endif

#include "sp/sp.h"


__SP_NAMESPACE
namespace dx   {
namespace dxgi {



template <typename interface_t>
HRESULT STDMETHODCALLTYPE swapchain2<interface_t>::SetSourceSize(UINT Width, UINT Height)
{
    HRESULT result = this->_interface->SetSourceSize(Width, Height);

    return result;
}

template <typename interface_t>
HRESULT STDMETHODCALLTYPE swapchain2<interface_t>::GetSourceSize(_Out_  UINT *pWidth, _Out_  UINT *pHeight)
{
    HRESULT result = this->_interface->GetSourceSize(pWidth, pHeight);

    return result;
}

template <typename interface_t>
HRESULT STDMETHODCALLTYPE swapchain2<interface_t>::SetMaximumFrameLatency(UINT MaxLatency)
{
    HRESULT result = this->_interface->SetMaximumFrameLatency(MaxLatency);

    return result;
}

template <typename interface_t>
HRESULT STDMETHODCALLTYPE swapchain2<interface_t>::GetMaximumFrameLatency(_Out_  UINT *pMaxLatency)
{
    HRESULT result = this->_interface->GetMaximumFrameLatency(pMaxLatency);

    return result;
}

template <typename interface_t>
HANDLE  STDMETHODCALLTYPE swapchain2<interface_t>::GetFrameLatencyWaitableObject()
{
    HANDLE result = this->_interface->GetFrameLatencyWaitableObject();

    return result;
}

template <typename interface_t>
HRESULT STDMETHODCALLTYPE swapchain2<interface_t>::SetMatrixTransform(const DXGI_MATRIX_3X2_F *pMatrix)
{
    HRESULT result = this->_interface->SetMatrixTransform(pMatrix);

    return result;
}

template <typename interface_t>
HRESULT STDMETHODCALLTYPE swapchain2<interface_t>::GetMatrixTransform(_Out_  DXGI_MATRIX_3X2_F *pMatrix)
{
    HRESULT result = this->_interface->GetMatrixTransform(pMatrix);

    return result;
}


} // namespace dxgi
} // namespace dx
__SP_NAMESPACE_CLOSE // namespace sp


#endif // _WIN32
