/*
    CONTRIBUTORS:
        Sean Pesce

    References:
        https://msdn.microsoft.com/en-us/library/windows/desktop/bb174523%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396
*/
#ifdef _WIN32

#ifndef SP_DX_DXGI_IDXGI_ADAPTER_WRAPPER_CLASS_H_
#error Invalid header inclusion "dx/dxgi/interface/idxgiadapter_impl.hpp" (Use "dx/dxgi/interface/idxgiadapter.h" instead)
#endif

#include "sp/sp.h"


__SP_NAMESPACE
namespace dx   {
namespace dxgi {


template <typename interface_t>
HRESULT STDMETHODCALLTYPE adapter<interface_t>::EnumOutputs(/* [in] */ UINT Output, _COM_Outptr_  IDXGIOutput **ppOutput)
{
    HRESULT result = this->_interface->EnumOutputs(Output, ppOutput);

    return result;
}

template <typename interface_t>
HRESULT STDMETHODCALLTYPE adapter<interface_t>::GetDesc(_Out_ DXGI_ADAPTER_DESC *pDesc)
{
    HRESULT result = this->_interface->GetDesc(pDesc);

    return result;
}

template <typename interface_t>
HRESULT STDMETHODCALLTYPE adapter<interface_t>::CheckInterfaceSupport(_In_ REFGUID InterfaceName, _Out_ LARGE_INTEGER *pUMDVersion)
{
    HRESULT result = this->_interface->CheckInterfaceSupport(InterfaceName, pUMDVersion);

    return result;
}


} // namespace dxgi
} // namespace dx
__SP_NAMESPACE_CLOSE // namespace sp


#endif // _WIN32
