/*
    CONTRIBUTORS:
        Sean Pesce

    References:
        https://msdn.microsoft.com/en-us/library/windows/desktop/ff471331%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396
*/
#ifdef _WIN32

#ifndef SP_DX_DXGI_IDXGI_DEVICE1_WRAPPER_CLASS_H_
#error Invalid header inclusion "dx/dxgi/interface/device/idxgidevice1_impl.hpp" (Use "dx/dxgi/interface/device/idxgidevice1.h" instead)
#endif

#include "sp/sp.h"


__SP_NAMESPACE
namespace dx   {
namespace dxgi {


template <typename interface_t>
HRESULT STDMETHODCALLTYPE device1<interface_t>::SetMaximumFrameLatency(/* [in] */ UINT MaxLatency)
{
    HRESULT result = this->_interface->SetMaximumFrameLatency(MaxLatency);

    return result;
}

template <typename interface_t>
HRESULT STDMETHODCALLTYPE device1<interface_t>::GetMaximumFrameLatency(_Out_  UINT *pMaxLatency)
{
    HRESULT result = this->_interface->GetMaximumFrameLatency(pMaxLatency);

    return result;
}




} // namespace dxgi
} // namespace dx
__SP_NAMESPACE_CLOSE // namespace sp


#endif // _WIN32
