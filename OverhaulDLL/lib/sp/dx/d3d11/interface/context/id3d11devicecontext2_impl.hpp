/*
    CONTRIBUTORS:
        Sean Pesce

    References:
        https://msdn.microsoft.com/en-us/library/windows/desktop/dn280498(v=vs.85).aspx
*/
#ifdef _WIN32

#ifndef SP_DX_D3D11_ID3D11DEVICE_CONTEXT2_WRAPPER_CLASS_H_
#error Invalid header inclusion "dx/d3d11/interface/context/id3d11devicecontext2_impl.hpp" (Use "dx/d3d11/interface/context/id3d11devicecontext2.h" instead)
#endif

#include "sp/sp.h"


__SP_NAMESPACE
namespace dx    {
namespace d3d11 {



template <typename interface_t>
HRESULT STDMETHODCALLTYPE context2<interface_t>::UpdateTileMappings(
            _In_  ID3D11Resource *pTiledResource,
            _In_  UINT NumTiledResourceRegions,
            _In_reads_opt_(NumTiledResourceRegions)  const D3D11_TILED_RESOURCE_COORDINATE *pTiledResourceRegionStartCoordinates,
            _In_reads_opt_(NumTiledResourceRegions)  const D3D11_TILE_REGION_SIZE *pTiledResourceRegionSizes,
            _In_opt_  ID3D11Buffer *pTilePool,
            _In_  UINT NumRanges,
            _In_reads_opt_(NumRanges)  const UINT *pRangeFlags,
            _In_reads_opt_(NumRanges)  const UINT *pTilePoolStartOffsets,
            _In_reads_opt_(NumRanges)  const UINT *pRangeTileCounts,
            _In_  UINT Flags)
{
    HRESULT result = this->_interface->UpdateTileMappings(
                                            pTiledResource,
                                            NumTiledResourceRegions,
                                            pTiledResourceRegionStartCoordinates,
                                            pTiledResourceRegionSizes,
                                            pTilePool,
                                            NumRanges,
                                            pRangeFlags,
                                            pTilePoolStartOffsets,
                                            pRangeTileCounts,
                                            Flags);

    return result;
}



template <typename interface_t>
HRESULT STDMETHODCALLTYPE context2<interface_t>::CopyTileMappings(
            _In_  ID3D11Resource *pDestTiledResource,
            _In_  const D3D11_TILED_RESOURCE_COORDINATE *pDestRegionStartCoordinate,
            _In_  ID3D11Resource *pSourceTiledResource,
            _In_  const D3D11_TILED_RESOURCE_COORDINATE *pSourceRegionStartCoordinate,
            _In_  const D3D11_TILE_REGION_SIZE *pTileRegionSize,
            _In_  UINT Flags)
{
    HRESULT result = this->_interface->CopyTileMappings(
                                pDestTiledResource,
                                pDestRegionStartCoordinate,
                                pSourceTiledResource,
                                pSourceRegionStartCoordinate,
                                pTileRegionSize,
                                Flags);

    return result;
}



template <typename interface_t>
void STDMETHODCALLTYPE context2<interface_t>::CopyTiles(
            _In_  ID3D11Resource *pTiledResource,
            _In_  const D3D11_TILED_RESOURCE_COORDINATE *pTileRegionStartCoordinate,
            _In_  const D3D11_TILE_REGION_SIZE *pTileRegionSize,
            _In_  ID3D11Buffer *pBuffer,
            _In_  UINT64 BufferStartOffsetInBytes,
            _In_  UINT Flags)
{
    this->_interface->CopyTiles(
                        pTiledResource,
                        pTileRegionStartCoordinate,
                        pTileRegionSize,
                        pBuffer,
                        BufferStartOffsetInBytes,
                        Flags);
}



template <typename interface_t>
void STDMETHODCALLTYPE context2<interface_t>::UpdateTiles(
            _In_  ID3D11Resource *pDestTiledResource,
            _In_  const D3D11_TILED_RESOURCE_COORDINATE *pDestTileRegionStartCoordinate,
            _In_  const D3D11_TILE_REGION_SIZE *pDestTileRegionSize,
            _In_  const void *pSourceTileData,
            _In_  UINT Flags)
{
    this->_interface->UpdateTiles(
                        pDestTiledResource,
                        pDestTileRegionStartCoordinate,
                        pDestTileRegionSize,
                        pSourceTileData,
                        Flags);
}



template <typename interface_t>
HRESULT STDMETHODCALLTYPE context2<interface_t>::ResizeTilePool(_In_ ID3D11Buffer *pTilePool, _In_ UINT64 NewSizeInBytes)
{
    HRESULT result = this->_interface->ResizeTilePool(pTilePool, NewSizeInBytes);

    return result;
}



template <typename interface_t>
void STDMETHODCALLTYPE context2<interface_t>::TiledResourceBarrier(
            _In_opt_  ID3D11DeviceChild *pTiledResourceOrViewAccessBeforeBarrier,
            _In_opt_  ID3D11DeviceChild *pTiledResourceOrViewAccessAfterBarrier)
{
    this->_interface->TiledResourceBarrier(pTiledResourceOrViewAccessBeforeBarrier, pTiledResourceOrViewAccessAfterBarrier);
}



template <typename interface_t>
BOOL STDMETHODCALLTYPE context2<interface_t>::IsAnnotationEnabled()
{
    BOOL result = this->_interface->IsAnnotationEnabled();

    return result;
}


template <typename interface_t>
void STDMETHODCALLTYPE context2<interface_t>::SetMarkerInt(_In_ LPCWSTR pLabel, INT Data)
{
    this->_interface->SetMarkerInt(pLabel, Data);
}


template <typename interface_t>
void STDMETHODCALLTYPE context2<interface_t>::BeginEventInt(_In_ LPCWSTR pLabel, INT Data)
{
    this->_interface->BeginEventInt(pLabel, Data);
}


template <typename interface_t>
void STDMETHODCALLTYPE context2<interface_t>::EndEvent()
{
    this->_interface->EndEvent();
}





} // namespace d3d11
} // namespace dx
__SP_NAMESPACE_CLOSE // namespace sp




#endif // _WIN32
