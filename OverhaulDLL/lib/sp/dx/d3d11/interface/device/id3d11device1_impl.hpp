/*
    CONTRIBUTORS:
        Sean Pesce

    References:
        https://msdn.microsoft.com/en-us/library/windows/desktop/hh404575(v=vs.85).aspx
*/
#ifdef _WIN32

#ifndef SP_DX_D3D11_ID3D11DEVICE1_WRAPPER_CLASS_H_
#error Invalid header inclusion "dx/d3d11/interface/device/id3d11device1_impl.hpp" (Use "dx/d3d11/interface/device/id3d11device1.h" instead)
#endif

#include "sp/sp.h"


__SP_NAMESPACE
namespace dx    {
namespace d3d11 {



template <typename interface_t>
void STDMETHODCALLTYPE device1<interface_t>::GetImmediateContext1(_Out_  ID3D11DeviceContext1 **ppImmediateContext)
{
    this->_interface->GetImmediateContext1(ppImmediateContext);
}



template <typename interface_t>
HRESULT STDMETHODCALLTYPE device1<interface_t>::CreateDeferredContext1(UINT ContextFlags, _Out_opt_  ID3D11DeviceContext1 **ppDeferredContext)
{
    HRESULT result = this->_interface->CreateDeferredContext1(ContextFlags, ppDeferredContext);

    return result;
}



template <typename interface_t>
HRESULT STDMETHODCALLTYPE device1<interface_t>::CreateBlendState1(_In_  const D3D11_BLEND_DESC1 *pBlendStateDesc, _Out_opt_  ID3D11BlendState1 **ppBlendState)
{
    HRESULT result = this->_interface->CreateBlendState1(pBlendStateDesc, ppBlendState);

    return result;
}



template <typename interface_t>
HRESULT STDMETHODCALLTYPE device1<interface_t>::CreateRasterizerState1(
            _In_  const D3D11_RASTERIZER_DESC1 *pRasterizerDesc,
            _Out_opt_  ID3D11RasterizerState1 **ppRasterizerState
)
{
    HRESULT result = this->_interface->CreateRasterizerState1(pRasterizerDesc, ppRasterizerState);

    return result;
}



template <typename interface_t>
HRESULT STDMETHODCALLTYPE device1<interface_t>::CreateDeviceContextState(
            UINT Flags,
            _In_reads_(FeatureLevels)  const D3D_FEATURE_LEVEL *pFeatureLevels,
            UINT FeatureLevels,
            UINT SDKVersion,
            REFIID EmulatedInterface,
            _Out_opt_  D3D_FEATURE_LEVEL *pChosenFeatureLevel,
            _Out_opt_  ID3DDeviceContextState **ppContextState
)
{
    HRESULT result = this->_interface->CreateDeviceContextState(Flags, pFeatureLevels, FeatureLevels, SDKVersion, EmulatedInterface, pChosenFeatureLevel, ppContextState);

    return result;
}



template <typename interface_t>
HRESULT STDMETHODCALLTYPE device1<interface_t>::OpenSharedResource1(
            _In_  HANDLE hResource,
            _In_  REFIID returnedInterface,
            _Out_  void **ppResource
)
{
    HRESULT result = this->_interface->OpenSharedResource1(hResource, returnedInterface, ppResource);

    return result;
}



template <typename interface_t>
HRESULT STDMETHODCALLTYPE device1<interface_t>::OpenSharedResourceByName(
            _In_  LPCWSTR lpName,
            _In_  DWORD dwDesiredAccess,
            _In_  REFIID returnedInterface,
            _Out_  void **ppResource
)
{
    HRESULT result = this->_interface->OpenSharedResourceByName(lpName, dwDesiredAccess, returnedInterface, ppResource);

    return result;
}



} // namespace d3d11
} // namespace dx
__SP_NAMESPACE_CLOSE // namespace sp


#endif // _WIN32
