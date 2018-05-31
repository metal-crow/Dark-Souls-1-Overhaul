/*
    CONTRIBUTORS:
        Sean Pesce

    References:
        https://msdn.microsoft.com/en-us/library/windows/desktop/dn912875(v=vs.85).aspx
*/
#ifdef _WIN32
#pragma once

#ifndef SP_DX_D3D11_ID3D11DEVICE_CONTEXT3_WRAPPER_CLASS_H_
#define SP_DX_D3D11_ID3D11DEVICE_CONTEXT3_WRAPPER_CLASS_H_


#include "sp/dx/d3d11/interface/context/id3d11devicecontext2.h"
#include "sp/log.h"
#include "D3D11_3.h"



__SP_NAMESPACE
namespace dx {
namespace d3d11 {


template <class interface_t = ID3D11DeviceContext3>
class context3 : public context2 <interface_t> {
protected:

    virtual inline void _wrap(interface_t **original) override
    {
        this->_interface = *original;
        *original = this;
    }

public:
    context3 () {}

    context3 (interface_t **original)
    {
        if (original && *original)
        {
            SP_LOG_DBG("[D3D11] Device context created (UUID = %s)\n", sp::os::win::com::guid::to_string(__uuidof(**original)).c_str());
        }
        this->_wrap(original);
    }

    virtual ~context3 () {}


    // Wrapped interface methods

    virtual void STDMETHODCALLTYPE Flush1(D3D11_CONTEXT_TYPE ContextType, _In_opt_ HANDLE hEvent);

    virtual void STDMETHODCALLTYPE SetHardwareProtectionState(_In_ BOOL HwProtectionEnable);

    virtual void STDMETHODCALLTYPE GetHardwareProtectionState(_Out_ BOOL *pHwProtectionEnable);


}; // class dx::d3d11::context3



} // namespace d3d11
} // namespace dx
__SP_NAMESPACE_CLOSE // namespace sp


template class sp::dx::d3d11::context3<ID3D11DeviceContext3>;


// Implementation
#include "sp/dx/d3d11/interface/context/id3d11devicecontext3_impl.hpp"


#endif // SP_DX_D3D11_ID3D11DEVICE_CONTEXT3_WRAPPER_CLASS_H_


#endif // _WIN32
