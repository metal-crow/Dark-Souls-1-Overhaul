/*
    CONTRIBUTORS:
        Sean Pesce

    References:
        https://msdn.microsoft.com/en-us/library/windows/desktop/ff476385(v=vs.85).aspx
*/
#ifdef _WIN32
#pragma once

#ifndef SP_DX_D3D11_ID3D11DEVICE_CONTEXT_WRAPPER_CLASS_H_
#define SP_DX_D3D11_ID3D11DEVICE_CONTEXT_WRAPPER_CLASS_H_


#include "sp/dx/d3d11/interface/id3d11devicechild.h"
#include "sp/log.h"
#include "D3D11.h"



__SP_NAMESPACE
namespace dx    {
namespace d3d11 {


template <class interface_t = ID3D11DeviceContext>
class context : public device_child<interface_t> {
protected:

    virtual inline void _wrap(interface_t **original) override
    {
        this->_interface = *original;
        *original = this;
    }

public:
    context() {}

    context(interface_t **original)
    {
        if (original && *original)
        {
            SP_LOG_DBG("[D3D11] Device context created (UUID = %s)\n", sp::os::win::com::guid::to_string(__uuidof(**original)).c_str());
        }
        this->_wrap(original);
    }

    virtual ~context() {}


    // Wrapped interface methods
    

    virtual void STDMETHODCALLTYPE VSSetConstantBuffers(_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)  UINT NumBuffers, _In_reads_opt_(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers);

    virtual void STDMETHODCALLTYPE PSSetShaderResources(_In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot)  UINT NumViews, _In_reads_opt_(NumViews)  ID3D11ShaderResourceView *const *ppShaderResourceViews);

    virtual void STDMETHODCALLTYPE PSSetShader(_In_opt_  ID3D11PixelShader *pPixelShader, _In_reads_opt_(NumClassInstances)  ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances);

    virtual void STDMETHODCALLTYPE PSSetSamplers(_In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot)  UINT NumSamplers, _In_reads_opt_(NumSamplers)  ID3D11SamplerState *const *ppSamplers);

    virtual void STDMETHODCALLTYPE VSSetShader(_In_opt_  ID3D11VertexShader *pVertexShader, _In_reads_opt_(NumClassInstances)  ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances);

    virtual void STDMETHODCALLTYPE DrawIndexed(_In_  UINT IndexCount, _In_  UINT StartIndexLocation, _In_  INT BaseVertexLocation);

    virtual void STDMETHODCALLTYPE Draw(_In_  UINT VertexCount, _In_  UINT StartVertexLocation);

    virtual HRESULT STDMETHODCALLTYPE Map(_In_  ID3D11Resource *pResource, _In_  UINT Subresource, _In_  D3D11_MAP MapType, _In_  UINT MapFlags, _Out_opt_  D3D11_MAPPED_SUBRESOURCE *pMappedResource);

    virtual void STDMETHODCALLTYPE Unmap(_In_  ID3D11Resource *pResource, _In_  UINT Subresource);

    virtual void STDMETHODCALLTYPE PSSetConstantBuffers(_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)  UINT NumBuffers, _In_reads_opt_(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers);

    virtual void STDMETHODCALLTYPE IASetInputLayout(_In_opt_  ID3D11InputLayout *pInputLayout);

    virtual void STDMETHODCALLTYPE IASetVertexBuffers(_In_range_(0, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT - StartSlot)  UINT NumBuffers, _In_reads_opt_(NumBuffers)  ID3D11Buffer *const *ppVertexBuffers, _In_reads_opt_(NumBuffers)  const UINT *pStrides, _In_reads_opt_(NumBuffers)  const UINT *pOffsets);

    virtual void STDMETHODCALLTYPE IASetIndexBuffer(_In_opt_  ID3D11Buffer *pIndexBuffer, _In_  DXGI_FORMAT Format, _In_  UINT Offset);

    virtual void STDMETHODCALLTYPE DrawIndexedInstanced(_In_  UINT IndexCountPerInstance, _In_  UINT InstanceCount, _In_  UINT StartIndexLocation, _In_  INT BaseVertexLocation, _In_  UINT StartInstanceLocation);

    virtual void STDMETHODCALLTYPE DrawInstanced(_In_  UINT VertexCountPerInstance, _In_  UINT InstanceCount, _In_  UINT StartVertexLocation, _In_  UINT StartInstanceLocation);

    virtual void STDMETHODCALLTYPE GSSetConstantBuffers(_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)  UINT NumBuffers, _In_reads_opt_(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers);

    virtual void STDMETHODCALLTYPE GSSetShader(_In_opt_  ID3D11GeometryShader *pShader, _In_reads_opt_(NumClassInstances)  ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances);

    virtual void STDMETHODCALLTYPE IASetPrimitiveTopology(_In_  D3D11_PRIMITIVE_TOPOLOGY Topology);

    virtual void STDMETHODCALLTYPE VSSetShaderResources(_In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot)  UINT NumViews, _In_reads_opt_(NumViews)  ID3D11ShaderResourceView *const *ppShaderResourceViews);

    virtual void STDMETHODCALLTYPE VSSetSamplers(_In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot)  UINT NumSamplers, _In_reads_opt_(NumSamplers)  ID3D11SamplerState *const *ppSamplers);

    virtual void STDMETHODCALLTYPE Begin(_In_  ID3D11Asynchronous *pAsync);

    virtual void STDMETHODCALLTYPE End(_In_  ID3D11Asynchronous *pAsync);

    virtual HRESULT STDMETHODCALLTYPE GetData(_In_  ID3D11Asynchronous *pAsync, _Out_writes_bytes_opt_(DataSize)  void *pData, _In_  UINT DataSize, _In_  UINT GetDataFlags);

    virtual void STDMETHODCALLTYPE SetPredication(_In_opt_  ID3D11Predicate *pPredicate, _In_  BOOL PredicateValue);

    virtual void STDMETHODCALLTYPE GSSetShaderResources(_In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot)  UINT NumViews, _In_reads_opt_(NumViews)  ID3D11ShaderResourceView *const *ppShaderResourceViews);

    virtual void STDMETHODCALLTYPE GSSetSamplers(_In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot)  UINT NumSamplers, _In_reads_opt_(NumSamplers)  ID3D11SamplerState *const *ppSamplers);

    virtual void STDMETHODCALLTYPE OMSetRenderTargets(_In_range_(0, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT)  UINT NumViews, _In_reads_opt_(NumViews)  ID3D11RenderTargetView *const *ppRenderTargetViews, _In_opt_  ID3D11DepthStencilView *pDepthStencilView);

    virtual void STDMETHODCALLTYPE OMSetRenderTargetsAndUnorderedAccessViews(_In_  UINT NumRTVs, _In_reads_opt_(NumRTVs)  ID3D11RenderTargetView *const *ppRenderTargetViews, _In_opt_  ID3D11DepthStencilView *pDepthStencilView, _In_range_(0, D3D11_1_UAV_SLOT_COUNT - 1)  UINT UAVStartSlot, _In_  UINT NumUAVs, _In_reads_opt_(NumUAVs)  ID3D11UnorderedAccessView *const *ppUnorderedAccessViews, _In_reads_opt_(NumUAVs)  const UINT *pUAVInitialCounts);

    virtual void STDMETHODCALLTYPE OMSetBlendState(_In_opt_  ID3D11BlendState *pBlendState, _In_opt_  const FLOAT BlendFactor[4], _In_  UINT SampleMask);

    virtual void STDMETHODCALLTYPE OMSetDepthStencilState(_In_opt_  ID3D11DepthStencilState *pDepthStencilState, _In_  UINT StencilRef);

    virtual void STDMETHODCALLTYPE SOSetTargets(_In_range_(0, D3D11_SO_BUFFER_SLOT_COUNT)  UINT NumBuffers, _In_reads_opt_(NumBuffers)  ID3D11Buffer *const *ppSOTargets, _In_reads_opt_(NumBuffers)  const UINT *pOffsets);

    virtual void STDMETHODCALLTYPE DrawAuto();

    virtual void STDMETHODCALLTYPE DrawIndexedInstancedIndirect(_In_  ID3D11Buffer *pBufferForArgs, _In_  UINT AlignedByteOffsetForArgs);

    virtual void STDMETHODCALLTYPE DrawInstancedIndirect(_In_  ID3D11Buffer *pBufferForArgs, _In_  UINT AlignedByteOffsetForArgs);

    virtual void STDMETHODCALLTYPE Dispatch(_In_  UINT ThreadGroupCountX, _In_  UINT ThreadGroupCountY, _In_  UINT ThreadGroupCountZ);

    virtual void STDMETHODCALLTYPE DispatchIndirect(_In_  ID3D11Buffer *pBufferForArgs, _In_  UINT AlignedByteOffsetForArgs);

    virtual void STDMETHODCALLTYPE RSSetState(_In_opt_  ID3D11RasterizerState *pRasterizerState);

    virtual void STDMETHODCALLTYPE RSSetViewports(_In_range_(0, D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE)  UINT NumViewports, _In_reads_opt_(NumViewports)  const D3D11_VIEWPORT *pViewports);

    virtual void STDMETHODCALLTYPE RSSetScissorRects(_In_range_(0, D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE)  UINT NumRects, _In_reads_opt_(NumRects)  const D3D11_RECT *pRects);

    virtual void STDMETHODCALLTYPE CopySubresourceRegion(_In_  ID3D11Resource *pDstResource, _In_  UINT DstSubresource, _In_  UINT DstX, _In_  UINT DstY, _In_  UINT DstZ, _In_  ID3D11Resource *pSrcResource, _In_  UINT SrcSubresource, _In_opt_  const D3D11_BOX *pSrcBox);

    virtual void STDMETHODCALLTYPE CopyResource(_In_  ID3D11Resource *pDstResource, _In_  ID3D11Resource *pSrcResource);

    virtual void STDMETHODCALLTYPE UpdateSubresource(_In_  ID3D11Resource *pDstResource, _In_  UINT DstSubresource, _In_opt_  const D3D11_BOX *pDstBox, _In_  const void *pSrcData, _In_  UINT SrcRowPitch, _In_  UINT SrcDepthPitch);

    virtual void STDMETHODCALLTYPE CopyStructureCount(_In_  ID3D11Buffer *pDstBuffer, _In_  UINT DstAlignedByteOffset, _In_  ID3D11UnorderedAccessView *pSrcView);

    virtual void STDMETHODCALLTYPE ClearRenderTargetView(_In_  ID3D11RenderTargetView *pRenderTargetView, _In_  const FLOAT ColorRGBA[4]);

    virtual void STDMETHODCALLTYPE ClearUnorderedAccessViewUint(_In_  ID3D11UnorderedAccessView *pUnorderedAccessView, _In_  const UINT Values[4]);

    virtual void STDMETHODCALLTYPE ClearUnorderedAccessViewFloat(_In_  ID3D11UnorderedAccessView *pUnorderedAccessView, _In_  const FLOAT Values[4]);

    virtual void STDMETHODCALLTYPE ClearDepthStencilView(_In_  ID3D11DepthStencilView *pDepthStencilView, _In_  UINT ClearFlags, _In_  FLOAT Depth, _In_  UINT8 Stencil);

    virtual void STDMETHODCALLTYPE GenerateMips(_In_  ID3D11ShaderResourceView *pShaderResourceView);

    virtual void STDMETHODCALLTYPE SetResourceMinLOD(_In_  ID3D11Resource *pResource, FLOAT MinLOD);

    virtual FLOAT STDMETHODCALLTYPE GetResourceMinLOD(_In_  ID3D11Resource *pResource);

    virtual void STDMETHODCALLTYPE ResolveSubresource(_In_  ID3D11Resource *pDstResource, _In_  UINT DstSubresource, _In_  ID3D11Resource *pSrcResource, _In_  UINT SrcSubresource, _In_  DXGI_FORMAT Format);

    virtual void STDMETHODCALLTYPE ExecuteCommandList(_In_  ID3D11CommandList *pCommandList, BOOL RestoreContextState);

    virtual void STDMETHODCALLTYPE HSSetShaderResources(_In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot)  UINT NumViews, _In_reads_opt_(NumViews)  ID3D11ShaderResourceView *const *ppShaderResourceViews);

    virtual void STDMETHODCALLTYPE HSSetShader(_In_opt_  ID3D11HullShader *pHullShader, _In_reads_opt_(NumClassInstances)  ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances);

    virtual void STDMETHODCALLTYPE HSSetSamplers(_In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot)  UINT NumSamplers, _In_reads_opt_(NumSamplers)  ID3D11SamplerState *const *ppSamplers);

    virtual void STDMETHODCALLTYPE HSSetConstantBuffers(_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)  UINT NumBuffers, _In_reads_opt_(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers);

    virtual void STDMETHODCALLTYPE DSSetShaderResources(_In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot)  UINT NumViews, _In_reads_opt_(NumViews)  ID3D11ShaderResourceView *const *ppShaderResourceViews);

    virtual void STDMETHODCALLTYPE DSSetShader(_In_opt_  ID3D11DomainShader *pDomainShader, _In_reads_opt_(NumClassInstances)  ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances);

    virtual void STDMETHODCALLTYPE DSSetSamplers(_In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot)  UINT NumSamplers, _In_reads_opt_(NumSamplers)  ID3D11SamplerState *const *ppSamplers);

    virtual void STDMETHODCALLTYPE DSSetConstantBuffers(_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)  UINT NumBuffers, _In_reads_opt_(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers);

    virtual void STDMETHODCALLTYPE CSSetShaderResources(_In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot)  UINT NumViews, _In_reads_opt_(NumViews)  ID3D11ShaderResourceView *const *ppShaderResourceViews);

    virtual void STDMETHODCALLTYPE CSSetUnorderedAccessViews(_In_range_(0, D3D11_1_UAV_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_1_UAV_SLOT_COUNT - StartSlot)  UINT NumUAVs, _In_reads_opt_(NumUAVs)  ID3D11UnorderedAccessView *const *ppUnorderedAccessViews, _In_reads_opt_(NumUAVs)  const UINT *pUAVInitialCounts);

    virtual void STDMETHODCALLTYPE CSSetShader(_In_opt_  ID3D11ComputeShader *pComputeShader, _In_reads_opt_(NumClassInstances)  ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances);

    virtual void STDMETHODCALLTYPE CSSetSamplers(_In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot)  UINT NumSamplers, _In_reads_opt_(NumSamplers)  ID3D11SamplerState *const *ppSamplers);

    virtual void STDMETHODCALLTYPE CSSetConstantBuffers(_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)  UINT NumBuffers, _In_reads_opt_(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers);

    virtual void STDMETHODCALLTYPE VSGetConstantBuffers(_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)  UINT NumBuffers, _Out_writes_opt_(NumBuffers)  ID3D11Buffer **ppConstantBuffers);

    virtual void STDMETHODCALLTYPE PSGetShaderResources(_In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot)  UINT NumViews, _Out_writes_opt_(NumViews)  ID3D11ShaderResourceView **ppShaderResourceViews);

    virtual void STDMETHODCALLTYPE PSGetShader(_Outptr_result_maybenull_  ID3D11PixelShader **ppPixelShader, _Out_writes_opt_(*pNumClassInstances)  ID3D11ClassInstance **ppClassInstances, _Inout_opt_  UINT *pNumClassInstances);

    virtual void STDMETHODCALLTYPE PSGetSamplers(_In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot)  UINT NumSamplers, _Out_writes_opt_(NumSamplers)  ID3D11SamplerState **ppSamplers);

    virtual void STDMETHODCALLTYPE VSGetShader(_Outptr_result_maybenull_  ID3D11VertexShader **ppVertexShader, _Out_writes_opt_(*pNumClassInstances)  ID3D11ClassInstance **ppClassInstances, _Inout_opt_  UINT *pNumClassInstances);

    virtual void STDMETHODCALLTYPE PSGetConstantBuffers(_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)  UINT NumBuffers, _Out_writes_opt_(NumBuffers)  ID3D11Buffer **ppConstantBuffers);

    virtual void STDMETHODCALLTYPE IAGetInputLayout(_Outptr_result_maybenull_  ID3D11InputLayout **ppInputLayout);

    virtual void STDMETHODCALLTYPE IAGetVertexBuffers(_In_range_(0, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT - StartSlot)  UINT NumBuffers, _Out_writes_opt_(NumBuffers)  ID3D11Buffer **ppVertexBuffers, _Out_writes_opt_(NumBuffers)  UINT *pStrides, _Out_writes_opt_(NumBuffers)  UINT *pOffsets);

    virtual void STDMETHODCALLTYPE IAGetIndexBuffer(_Outptr_opt_result_maybenull_  ID3D11Buffer **pIndexBuffer, _Out_opt_  DXGI_FORMAT *Format, _Out_opt_  UINT *Offset);

    virtual void STDMETHODCALLTYPE GSGetConstantBuffers(_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)  UINT NumBuffers, _Out_writes_opt_(NumBuffers)  ID3D11Buffer **ppConstantBuffers);

    virtual void STDMETHODCALLTYPE GSGetShader(_Outptr_result_maybenull_  ID3D11GeometryShader **ppGeometryShader, _Out_writes_opt_(*pNumClassInstances)  ID3D11ClassInstance **ppClassInstances, _Inout_opt_  UINT *pNumClassInstances);

    virtual void STDMETHODCALLTYPE IAGetPrimitiveTopology(_Out_  D3D11_PRIMITIVE_TOPOLOGY *pTopology);

    virtual void STDMETHODCALLTYPE VSGetShaderResources(_In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot)  UINT NumViews, _Out_writes_opt_(NumViews)  ID3D11ShaderResourceView **ppShaderResourceViews);

    virtual void STDMETHODCALLTYPE VSGetSamplers(_In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot)  UINT NumSamplers, _Out_writes_opt_(NumSamplers)  ID3D11SamplerState **ppSamplers);

    virtual void STDMETHODCALLTYPE GetPredication(_Outptr_opt_result_maybenull_  ID3D11Predicate **ppPredicate, _Out_opt_  BOOL *pPredicateValue);

    virtual void STDMETHODCALLTYPE GSGetShaderResources(_In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot)  UINT NumViews, _Out_writes_opt_(NumViews)  ID3D11ShaderResourceView **ppShaderResourceViews);

    virtual void STDMETHODCALLTYPE GSGetSamplers(_In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot)  UINT NumSamplers, _Out_writes_opt_(NumSamplers)  ID3D11SamplerState **ppSamplers);

    virtual void STDMETHODCALLTYPE OMGetRenderTargets(_In_range_(0, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT)  UINT NumViews, _Out_writes_opt_(NumViews)  ID3D11RenderTargetView **ppRenderTargetViews, _Outptr_opt_result_maybenull_  ID3D11DepthStencilView **ppDepthStencilView);

    virtual void STDMETHODCALLTYPE OMGetRenderTargetsAndUnorderedAccessViews(_In_range_(0, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT)  UINT NumRTVs, _Out_writes_opt_(NumRTVs)  ID3D11RenderTargetView **ppRenderTargetViews, _Outptr_opt_result_maybenull_  ID3D11DepthStencilView **ppDepthStencilView, _In_range_(0, D3D11_PS_CS_UAV_REGISTER_COUNT - 1)  UINT UAVStartSlot, _In_range_(0, D3D11_PS_CS_UAV_REGISTER_COUNT - UAVStartSlot)  UINT NumUAVs, _Out_writes_opt_(NumUAVs)  ID3D11UnorderedAccessView **ppUnorderedAccessViews);

    virtual void STDMETHODCALLTYPE OMGetBlendState(_Outptr_opt_result_maybenull_  ID3D11BlendState **ppBlendState, _Out_opt_  FLOAT BlendFactor[4], _Out_opt_  UINT *pSampleMask);

    virtual void STDMETHODCALLTYPE OMGetDepthStencilState(_Outptr_opt_result_maybenull_  ID3D11DepthStencilState **ppDepthStencilState, _Out_opt_  UINT *pStencilRef);

    virtual void STDMETHODCALLTYPE SOGetTargets(_In_range_(0, D3D11_SO_BUFFER_SLOT_COUNT)  UINT NumBuffers, _Out_writes_opt_(NumBuffers)  ID3D11Buffer **ppSOTargets);

    virtual void STDMETHODCALLTYPE RSGetState(_Outptr_result_maybenull_  ID3D11RasterizerState **ppRasterizerState);

    virtual void STDMETHODCALLTYPE RSGetViewports(_Inout_ /*_range(0, D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE )*/   UINT *pNumViewports, _Out_writes_opt_(*pNumViewports)  D3D11_VIEWPORT *pViewports);

    virtual void STDMETHODCALLTYPE RSGetScissorRects(_Inout_ /*_range(0, D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE )*/   UINT *pNumRects, _Out_writes_opt_(*pNumRects)  D3D11_RECT *pRects);

    virtual void STDMETHODCALLTYPE HSGetShaderResources(_In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot)  UINT NumViews, _Out_writes_opt_(NumViews)  ID3D11ShaderResourceView **ppShaderResourceViews);

    virtual void STDMETHODCALLTYPE HSGetShader(_Outptr_result_maybenull_  ID3D11HullShader **ppHullShader, _Out_writes_opt_(*pNumClassInstances)  ID3D11ClassInstance **ppClassInstances, _Inout_opt_  UINT *pNumClassInstances);

    virtual void STDMETHODCALLTYPE HSGetSamplers(_In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot)  UINT NumSamplers, _Out_writes_opt_(NumSamplers)  ID3D11SamplerState **ppSamplers);

    virtual void STDMETHODCALLTYPE HSGetConstantBuffers(_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)  UINT NumBuffers, _Out_writes_opt_(NumBuffers)  ID3D11Buffer **ppConstantBuffers);

    virtual void STDMETHODCALLTYPE DSGetShaderResources(_In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot)  UINT NumViews, _Out_writes_opt_(NumViews)  ID3D11ShaderResourceView **ppShaderResourceViews);

    virtual void STDMETHODCALLTYPE DSGetShader(_Outptr_result_maybenull_  ID3D11DomainShader **ppDomainShader, _Out_writes_opt_(*pNumClassInstances)  ID3D11ClassInstance **ppClassInstances, _Inout_opt_  UINT *pNumClassInstances);

    virtual void STDMETHODCALLTYPE DSGetSamplers(_In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot)  UINT NumSamplers, _Out_writes_opt_(NumSamplers)  ID3D11SamplerState **ppSamplers);

    virtual void STDMETHODCALLTYPE DSGetConstantBuffers(_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)  UINT NumBuffers, _Out_writes_opt_(NumBuffers)  ID3D11Buffer **ppConstantBuffers);

    virtual void STDMETHODCALLTYPE CSGetShaderResources(_In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot)  UINT NumViews, _Out_writes_opt_(NumViews)  ID3D11ShaderResourceView **ppShaderResourceViews);

    virtual void STDMETHODCALLTYPE CSGetUnorderedAccessViews(_In_range_(0, D3D11_1_UAV_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_1_UAV_SLOT_COUNT - StartSlot)  UINT NumUAVs, _Out_writes_opt_(NumUAVs)  ID3D11UnorderedAccessView **ppUnorderedAccessViews);

    virtual void STDMETHODCALLTYPE CSGetShader(_Outptr_result_maybenull_  ID3D11ComputeShader **ppComputeShader, _Out_writes_opt_(*pNumClassInstances)  ID3D11ClassInstance **ppClassInstances, _Inout_opt_  UINT *pNumClassInstances);

    virtual void STDMETHODCALLTYPE CSGetSamplers(_In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot)  UINT NumSamplers, _Out_writes_opt_(NumSamplers)  ID3D11SamplerState **ppSamplers);

    virtual void STDMETHODCALLTYPE CSGetConstantBuffers(_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)  UINT NumBuffers, _Out_writes_opt_(NumBuffers)  ID3D11Buffer **ppConstantBuffers);

    virtual void STDMETHODCALLTYPE ClearState();

    virtual void STDMETHODCALLTYPE Flush();

    virtual D3D11_DEVICE_CONTEXT_TYPE STDMETHODCALLTYPE GetType();

    virtual UINT STDMETHODCALLTYPE GetContextFlags();

    virtual HRESULT STDMETHODCALLTYPE FinishCommandList(BOOL RestoreDeferredContextState, _COM_Outptr_opt_  ID3D11CommandList **ppCommandList);


}; // class dx::d3d11::context



} // namespace d3d11
} // namespace dx
__SP_NAMESPACE_CLOSE // namespace sp


template class sp::dx::d3d11::context<ID3D11DeviceContext>;


// Implementation
#include "sp/dx/d3d11/interface/context/id3d11devicecontext_impl.hpp"


#endif // SP_DX_D3D11_ID3D11DEVICE_CONTEXT_WRAPPER_CLASS_H_


#endif // _WIN32
