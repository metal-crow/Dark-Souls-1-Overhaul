/*
    CONTRIBUTORS:
        Sean Pesce

    Multi-level pointer class
*/

#pragma once

#ifndef SP_UTILITY_MEMORY_POINTER_CLASS_H_
#define SP_UTILITY_MEMORY_POINTER_CLASS_H_

#include "sp/sp.h"
#include "sp/memory.h"

#include <vector>


__SP_NAMESPACE
namespace mem {


template<class T>
class pointer {

protected:

    void* _base = NULL; // Starting address of multi-level pointer
    std::vector<long> _offsets; // Chain of offsets used to resolve the final address


public:

    // Constructors/destructors
    pointer(void* starting_address = NULL, const std::vector<long>& new_offsets = {})
    {
        this->_base   = starting_address;
        this->_offsets = new_offsets;
    }

    inline ~pointer(){}


    pointer& operator=(const sp::mem::pointer<T>& other)
    {
        if (this != &other)
        {
            this->assign(other);
        }
        return *this;
    }

    T& operator*()
    {
        return *this->resolve();
    }

    inline void assign(const sp::mem::pointer<T>& other)
    {
        this->_base = other.base();
        this->_offsets = other.offsets();
    }


    // Accessors

    inline void* base() const
    {
        return this->_base;
    }

    inline std::vector<long> offsets() const
    {
        return this->_offsets;
    }

    inline size_t offset_count() const
    {
        return this->_offsets.size();
    }

    // Returns final resolved address (or NULL if memory is inaccessible)
    T* resolve() const
    {
        void *address = this->_base;

        MEMORY_BASIC_INFORMATION mbi; // Used to check if memory is readable

        for (auto offset : this->_offsets)
        {
            // Check if memory is readable
            if (sp::mem::virtual_query(address, &mbi, sizeof(MEMORY_BASIC_INFORMATION)) == 0)
                return NULL;

            #ifdef _WIN32
            if (mbi.State != MEM_COMMIT)
                return NULL;
            #endif // _WIN32

            if (mbi.Protect == MEM_PROTECT_NONE || mbi.Protect == MEM_PROTECT_X)
                return NULL;

            // Memory is readable, continue resolution of pointer
            address = (void*)((*(uint64_t*)address) + offset);
        }

        // Check if resolved address is readable
        if (sp::mem::virtual_query(address, &mbi, sizeof(MEMORY_BASIC_INFORMATION)) == 0)
            return NULL;

        #ifdef _WIN32
        if (mbi.State != MEM_COMMIT)
            return NULL;
        #endif // _WIN32

        if (mbi.Protect == MEM_PROTECT_NONE || mbi.Protect == MEM_PROTECT_X)
            return NULL;

        // Resolved address is readable
        return (T*)address;
    }

    // @TODO: Overload operator[] to access intermediate pointer addresses


    /*
        Copies a value from the resolved address to the given buffer.

        @param buffer    Buffer to store the value that was read from the resolved address

        @return          SP_ERR_SUCCESS on success, SP_ERR_INVALID_DATA if pointer resolution fails,
                         and SP_ERR_INVALID_ADDRESS if the provided buffer is NULL.
    */
    inline uint32_t copy(T* buffer) const
    {
        if (buffer == NULL)
        {
            return SP_ERR_INVALID_ADDRESS;
        }
        T* resolved_address = this->resolve();
        if (resolved_address == NULL)
        {
            return SP_ERR_INVALID_DATA;
        }
        *buffer = *resolved_address;
        return SP_ERR_SUCCESS;
    }


    // Mutators

    inline void set_base(void* base)
    {
        this->_base = base;
    }

    inline void set_offsets(const std::vector<long>& offsets)
    {
        this->_offsets = offsets;
    }


    /*
        Writes a value to the resolved address.

        @param value    Value to be written to the resolved address

        @return         SP_ERR_SUCCESS on success; if pointer resolution fails,
                        SP_ERR_INVALID_DATA is returned instead.
    */
    inline uint32_t write(T& value) const
    {
        T* resolved_address = this->resolve();
        if (resolved_address == NULL)
        {
            return SP_ERR_INVALID_DATA;
        }
        *resolved_address = value;
        return SP_ERR_SUCCESS;
    }

}; // class pointer


} // namespace mem
__SP_NAMESPACE_CLOSE // namespace sp

#endif // SP_UTILITY_MEMORY_POINTER_CLASS_H_
