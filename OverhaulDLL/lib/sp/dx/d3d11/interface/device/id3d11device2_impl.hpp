/*
    CONTRIBUTORS:
        Sean Pesce

    References:
        https://msdn.microsoft.com/en-us/library/windows/desktop/dn280493%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396
*/
#ifdef _WIN32

#ifndef SP_DX_D3D11_ID3D11DEVICE2_WRAPPER_CLASS_H_
#error Invalid header inclusion "dx/d3d11/interface/device/id3d11device2_impl.hpp" (Use "dx/d3d11/interface/device/id3d11device2.h" instead)
#endif

#include "sp/sp.h"


__SP_NAMESPACE
namespace dx    {
namespace d3d11 {



template <typename interface_t>
void STDMETHODCALLTYPE device2<interface_t>::GetImmediateContext2(_Out_  ID3D11DeviceContext2 **ppImmediateContext)
{
    _interface->GetImmediateContext2(ppImmediateContext);
}



template <typename interface_t>
HRESULT STDMETHODCALLTYPE device2<interface_t>::CreateDeferredContext2(UINT ContextFlags, _Out_opt_  ID3D11DeviceContext2 **ppDeferredContext)
{
    HRESULT result = _interface->CreateDeferredContext2(ContextFlags, ppDeferredContext);

    return result;
}



template <typename interface_t>
void STDMETHODCALLTYPE device2<interface_t>::GetResourceTiling(
    _In_  ID3D11Resource *pTiledResource,
    _Out_opt_  UINT *pNumTilesForEntireResource,
    _Out_opt_  D3D11_PACKED_MIP_DESC *pPackedMipDesc,
    _Out_opt_  D3D11_TILE_SHAPE *pStandardTileShapeForNonPackedMips,
    _Inout_opt_  UINT *pNumSubresourceTilings,
    _In_  UINT FirstSubresourceTilingToGet,
    _Out_writes_(*pNumSubresourceTilings)  D3D11_SUBRESOURCE_TILING *pSubresourceTilingsForNonPackedMips )
{
    _interface->GetResourceTiling(
                    pTiledResource,
                    pNumTilesForEntireResource,
                    pPackedMipDesc,
                    pStandardTileShapeForNonPackedMips,
                    pNumSubresourceTilings,
                    FirstSubresourceTilingToGet,
                    pSubresourceTilingsForNonPackedMips);
}



template <typename interface_t>
HRESULT STDMETHODCALLTYPE device2<interface_t>::CheckMultisampleQualityLevels1(
    _In_  DXGI_FORMAT Format,
    _In_  UINT SampleCount,
    _In_  UINT Flags,
    _Out_  UINT *pNumQualityLevels )
{
    HRESULT result = _interface->CheckMultisampleQualityLevels1(Format, SampleCount, Flags, pNumQualityLevels);

    return result;
}



} // namespace d3d11
} // namespace dx
__SP_NAMESPACE_CLOSE // namespace sp


#endif // _WIN32
