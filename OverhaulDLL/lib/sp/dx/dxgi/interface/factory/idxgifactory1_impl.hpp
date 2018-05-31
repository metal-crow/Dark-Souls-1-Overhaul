/*
    CONTRIBUTORS:
        Sean Pesce

    References:
        https://msdn.microsoft.com/en-us/library/windows/desktop/ff471335(v=vs.85).aspx
*/
#ifdef _WIN32

#ifndef SP_DX_DXGI_IDXGIFACTORY1_WRAPPER_CLASS_H_
#error Invalid header inclusion "dx/dxgi/interface/factory/idxgifactory1_impl.hpp" (Use "dx/dxgi/interface/factory/idxgifactory1.h" instead)
#endif

#include "sp/sp.h"


__SP_NAMESPACE
namespace dx   {
namespace dxgi {


template <typename interface_t>
HRESULT factory1<interface_t>::EnumAdapters1(UINT Adapter, IDXGIAdapter1 **ppAdapter)
{
    return this->_interface->EnumAdapters1(Adapter, ppAdapter);
}


template <typename interface_t>
BOOL factory1<interface_t>::IsCurrent()
{
    return this->_interface->IsCurrent();
}


} // namespace dxgi
} // namespace dx
__SP_NAMESPACE_CLOSE // namespace sp


#endif // _WIN32
