/*
    CONTRIBUTORS:
        Sean Pesce

    References:
        https://msdn.microsoft.com/en-us/library/windows/desktop/dn280345(v=vs.85).aspx
*/
#ifdef _WIN32

#ifndef SP_DX_DXGI_IDXGI_DEVICE3_WRAPPER_CLASS_H_
#error Invalid header inclusion "dx/dxgi/interface/device/idxgidevice3_impl.hpp" (Use "dx/dxgi/interface/device/idxgidevice3.h" instead)
#endif

#include "sp/sp.h"


__SP_NAMESPACE
namespace dx   {
namespace dxgi {


template <typename interface_t>
void STDMETHODCALLTYPE device3<interface_t>::Trim()
{
    this->_interface->Trim();
}




} // namespace dxgi
} // namespace dx
__SP_NAMESPACE_CLOSE // namespace sp


#endif // _WIN32
