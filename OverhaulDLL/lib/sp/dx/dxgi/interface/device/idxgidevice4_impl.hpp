/*
    CONTRIBUTORS:
        Sean Pesce

    References:
        https://msdn.microsoft.com/en-us/library/windows/desktop/mt732704(v=vs.85).aspx
*/
#ifdef _WIN32

#ifndef SP_DX_DXGI_IDXGI_DEVICE4_WRAPPER_CLASS_H_
#error Invalid header inclusion "dx/dxgi/interface/device/idxgidevice4_impl.hpp" (Use "dx/dxgi/interface/device/idxgidevice4.h" instead)
#endif

#include "sp/sp.h"


__SP_NAMESPACE
namespace dx   {
namespace dxgi {


template <typename interface_t>
HRESULT STDMETHODCALLTYPE device4<interface_t>::OfferResources1(_In_  UINT NumResources, _In_reads_(NumResources)  IDXGIResource *const *ppResources, _In_  DXGI_OFFER_RESOURCE_PRIORITY Priority, _In_  UINT Flags)
{
    HRESULT result = this->_interface->OfferResources1(NumResources, ppResources, Priority, Flags);

    return result;
}


template <typename interface_t>
HRESULT STDMETHODCALLTYPE device4<interface_t>::ReclaimResources1(_In_  UINT NumResources, _In_reads_(NumResources)  IDXGIResource *const *ppResources, _Out_writes_all_(NumResources)  DXGI_RECLAIM_RESOURCE_RESULTS *pResults)
{
    HRESULT result = this->_interface->ReclaimResources1(NumResources, ppResources, pResults);

    return result;
}


} // namespace dxgi
} // namespace dx
__SP_NAMESPACE_CLOSE // namespace sp


#endif // _WIN32
