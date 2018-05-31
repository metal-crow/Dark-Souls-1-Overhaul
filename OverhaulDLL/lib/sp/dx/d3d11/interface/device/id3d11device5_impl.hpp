/*
    CONTRIBUTORS:
        Sean Pesce

    References:
        https://msdn.microsoft.com/en-us/library/windows/desktop/mt492478%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396
*/
#ifdef _WIN32

#ifndef SP_DX_D3D11_ID3D11DEVICE5_WRAPPER_CLASS_H_
#error Invalid header inclusion "dx/d3d11/interface/device/id3d11device5_impl.hpp" (Use "dx/d3d11/interface/device/id3d11device5.h" instead)
#endif

#include "sp/sp.h"


__SP_NAMESPACE
namespace dx    {
namespace d3d11 {



template <typename interface_t>
HRESULT STDMETHODCALLTYPE device5<interface_t>::OpenSharedFence(
    _In_  HANDLE hFence,
    _In_  REFIID ReturnedInterface,
    _COM_Outptr_opt_  void **ppFence
)
{
    HRESULT result = this->_interface->OpenSharedFence(hFence, ReturnedInterface, ppFence);

    return result;
}


template <typename interface_t>
HRESULT STDMETHODCALLTYPE device5<interface_t>::CreateFence(
    _In_  UINT64 InitialValue,
    _In_  D3D11_FENCE_FLAG Flags,
    _In_  REFIID ReturnedInterface,
    _COM_Outptr_opt_  void **ppFence
)
{
    HRESULT result = this->_interface->CreateFence(InitialValue, Flags, ReturnedInterface, ppFence);

    return result;
}



} // namespace d3d11
} // namespace dx
__SP_NAMESPACE_CLOSE // namespace sp


#endif // _WIN32
