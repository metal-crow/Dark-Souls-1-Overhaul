/*
    CONTRIBUTORS:
        Sean Pesce

    References:
        https://msdn.microsoft.com/en-us/library/windows/desktop/ff476385(v=vs.85).aspx
*/
#ifdef _WIN32

#ifndef SP_DX_D3D11_ID3D11DEVICE_CONTEXT_WRAPPER_CLASS_H_
#error Invalid header inclusion "dx/d3d11/interface/context/id3d11devicecontext_impl.hpp" (Use "dx/d3d11/interface/context/id3d11devicecontext.h" instead)
#endif

#include "sp/sp.h"


__SP_NAMESPACE
namespace dx    {
namespace d3d11 {



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::VSSetConstantBuffers(_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)  UINT NumBuffers, _In_reads_opt_(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers)
{
    this->_interface->VSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::PSSetShaderResources(_In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot)  UINT NumViews, _In_reads_opt_(NumViews)  ID3D11ShaderResourceView *const *ppShaderResourceViews)
{
    this->_interface->PSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::PSSetShader(_In_opt_  ID3D11PixelShader *pPixelShader, _In_reads_opt_(NumClassInstances)  ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances)
{
    this->_interface->PSSetShader(pPixelShader, ppClassInstances, NumClassInstances);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::PSSetSamplers(_In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot)  UINT NumSamplers, _In_reads_opt_(NumSamplers)  ID3D11SamplerState *const *ppSamplers)
{
    this->_interface->PSSetSamplers(StartSlot, NumSamplers, ppSamplers);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::VSSetShader(_In_opt_  ID3D11VertexShader *pVertexShader, _In_reads_opt_(NumClassInstances)  ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances)
{
    this->_interface->VSSetShader(pVertexShader, ppClassInstances, NumClassInstances);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::DrawIndexed(_In_  UINT IndexCount, _In_  UINT StartIndexLocation, _In_  INT BaseVertexLocation)
{
    this->_interface->DrawIndexed(IndexCount, StartIndexLocation, BaseVertexLocation);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::Draw(_In_  UINT VertexCount, _In_  UINT StartVertexLocation)
{
    this->_interface->Draw(VertexCount, StartVertexLocation);
}



template <typename interface_t>
HRESULT STDMETHODCALLTYPE context<interface_t>::Map(_In_  ID3D11Resource *pResource, _In_  UINT Subresource, _In_  D3D11_MAP MapType, _In_  UINT MapFlags, _Out_opt_  D3D11_MAPPED_SUBRESOURCE *pMappedResource)
{
    HRESULT result = this->_interface->Map(pResource, Subresource, MapType, MapFlags, pMappedResource);

    return result;
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::Unmap(_In_  ID3D11Resource *pResource, _In_  UINT Subresource)
{
    this->_interface->Unmap(pResource, Subresource);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::PSSetConstantBuffers(_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)  UINT NumBuffers, _In_reads_opt_(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers)
{
    this->_interface->PSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::IASetInputLayout(_In_opt_  ID3D11InputLayout *pInputLayout)
{
    this->_interface->IASetInputLayout(pInputLayout);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::IASetVertexBuffers(_In_range_(0, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT - StartSlot)  UINT NumBuffers, _In_reads_opt_(NumBuffers)  ID3D11Buffer *const *ppVertexBuffers, _In_reads_opt_(NumBuffers)  const UINT *pStrides, _In_reads_opt_(NumBuffers)  const UINT *pOffsets)
{
    this->_interface->IASetVertexBuffers(StartSlot, NumBuffers, ppVertexBuffers, pStrides, pOffsets);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::IASetIndexBuffer(_In_opt_  ID3D11Buffer *pIndexBuffer, _In_  DXGI_FORMAT Format, _In_  UINT Offset)
{
    this->_interface->IASetIndexBuffer(pIndexBuffer, Format, Offset);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::DrawIndexedInstanced(_In_  UINT IndexCountPerInstance, _In_  UINT InstanceCount, _In_  UINT StartIndexLocation, _In_  INT BaseVertexLocation, _In_  UINT StartInstanceLocation)
{
    this->_interface->DrawIndexedInstanced(IndexCountPerInstance, InstanceCount, StartIndexLocation, BaseVertexLocation, StartInstanceLocation);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::DrawInstanced(_In_  UINT VertexCountPerInstance, _In_  UINT InstanceCount, _In_  UINT StartVertexLocation, _In_  UINT StartInstanceLocation)
{
    this->_interface->DrawInstanced(VertexCountPerInstance, InstanceCount, StartVertexLocation, StartInstanceLocation);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::GSSetConstantBuffers(_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)  UINT NumBuffers, _In_reads_opt_(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers)
{
    this->_interface->GSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::GSSetShader(_In_opt_  ID3D11GeometryShader *pShader, _In_reads_opt_(NumClassInstances)  ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances)
{
    this->_interface->GSSetShader(pShader, ppClassInstances, NumClassInstances);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::IASetPrimitiveTopology(_In_  D3D11_PRIMITIVE_TOPOLOGY Topology)
{
    this->_interface->IASetPrimitiveTopology(Topology);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::VSSetShaderResources(_In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot)  UINT NumViews, _In_reads_opt_(NumViews)  ID3D11ShaderResourceView *const *ppShaderResourceViews)
{
    this->_interface->VSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::VSSetSamplers(_In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot)  UINT NumSamplers, _In_reads_opt_(NumSamplers)  ID3D11SamplerState *const *ppSamplers)
{
    this->_interface->VSSetSamplers(StartSlot, NumSamplers, ppSamplers);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::Begin(_In_  ID3D11Asynchronous *pAsync)
{
    this->_interface->Begin(pAsync);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::End(_In_  ID3D11Asynchronous *pAsync)
{
    this->_interface->End(pAsync);
}



template <typename interface_t>
HRESULT STDMETHODCALLTYPE context<interface_t>::GetData(_In_  ID3D11Asynchronous *pAsync, _Out_writes_bytes_opt_(DataSize)  void *pData, _In_  UINT DataSize, _In_  UINT GetDataFlags)
{
    HRESULT result = this->_interface->GetData(pAsync, pData, DataSize, GetDataFlags);

    return result;
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::SetPredication(_In_opt_  ID3D11Predicate *pPredicate, _In_  BOOL PredicateValue)
{
    this->_interface->SetPredication(pPredicate, PredicateValue);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::GSSetShaderResources(_In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot)  UINT NumViews, _In_reads_opt_(NumViews)  ID3D11ShaderResourceView *const *ppShaderResourceViews)
{
    this->_interface->GSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::GSSetSamplers(_In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot)  UINT NumSamplers, _In_reads_opt_(NumSamplers)  ID3D11SamplerState *const *ppSamplers)
{
    this->_interface->GSSetSamplers(StartSlot, NumSamplers, ppSamplers);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::OMSetRenderTargets(_In_range_(0, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT)  UINT NumViews, _In_reads_opt_(NumViews)  ID3D11RenderTargetView *const *ppRenderTargetViews, _In_opt_  ID3D11DepthStencilView *pDepthStencilView)
{
    this->_interface->OMSetRenderTargets(NumViews, ppRenderTargetViews, pDepthStencilView);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::OMSetRenderTargetsAndUnorderedAccessViews(_In_  UINT NumRTVs, _In_reads_opt_(NumRTVs)  ID3D11RenderTargetView *const *ppRenderTargetViews, _In_opt_  ID3D11DepthStencilView *pDepthStencilView, _In_range_(0, D3D11_1_UAV_SLOT_COUNT - 1)  UINT UAVStartSlot, _In_  UINT NumUAVs, _In_reads_opt_(NumUAVs)  ID3D11UnorderedAccessView *const *ppUnorderedAccessViews, _In_reads_opt_(NumUAVs)  const UINT *pUAVInitialCounts)
{
    this->_interface->OMSetRenderTargetsAndUnorderedAccessViews(NumRTVs, ppRenderTargetViews, pDepthStencilView, UAVStartSlot, NumUAVs, ppUnorderedAccessViews, pUAVInitialCounts);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::OMSetBlendState(_In_opt_  ID3D11BlendState *pBlendState, _In_opt_  const FLOAT BlendFactor[4], _In_  UINT SampleMask)
{
    this->_interface->OMSetBlendState(pBlendState, BlendFactor, SampleMask);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::OMSetDepthStencilState(_In_opt_  ID3D11DepthStencilState *pDepthStencilState, _In_  UINT StencilRef)
{
    this->_interface->OMSetDepthStencilState(pDepthStencilState, StencilRef);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::SOSetTargets(_In_range_(0, D3D11_SO_BUFFER_SLOT_COUNT)  UINT NumBuffers, _In_reads_opt_(NumBuffers)  ID3D11Buffer *const *ppSOTargets, _In_reads_opt_(NumBuffers)  const UINT *pOffsets)
{
    this->_interface->SOSetTargets(NumBuffers, ppSOTargets, pOffsets);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::DrawAuto()
{
    this->_interface->DrawAuto();
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::DrawIndexedInstancedIndirect(_In_  ID3D11Buffer *pBufferForArgs, _In_  UINT AlignedByteOffsetForArgs)
{
    this->_interface->DrawIndexedInstancedIndirect(pBufferForArgs, AlignedByteOffsetForArgs);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::DrawInstancedIndirect(_In_  ID3D11Buffer *pBufferForArgs, _In_  UINT AlignedByteOffsetForArgs)
{
    this->_interface->DrawInstancedIndirect(pBufferForArgs, AlignedByteOffsetForArgs);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::Dispatch(_In_  UINT ThreadGroupCountX, _In_  UINT ThreadGroupCountY, _In_  UINT ThreadGroupCountZ)
{
    this->_interface->Dispatch(ThreadGroupCountX, ThreadGroupCountY, ThreadGroupCountZ);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::DispatchIndirect(_In_  ID3D11Buffer *pBufferForArgs, _In_  UINT AlignedByteOffsetForArgs)
{
    this->_interface->DispatchIndirect(pBufferForArgs, AlignedByteOffsetForArgs);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::RSSetState(_In_opt_  ID3D11RasterizerState *pRasterizerState)
{
    this->_interface->RSSetState(pRasterizerState);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::RSSetViewports(_In_range_(0, D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE)  UINT NumViewports, _In_reads_opt_(NumViewports)  const D3D11_VIEWPORT *pViewports)
{
    this->_interface->RSSetViewports(NumViewports, pViewports);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::RSSetScissorRects(_In_range_(0, D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE)  UINT NumRects, _In_reads_opt_(NumRects)  const D3D11_RECT *pRects)
{
    this->_interface->RSSetScissorRects(NumRects, pRects);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::CopySubresourceRegion(_In_  ID3D11Resource *pDstResource, _In_  UINT DstSubresource, _In_  UINT DstX, _In_  UINT DstY, _In_  UINT DstZ, _In_  ID3D11Resource *pSrcResource, _In_  UINT SrcSubresource, _In_opt_  const D3D11_BOX *pSrcBox)
{
    this->_interface->CopySubresourceRegion(pDstResource, DstSubresource, DstX, DstY, DstZ, pSrcResource, SrcSubresource, pSrcBox);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::CopyResource(_In_  ID3D11Resource *pDstResource, _In_  ID3D11Resource *pSrcResource)
{
    this->_interface->CopyResource(pDstResource, pSrcResource);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::UpdateSubresource(_In_  ID3D11Resource *pDstResource, _In_  UINT DstSubresource, _In_opt_  const D3D11_BOX *pDstBox, _In_  const void *pSrcData, _In_  UINT SrcRowPitch, _In_  UINT SrcDepthPitch)
{
    this->_interface->UpdateSubresource(pDstResource, DstSubresource, pDstBox, pSrcData, SrcRowPitch, SrcDepthPitch);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::CopyStructureCount(_In_  ID3D11Buffer *pDstBuffer, _In_  UINT DstAlignedByteOffset, _In_  ID3D11UnorderedAccessView *pSrcView)
{
    this->_interface->CopyStructureCount(pDstBuffer, DstAlignedByteOffset, pSrcView);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::ClearRenderTargetView(_In_  ID3D11RenderTargetView *pRenderTargetView, _In_  const FLOAT ColorRGBA[4])
{
    this->_interface->ClearRenderTargetView(pRenderTargetView, ColorRGBA);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::ClearUnorderedAccessViewUint(_In_  ID3D11UnorderedAccessView *pUnorderedAccessView, _In_  const UINT Values[4])
{
    this->_interface->ClearUnorderedAccessViewUint(pUnorderedAccessView, Values);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::ClearUnorderedAccessViewFloat(_In_  ID3D11UnorderedAccessView *pUnorderedAccessView, _In_  const FLOAT Values[4])
{
    this->_interface->ClearUnorderedAccessViewFloat(pUnorderedAccessView, Values);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::ClearDepthStencilView(_In_  ID3D11DepthStencilView *pDepthStencilView, _In_  UINT ClearFlags, _In_  FLOAT Depth, _In_  UINT8 Stencil)
{
    this->_interface->ClearDepthStencilView(pDepthStencilView, ClearFlags, Depth, Stencil);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::GenerateMips(_In_  ID3D11ShaderResourceView *pShaderResourceView)
{
    this->_interface->GenerateMips(pShaderResourceView);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::SetResourceMinLOD(_In_  ID3D11Resource *pResource, FLOAT MinLOD)
{
    this->_interface->SetResourceMinLOD(pResource, MinLOD);
}



template <typename interface_t>
FLOAT STDMETHODCALLTYPE context<interface_t>::GetResourceMinLOD(_In_  ID3D11Resource *pResource)
{
    FLOAT result = this->_interface->GetResourceMinLOD(pResource);

    return result;
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::ResolveSubresource(_In_  ID3D11Resource *pDstResource, _In_  UINT DstSubresource, _In_  ID3D11Resource *pSrcResource, _In_  UINT SrcSubresource, _In_  DXGI_FORMAT Format)
{
    this->_interface->ResolveSubresource(pDstResource, DstSubresource, pSrcResource, SrcSubresource, Format);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::ExecuteCommandList(_In_  ID3D11CommandList *pCommandList, BOOL RestoreContextState)
{
    this->_interface->ExecuteCommandList(pCommandList, RestoreContextState);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::HSSetShaderResources(_In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot)  UINT NumViews, _In_reads_opt_(NumViews)  ID3D11ShaderResourceView *const *ppShaderResourceViews)
{
    this->_interface->HSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::HSSetShader(_In_opt_  ID3D11HullShader *pHullShader, _In_reads_opt_(NumClassInstances)  ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances)
{
    this->_interface->HSSetShader(pHullShader, ppClassInstances, NumClassInstances);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::HSSetSamplers(_In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot)  UINT NumSamplers, _In_reads_opt_(NumSamplers)  ID3D11SamplerState *const *ppSamplers)
{
    this->_interface->HSSetSamplers(StartSlot, NumSamplers, ppSamplers);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::HSSetConstantBuffers(_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)  UINT NumBuffers, _In_reads_opt_(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers)
{
    this->_interface->HSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::DSSetShaderResources(_In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot)  UINT NumViews, _In_reads_opt_(NumViews)  ID3D11ShaderResourceView *const *ppShaderResourceViews)
{
    this->_interface->DSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::DSSetShader(_In_opt_  ID3D11DomainShader *pDomainShader, _In_reads_opt_(NumClassInstances)  ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances)
{
    this->_interface->DSSetShader(pDomainShader, ppClassInstances, NumClassInstances);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::DSSetSamplers(_In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot)  UINT NumSamplers, _In_reads_opt_(NumSamplers)  ID3D11SamplerState *const *ppSamplers)
{
    this->_interface->DSSetSamplers(StartSlot, NumSamplers, ppSamplers);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::DSSetConstantBuffers(_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)  UINT NumBuffers, _In_reads_opt_(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers)
{
    this->_interface->DSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::CSSetShaderResources(_In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot)  UINT NumViews, _In_reads_opt_(NumViews)  ID3D11ShaderResourceView *const *ppShaderResourceViews)
{
    this->_interface->CSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::CSSetUnorderedAccessViews(_In_range_(0, D3D11_1_UAV_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_1_UAV_SLOT_COUNT - StartSlot)  UINT NumUAVs, _In_reads_opt_(NumUAVs)  ID3D11UnorderedAccessView *const *ppUnorderedAccessViews, _In_reads_opt_(NumUAVs)  const UINT *pUAVInitialCounts)
{
    this->_interface->CSSetUnorderedAccessViews(StartSlot, NumUAVs, ppUnorderedAccessViews, pUAVInitialCounts);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::CSSetShader(_In_opt_  ID3D11ComputeShader *pComputeShader, _In_reads_opt_(NumClassInstances)  ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances)
{
    this->_interface->CSSetShader(pComputeShader, ppClassInstances, NumClassInstances);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::CSSetSamplers(_In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot)  UINT NumSamplers, _In_reads_opt_(NumSamplers)  ID3D11SamplerState *const *ppSamplers)
{
    this->_interface->CSSetSamplers(StartSlot, NumSamplers, ppSamplers);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::CSSetConstantBuffers(_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)  UINT NumBuffers, _In_reads_opt_(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers)
{
    this->_interface->CSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::VSGetConstantBuffers(_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)  UINT NumBuffers, _Out_writes_opt_(NumBuffers)  ID3D11Buffer **ppConstantBuffers)
{
    this->_interface->VSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::PSGetShaderResources(_In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot)  UINT NumViews, _Out_writes_opt_(NumViews)  ID3D11ShaderResourceView **ppShaderResourceViews)
{
    this->_interface->PSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::PSGetShader(_Outptr_result_maybenull_  ID3D11PixelShader **ppPixelShader, _Out_writes_opt_(*pNumClassInstances)  ID3D11ClassInstance **ppClassInstances, _Inout_opt_  UINT *pNumClassInstances)
{
    this->_interface->PSGetShader(ppPixelShader, ppClassInstances, pNumClassInstances);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::PSGetSamplers(_In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot)  UINT NumSamplers, _Out_writes_opt_(NumSamplers)  ID3D11SamplerState **ppSamplers)
{
    this->_interface->PSGetSamplers(StartSlot, NumSamplers, ppSamplers);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::VSGetShader(_Outptr_result_maybenull_  ID3D11VertexShader **ppVertexShader, _Out_writes_opt_(*pNumClassInstances)  ID3D11ClassInstance **ppClassInstances, _Inout_opt_  UINT *pNumClassInstances)
{
    this->_interface->VSGetShader(ppVertexShader, ppClassInstances, pNumClassInstances);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::PSGetConstantBuffers(_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)  UINT NumBuffers, _Out_writes_opt_(NumBuffers)  ID3D11Buffer **ppConstantBuffers)
{
    this->_interface->PSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::IAGetInputLayout(_Outptr_result_maybenull_  ID3D11InputLayout **ppInputLayout)
{
    this->_interface->IAGetInputLayout(ppInputLayout);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::IAGetVertexBuffers(_In_range_(0, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT - StartSlot)  UINT NumBuffers, _Out_writes_opt_(NumBuffers)  ID3D11Buffer **ppVertexBuffers, _Out_writes_opt_(NumBuffers)  UINT *pStrides, _Out_writes_opt_(NumBuffers)  UINT *pOffsets)
{
    this->_interface->IAGetVertexBuffers(StartSlot, NumBuffers, ppVertexBuffers, pStrides, pOffsets);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::IAGetIndexBuffer(_Outptr_opt_result_maybenull_  ID3D11Buffer **pIndexBuffer, _Out_opt_  DXGI_FORMAT *Format, _Out_opt_  UINT *Offset)
{
    this->_interface->IAGetIndexBuffer(pIndexBuffer, Format, Offset);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::GSGetConstantBuffers(_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)  UINT NumBuffers, _Out_writes_opt_(NumBuffers)  ID3D11Buffer **ppConstantBuffers)
{
    this->_interface->GSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::GSGetShader(_Outptr_result_maybenull_  ID3D11GeometryShader **ppGeometryShader, _Out_writes_opt_(*pNumClassInstances)  ID3D11ClassInstance **ppClassInstances, _Inout_opt_  UINT *pNumClassInstances)
{
    this->_interface->GSGetShader(ppGeometryShader, ppClassInstances, pNumClassInstances);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::IAGetPrimitiveTopology(_Out_  D3D11_PRIMITIVE_TOPOLOGY *pTopology)
{
    this->_interface->IAGetPrimitiveTopology(pTopology);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::VSGetShaderResources(_In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot)  UINT NumViews, _Out_writes_opt_(NumViews)  ID3D11ShaderResourceView **ppShaderResourceViews)
{
    this->_interface->VSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::VSGetSamplers(_In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot)  UINT NumSamplers, _Out_writes_opt_(NumSamplers)  ID3D11SamplerState **ppSamplers)
{
    this->_interface->VSGetSamplers(StartSlot, NumSamplers, ppSamplers);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::GetPredication(_Outptr_opt_result_maybenull_  ID3D11Predicate **ppPredicate, _Out_opt_  BOOL *pPredicateValue)
{
    this->_interface->GetPredication(ppPredicate, pPredicateValue);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::GSGetShaderResources(_In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot)  UINT NumViews, _Out_writes_opt_(NumViews)  ID3D11ShaderResourceView **ppShaderResourceViews)
{
    this->_interface->GSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::GSGetSamplers(_In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot)  UINT NumSamplers, _Out_writes_opt_(NumSamplers)  ID3D11SamplerState **ppSamplers)
{
    this->_interface->GSGetSamplers(StartSlot, NumSamplers, ppSamplers);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::OMGetRenderTargets(_In_range_(0, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT)  UINT NumViews, _Out_writes_opt_(NumViews)  ID3D11RenderTargetView **ppRenderTargetViews, _Outptr_opt_result_maybenull_  ID3D11DepthStencilView **ppDepthStencilView)
{
    this->_interface->OMGetRenderTargets(NumViews, ppRenderTargetViews, ppDepthStencilView);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::OMGetRenderTargetsAndUnorderedAccessViews(_In_range_(0, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT)  UINT NumRTVs, _Out_writes_opt_(NumRTVs)  ID3D11RenderTargetView **ppRenderTargetViews, _Outptr_opt_result_maybenull_  ID3D11DepthStencilView **ppDepthStencilView, _In_range_(0, D3D11_PS_CS_UAV_REGISTER_COUNT - 1)  UINT UAVStartSlot, _In_range_(0, D3D11_PS_CS_UAV_REGISTER_COUNT - UAVStartSlot)  UINT NumUAVs, _Out_writes_opt_(NumUAVs)  ID3D11UnorderedAccessView **ppUnorderedAccessViews)
{
    this->_interface->OMGetRenderTargetsAndUnorderedAccessViews(NumRTVs, ppRenderTargetViews, ppDepthStencilView, UAVStartSlot, NumUAVs, ppUnorderedAccessViews);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::OMGetBlendState(_Outptr_opt_result_maybenull_  ID3D11BlendState **ppBlendState, _Out_opt_  FLOAT BlendFactor[4], _Out_opt_  UINT *pSampleMask)
{
    this->_interface->OMGetBlendState(ppBlendState, BlendFactor, pSampleMask);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::OMGetDepthStencilState(_Outptr_opt_result_maybenull_  ID3D11DepthStencilState **ppDepthStencilState, _Out_opt_  UINT *pStencilRef)
{
    this->_interface->OMGetDepthStencilState(ppDepthStencilState, pStencilRef);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::SOGetTargets(_In_range_(0, D3D11_SO_BUFFER_SLOT_COUNT)  UINT NumBuffers, _Out_writes_opt_(NumBuffers)  ID3D11Buffer **ppSOTargets)
{
    this->_interface->SOGetTargets(NumBuffers, ppSOTargets);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::RSGetState(_Outptr_result_maybenull_  ID3D11RasterizerState **ppRasterizerState)
{
    this->_interface->RSGetState(ppRasterizerState);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::RSGetViewports(_Inout_ /*_range(0, D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE )*/   UINT *pNumViewports, _Out_writes_opt_(*pNumViewports)  D3D11_VIEWPORT *pViewports)
{
    this->_interface->RSGetViewports(pNumViewports, pViewports);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::RSGetScissorRects(_Inout_ /*_range(0, D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE )*/   UINT *pNumRects, _Out_writes_opt_(*pNumRects)  D3D11_RECT *pRects)
{
    this->_interface->RSGetScissorRects(pNumRects, pRects);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::HSGetShaderResources(_In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot)  UINT NumViews, _Out_writes_opt_(NumViews)  ID3D11ShaderResourceView **ppShaderResourceViews)
{
    this->_interface->HSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::HSGetShader(_Outptr_result_maybenull_  ID3D11HullShader **ppHullShader, _Out_writes_opt_(*pNumClassInstances)  ID3D11ClassInstance **ppClassInstances, _Inout_opt_  UINT *pNumClassInstances)
{
    this->_interface->HSGetShader(ppHullShader, ppClassInstances, pNumClassInstances);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::HSGetSamplers(_In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot)  UINT NumSamplers, _Out_writes_opt_(NumSamplers)  ID3D11SamplerState **ppSamplers)
{
    this->_interface->HSGetSamplers(StartSlot, NumSamplers, ppSamplers);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::HSGetConstantBuffers(_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)  UINT NumBuffers, _Out_writes_opt_(NumBuffers)  ID3D11Buffer **ppConstantBuffers)
{
    this->_interface->HSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::DSGetShaderResources(_In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot)  UINT NumViews, _Out_writes_opt_(NumViews)  ID3D11ShaderResourceView **ppShaderResourceViews)
{
    this->_interface->DSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::DSGetShader(_Outptr_result_maybenull_  ID3D11DomainShader **ppDomainShader, _Out_writes_opt_(*pNumClassInstances)  ID3D11ClassInstance **ppClassInstances, _Inout_opt_  UINT *pNumClassInstances)
{
    this->_interface->DSGetShader(ppDomainShader, ppClassInstances, pNumClassInstances);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::DSGetSamplers(_In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot)  UINT NumSamplers, _Out_writes_opt_(NumSamplers)  ID3D11SamplerState **ppSamplers)
{
    this->_interface->DSGetSamplers(StartSlot, NumSamplers, ppSamplers);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::DSGetConstantBuffers(_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)  UINT NumBuffers, _Out_writes_opt_(NumBuffers)  ID3D11Buffer **ppConstantBuffers)
{
    this->_interface->DSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::CSGetShaderResources(_In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot)  UINT NumViews, _Out_writes_opt_(NumViews)  ID3D11ShaderResourceView **ppShaderResourceViews)
{
    this->_interface->CSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::CSGetUnorderedAccessViews(_In_range_(0, D3D11_1_UAV_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_1_UAV_SLOT_COUNT - StartSlot)  UINT NumUAVs, _Out_writes_opt_(NumUAVs)  ID3D11UnorderedAccessView **ppUnorderedAccessViews)
{
    this->_interface->CSGetUnorderedAccessViews(StartSlot, NumUAVs, ppUnorderedAccessViews);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::CSGetShader(_Outptr_result_maybenull_  ID3D11ComputeShader **ppComputeShader, _Out_writes_opt_(*pNumClassInstances)  ID3D11ClassInstance **ppClassInstances, _Inout_opt_  UINT *pNumClassInstances)
{
    this->_interface->CSGetShader(ppComputeShader, ppClassInstances, pNumClassInstances);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::CSGetSamplers(_In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot)  UINT NumSamplers, _Out_writes_opt_(NumSamplers)  ID3D11SamplerState **ppSamplers)
{
    this->_interface->CSGetSamplers(StartSlot, NumSamplers, ppSamplers);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::CSGetConstantBuffers(_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)  UINT NumBuffers, _Out_writes_opt_(NumBuffers)  ID3D11Buffer **ppConstantBuffers)
{
    this->_interface->CSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::ClearState()
{
    this->_interface->ClearState();
}



template <typename interface_t>
void STDMETHODCALLTYPE context<interface_t>::Flush()
{
    this->_interface->Flush();
}



template <typename interface_t>
D3D11_DEVICE_CONTEXT_TYPE STDMETHODCALLTYPE context<interface_t>::GetType()
{
    D3D11_DEVICE_CONTEXT_TYPE result = this->_interface->GetType();

    return result;
}



template <typename interface_t>
UINT STDMETHODCALLTYPE context<interface_t>::GetContextFlags()
{
    UINT result = this->_interface->GetContextFlags();

    return result;
}



template <typename interface_t>
HRESULT STDMETHODCALLTYPE context<interface_t>::FinishCommandList(BOOL RestoreDeferredContextState, _COM_Outptr_opt_  ID3D11CommandList **ppCommandList)
{
    HRESULT result = this->_interface->FinishCommandList(RestoreDeferredContextState, ppCommandList);

    return result;
}







} // namespace d3d11
} // namespace dx
__SP_NAMESPACE_CLOSE // namespace sp




#endif // _WIN32
