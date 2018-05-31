/*
    CONTRIBUTORS:
        Sean Pesce

    References:
        https://msdn.microsoft.com/en-us/library/windows/desktop/mt492481(v=vs.85).aspx
*/
#ifdef _WIN32
#pragma once

#ifndef SP_DX_D3D11_ID3D11DEVICE_CONTEXT4_WRAPPER_CLASS_H_
#define SP_DX_D3D11_ID3D11DEVICE_CONTEXT4_WRAPPER_CLASS_H_


#include "sp/dx/d3d11/interface/context/id3d11devicecontext3.h"
#include "sp/log.h"
#include "D3D11_3.h"



__SP_NAMESPACE
namespace dx {
namespace d3d11 {


template <class interface_t = ID3D11DeviceContext4>
class context4 : public context3 <interface_t> {
protected:

    virtual inline void _wrap(interface_t **original) override
    {
        this->_interface = *original;
        *original = this;
    }

public:
    context4 () {}

    context4 (interface_t **original)
    {
        if (original && *original)
        {
            SP_LOG_DBG("[D3D11] Device context created (UUID = %s)\n", sp::os::win::com::guid::to_string(__uuidof(**original)).c_str());
        }
        this->_wrap(original);
    }

    virtual ~context4 () {}


    // Wrapped interface methods


    virtual HRESULT STDMETHODCALLTYPE Signal(_In_ ID3D11Fence *pFence, _In_ UINT64 Value);


    virtual HRESULT STDMETHODCALLTYPE Wait(_In_ ID3D11Fence *pFence, _In_ UINT64 Value);


}; // class dx::d3d11::context4



} // namespace d3d11
} // namespace dx
__SP_NAMESPACE_CLOSE // namespace sp


template class sp::dx::d3d11::context4<ID3D11DeviceContext4>;


// Implementation
#include "sp/dx/d3d11/interface/context/id3d11devicecontext4_impl.hpp"


#endif // SP_DX_D3D11_ID3D11DEVICE_CONTEXT4_WRAPPER_CLASS_H_


#endif // _WIN32
