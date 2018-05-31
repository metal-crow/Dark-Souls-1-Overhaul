/*
    CONTRIBUTORS:
        Sean Pesce

    References:
        https://msdn.microsoft.com/en-us/library/windows/desktop/mt732707(v=vs.85).aspx
*/
#ifdef _WIN32

#ifndef SP_DX_DXGI_IDXGI_SWAPCHAIN4_WRAPPER_CLASS_H_
#error Invalid header inclusion "dx/dxgi/interface/swapchain/idxgiswapchain4_impl.hpp" (Use "dx/dxgi/interface/swapchain/idxgiswapchain4.h" instead)
#endif

#include "sp/sp.h"


__SP_NAMESPACE
namespace dx   {
namespace dxgi {


template <typename interface_t>
HRESULT STDMETHODCALLTYPE swapchain4<interface_t>::SetHDRMetaData(_In_  DXGI_HDR_METADATA_TYPE Type, _In_  UINT Size, _In_reads_opt_(Size)  void *pMetaData)
{
    HRESULT result = this->_interface->SetHDRMetaData(Type, Size, pMetaData);

    return result;
}


} // namespace dxgi
} // namespace dx
__SP_NAMESPACE_CLOSE // namespace sp


#endif // _WIN32
