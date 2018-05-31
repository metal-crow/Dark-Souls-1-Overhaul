/*
    CONTRIBUTORS:
        Sean Pesce

    References:
        https://msdn.microsoft.com/en-us/library/windows/desktop/bb174535(v=vs.85).aspx
*/
#ifdef _WIN32

#ifndef SP_DX_DXGI_IDXGIFACTORY_WRAPPER_CLASS_H_
#error Invalid header inclusion "dx/dxgi/interface/factory/idxgifactory_impl.hpp" (Use "dx/dxgi/interface/factory/idxgifactory.h" instead)
#endif

#include "sp/sp.h"


__SP_NAMESPACE
namespace dx   {
namespace dxgi {


template <typename interface_t>
HRESULT factory<interface_t>::CreateSoftwareAdapter(HMODULE Module, IDXGIAdapter **ppAdapter)
{
    return this->_interface->CreateSoftwareAdapter(Module, ppAdapter);
}


template <typename interface_t>
HRESULT factory<interface_t>::CreateSwapChain(IUnknown *pDevice, DXGI_SWAP_CHAIN_DESC *pDesc, IDXGISwapChain **ppSwapChain)
{
    return this->_interface->CreateSwapChain(pDevice, pDesc, ppSwapChain);
}


template <typename interface_t>
HRESULT factory<interface_t>::EnumAdapters(UINT Adapter, IDXGIAdapter **ppAdapter)
{
    return this->_interface->EnumAdapters(Adapter, ppAdapter);
}


template <typename interface_t>
HRESULT factory<interface_t>::GetWindowAssociation(HWND *pWindowHandle)
{
    return this->_interface->GetWindowAssociation(pWindowHandle);
}


template <typename interface_t>
HRESULT factory<interface_t>::MakeWindowAssociation(HWND WindowHandle, UINT Flags)
{
    return this->_interface->MakeWindowAssociation(WindowHandle, Flags);
}


} // namespace dxgi
} // namespace dx
__SP_NAMESPACE_CLOSE // namespace sp


#endif // _WIN32
