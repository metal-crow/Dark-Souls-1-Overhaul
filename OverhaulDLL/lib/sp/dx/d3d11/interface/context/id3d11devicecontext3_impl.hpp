/*
    CONTRIBUTORS:
        Sean Pesce

    References:
        https://msdn.microsoft.com/en-us/library/windows/desktop/dn912875(v=vs.85).aspx
*/
#ifdef _WIN32

#ifndef SP_DX_D3D11_ID3D11DEVICE_CONTEXT3_WRAPPER_CLASS_H_
#error Invalid header inclusion "dx/d3d11/interface/context/id3d11devicecontext3_impl.hpp" (Use "dx/d3d11/interface/context/id3d11devicecontext3.h" instead)
#endif

#include "sp/sp.h"


__SP_NAMESPACE
namespace dx    {
namespace d3d11 {


template <typename interface_t>
void STDMETHODCALLTYPE context3<interface_t>::Flush1(D3D11_CONTEXT_TYPE ContextType, _In_opt_ HANDLE hEvent)
{
    this->_interface->Flush1(ontextType, hEvent);
}


template <typename interface_t>
void STDMETHODCALLTYPE context3<interface_t>::SetHardwareProtectionState(_In_ BOOL HwProtectionEnable)
{
    this->_interface->SetHardwareProtectionState(HwProtectionEnable);
}


template <typename interface_t>
void STDMETHODCALLTYPE context3<interface_t>::GetHardwareProtectionState(_Out_ BOOL *pHwProtectionEnable)
{
    this->_interface->GetHardwareProtectionState(pHwProtectionEnable);
}



} // namespace d3d11
} // namespace dx
__SP_NAMESPACE_CLOSE // namespace sp



#endif // _WIN32
