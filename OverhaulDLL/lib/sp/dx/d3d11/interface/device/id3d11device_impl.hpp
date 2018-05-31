/*
    CONTRIBUTORS:
        Sean Pesce

    References:
        https://msdn.microsoft.com/en-us/library/windows/desktop/ff476379(v=vs.85).aspx
*/
#ifdef _WIN32

#ifndef SP_DX_D3D11_ID3D11DEVICE_WRAPPER_CLASS_H_
#error Invalid header inclusion "dx/d3d11/interface/device/id3d11device_impl.hpp" (Use "dx/d3d11/interface/device/id3d11device.h" instead)
#endif

#include "sp/sp.h"


__SP_NAMESPACE
namespace dx    {
namespace d3d11 {



template <typename interface_t>
HRESULT STDMETHODCALLTYPE device<interface_t>::CreateBuffer(
            _In_ const D3D11_BUFFER_DESC *pDesc,
            _In_opt_ const D3D11_SUBRESOURCE_DATA *pInitialData,
            _Out_opt_ ID3D11Buffer **ppBuffer )
{
    HRESULT result = this->_interface->CreateBuffer(pDesc, pInitialData, ppBuffer);
    
    return result;
}



template <typename interface_t>
HRESULT STDMETHODCALLTYPE device<interface_t>::CreateTexture1D(
            _In_ const D3D11_TEXTURE1D_DESC *pDesc,
            _In_reads_opt_(_Inexpressible_(pDesc->MipLevels * pDesc->ArraySize)) const D3D11_SUBRESOURCE_DATA *pInitialData,
            _Out_opt_ ID3D11Texture1D **ppTexture1D )
{
    HRESULT result = this->_interface->CreateTexture1D( pDesc, pInitialData, ppTexture1D );
    
    return result;
}



template <typename interface_t>
HRESULT STDMETHODCALLTYPE device<interface_t>::CreateTexture2D(
            _In_ const D3D11_TEXTURE2D_DESC *pDesc,
            _In_reads_opt_(_Inexpressible_(pDesc->MipLevels * pDesc->ArraySize)) const D3D11_SUBRESOURCE_DATA *pInitialData,
            _Out_opt_ ID3D11Texture2D **ppTexture2D )
{
    HRESULT result = this->_interface->CreateTexture2D( pDesc, pInitialData, ppTexture2D );
    
    return result;
}



template <typename interface_t>
HRESULT STDMETHODCALLTYPE device<interface_t>::CreateTexture3D(
            _In_ const D3D11_TEXTURE3D_DESC *pDesc,
            _In_reads_opt_(_Inexpressible_(pDesc->MipLevels)) const D3D11_SUBRESOURCE_DATA *pInitialData,
            _Out_opt_ ID3D11Texture3D **ppTexture3D )
{
    HRESULT result = this->_interface->CreateTexture3D(pDesc, pInitialData, ppTexture3D );
    
    return result;
}



template <typename interface_t>
HRESULT STDMETHODCALLTYPE device<interface_t>::CreateShaderResourceView(
            _In_ ID3D11Resource *pResource,
            _In_opt_ const D3D11_SHADER_RESOURCE_VIEW_DESC *pDesc,
            _Out_opt_ ID3D11ShaderResourceView **ppSRView )
{
    HRESULT result = this->_interface->CreateShaderResourceView(pResource, pDesc, ppSRView );
    
    return result;
}



template <typename interface_t>
HRESULT STDMETHODCALLTYPE device<interface_t>::CreateUnorderedAccessView(
            _In_ ID3D11Resource *pResource,
            _In_opt_ const D3D11_UNORDERED_ACCESS_VIEW_DESC *pDesc,
            _Out_opt_ ID3D11UnorderedAccessView **ppUAView )
{
    HRESULT result = this->_interface->CreateUnorderedAccessView(pResource, pDesc, ppUAView );
    
    return result;
}



template <typename interface_t>
HRESULT STDMETHODCALLTYPE device<interface_t>::CreateRenderTargetView(
            _In_ ID3D11Resource *pResource,
            _In_opt_ const D3D11_RENDER_TARGET_VIEW_DESC *pDesc,
            _Out_opt_ ID3D11RenderTargetView **ppRTView )
{
    HRESULT result = this->_interface->CreateRenderTargetView(pResource, pDesc, ppRTView );
    
    return result;
}



template <typename interface_t>
HRESULT STDMETHODCALLTYPE device<interface_t>::CreateDepthStencilView(
            _In_ ID3D11Resource *pResource,
            _In_opt_ const D3D11_DEPTH_STENCIL_VIEW_DESC *pDesc,
            _Out_opt_ ID3D11DepthStencilView **ppDepthStencilView )
{
    HRESULT result = this->_interface->CreateDepthStencilView(pResource, pDesc, ppDepthStencilView );
    
    return result;
}



template <typename interface_t>
HRESULT STDMETHODCALLTYPE device<interface_t>::CreateInputLayout(
            _In_reads_(NumElements) const D3D11_INPUT_ELEMENT_DESC *pInputElementDescs,
            _In_range_(0, D3D11_IA_VERTEX_INPUT_STRUCTURE_ELEMENT_COUNT) UINT NumElements,
            _In_reads_(BytecodeLength) const void *pShaderBytecodeWithInputSignature,
            _In_ SIZE_T BytecodeLength,
            _Out_opt_ ID3D11InputLayout **ppInputLayout )
{
    HRESULT result = this->_interface->CreateInputLayout(pInputElementDescs,
                                                    NumElements,
                                                    pShaderBytecodeWithInputSignature,
                                                    BytecodeLength,
                                                    ppInputLayout );
    
    return result;
}



template <typename interface_t>
HRESULT STDMETHODCALLTYPE device<interface_t>::CreateVertexShader(
            _In_reads_(BytecodeLength) const void *pShaderBytecode,
            _In_ SIZE_T BytecodeLength,
            _In_opt_ ID3D11ClassLinkage *pClassLinkage,
            _Out_opt_ ID3D11VertexShader **ppVertexShader )
{
    HRESULT result = this->_interface->CreateVertexShader(pShaderBytecode,BytecodeLength,pClassLinkage,ppVertexShader );
    
    return result;
}



template <typename interface_t>
HRESULT STDMETHODCALLTYPE device<interface_t>::CreateGeometryShader(
            _In_reads_(BytecodeLength) const void *pShaderBytecode,
            _In_ SIZE_T BytecodeLength,
            _In_opt_ ID3D11ClassLinkage *pClassLinkage,
            _Out_opt_ ID3D11GeometryShader **ppGeometryShader )
{
    HRESULT result = this->_interface->CreateGeometryShader(pShaderBytecode,BytecodeLength,pClassLinkage,ppGeometryShader );
    
    return result;
}



template <typename interface_t>
HRESULT STDMETHODCALLTYPE device<interface_t>::CreateGeometryShaderWithStreamOutput(
            _In_reads_(BytecodeLength) const void *pShaderBytecode,
            _In_ SIZE_T BytecodeLength,
            _In_reads_opt_(NumEntries) const D3D11_SO_DECLARATION_ENTRY *pSODeclaration,
            _In_range_(0, D3D11_SO_STREAM_COUNT * D3D11_SO_OUTPUT_COMPONENT_COUNT) UINT NumEntries,
            _In_reads_opt_(NumStrides) const UINT *pBufferStrides,
            _In_range_(0, D3D11_SO_BUFFER_SLOT_COUNT) UINT NumStrides,
            _In_ UINT RasterizedStream, _In_opt_ ID3D11ClassLinkage *pClassLinkage,
            _Out_opt_ ID3D11GeometryShader **ppGeometryShader )
{
    HRESULT result = this->_interface->CreateGeometryShaderWithStreamOutput(
        pShaderBytecode, BytecodeLength, pSODeclaration, NumEntries, pBufferStrides, NumStrides, RasterizedStream, pClassLinkage, ppGeometryShader);
    
    return result;
}



template <typename interface_t>
HRESULT STDMETHODCALLTYPE device<interface_t>::CreatePixelShader(
            _In_reads_(BytecodeLength) const void *pShaderBytecode,
            _In_ SIZE_T BytecodeLength,
            _In_opt_ ID3D11ClassLinkage *pClassLinkage,
            _Out_opt_ ID3D11PixelShader **ppPixelShader )
{
    HRESULT result = this->_interface->CreatePixelShader(pShaderBytecode,
            BytecodeLength,
            pClassLinkage,
            ppPixelShader );
    
    return result;
}



template <typename interface_t>
HRESULT STDMETHODCALLTYPE device<interface_t>::CreateHullShader(
            _In_reads_(BytecodeLength) const void *pShaderBytecode,
            _In_ SIZE_T BytecodeLength,
            _In_opt_ ID3D11ClassLinkage *pClassLinkage,
            _Out_opt_ ID3D11HullShader **ppHullShader )
{
    HRESULT result = this->_interface->CreateHullShader(
            pShaderBytecode,
            BytecodeLength,
            pClassLinkage,
            ppHullShader );
    
    return result;
}



template <typename interface_t>
HRESULT STDMETHODCALLTYPE device<interface_t>::CreateDomainShader(
            _In_reads_(BytecodeLength) const void *pShaderBytecode,
            _In_ SIZE_T BytecodeLength,
            _In_opt_ ID3D11ClassLinkage *pClassLinkage,
            _Out_opt_ ID3D11DomainShader **ppDomainShader )
{
    HRESULT result = this->_interface->CreateDomainShader(
            pShaderBytecode,
            BytecodeLength,
            pClassLinkage,
            ppDomainShader );
    
    return result;
}



template <typename interface_t>
HRESULT STDMETHODCALLTYPE device<interface_t>::CreateComputeShader(
            _In_reads_(BytecodeLength) const void *pShaderBytecode,
            _In_ SIZE_T BytecodeLength,
            _In_opt_ ID3D11ClassLinkage *pClassLinkage,
            _Out_opt_ ID3D11ComputeShader **ppComputeShader )
{
    HRESULT result = this->_interface->CreateComputeShader(
            pShaderBytecode,
            BytecodeLength,
            pClassLinkage,
            ppComputeShader );
    
    return result;
}



template <typename interface_t>
HRESULT STDMETHODCALLTYPE device<interface_t>::CreateClassLinkage(_Out_ ID3D11ClassLinkage **ppLinkage)
{
    HRESULT result = this->_interface->CreateClassLinkage(ppLinkage);
    
    return result;
}



template <typename interface_t>
HRESULT STDMETHODCALLTYPE device<interface_t>::CreateBlendState(
            _In_ const D3D11_BLEND_DESC *pBlendStateDesc,
            _Out_opt_ ID3D11BlendState **ppBlendState )
{
    HRESULT result = this->_interface->CreateBlendState(pBlendStateDesc, ppBlendState );
    
    return result;
}



template <typename interface_t>
HRESULT STDMETHODCALLTYPE device<interface_t>::CreateDepthStencilState(
            _In_ const D3D11_DEPTH_STENCIL_DESC *pDepthStencilDesc,
            _Out_opt_ ID3D11DepthStencilState **ppDepthStencilState )
{
    HRESULT result = this->_interface->CreateDepthStencilState(pDepthStencilDesc, ppDepthStencilState );
    
    return result;
}



template <typename interface_t>
HRESULT STDMETHODCALLTYPE device<interface_t>::CreateRasterizerState(
            _In_ const D3D11_RASTERIZER_DESC *pRasterizerDesc,
            _Out_opt_ ID3D11RasterizerState **ppRasterizerState )
{
    HRESULT result = this->_interface->CreateRasterizerState(pRasterizerDesc, ppRasterizerState );
    
    return result;
}



template <typename interface_t>
HRESULT STDMETHODCALLTYPE device<interface_t>::CreateSamplerState(_In_ const D3D11_SAMPLER_DESC *pSamplerDesc, _Out_opt_ ID3D11SamplerState **ppSamplerState)
{
    HRESULT result = this->_interface->CreateSamplerState(pSamplerDesc, ppSamplerState);
    
    return result;
}



template <typename interface_t>
HRESULT STDMETHODCALLTYPE device<interface_t>::CreateQuery(_In_ const D3D11_QUERY_DESC *pQueryDesc, _Out_opt_ ID3D11Query **ppQuery)
{
    HRESULT result = this->_interface->CreateQuery(pQueryDesc, ppQuery);
    
    return result;
}



template <typename interface_t>
HRESULT STDMETHODCALLTYPE device<interface_t>::CreatePredicate(_In_ const D3D11_QUERY_DESC *pPredicateDesc, _Out_opt_ ID3D11Predicate **ppPredicate)
{
    HRESULT result = this->_interface->CreatePredicate(pPredicateDesc, ppPredicate);
    
    return result;
}



template <typename interface_t>
HRESULT STDMETHODCALLTYPE device<interface_t>::CreateCounter(_In_ const D3D11_COUNTER_DESC *pCounterDesc, _Out_opt_ ID3D11Counter **ppCounter)
{
    HRESULT result = this->_interface->CreateCounter(pCounterDesc, ppCounter);
    
    return result;
}



template <typename interface_t>
HRESULT STDMETHODCALLTYPE device<interface_t>::CreateDeferredContext(UINT ContextFlags, _Out_opt_ ID3D11DeviceContext **ppDeferredContext)
{
    HRESULT result = this->_interface->CreateDeferredContext(ContextFlags, ppDeferredContext);
    
    return result;
}



template <typename interface_t>
HRESULT STDMETHODCALLTYPE device<interface_t>::OpenSharedResource(_In_ HANDLE hResource, _In_ REFIID ReturnedInterface, _Out_opt_ void **ppResource)
{
    HRESULT result = this->_interface->OpenSharedResource(hResource, ReturnedInterface, ppResource);
    
    return result;
}



template <typename interface_t>
HRESULT STDMETHODCALLTYPE device<interface_t>::CheckFormatSupport(_In_ DXGI_FORMAT Format, _Out_ UINT *pFormatSupport)
{
    HRESULT result = this->_interface->CheckFormatSupport(Format, pFormatSupport);
    
    return result;
}



template <typename interface_t>
HRESULT STDMETHODCALLTYPE device<interface_t>::CheckMultisampleQualityLevels(_In_ DXGI_FORMAT Format, _In_ UINT SampleCount, _Out_ UINT *pNumQualityLevels)
{
    HRESULT result = this->_interface->CheckMultisampleQualityLevels(Format, SampleCount, pNumQualityLevels);
    
    return result;
}



template <typename interface_t>
void STDMETHODCALLTYPE device<interface_t>::CheckCounterInfo(_Out_ D3D11_COUNTER_INFO *pCounterInfo)
{
    this->_interface->CheckCounterInfo(pCounterInfo);
}



template <typename interface_t>
HRESULT STDMETHODCALLTYPE device<interface_t>::CheckCounter(
            _In_ const D3D11_COUNTER_DESC *pDesc,
            _Out_ D3D11_COUNTER_TYPE *pType,
            _Out_ UINT *pActiveCounters,
            _Out_writes_opt_(*pNameLength) LPSTR szName,
            _Inout_opt_ UINT *pNameLength,
            _Out_writes_opt_(*pUnitsLength) LPSTR szUnits,
            _Inout_opt_ UINT *pUnitsLength,
            _Out_writes_opt_(*pDescriptionLength) LPSTR szDescription,
            _Inout_opt_ UINT *pDescriptionLength )
{
    HRESULT result = this->_interface->CheckCounter(
            pDesc,
            pType,
            pActiveCounters,
            szName,
            pNameLength,
            szUnits,
            pUnitsLength,
            szDescription,
            pDescriptionLength );
    
    return result;
}



template <typename interface_t>
HRESULT STDMETHODCALLTYPE device<interface_t>::CheckFeatureSupport(
            D3D11_FEATURE Feature,
            _Out_writes_bytes_(FeatureSupportDataSize) void *pFeatureSupportData,
            UINT FeatureSupportDataSize )
{
    HRESULT result = this->_interface->CheckFeatureSupport(
            Feature,
            pFeatureSupportData,
            FeatureSupportDataSize );
    
    return result;
}



template <typename interface_t>
HRESULT STDMETHODCALLTYPE device<interface_t>::GetPrivateData(_In_ REFGUID guid, _Inout_ UINT *pDataSize, _Out_writes_bytes_opt_(*pDataSize) void *pData)
{
    HRESULT result = this->_interface->GetPrivateData(guid, pDataSize, pData);
    
    return result;
}



template <typename interface_t>
HRESULT STDMETHODCALLTYPE device<interface_t>::SetPrivateData(_In_ REFGUID guid, _In_ UINT DataSize, _In_reads_bytes_opt_(DataSize) const void *pData)
{
    HRESULT result = this->_interface->SetPrivateData(guid, DataSize, pData);
    
    return result;
}



template <typename interface_t>
HRESULT STDMETHODCALLTYPE device<interface_t>::SetPrivateDataInterface(_In_ REFGUID guid, _In_opt_ const IUnknown *pData)
{
    HRESULT result = this->_interface->SetPrivateDataInterface(guid, pData);
    
    return result;
}



template <typename interface_t>
D3D_FEATURE_LEVEL STDMETHODCALLTYPE device<interface_t>::GetFeatureLevel()
{
    D3D_FEATURE_LEVEL result = this->_interface->GetFeatureLevel();
    
    return result;
}



template <typename interface_t>
UINT STDMETHODCALLTYPE device<interface_t>::GetCreationFlags()
{
    UINT result = this->_interface->GetCreationFlags();
    
    return result;
}



template <typename interface_t>
HRESULT STDMETHODCALLTYPE device<interface_t>::GetDeviceRemovedReason()
{
    HRESULT result = this->_interface->GetDeviceRemovedReason();
    
    return result;
}



template <typename interface_t>
void STDMETHODCALLTYPE device<interface_t>::GetImmediateContext(_Out_ ID3D11DeviceContext **ppImmediateContext)
{
    this->_interface->GetImmediateContext(ppImmediateContext);
}



template <typename interface_t>
HRESULT STDMETHODCALLTYPE device<interface_t>::SetExceptionMode(UINT RaiseFlags)
{
    HRESULT result = this->_interface->SetExceptionMode(RaiseFlags);
    
    return result;
}



template <typename interface_t>
UINT STDMETHODCALLTYPE device<interface_t>::GetExceptionMode()
{
    UINT result = this->_interface->GetExceptionMode();
    
    return result;
}



} // namespace d3d11
} // namespace dx
__SP_NAMESPACE_CLOSE // namespace sp




#endif // _WIN32
