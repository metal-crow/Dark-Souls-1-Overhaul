/*
    CONTRIBUTORS:
        Sean Pesce

    References:
        https://msdn.microsoft.com/en-us/library/windows/desktop/ms680509(v=vs.85).aspx
*/
#ifdef _WIN32
#pragma once

#ifndef SP_WINDOWS_COM_IUNKNOWN_WRAPPER_CLASS_H_
#define SP_WINDOWS_COM_IUNKNOWN_WRAPPER_CLASS_H_

#include "sp/sp.h"
#include "Unknwn.h"


__SP_NAMESPACE
namespace os  {
namespace win {
namespace com {


template <class interface_t = IUnknown>
class iunknown : public interface_t {
protected:
    interface_t*  _interface;
    unsigned long _ref_count;

    virtual inline void _wrap(interface_t **original)
    {
        _interface = *original;
        *original = this;
    }

public:

    iunknown(){}

    iunknown(interface_t **original)
    {
        this->_wrap(original);
    }

    virtual ~iunknown() {}


    virtual inline const GUID guid()
    {
        return __uuidof(interface_t);
    }


    // Wrapped interface methods
    virtual ULONG   AddRef();
    virtual HRESULT QueryInterface(REFIID riid, void **ppvObject);
    virtual ULONG   Release();

    virtual inline interface_t*  get() const;
    virtual inline void          set(interface_t* new_interface);
    virtual inline unsigned long ref_count() const;
    virtual inline void          set_ref_count(unsigned long count);
}; // class iunknown


} // namespace com
} // namespace win
} // namespace os
__SP_NAMESPACE_CLOSE // namespace sp


// Implementation
#include "sp/os/windows/com/interface/iunknown_impl.hpp"


#endif // SP_WINDOWS_COM_IUNKNOWN_WRAPPER_CLASS_H_


#endif // _WIN32
