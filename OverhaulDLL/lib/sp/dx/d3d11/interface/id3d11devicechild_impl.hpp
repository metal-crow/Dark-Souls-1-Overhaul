/*
    CONTRIBUTORS:
        Sean Pesce

    References:
        https://msdn.microsoft.com/en-us/library/windows/desktop/ff476380(v=vs.85).aspx
*/
#ifdef _WIN32

#ifndef SP_DX_D3D11_ID3D11DEVICE_CHILD_WRAPPER_CLASS_H_
#error Invalid header inclusion "dx/d3d11/interface/id3d11devicechild_impl.hpp" (Use "dx/d3d11/interface/id3d11devicechild.h" instead)
#endif

#include "sp/sp.h"


__SP_NAMESPACE
namespace dx    {
namespace d3d11 {



template <typename interface_t>
void STDMETHODCALLTYPE device_child<interface_t>::GetDevice(_Outptr_  ID3D11Device **ppDevice)
{
    this->_interface->GetDevice(ppDevice);
}


template <typename interface_t>
HRESULT STDMETHODCALLTYPE device_child<interface_t>::GetPrivateData(
    _In_  REFGUID guid,
    _Inout_  UINT *pDataSize,
    _Out_writes_bytes_opt_(*pDataSize)  void *pData )
{
    HRESULT result = this->_interface->GetPrivateData(guid, pDataSize, pData);

    return result;
}


template <typename interface_t>
HRESULT STDMETHODCALLTYPE device_child<interface_t>::SetPrivateData(
    _In_  REFGUID guid,
    _In_  UINT DataSize,
    _In_reads_bytes_opt_(DataSize)  const void *pData )
{
    HRESULT result = this->_interface->SetPrivateData(guid, DataSize, pData);

    return result;
}


template <typename interface_t>
HRESULT STDMETHODCALLTYPE device_child<interface_t>::SetPrivateDataInterface(_In_ REFGUID guid, _In_opt_  const IUnknown *pData)
{
    HRESULT result = this->_interface->SetPrivateDataInterface(guid, pData);

    return result;
}



} // namespace d3d11
} // namespace dx
__SP_NAMESPACE_CLOSE // namespace sp




#endif // _WIN32
