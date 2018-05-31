/*
    CONTRIBUTORS:
        Sean Pesce

    References:
        https://msdn.microsoft.com/en-us/library/windows/desktop/hh404575(v=vs.85).aspx
*/
#ifdef _WIN32
#pragma once

#ifndef SP_DX_D3D11_ID3D11DEVICE1_WRAPPER_CLASS_H_
#define SP_DX_D3D11_ID3D11DEVICE1_WRAPPER_CLASS_H_

#include "sp/dx/d3d11/interface/device/id3d11device.h"
#include "D3D11_1.h"


__SP_NAMESPACE
namespace dx    {
namespace d3d11 {


template <class interface_t = ID3D11Device1>
class device1 : public device<interface_t> {
protected:

    virtual inline void _wrap(interface_t **original) override
    {
        this->_interface = *original;
        *original = this;
    }

public:
    device1() {}

    device1(interface_t **original)
    {
        if (original && *original)
        {
            SP_LOG_DBG("[D3D11] Device created (UUID = %s)\n", sp::os::win::com::guid::to_string(__uuidof(**original)).c_str());
        }
        this->_wrap(original);
    }

    virtual ~device1() {}


    // Wrapped interface methods


    virtual void STDMETHODCALLTYPE GetImmediateContext1(_Out_  ID3D11DeviceContext1 **ppImmediateContext);



    virtual HRESULT STDMETHODCALLTYPE CreateDeferredContext1(UINT ContextFlags, _Out_opt_  ID3D11DeviceContext1 **ppDeferredContext);



    virtual HRESULT STDMETHODCALLTYPE CreateBlendState1(_In_  const D3D11_BLEND_DESC1 *pBlendStateDesc, _Out_opt_  ID3D11BlendState1 **ppBlendState);



    virtual HRESULT STDMETHODCALLTYPE CreateRasterizerState1(
                _In_  const D3D11_RASTERIZER_DESC1 *pRasterizerDesc,
                _Out_opt_  ID3D11RasterizerState1 **ppRasterizerState
    );



    virtual HRESULT STDMETHODCALLTYPE CreateDeviceContextState(
                UINT Flags,
                _In_reads_(FeatureLevels)  const D3D_FEATURE_LEVEL *pFeatureLevels,
                UINT FeatureLevels,
                UINT SDKVersion,
                REFIID EmulatedInterface,
                _Out_opt_  D3D_FEATURE_LEVEL *pChosenFeatureLevel,
                _Out_opt_  ID3DDeviceContextState **ppContextState
    );



    virtual HRESULT STDMETHODCALLTYPE OpenSharedResource1(
                _In_  HANDLE hResource,
                _In_  REFIID returnedInterface,
                _Out_  void **ppResource
    );



    virtual HRESULT STDMETHODCALLTYPE OpenSharedResourceByName(
                _In_  LPCWSTR lpName,
                _In_  DWORD dwDesiredAccess,
                _In_  REFIID returnedInterface,
                _Out_  void **ppResource
    );


}; // class dx::d3d11::device1


} // namespace d3d11
} // namespace dx
__SP_NAMESPACE_CLOSE // namespace sp


template class sp::dx::d3d11::device1<ID3D11Device1>;


// Implementation
#include "sp/dx/d3d11/interface/device/id3d11device1_impl.hpp"


#endif // SP_DX_D3D11_ID3D11DEVICE1_WRAPPER_CLASS_H_


#endif // _WIN32
