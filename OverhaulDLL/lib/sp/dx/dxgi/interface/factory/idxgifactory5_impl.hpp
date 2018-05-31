/*
    CONTRIBUTORS:
        Sean Pesce

    References:
        https://msdn.microsoft.com/en-us/library/windows/desktop/mt722566(v=vs.85).aspx
*/
#ifdef _WIN32

#ifndef SP_DX_DXGI_IDXGIFACTORY5_WRAPPER_CLASS_H_
#error Invalid header inclusion "dx/dxgi/interface/factory/idxgifactory5_impl.hpp" (Use "dx/dxgi/interface/factory/idxgifactory5.h" instead)
#endif

#include "sp/sp.h"


__SP_NAMESPACE
namespace dx   {
namespace dxgi {


template <typename interface_t>
HRESULT factory5<interface_t>::CheckFeatureSupport(DXGI_FEATURE Feature, void *pFeatureSupportData, UINT FeatureSupportDataSize)
{
    return this->_interface->CheckFeatureSupport(Feature, pFeatureSupportData, FeatureSupportDataSize);
}


} // namespace dxgi
} // namespace dx
__SP_NAMESPACE_CLOSE // namespace sp


#endif // _WIN32
