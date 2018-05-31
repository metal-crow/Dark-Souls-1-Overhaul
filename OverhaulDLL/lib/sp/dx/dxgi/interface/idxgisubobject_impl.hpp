/*
    CONTRIBUTORS:
        Sean Pesce

    References:
        https://msdn.microsoft.com/en-us/library/windows/desktop/bb174528(v=vs.85).aspx
*/
#ifdef _WIN32

#ifndef SP_DX_DXGI_IDXGI_DEVICE_SUBOBJECT_WRAPPER_CLASS_H_
#error Invalid header inclusion "dx/dxgi/interface/idxgisubobject_impl.hpp" (Use "dx/dxgi/interface/idxgisubobject.h" instead)
#endif

#include "sp/sp.h"


__SP_NAMESPACE
namespace dx   {
namespace dxgi {


template <typename interface_t>
HRESULT STDMETHODCALLTYPE subobject<interface_t>::GetDevice(_In_ REFIID riid, _COM_Outptr_ void **ppDevice)
{
    HRESULT result = this->_interface->GetDevice(riid, ppDevice);

    return result;
}


} // namespace dxgi
} // namespace dx
__SP_NAMESPACE_CLOSE // namespace sp


#endif // _WIN32
