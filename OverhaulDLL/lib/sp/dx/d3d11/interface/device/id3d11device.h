/*
    CONTRIBUTORS:
        Sean Pesce

    References:
        https://msdn.microsoft.com/en-us/library/windows/desktop/ff476379(v=vs.85).aspx
*/
#ifdef _WIN32
#pragma once

#ifndef SP_DX_D3D11_ID3D11DEVICE_WRAPPER_CLASS_H_
#define SP_DX_D3D11_ID3D11DEVICE_WRAPPER_CLASS_H_


#include "sp/os/windows/com/interface/iunknown.h"
#include "D3D11.h"
#include "sp/os/windows/com/com_main.h"
#include "sp/log.h"



__SP_NAMESPACE
namespace dx    {
namespace d3d11 {


template <class interface_t = ID3D11Device>
class device : public sp::os::win::com::iunknown<interface_t> {
protected:

    virtual inline void _wrap(interface_t **original) override
    {
        this->_interface = *original;
        *original = this;
    }

public:
    device() {}

    device(interface_t **original)
    {
        if (original && *original)
        {
            SP_LOG_DBG("[D3D11] Device created (UUID = %s)\n", sp::os::win::com::guid::to_string(__uuidof(**original)).c_str());
        }
        this->_wrap(original);
    }

    virtual ~device() {}

    // Wrapped interface methods


    virtual HRESULT STDMETHODCALLTYPE CreateBuffer(
        _In_ const D3D11_BUFFER_DESC *pDesc,
        _In_opt_ const D3D11_SUBRESOURCE_DATA *pInitialData,
        _Out_opt_ ID3D11Buffer **ppBuffer
    );



    virtual HRESULT STDMETHODCALLTYPE CreateTexture1D(
        _In_ const D3D11_TEXTURE1D_DESC *pDesc,
        _In_reads_opt_(_Inexpressible_(pDesc->MipLevels * pDesc->ArraySize)) const D3D11_SUBRESOURCE_DATA *pInitialData,
        _Out_opt_ ID3D11Texture1D **ppTexture1D
    );



    virtual HRESULT STDMETHODCALLTYPE CreateTexture2D(
        _In_ const D3D11_TEXTURE2D_DESC *pDesc,
        _In_reads_opt_(_Inexpressible_(pDesc->MipLevels * pDesc->ArraySize)) const D3D11_SUBRESOURCE_DATA *pInitialData,
        _Out_opt_ ID3D11Texture2D **ppTexture2D
    );



    virtual HRESULT STDMETHODCALLTYPE CreateTexture3D(
        _In_ const D3D11_TEXTURE3D_DESC *pDesc,
        _In_reads_opt_(_Inexpressible_(pDesc->MipLevels)) const D3D11_SUBRESOURCE_DATA *pInitialData,
        _Out_opt_ ID3D11Texture3D **ppTexture3D
    );



    virtual HRESULT STDMETHODCALLTYPE CreateShaderResourceView(
        _In_ ID3D11Resource *pResource,
        _In_opt_ const D3D11_SHADER_RESOURCE_VIEW_DESC *pDesc,
        _Out_opt_ ID3D11ShaderResourceView **ppSRView
    );



    virtual HRESULT STDMETHODCALLTYPE CreateUnorderedAccessView(
        _In_ ID3D11Resource *pResource,
        _In_opt_ const D3D11_UNORDERED_ACCESS_VIEW_DESC *pDesc,
        _Out_opt_ ID3D11UnorderedAccessView **ppUAView
    );



    virtual HRESULT STDMETHODCALLTYPE CreateRenderTargetView(
        _In_ ID3D11Resource *pResource,
        _In_opt_ const D3D11_RENDER_TARGET_VIEW_DESC *pDesc,
        _Out_opt_ ID3D11RenderTargetView **ppRTView
    );



    virtual HRESULT STDMETHODCALLTYPE CreateDepthStencilView(
        _In_ ID3D11Resource *pResource,
        _In_opt_ const D3D11_DEPTH_STENCIL_VIEW_DESC *pDesc,
        _Out_opt_ ID3D11DepthStencilView **ppDepthStencilView
    );



    virtual HRESULT STDMETHODCALLTYPE CreateInputLayout(
        _In_reads_(NumElements) const D3D11_INPUT_ELEMENT_DESC *pInputElementDescs,
        _In_range_(0, D3D11_IA_VERTEX_INPUT_STRUCTURE_ELEMENT_COUNT) UINT NumElements,
        _In_reads_(BytecodeLength) const void *pShaderBytecodeWithInputSignature,
        _In_ SIZE_T BytecodeLength,
        _Out_opt_ ID3D11InputLayout **ppInputLayout
    );



    virtual HRESULT STDMETHODCALLTYPE CreateVertexShader(
        _In_reads_(BytecodeLength) const void *pShaderBytecode,
        _In_ SIZE_T BytecodeLength,
        _In_opt_ ID3D11ClassLinkage *pClassLinkage,
        _Out_opt_ ID3D11VertexShader **ppVertexShader
    );



    virtual HRESULT STDMETHODCALLTYPE CreateGeometryShader(
        _In_reads_(BytecodeLength) const void *pShaderBytecode,
        _In_ SIZE_T BytecodeLength,
        _In_opt_ ID3D11ClassLinkage *pClassLinkage,
        _Out_opt_ ID3D11GeometryShader **ppGeometryShader
    );



    virtual HRESULT STDMETHODCALLTYPE CreateGeometryShaderWithStreamOutput(
        _In_reads_(BytecodeLength) const void *pShaderBytecode,
        _In_ SIZE_T BytecodeLength,
        _In_reads_opt_(NumEntries) const D3D11_SO_DECLARATION_ENTRY *pSODeclaration,
        _In_range_(0, D3D11_SO_STREAM_COUNT * D3D11_SO_OUTPUT_COMPONENT_COUNT) UINT NumEntries,
        _In_reads_opt_(NumStrides) const UINT *pBufferStrides,
        _In_range_(0, D3D11_SO_BUFFER_SLOT_COUNT) UINT NumStrides,
        _In_ UINT RasterizedStream, _In_opt_ ID3D11ClassLinkage *pClassLinkage,
        _Out_opt_ ID3D11GeometryShader **ppGeometryShader
    );



    virtual HRESULT STDMETHODCALLTYPE CreatePixelShader(
        _In_reads_(BytecodeLength) const void *pShaderBytecode,
        _In_ SIZE_T BytecodeLength,
        _In_opt_ ID3D11ClassLinkage *pClassLinkage,
        _Out_opt_ ID3D11PixelShader **ppPixelShader
    );



    virtual HRESULT STDMETHODCALLTYPE CreateHullShader(
        _In_reads_(BytecodeLength) const void *pShaderBytecode,
        _In_ SIZE_T BytecodeLength,
        _In_opt_ ID3D11ClassLinkage *pClassLinkage,
        _Out_opt_ ID3D11HullShader **ppHullShader
    );



    virtual HRESULT STDMETHODCALLTYPE CreateDomainShader(
        _In_reads_(BytecodeLength) const void *pShaderBytecode,
        _In_ SIZE_T BytecodeLength,
        _In_opt_ ID3D11ClassLinkage *pClassLinkage,
        _Out_opt_ ID3D11DomainShader **ppDomainShader
    );



    virtual HRESULT STDMETHODCALLTYPE CreateComputeShader(
        _In_reads_(BytecodeLength) const void *pShaderBytecode,
        _In_ SIZE_T BytecodeLength,
        _In_opt_ ID3D11ClassLinkage *pClassLinkage,
        _Out_opt_ ID3D11ComputeShader **ppComputeShader
    );



    virtual HRESULT STDMETHODCALLTYPE CreateClassLinkage(_Out_ ID3D11ClassLinkage **ppLinkage);



    virtual HRESULT STDMETHODCALLTYPE CreateBlendState(
        _In_ const D3D11_BLEND_DESC *pBlendStateDesc,
        _Out_opt_ ID3D11BlendState **ppBlendState
    );



    virtual HRESULT STDMETHODCALLTYPE CreateDepthStencilState(
        _In_ const D3D11_DEPTH_STENCIL_DESC *pDepthStencilDesc,
        _Out_opt_ ID3D11DepthStencilState **ppDepthStencilState
    );



    virtual HRESULT STDMETHODCALLTYPE CreateRasterizerState(
        _In_ const D3D11_RASTERIZER_DESC *pRasterizerDesc,
        _Out_opt_ ID3D11RasterizerState **ppRasterizerState
    );



    virtual HRESULT STDMETHODCALLTYPE CreateSamplerState(_In_ const D3D11_SAMPLER_DESC *pSamplerDesc, _Out_opt_ ID3D11SamplerState **ppSamplerState);



    virtual HRESULT STDMETHODCALLTYPE CreateQuery(_In_ const D3D11_QUERY_DESC *pQueryDesc, _Out_opt_ ID3D11Query **ppQuery);



    virtual HRESULT STDMETHODCALLTYPE CreatePredicate(_In_ const D3D11_QUERY_DESC *pPredicateDesc, _Out_opt_ ID3D11Predicate **ppPredicate);



    virtual HRESULT STDMETHODCALLTYPE CreateCounter(_In_ const D3D11_COUNTER_DESC *pCounterDesc, _Out_opt_ ID3D11Counter **ppCounter);



    virtual HRESULT STDMETHODCALLTYPE CreateDeferredContext(UINT ContextFlags, _Out_opt_ ID3D11DeviceContext **ppDeferredContext);



    virtual HRESULT STDMETHODCALLTYPE OpenSharedResource(_In_ HANDLE hResource, _In_ REFIID ReturnedInterface, _Out_opt_ void **ppResource);



    virtual HRESULT STDMETHODCALLTYPE CheckFormatSupport(_In_ DXGI_FORMAT Format, _Out_ UINT *pFormatSupport);



    virtual HRESULT STDMETHODCALLTYPE CheckMultisampleQualityLevels(_In_ DXGI_FORMAT Format, _In_ UINT SampleCount, _Out_ UINT *pNumQualityLevels);



    virtual void STDMETHODCALLTYPE CheckCounterInfo(_Out_ D3D11_COUNTER_INFO *pCounterInfo);



    virtual HRESULT STDMETHODCALLTYPE CheckCounter(
        _In_ const D3D11_COUNTER_DESC *pDesc,
        _Out_ D3D11_COUNTER_TYPE *pType,
        _Out_ UINT *pActiveCounters,
        _Out_writes_opt_(*pNameLength) LPSTR szName,
        _Inout_opt_ UINT *pNameLength,
        _Out_writes_opt_(*pUnitsLength) LPSTR szUnits,
        _Inout_opt_ UINT *pUnitsLength,
        _Out_writes_opt_(*pDescriptionLength)
        LPSTR szDescription,
        _Inout_opt_ UINT *pDescriptionLength
    );



    virtual HRESULT STDMETHODCALLTYPE CheckFeatureSupport(
        D3D11_FEATURE Feature,
        _Out_writes_bytes_(FeatureSupportDataSize)
        void *pFeatureSupportData,
        UINT FeatureSupportDataSize
    );



    virtual HRESULT STDMETHODCALLTYPE GetPrivateData(_In_ REFGUID guid, _Inout_ UINT *pDataSize, _Out_writes_bytes_opt_(*pDataSize) void *pData);



    virtual HRESULT STDMETHODCALLTYPE SetPrivateData(_In_ REFGUID guid, _In_ UINT DataSize, _In_reads_bytes_opt_(DataSize) const void *pData);



    virtual HRESULT STDMETHODCALLTYPE SetPrivateDataInterface(_In_ REFGUID guid, _In_opt_ const IUnknown *pData);



    virtual D3D_FEATURE_LEVEL STDMETHODCALLTYPE GetFeatureLevel();



    virtual UINT STDMETHODCALLTYPE GetCreationFlags();



    virtual HRESULT STDMETHODCALLTYPE GetDeviceRemovedReason();



    virtual void STDMETHODCALLTYPE GetImmediateContext(_Out_ ID3D11DeviceContext **ppImmediateContext);



    HRESULT STDMETHODCALLTYPE SetExceptionMode(UINT RaiseFlags);



    virtual UINT STDMETHODCALLTYPE GetExceptionMode();


}; // class dx::d3d11::device

typedef device<ID3D11Device> device0;


} // namespace d3d11
} // namespace dx
__SP_NAMESPACE_CLOSE // namespace sp


template class sp::dx::d3d11::device<ID3D11Device>;


// Implementation
#include "sp/dx/d3d11/interface/device/id3d11device_impl.hpp"


#endif // SP_DX_D3D11_ID3D11DEVICE_WRAPPER_CLASS_H_


#endif // _WIN32
