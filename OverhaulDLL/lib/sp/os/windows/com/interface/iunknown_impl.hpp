/*
    CONTRIBUTORS:
        Sean Pesce

    References:
        https://msdn.microsoft.com/en-us/library/windows/desktop/ms680509(v=vs.85).aspx
*/
#ifdef _WIN32

#ifndef SP_WINDOWS_COM_IUNKNOWN_WRAPPER_CLASS_H_
#error Invalid header inclusion "windows/com/interface/iunknown_impl.hpp" (Use "windows/com/interface/iunknown.h" instead)
#endif

#include "sp/sp.h"


__SP_NAMESPACE
namespace os  {
namespace win {
namespace com {



template <typename interface_t>
ULONG iunknown<interface_t>::AddRef()
{
    _ref_count = _interface->AddRef();
    return _ref_count;
}


template <typename interface_t>
HRESULT iunknown<interface_t>::QueryInterface(REFIID riid, void **ppvObject)
{
    return _interface->QueryInterface(riid, ppvObject);
}


template <typename interface_t>
ULONG iunknown<interface_t>::Release()
{
    _ref_count = _interface->Release();
    return _ref_count;
}


template <typename interface_t>
inline interface_t* iunknown<interface_t>::get() const
{
    return _interface;
}


template <typename interface_t>
inline void iunknown<interface_t>::set(interface_t *new_interface)
{
    _interface = new_interface;
}


template <typename interface_t>
inline unsigned long iunknown<interface_t>::ref_count() const
{
    return _ref_count;
}


template <typename interface_t>
inline void iunknown<interface_t>::set_ref_count(unsigned long count)
{
    _ref_count = count;
}


} // namespace com
} // namespace win
} // namespace os
__SP_NAMESPACE_CLOSE // namespace sp


template class sp::os::win::com::iunknown<IUnknown>;


#endif // _WIN32
