/*
    CONTRIBUTORS:
        Sean Pesce

    References:
        https://msdn.microsoft.com/en-us/library/windows/desktop/mt492481(v=vs.85).aspx
*/
#ifdef _WIN32

#ifndef SP_DX_D3D11_ID3D11DEVICE_CONTEXT4_WRAPPER_CLASS_H_
#error Invalid header inclusion "dx/d3d11/interface/context/id3d11devicecontext4_impl.hpp" (Use "dx/d3d11/interface/context/id3d11devicecontext4.h" instead)
#endif

#include "sp/sp.h"


__SP_NAMESPACE
namespace dx    {
namespace d3d11 {


template <typename interface_t>
HRESULT STDMETHODCALLTYPE context4<interface_t>::Signal(_In_ ID3D11Fence *pFence, _In_ UINT64 Value)
{
    HRESULT result = this->_interface->Signal(pFence, Value);

    return result;
}


template <typename interface_t>
HRESULT STDMETHODCALLTYPE context4<interface_t>::Wait(_In_ ID3D11Fence *pFence, _In_ UINT64 Value)
{
    HRESULT result = this->_interface->Wait(pFence, Value);

    return result;
}



} // namespace d3d11
} // namespace dx
__SP_NAMESPACE_CLOSE // namespace sp




#endif // _WIN32
