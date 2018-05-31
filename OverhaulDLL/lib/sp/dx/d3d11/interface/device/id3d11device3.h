/*
    CONTRIBUTORS:
        Sean Pesce

    References:
        https://msdn.microsoft.com/en-us/library/windows/desktop/dn899218(v=vs.85).aspx
*/
#ifdef _WIN32
#pragma once

#ifndef SP_DX_D3D11_ID3D11DEVICE3_WRAPPER_CLASS_H_
#define SP_DX_D3D11_ID3D11DEVICE3_WRAPPER_CLASS_H_

#include "sp/dx/d3d11/interface/device/id3d11device2.h"
#include "D3D11_3.h"


__SP_NAMESPACE
namespace dx    {
namespace d3d11 {


template <class interface_t = ID3D11Device3>
class device3 : public device2<interface_t> {
protected:

    virtual inline void _wrap(interface_t **original) override
    {
        this->_interface = *original;
        *original = this;
    }

public:
    device3() {}

    device3(interface_t **original)
    {
        if (original && *original)
        {
            SP_LOG_DBG("[D3D11] Device created (UUID = %s)\n", sp::os::win::com::guid::to_string(__uuidof(**original)).c_str());
        }
        this->_wrap(original);
    }

    virtual ~device3() {}


    // Wrapped interface methods



    virtual HRESULT STDMETHODCALLTYPE CreateTexture2D1(
        _In_  const D3D11_TEXTURE2D_DESC1 *pDesc1,
        _In_reads_opt_(_Inexpressible_(pDesc1->MipLevels * pDesc1->ArraySize))  const D3D11_SUBRESOURCE_DATA *pInitialData,
        _COM_Outptr_opt_  ID3D11Texture2D1 **ppTexture2D
    );



    virtual HRESULT STDMETHODCALLTYPE CreateTexture3D1(
        _In_  const D3D11_TEXTURE3D_DESC1 *pDesc1,
        _In_reads_opt_(_Inexpressible_(pDesc1->MipLevels))  const D3D11_SUBRESOURCE_DATA *pInitialData,
        _COM_Outptr_opt_  ID3D11Texture3D1 **ppTexture3D
    );



    virtual HRESULT STDMETHODCALLTYPE CreateRasterizerState2(
        _In_  const D3D11_RASTERIZER_DESC2 *pRasterizerDesc,
        _COM_Outptr_opt_  ID3D11RasterizerState2 **ppRasterizerState
    );



    virtual HRESULT STDMETHODCALLTYPE CreateShaderResourceView1(
        _In_  ID3D11Resource *pResource,
        _In_opt_  const D3D11_SHADER_RESOURCE_VIEW_DESC1 *pDesc1,
        _COM_Outptr_opt_  ID3D11ShaderResourceView1 **ppSRView1
    );



    virtual HRESULT STDMETHODCALLTYPE CreateUnorderedAccessView1(
        _In_  ID3D11Resource *pResource,
        _In_opt_  const D3D11_UNORDERED_ACCESS_VIEW_DESC1 *pDesc1,
        _COM_Outptr_opt_  ID3D11UnorderedAccessView1 **ppUAView1
    );



    virtual HRESULT STDMETHODCALLTYPE CreateRenderTargetView1(
        _In_  ID3D11Resource *pResource,
        _In_opt_  const D3D11_RENDER_TARGET_VIEW_DESC1 *pDesc1,
        _COM_Outptr_opt_  ID3D11RenderTargetView1 **ppRTView1
    );



    virtual HRESULT STDMETHODCALLTYPE CreateQuery1(
        _In_  const D3D11_QUERY_DESC1 *pQueryDesc1,
        _COM_Outptr_opt_  ID3D11Query1 **ppQuery1
    );



    virtual void STDMETHODCALLTYPE GetImmediateContext3(
        _Outptr_  ID3D11DeviceContext3 **ppImmediateContext
    );



    virtual HRESULT STDMETHODCALLTYPE CreateDeferredContext3(
        UINT ContextFlags,
        _COM_Outptr_opt_  ID3D11DeviceContext3 **ppDeferredContext
    );



    virtual void STDMETHODCALLTYPE WriteToSubresource(
        _In_  ID3D11Resource *pDstResource,
        _In_  UINT DstSubresource,
        _In_opt_  const D3D11_BOX *pDstBox,
        _In_  const void *pSrcData,
        _In_  UINT SrcRowPitch,
        _In_  UINT SrcDepthPitch
    );



    virtual void STDMETHODCALLTYPE ReadFromSubresource(
        _Out_  void *pDstData,
        _In_  UINT DstRowPitch,
        _In_  UINT DstDepthPitch,
        _In_  ID3D11Resource *pSrcResource,
        _In_  UINT SrcSubresource,
        _In_opt_  const D3D11_BOX *pSrcBox
    );
    


}; // class dx::d3d11::device3


} // namespace d3d11
} // namespace dx
__SP_NAMESPACE_CLOSE // namespace sp


template class sp::dx::d3d11::device3<ID3D11Device3>;


// Implementation
#include "sp/dx/d3d11/interface/device/id3d11device3_impl.hpp"


#endif // SP_DX_D3D11_ID3D11DEVICE3_WRAPPER_CLASS_H_


#endif // _WIN32
