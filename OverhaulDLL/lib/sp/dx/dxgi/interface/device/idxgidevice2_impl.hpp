/*
    CONTRIBUTORS:
        Sean Pesce

    References:
        https://msdn.microsoft.com/en-us/library/windows/desktop/hh404543(v=vs.85).aspx
*/
#ifdef _WIN32

#ifndef SP_DX_DXGI_IDXGI_DEVICE2_WRAPPER_CLASS_H_
#error Invalid header inclusion "dx/dxgi/interface/device/idxgidevice2_impl.hpp" (Use "dx/dxgi/interface/device/idxgidevice2.h" instead)
#endif

#include "sp/sp.h"


__SP_NAMESPACE
namespace dx   {
namespace dxgi {



template <typename interface_t>
HRESULT STDMETHODCALLTYPE device2<interface_t>::OfferResources(_In_  UINT NumResources, _In_reads_(NumResources)  IDXGIResource *const *ppResources, _In_  DXGI_OFFER_RESOURCE_PRIORITY Priority)
{
    HRESULT result = this->_interface->OfferResources(NumResources, ppResources, Priority);

    return result;
}


template <typename interface_t>
HRESULT STDMETHODCALLTYPE device2<interface_t>::ReclaimResources(_In_  UINT NumResources, _In_reads_(NumResources)  IDXGIResource *const *ppResources, _Out_writes_all_opt_(NumResources)  BOOL *pDiscarded)
{
    HRESULT result = this->_interface->ReclaimResources(NumResources, ppResources, pDiscarded);

    return result;
}


template <typename interface_t>
HRESULT STDMETHODCALLTYPE device2<interface_t>::EnqueueSetEvent(_In_  HANDLE hEvent)
{
    HRESULT result = this->_interface->EnqueueSetEvent(hEvent);

    return result;
}



} // namespace dxgi
} // namespace dx
__SP_NAMESPACE_CLOSE // namespace sp


#endif // _WIN32
