/*
    CONTRIBUTORS:
        Sean Pesce

    References:
        https://msdn.microsoft.com/en-us/library/windows/desktop/bb174527(v=vs.85).aspx
*/
#ifdef _WIN32

#ifndef SP_DX_DXGI_IDXGI_DEVICE_WRAPPER_CLASS_H_
#error Invalid header inclusion "dx/dxgi/interface/device/idxgidevice_impl.hpp" (Use "dx/dxgi/interface/device/idxgidevice.h" instead)
#endif

#include "sp/sp.h"


__SP_NAMESPACE
namespace dx   {
namespace dxgi {



template <typename interface_t>
HRESULT STDMETHODCALLTYPE device<interface_t>::GetAdapter(_COM_Outptr_  IDXGIAdapter **pAdapter)
{
    HRESULT result = this->_interface->GetAdapter(pAdapter);

    return result;
}

template <typename interface_t>
HRESULT STDMETHODCALLTYPE device<interface_t>::CreateSurface(_In_  const DXGI_SURFACE_DESC *pDesc, UINT NumSurfaces, DXGI_USAGE Usage, _In_opt_  const DXGI_SHARED_RESOURCE *pSharedResource, _COM_Outptr_  IDXGISurface **ppSurface)
{
    HRESULT result = this->_interface->CreateSurface(pDesc, NumSurfaces, Usage, pSharedResource, ppSurface);

    return result;
}


template <typename interface_t>
HRESULT STDMETHODCALLTYPE device<interface_t>::QueryResourceResidency(_In_reads_(NumResources)  IUnknown *const *ppResources, _Out_writes_(NumResources)  DXGI_RESIDENCY *pResidencyStatus, UINT NumResources)
{
    HRESULT result = this->_interface->QueryResourceResidency(ppResources, pResidencyStatus, NumResources);

    return result;
}


template <typename interface_t>
HRESULT STDMETHODCALLTYPE device<interface_t>::SetGPUThreadPriority(INT Priority)
{
    HRESULT result = this->_interface->SetGPUThreadPriority(Priority);

    return result;
}


template <typename interface_t>
HRESULT STDMETHODCALLTYPE device<interface_t>::GetGPUThreadPriority(_Out_  INT *pPriority)
{
    HRESULT result = this->_interface->GetGPUThreadPriority(pPriority);

    return result;
}


} // namespace dxgi
} // namespace dx
__SP_NAMESPACE_CLOSE // namespace sp


#endif // _WIN32
