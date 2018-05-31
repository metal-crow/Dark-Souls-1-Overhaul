/*
    CONTRIBUTORS:
        Sean Pesce

    References:
        https://msdn.microsoft.com/en-us/library/windows/desktop/mt492478%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396
*/
#ifdef _WIN32
#pragma once

#ifndef SP_DX_D3D11_ID3D11DEVICE5_WRAPPER_CLASS_H_
#define SP_DX_D3D11_ID3D11DEVICE5_WRAPPER_CLASS_H_

#include "sp/dx/d3d11/interface/device/id3d11device4.h"
#include "D3D11_4.h"


__SP_NAMESPACE
namespace dx    {
namespace d3d11 {


template <class interface_t = ID3D11Device5>
class device5 : public device4<interface_t> {
protected:

    virtual inline void _wrap(interface_t **original) override
    {
        this->_interface = *original;
        *original = this;
    }

public:
    device5() {}

    device5(interface_t **original)
    {
        if (original && *original)
        {
            SP_LOG_DBG("[D3D11] Device created (UUID = %s)\n", sp::os::win::com::guid::to_string(__uuidof(**original)).c_str());
        }
        this->_wrap(original);
    }

    virtual ~device5() {}


    // Wrapped interface methods


    virtual HRESULT STDMETHODCALLTYPE OpenSharedFence(
        _In_  HANDLE hFence,
        _In_  REFIID ReturnedInterface,
        _COM_Outptr_opt_  void **ppFence
    );


    virtual HRESULT STDMETHODCALLTYPE CreateFence(
        _In_  UINT64 InitialValue,
        _In_  D3D11_FENCE_FLAG Flags,
        _In_  REFIID ReturnedInterface,
        _COM_Outptr_opt_  void **ppFence
    );


}; // class dx::d3d11::device5


} // namespace d3d11
} // namespace dx
__SP_NAMESPACE_CLOSE // namespace sp


template class sp::dx::d3d11::device5<ID3D11Device5>;


// Implementation
#include "sp/dx/d3d11/interface/device/id3d11device5_impl.hpp"


#endif // SP_DX_D3D11_ID3D11DEVICE5_WRAPPER_CLASS_H_


#endif // _WIN32
