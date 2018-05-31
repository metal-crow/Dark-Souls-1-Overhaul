/*
    CONTRIBUTORS:
        Sean Pesce

    References:
        https://msdn.microsoft.com/en-us/library/windows/desktop/mt589889%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396
*/
#ifdef _WIN32

#ifndef SP_DX_D3D11_ID3D11DEVICE4_WRAPPER_CLASS_H_
#error Invalid header inclusion "dx/d3d11/interface/device/id3d11device4_impl.hpp" (Use "dx/d3d11/interface/device/id3d11device4.h" instead)
#endif

#include "sp/sp.h"


__SP_NAMESPACE
namespace dx    {
namespace d3d11 {



template <typename interface_t>
HRESULT STDMETHODCALLTYPE device4<interface_t>::RegisterDeviceRemovedEvent(_In_ HANDLE hEvent, _Out_ DWORD *pdwCookie)
{
    HRESULT result = this->_interface->RegisterDeviceRemovedEvent(hEvent, pdwCookie);

    return result;
}


template <typename interface_t>
void STDMETHODCALLTYPE device4<interface_t>::UnregisterDeviceRemoved(_In_ DWORD dwCookie)
{
    this->_interface->UnregisterDeviceRemoved(dwCookie);
}



} // namespace d3d11
} // namespace dx
__SP_NAMESPACE_CLOSE // namespace sp


#endif // _WIN32
