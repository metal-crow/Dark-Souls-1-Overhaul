/*
    CONTRIBUTORS:
        Sean Pesce

    References:
        https://msdn.microsoft.com/en-us/library/windows/desktop/dn899218(v=vs.85).aspx
*/
#ifdef _WIN32

#ifndef SP_DX_D3D11_ID3D11DEVICE3_WRAPPER_CLASS_H_
#error Invalid header inclusion "dx/d3d11/interface/device/id3d11device3_impl.hpp" (Use "dx/d3d11/interface/device/id3d11device3.h" instead)
#endif

#include "sp/sp.h"


__SP_NAMESPACE
namespace dx    {
namespace d3d11 {



template <typename interface_t>
HRESULT STDMETHODCALLTYPE device3<interface_t>::CreateTexture2D1(
    _In_  const D3D11_TEXTURE2D_DESC1 *pDesc1,
    _In_reads_opt_(_Inexpressible_(pDesc1->MipLevels * pDesc1->ArraySize))  const D3D11_SUBRESOURCE_DATA *pInitialData,
    _COM_Outptr_opt_  ID3D11Texture2D1 **ppTexture2D )
{
    HRESULT result = this->_interface->CreateTexture2D1(pDesc1, pInitialData, ppTexture2D);

    return result;
}



template <typename interface_t>
HRESULT STDMETHODCALLTYPE device3<interface_t>::CreateTexture3D1(
    _In_  const D3D11_TEXTURE3D_DESC1 *pDesc1,
    _In_reads_opt_(_Inexpressible_(pDesc1->MipLevels))  const D3D11_SUBRESOURCE_DATA *pInitialData,
    _COM_Outptr_opt_  ID3D11Texture3D1 **ppTexture3D )
{
    HRESULT result = this->_interface->CreateTexture3D1(pDesc1, pInitialData, ppTexture3D);

    return result;
}



template <typename interface_t>
HRESULT STDMETHODCALLTYPE device3<interface_t>::CreateRasterizerState2(
    _In_  const D3D11_RASTERIZER_DESC2 *pRasterizerDesc,
    _COM_Outptr_opt_  ID3D11RasterizerState2 **ppRasterizerState )
{
    HRESULT result = this->_interface->CreateRasterizerState2(pRasterizerDesc, ppRasterizerState);

    return result;
}



template <typename interface_t>
HRESULT STDMETHODCALLTYPE device3<interface_t>::CreateShaderResourceView1(
    _In_  ID3D11Resource *pResource,
    _In_opt_  const D3D11_SHADER_RESOURCE_VIEW_DESC1 *pDesc1,
    _COM_Outptr_opt_  ID3D11ShaderResourceView1 **ppSRView1 )
{
    HRESULT result = this->_interface->CreateShaderResourceView1(pResource, pDesc1, ppSRView1);

    return result;
}



template <typename interface_t>
HRESULT STDMETHODCALLTYPE device3<interface_t>::CreateUnorderedAccessView1(
    _In_  ID3D11Resource *pResource,
    _In_opt_  const D3D11_UNORDERED_ACCESS_VIEW_DESC1 *pDesc1,
    _COM_Outptr_opt_  ID3D11UnorderedAccessView1 **ppUAView1 )
{
    HRESULT result = this->_interface->CreateUnorderedAccessView1(pResource, pDesc1, ppUAView1);

    return result;
}



template <typename interface_t>
HRESULT STDMETHODCALLTYPE device3<interface_t>::CreateRenderTargetView1(
    _In_  ID3D11Resource *pResource,
    _In_opt_  const D3D11_RENDER_TARGET_VIEW_DESC1 *pDesc1,
    _COM_Outptr_opt_  ID3D11RenderTargetView1 **ppRTView1 )
{
    HRESULT result = this->_interface->CreateRenderTargetView1(pResource, pDesc1, ppRTView1);

    return result;
}



template <typename interface_t>
HRESULT STDMETHODCALLTYPE device3<interface_t>::CreateQuery1(
    _In_  const D3D11_QUERY_DESC1 *pQueryDesc1,
    _COM_Outptr_opt_  ID3D11Query1 **ppQuery1 )
{
    HRESULT result = this->_interface->CreateQuery1(pQueryDesc1, ppQuery1);

    return result;
}



template <typename interface_t>
void STDMETHODCALLTYPE device3<interface_t>::GetImmediateContext3(
    _Outptr_  ID3D11DeviceContext3 **ppImmediateContext )
{
    this->_interface->GetImmediateContext3(ppImmediateContext);
}



template <typename interface_t>
HRESULT STDMETHODCALLTYPE device3<interface_t>::CreateDeferredContext3(UINT ContextFlags, _COM_Outptr_opt_  ID3D11DeviceContext3 **ppDeferredContext )
{
    HRESULT result = this->_interface->CreateDeferredContext3(ContextFlags, ppDeferredContext);

    return result;
}



template <typename interface_t>
void STDMETHODCALLTYPE device3<interface_t>::WriteToSubresource(
    _In_  ID3D11Resource *pDstResource,
    _In_  UINT DstSubresource,
    _In_opt_  const D3D11_BOX *pDstBox,
    _In_  const void *pSrcData,
    _In_  UINT SrcRowPitch,
    _In_  UINT SrcDepthPitch )
{
    this->_interface->WriteToSubresource(pDstResource, DstSubresource, pDstBox, pSrcData, SrcRowPitch, SrcDepthPitch);
}



template <typename interface_t>
void STDMETHODCALLTYPE device3<interface_t>::ReadFromSubresource(
    _Out_  void *pDstData,
    _In_  UINT DstRowPitch,
    _In_  UINT DstDepthPitch,
    _In_  ID3D11Resource *pSrcResource,
    _In_  UINT SrcSubresource,
    _In_opt_  const D3D11_BOX *pSrcBox )
{
    this->_interface->ReadFromSubresource(pDstData, DstRowPitch, DstDepthPitch, pSrcResource, SrcSubresource, pSrcBox);
}



} // namespace d3d11
} // namespace dx
__SP_NAMESPACE_CLOSE // namespace sp


#endif // _WIN32
