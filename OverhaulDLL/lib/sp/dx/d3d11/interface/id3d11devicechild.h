/*
    CONTRIBUTORS:
        Sean Pesce

    References:
        https://msdn.microsoft.com/en-us/library/windows/desktop/ff476380(v=vs.85).aspx
*/
#ifdef _WIN32
#pragma once

#ifndef SP_DX_D3D11_ID3D11DEVICE_CHILD_WRAPPER_CLASS_H_
#define SP_DX_D3D11_ID3D11DEVICE_CHILD_WRAPPER_CLASS_H_


#include "sp/os/windows/com/interface/iunknown.h"
#include "sp/log.h"
#include "D3D11.h"



__SP_NAMESPACE
namespace dx    {
namespace d3d11 {


template <class interface_t = ID3D11DeviceChild>
class device_child : public sp::os::win::com::iunknown<interface_t> {
protected:

    virtual inline void _wrap(interface_t **original) override
    {
        this->_interface = *original;
        *original = this;
    }

public:
    device_child() {}

    device_child(interface_t **original)
    {
        if (original && *original)
        {
            SP_LOG_DBG("[D3D11] Device child created (UUID = %s)\n", sp::os::win::com::guid::to_string(__uuidof(**original)).c_str());
        }
        this->_wrap(original);
    }

    virtual ~device_child() {}


    // Wrapped interface methods

    virtual void STDMETHODCALLTYPE GetDevice(_Outptr_  ID3D11Device **ppDevice);


    virtual HRESULT STDMETHODCALLTYPE GetPrivateData(
        _In_  REFGUID guid,
        _Inout_  UINT *pDataSize,
        _Out_writes_bytes_opt_(*pDataSize)  void *pData
    );


    virtual HRESULT STDMETHODCALLTYPE SetPrivateData(
        _In_  REFGUID guid,
        _In_  UINT DataSize,
        _In_reads_bytes_opt_(DataSize)  const void *pData
    );


    virtual HRESULT STDMETHODCALLTYPE SetPrivateDataInterface(_In_  REFGUID guid, _In_opt_  const IUnknown *pData);


}; // class dx::d3d11::device_child



} // namespace d3d11
} // namespace dx
__SP_NAMESPACE_CLOSE // namespace sp


template class sp::dx::d3d11::device_child<ID3D11DeviceChild>;


// Implementation
#include "sp/dx/d3d11/interface/id3d11devicechild_impl.hpp"


#endif // SP_DX_D3D11_ID3D11DEVICE_CHILD_WRAPPER_CLASS_H_


#endif // _WIN32
