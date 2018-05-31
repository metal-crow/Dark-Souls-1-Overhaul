/*
    CONTRIBUTORS:
        Sean Pesce

    References:
        https://msdn.microsoft.com/en-us/library/windows/desktop/bb509553(v=vs.85).aspx
*/
#ifdef _WIN32
#pragma once

#ifndef SP_DX_DXGI_ERRORS_H_
#define SP_DX_DXGI_ERRORS_H_

#include "sp/sp.h"
#include <string>


__SP_NAMESPACE
namespace dx   {
namespace dxgi {
namespace err  {


// Returns name of the specified DirectX error code
std::string name(HRESULT code);


} // namespace err
} // namespace dxgi
} // namespace dx
__SP_NAMESPACE_CLOSE // namespace sp

#endif // SP_DX_DXGI_ERRORS_H_


#endif // _WIN32
