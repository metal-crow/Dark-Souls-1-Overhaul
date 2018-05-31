/*
    CONTRIBUTORS:
        Sean Pesce

    References:
        https://msdn.microsoft.com/en-us/library/windows/desktop/mt589889%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396
*/
#ifdef _WIN32
#pragma once

#ifndef SP_DX_D3D11_ID3D11DEVICE4_WRAPPER_CLASS_H_
#define SP_DX_D3D11_ID3D11DEVICE4_WRAPPER_CLASS_H_

#include "sp/dx/d3d11/interface/device/id3d11device3.h"
#include "D3D11_4.h"


__SP_NAMESPACE
namespace dx    {
namespace d3d11 {


template <class interface_t = ID3D11Device4>
class device4 : public device3<interface_t> {
protected:

    virtual inline void _wrap(interface_t **original) override
    {
        this->_interface = *original;
        *original = this;
    }

public:
    device4() {}

    device4(interface_t **original)
    {
        if (original && *original)
        {
            SP_LOG_DBG("[D3D11] Device created (UUID = %s)\n", sp::os::win::com::guid::to_string(__uuidof(**original)).c_str());
        }
        this->_wrap(original);
    }

    virtual ~device4() {}


    // Wrapped interface methods


    virtual HRESULT STDMETHODCALLTYPE RegisterDeviceRemovedEvent(_In_ HANDLE hEvent, _Out_ DWORD *pdwCookie);


    virtual void STDMETHODCALLTYPE UnregisterDeviceRemoved(_In_ DWORD dwCookie);


}; // class dx::d3d11::device4


} // namespace d3d11
} // namespace dx
__SP_NAMESPACE_CLOSE // namespace sp


template class sp::dx::d3d11::device4<ID3D11Device4>;


// Implementation
#include "sp/dx/d3d11/interface/device/id3d11device4_impl.hpp"


#endif // SP_DX_D3D11_ID3D11DEVICE4_WRAPPER_CLASS_H_


#endif // _WIN32
