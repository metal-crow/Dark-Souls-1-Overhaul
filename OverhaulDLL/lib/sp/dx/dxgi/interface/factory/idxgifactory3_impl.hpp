/*
    CONTRIBUTORS:
        Sean Pesce

    References:
        https://msdn.microsoft.com/en-us/library/windows/desktop/dn457942(v=vs.85).aspx
*/
#ifdef _WIN32

#ifndef SP_DX_DXGI_IDXGIFACTORY3_WRAPPER_CLASS_H_
#error Invalid header inclusion "dx/dxgi/interface/factory/idxgifactory3_impl.hpp" (Use "dx/dxgi/interface/factory/idxgifactory3.h" instead)
#endif

#include "sp/sp.h"


__SP_NAMESPACE
namespace dx   {
namespace dxgi {


template <typename interface_t>
UINT factory3<interface_t>::GetCreationFlags()
{
    return this->_interface->GetCreationFlags();
}


} // namespace dxgi
} // namespace dx
__SP_NAMESPACE_CLOSE // namespace sp


#endif // _WIN32
