/*
    CONTRIBUTORS:
        Sean Pesce

    References:
        https://msdn.microsoft.com/en-us/library/windows/desktop/hh404598(v=vs.85).aspx
*/
#ifdef _WIN32
#pragma once

#ifndef SP_DX_D3D11_ID3D11DEVICE_CONTEXT1_WRAPPER_CLASS_H_
#define SP_DX_D3D11_ID3D11DEVICE_CONTEXT1_WRAPPER_CLASS_H_


#include "sp/dx/d3d11/interface/context/id3d11devicecontext.h"
#include "sp/log.h"
#include "D3D11_1.h"



__SP_NAMESPACE
namespace dx {
namespace d3d11 {


template <class interface_t = ID3D11DeviceContext1>
class context1 : public context <interface_t> {
protected:
    virtual inline void _wrap(interface_t **original) override
    {
        this->_interface = *original;
        *original = this;
    }

public:
    context1 () {}

    context1 (interface_t **original)
    {
        if (original && *original)
        {
            SP_LOG_DBG("[D3D11] Device context created (UUID = %s)\n", sp::os::win::com::guid::to_string(__uuidof(**original)).c_str());
        }
        this->_wrap(original);
    }
    virtual ~context1 () {}


    // Wrapped interface methods


    virtual void STDMETHODCALLTYPE CopySubresourceRegion1(_In_ ID3D11Resource *pDstResource, _In_ UINT DstSubresource, _In_ UINT DstX, _In_ UINT DstY, _In_ UINT DstZ, _In_ ID3D11Resource *pSrcResource, _In_ UINT SrcSubresource, _In_opt_ const D3D11_BOX *pSrcBox, _In_ UINT CopyFlags);


    virtual void STDMETHODCALLTYPE UpdateSubresource1(_In_ ID3D11Resource *pDstResource, _In_ UINT DstSubresource, _In_opt_ const D3D11_BOX *pDstBox, _In_ const void *pSrcData, _In_ UINT SrcRowPitch, _In_ UINT SrcDepthPitch, _In_ UINT CopyFlags);


    virtual void STDMETHODCALLTYPE DiscardResource(_In_ ID3D11Resource *pResource);


    virtual void STDMETHODCALLTYPE DiscardView(_In_ ID3D11View *pResourceView);


    virtual void STDMETHODCALLTYPE VSSetConstantBuffers1(_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)  UINT NumBuffers, _In_reads_opt_(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers, _In_reads_opt_(NumBuffers)  const UINT *pFirstConstant, _In_reads_opt_(NumBuffers)  const UINT *pNumConstants);


    virtual void STDMETHODCALLTYPE HSSetConstantBuffers1(_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)  UINT NumBuffers, _In_reads_opt_(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers, _In_reads_opt_(NumBuffers)  const UINT *pFirstConstant, _In_reads_opt_(NumBuffers)  const UINT *pNumConstants);


    virtual void STDMETHODCALLTYPE DSSetConstantBuffers1(_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)  UINT NumBuffers, _In_reads_opt_(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers, _In_reads_opt_(NumBuffers)  const UINT *pFirstConstant, _In_reads_opt_(NumBuffers)  const UINT *pNumConstants);


    virtual void STDMETHODCALLTYPE GSSetConstantBuffers1(_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)  UINT NumBuffers, _In_reads_opt_(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers, _In_reads_opt_(NumBuffers)  const UINT *pFirstConstant, _In_reads_opt_(NumBuffers)  const UINT *pNumConstants);


    virtual void STDMETHODCALLTYPE PSSetConstantBuffers1(_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)  UINT NumBuffers, _In_reads_opt_(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers, _In_reads_opt_(NumBuffers)  const UINT *pFirstConstant, _In_reads_opt_(NumBuffers)  const UINT *pNumConstants);


    virtual void STDMETHODCALLTYPE CSSetConstantBuffers1(_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)  UINT NumBuffers, _In_reads_opt_(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers, _In_reads_opt_(NumBuffers)  const UINT *pFirstConstant, _In_reads_opt_(NumBuffers)  const UINT *pNumConstants);


    virtual void STDMETHODCALLTYPE VSGetConstantBuffers1(_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)  UINT NumBuffers, _Out_writes_opt_(NumBuffers)  ID3D11Buffer **ppConstantBuffers, _Out_writes_opt_(NumBuffers)  UINT *pFirstConstant, _Out_writes_opt_(NumBuffers)  UINT *pNumConstants);


    virtual void STDMETHODCALLTYPE HSGetConstantBuffers1(_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)  UINT NumBuffers, _Out_writes_opt_(NumBuffers)  ID3D11Buffer **ppConstantBuffers, _Out_writes_opt_(NumBuffers)  UINT *pFirstConstant, _Out_writes_opt_(NumBuffers)  UINT *pNumConstants);


    virtual void STDMETHODCALLTYPE DSGetConstantBuffers1(_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)  UINT NumBuffers, _Out_writes_opt_(NumBuffers)  ID3D11Buffer **ppConstantBuffers, _Out_writes_opt_(NumBuffers)  UINT *pFirstConstant, _Out_writes_opt_(NumBuffers)  UINT *pNumConstants);


    virtual void STDMETHODCALLTYPE GSGetConstantBuffers1(_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)  UINT NumBuffers, _Out_writes_opt_(NumBuffers)  ID3D11Buffer **ppConstantBuffers, _Out_writes_opt_(NumBuffers)  UINT *pFirstConstant, _Out_writes_opt_(NumBuffers)  UINT *pNumConstants);


    virtual void STDMETHODCALLTYPE PSGetConstantBuffers1(_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)  UINT NumBuffers, _Out_writes_opt_(NumBuffers)  ID3D11Buffer **ppConstantBuffers, _Out_writes_opt_(NumBuffers)  UINT *pFirstConstant, _Out_writes_opt_(NumBuffers)  UINT *pNumConstants);


    virtual void STDMETHODCALLTYPE CSGetConstantBuffers1(_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)  UINT NumBuffers, _Out_writes_opt_(NumBuffers)  ID3D11Buffer **ppConstantBuffers, _Out_writes_opt_(NumBuffers)  UINT *pFirstConstant, _Out_writes_opt_(NumBuffers)  UINT *pNumConstants);


    virtual void STDMETHODCALLTYPE SwapDeviceContextState(_In_ ID3DDeviceContextState *pState, _Outptr_opt_  ID3DDeviceContextState **ppPreviousState);


    virtual void STDMETHODCALLTYPE ClearView(_In_ ID3D11View *pView, _In_ const FLOAT Color[4], _In_reads_opt_(NumRects)  const D3D11_RECT *pRect, UINT NumRects);


    virtual void STDMETHODCALLTYPE DiscardView1(_In_ ID3D11View *pResourceView, _In_reads_opt_(NumRects)  const D3D11_RECT *pRects, UINT NumRects);

}; // class dx::d3d11::context1



} // namespace d3d11
} // namespace dx
__SP_NAMESPACE_CLOSE // namespace sp


template class sp::dx::d3d11::context1 <ID3D11DeviceContext1>;


// Implementation
#include "sp/dx/d3d11/interface/context/id3d11devicecontext1_impl.hpp"


#endif // SP_DX_D3D11_ID3D11DEVICE_CONTEXT1_WRAPPER_CLASS_H_


#endif // _WIN32
