/*
    CONTRIBUTORS:
        Sean Pesce

    References:
        https://msdn.microsoft.com/en-us/library/windows/desktop/dn280493%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396
*/
#ifdef _WIN32
#pragma once

#ifndef SP_DX_D3D11_ID3D11DEVICE2_WRAPPER_CLASS_H_
#define SP_DX_D3D11_ID3D11DEVICE2_WRAPPER_CLASS_H_

#include "sp/dx/d3d11/interface/device/id3d11device1.h"
#include "D3D11_2.h"


__SP_NAMESPACE
namespace dx    {
namespace d3d11 {


template <class interface_t = ID3D11Device2>
class device2 : public device1<interface_t> {
protected:

    virtual inline void _wrap(interface_t **original) override
    {
        this->_interface = *original;
        *original = this;
    }

public:
    device2() {}

    device2(interface_t **original)
    {
        if (original && *original)
        {
            SP_LOG_DBG("[D3D11] Device created (UUID = %s)\n", sp::os::win::com::guid::to_string(__uuidof(**original)).c_str());
        }
        this->_wrap(original);
    }

    virtual ~device2() {}



    // Wrapped interface methods


    virtual void STDMETHODCALLTYPE GetImmediateContext2(_Out_  ID3D11DeviceContext2 **ppImmediateContext);



    virtual HRESULT STDMETHODCALLTYPE CreateDeferredContext2(UINT ContextFlags, _Out_opt_  ID3D11DeviceContext2 **ppDeferredContext);



    virtual void STDMETHODCALLTYPE GetResourceTiling(
                _In_  ID3D11Resource *pTiledResource,
                _Out_opt_  UINT *pNumTilesForEntireResource,
                _Out_opt_  D3D11_PACKED_MIP_DESC *pPackedMipDesc,
                _Out_opt_  D3D11_TILE_SHAPE *pStandardTileShapeForNonPackedMips,
                _Inout_opt_  UINT *pNumSubresourceTilings,
                _In_  UINT FirstSubresourceTilingToGet,
                _Out_writes_(*pNumSubresourceTilings)  D3D11_SUBRESOURCE_TILING *pSubresourceTilingsForNonPackedMips
    );



    virtual HRESULT STDMETHODCALLTYPE CheckMultisampleQualityLevels1(
        _In_  DXGI_FORMAT Format,
        _In_  UINT SampleCount,
        _In_  UINT Flags,
        _Out_ UINT *pNumQualityLevels
    );


}; // class dx::d3d11::device2


} // namespace d3d11
} // namespace dx
__SP_NAMESPACE_CLOSE // namespace sp


template class sp::dx::d3d11::device2<ID3D11Device2>;


// Implementation
#include "sp/dx/d3d11/interface/device/id3d11device2_impl.hpp"


#endif // SP_DX_D3D11_ID3D11DEVICE2_WRAPPER_CLASS_H_


#endif // _WIN32
