/*
    CONTRIBUTORS:
        Sean Pesce

    References:
        https://msdn.microsoft.com/en-us/library/windows/desktop/hh404598(v=vs.85).aspx
*/
#ifdef _WIN32

#ifndef SP_DX_D3D11_ID3D11DEVICE_CONTEXT1_WRAPPER_CLASS_H_
#error Invalid header inclusion "dx/d3d11/interface/context/id3d11devicecontext1_impl.hpp" (Use "dx/d3d11/interface/context/id3d11devicecontext1.h" instead)
#endif

#include "sp/sp.h"


__SP_NAMESPACE
namespace dx    {
namespace d3d11 {



template <typename interface_t>
void STDMETHODCALLTYPE context1<interface_t>::CopySubresourceRegion1(_In_ ID3D11Resource *pDstResource, _In_ UINT DstSubresource, _In_ UINT DstX, _In_ UINT DstY, _In_ UINT DstZ, _In_ ID3D11Resource *pSrcResource, _In_ UINT SrcSubresource, _In_opt_ const D3D11_BOX *pSrcBox, _In_ UINT CopyFlags)
{
    this->_interface->CopySubresourceRegion1(pDstResource, DstSubresource, DstX, DstY, DstZ, pSrcResource, SrcSubresource, pSrcBox, CopyFlags);
}


template <typename interface_t>
void STDMETHODCALLTYPE context1<interface_t>::UpdateSubresource1(_In_ ID3D11Resource *pDstResource, _In_ UINT DstSubresource, _In_opt_ const D3D11_BOX *pDstBox, _In_ const void *pSrcData, _In_ UINT SrcRowPitch, _In_ UINT SrcDepthPitch, _In_ UINT CopyFlags)
{
    this->_interface->UpdateSubresource1(pDstResource, DstSubresource, pDstBox, pSrcData, SrcRowPitch, SrcDepthPitch, CopyFlags);
}


template <typename interface_t>
void STDMETHODCALLTYPE context1<interface_t>::DiscardResource(_In_ ID3D11Resource *pResource)
{
    this->_interface->DiscardResource(pResource);
}


template <typename interface_t>
void STDMETHODCALLTYPE context1<interface_t>::DiscardView(_In_ ID3D11View *pResourceView)
{
    this->_interface->DiscardView(pResourceView);
}


template <typename interface_t>
void STDMETHODCALLTYPE context1<interface_t>::VSSetConstantBuffers1(_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)  UINT NumBuffers, _In_reads_opt_(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers, _In_reads_opt_(NumBuffers)  const UINT *pFirstConstant, _In_reads_opt_(NumBuffers)  const UINT *pNumConstants)
{
    this->_interface->VSSetConstantBuffers1(StartSlot, NumBuffers, ppConstantBuffers, pFirstConstant, pNumConstants);
}


template <typename interface_t>
void STDMETHODCALLTYPE context1<interface_t>::HSSetConstantBuffers1(_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)  UINT NumBuffers, _In_reads_opt_(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers, _In_reads_opt_(NumBuffers)  const UINT *pFirstConstant, _In_reads_opt_(NumBuffers)  const UINT *pNumConstants)
{
    this->_interface->HSSetConstantBuffers1(StartSlot, NumBuffers, ppConstantBuffers, pFirstConstant, pNumConstants);
}


template <typename interface_t>
void STDMETHODCALLTYPE context1<interface_t>::DSSetConstantBuffers1(_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)  UINT NumBuffers, _In_reads_opt_(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers, _In_reads_opt_(NumBuffers)  const UINT *pFirstConstant, _In_reads_opt_(NumBuffers)  const UINT *pNumConstants)
{
    this->_interface->DSSetConstantBuffers1(StartSlot, NumBuffers, ppConstantBuffers, pFirstConstant, pNumConstants);
}


template <typename interface_t>
void STDMETHODCALLTYPE context1<interface_t>::GSSetConstantBuffers1(_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)  UINT NumBuffers, _In_reads_opt_(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers, _In_reads_opt_(NumBuffers)  const UINT *pFirstConstant, _In_reads_opt_(NumBuffers)  const UINT *pNumConstants)
{
    this->_interface->GSSetConstantBuffers1(StartSlot, NumBuffers, ppConstantBuffers, pFirstConstant, pNumConstants);
}


template <typename interface_t>
void STDMETHODCALLTYPE context1<interface_t>::PSSetConstantBuffers1(_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)  UINT NumBuffers, _In_reads_opt_(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers, _In_reads_opt_(NumBuffers)  const UINT *pFirstConstant, _In_reads_opt_(NumBuffers)  const UINT *pNumConstants)
{
    this->_interface->PSSetConstantBuffers1(StartSlot, NumBuffers, ppConstantBuffers, pFirstConstant, pNumConstants);
}


template <typename interface_t>
void STDMETHODCALLTYPE context1<interface_t>::CSSetConstantBuffers1(_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)  UINT NumBuffers, _In_reads_opt_(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers, _In_reads_opt_(NumBuffers)  const UINT *pFirstConstant, _In_reads_opt_(NumBuffers)  const UINT *pNumConstants)
{
    this->_interface->CSSetConstantBuffers1(StartSlot, NumBuffers, ppConstantBuffers, pFirstConstant, pNumConstants);
}


template <typename interface_t>
void STDMETHODCALLTYPE context1<interface_t>::VSGetConstantBuffers1(_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)  UINT NumBuffers, _Out_writes_opt_(NumBuffers)  ID3D11Buffer **ppConstantBuffers, _Out_writes_opt_(NumBuffers)  UINT *pFirstConstant, _Out_writes_opt_(NumBuffers)  UINT *pNumConstants)
{
    this->_interface->VSGetConstantBuffers1(StartSlot, NumBuffers, ppConstantBuffers, pFirstConstant, pNumConstants);
}


template <typename interface_t>
void STDMETHODCALLTYPE context1<interface_t>::HSGetConstantBuffers1(_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)  UINT NumBuffers, _Out_writes_opt_(NumBuffers)  ID3D11Buffer **ppConstantBuffers, _Out_writes_opt_(NumBuffers)  UINT *pFirstConstant, _Out_writes_opt_(NumBuffers)  UINT *pNumConstants)
{
    this->_interface->HSGetConstantBuffers1(StartSlot, NumBuffers, ppConstantBuffers, pFirstConstant, pNumConstants);
}


template <typename interface_t>
void STDMETHODCALLTYPE context1<interface_t>::DSGetConstantBuffers1(_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)  UINT NumBuffers, _Out_writes_opt_(NumBuffers)  ID3D11Buffer **ppConstantBuffers, _Out_writes_opt_(NumBuffers)  UINT *pFirstConstant, _Out_writes_opt_(NumBuffers)  UINT *pNumConstants)
{
    this->_interface->DSGetConstantBuffers1(StartSlot, NumBuffers, ppConstantBuffers, pFirstConstant, pNumConstants);
}


template <typename interface_t>
void STDMETHODCALLTYPE context1<interface_t>::GSGetConstantBuffers1(_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)  UINT NumBuffers, _Out_writes_opt_(NumBuffers)  ID3D11Buffer **ppConstantBuffers, _Out_writes_opt_(NumBuffers)  UINT *pFirstConstant, _Out_writes_opt_(NumBuffers)  UINT *pNumConstants)
{
    this->_interface->GSGetConstantBuffers1(StartSlot, NumBuffers, ppConstantBuffers, pFirstConstant, pNumConstants);
}


template <typename interface_t>
void STDMETHODCALLTYPE context1<interface_t>::PSGetConstantBuffers1(_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)  UINT NumBuffers, _Out_writes_opt_(NumBuffers)  ID3D11Buffer **ppConstantBuffers, _Out_writes_opt_(NumBuffers)  UINT *pFirstConstant, _Out_writes_opt_(NumBuffers)  UINT *pNumConstants)
{
    this->_interface->PSGetConstantBuffers1(StartSlot, NumBuffers, ppConstantBuffers, pFirstConstant, pNumConstants);
}


template <typename interface_t>
void STDMETHODCALLTYPE context1<interface_t>::CSGetConstantBuffers1(_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)  UINT StartSlot, _In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)  UINT NumBuffers, _Out_writes_opt_(NumBuffers)  ID3D11Buffer **ppConstantBuffers, _Out_writes_opt_(NumBuffers)  UINT *pFirstConstant, _Out_writes_opt_(NumBuffers)  UINT *pNumConstants)
{
    this->_interface->CSGetConstantBuffers1(StartSlot, NumBuffers, ppConstantBuffers, pFirstConstant, pNumConstants);
}


template <typename interface_t>
void STDMETHODCALLTYPE context1<interface_t>::SwapDeviceContextState(_In_ ID3DDeviceContextState *pState, _Outptr_opt_  ID3DDeviceContextState **ppPreviousState)
{
    this->_interface->SwapDeviceContextState(pState, ppPreviousState);
}


template <typename interface_t>
void STDMETHODCALLTYPE context1<interface_t>::ClearView(_In_ ID3D11View *pView, _In_ const FLOAT Color[4], _In_reads_opt_(NumRects)  const D3D11_RECT *pRect, UINT NumRects)
{
    this->_interface->ClearView(pView, Color, pRect, NumRects);
}


template <typename interface_t>
void STDMETHODCALLTYPE context1<interface_t>::DiscardView1(_In_ ID3D11View *pResourceView, _In_reads_opt_(NumRects)  const D3D11_RECT *pRects, UINT NumRects)
{
    this->_interface->DiscardView1(pResourceView, pRects, NumRects);
}




} // namespace d3d11
} // namespace dx
__SP_NAMESPACE_CLOSE // namespace sp




#endif // _WIN32
