/*
    CONTRIBUTORS:
        Sean Pesce

    References:
        https://msdn.microsoft.com/en-us/library/windows/desktop/dn280498(v=vs.85).aspx
*/
#ifdef _WIN32
#pragma once

#ifndef SP_DX_D3D11_ID3D11DEVICE_CONTEXT2_WRAPPER_CLASS_H_
#define SP_DX_D3D11_ID3D11DEVICE_CONTEXT2_WRAPPER_CLASS_H_


#include "sp/dx/d3d11/interface/context/id3d11devicecontext1.h"
#include "sp/log.h"
#include "D3D11_2.h"



__SP_NAMESPACE
namespace dx {
namespace d3d11 {


template <class interface_t = ID3D11DeviceContext2 >
class context2 : public context1 <interface_t> {
protected:

    virtual inline void _wrap(interface_t **original) override
    {
        this->_interface = *original;
        *original = this;
    }

public:
    context2 () {}

    context2 (interface_t **original)
    {
        if (original && *original)
        {
            SP_LOG_DBG("[D3D11] Device context created (UUID = %s)\n", sp::os::win::com::guid::to_string(__uuidof(**original)).c_str());
        }
        this->_wrap(original);
    }

    virtual ~context2 () {}


    // Wrapped interface methods


    virtual HRESULT STDMETHODCALLTYPE UpdateTileMappings(
        _In_  ID3D11Resource *pTiledResource,
        _In_  UINT NumTiledResourceRegions,
        _In_reads_opt_(NumTiledResourceRegions)  const D3D11_TILED_RESOURCE_COORDINATE *pTiledResourceRegionStartCoordinates,
        _In_reads_opt_(NumTiledResourceRegions)  const D3D11_TILE_REGION_SIZE *pTiledResourceRegionSizes,
        _In_opt_  ID3D11Buffer *pTilePool,
        _In_  UINT NumRanges,
        _In_reads_opt_(NumRanges)  const UINT *pRangeFlags,
        _In_reads_opt_(NumRanges)  const UINT *pTilePoolStartOffsets,
        _In_reads_opt_(NumRanges)  const UINT *pRangeTileCounts,
        _In_  UINT Flags
    );


    virtual HRESULT STDMETHODCALLTYPE CopyTileMappings(
        _In_  ID3D11Resource *pDestTiledResource,
        _In_  const D3D11_TILED_RESOURCE_COORDINATE *pDestRegionStartCoordinate,
        _In_  ID3D11Resource *pSourceTiledResource,
        _In_  const D3D11_TILED_RESOURCE_COORDINATE *pSourceRegionStartCoordinate,
        _In_  const D3D11_TILE_REGION_SIZE *pTileRegionSize,
        _In_  UINT Flags
    );


    virtual void STDMETHODCALLTYPE CopyTiles(
        _In_  ID3D11Resource *pTiledResource,
        _In_  const D3D11_TILED_RESOURCE_COORDINATE *pTileRegionStartCoordinate,
        _In_  const D3D11_TILE_REGION_SIZE *pTileRegionSize,
        _In_  ID3D11Buffer *pBuffer,
        _In_  UINT64 BufferStartOffsetInBytes,
        _In_  UINT Flags
    );


    virtual void STDMETHODCALLTYPE UpdateTiles(
        _In_  ID3D11Resource *pDestTiledResource,
        _In_  const D3D11_TILED_RESOURCE_COORDINATE *pDestTileRegionStartCoordinate,
        _In_  const D3D11_TILE_REGION_SIZE *pDestTileRegionSize,
        _In_  const void *pSourceTileData,
        _In_  UINT Flags
    );


    virtual HRESULT STDMETHODCALLTYPE ResizeTilePool(_In_ ID3D11Buffer *pTilePool, _In_ UINT64 NewSizeInBytes);


    virtual void STDMETHODCALLTYPE TiledResourceBarrier(
        _In_opt_  ID3D11DeviceChild *pTiledResourceOrViewAccessBeforeBarrier,
        _In_opt_  ID3D11DeviceChild *pTiledResourceOrViewAccessAfterBarrier
    );


    virtual BOOL STDMETHODCALLTYPE IsAnnotationEnabled();


    virtual void STDMETHODCALLTYPE SetMarkerInt(_In_ LPCWSTR pLabel, INT Data);


    virtual void STDMETHODCALLTYPE BeginEventInt(_In_ LPCWSTR pLabel, INT Data);


    virtual void STDMETHODCALLTYPE EndEvent();


}; // class dx::d3d11::context2



} // namespace d3d11
} // namespace dx
__SP_NAMESPACE_CLOSE // namespace sp


template class sp::dx::d3d11::context2<ID3D11DeviceContext2>;


// Implementation
#include "sp/dx/d3d11/interface/context/id3d11devicecontext2_impl.hpp"


#endif // SP_DX_D3D11_ID3D11DEVICE_CONTEXT2_WRAPPER_CLASS_H_


#endif // _WIN32
