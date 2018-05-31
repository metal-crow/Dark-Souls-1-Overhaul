/*
    CONTRIBUTORS:
        Sean Pesce

    References:
        https://msdn.microsoft.com/en-us/library/windows/desktop/mt427785(v=vs.85).aspx
*/
#ifdef _WIN32

#ifndef SP_DX_DXGI_IDXGIFACTORY4_WRAPPER_CLASS_H_
#error Invalid header inclusion "dx/dxgi/interface/factory/idxgifactory4_impl.hpp" (Use "dx/dxgi/interface/factory/idxgifactory4.h" instead)
#endif

#include "sp/sp.h"


__SP_NAMESPACE
namespace dx   {
namespace dxgi {


template <typename interface_t>
HRESULT factory4<interface_t>::EnumAdapterByLuid(LUID AdapterLuid, REFIID riid, void **ppvAdapter)
{
    return this->_interface->EnumAdapterByLuid(AdapterLuid, riid, ppvAdapter);
}


template <typename interface_t>
HRESULT factory4<interface_t>::EnumWarpAdapter(REFIID riid, void **ppvAdapter)
{
    return this->_interface->EnumWarpAdapter(riid, ppvAdapter);
}


} // namespace dxgi
} // namespace dx
__SP_NAMESPACE_CLOSE // namespace sp


#endif // _WIN32
