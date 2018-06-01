/*
    CONTRIBUTORS:
        Sean Pesce

    Restorable byte patch class
*/

#pragma once

#ifndef SP_UTILITY_MEMORY_BYTE_ARRAY_PATCH_H_
#define SP_UTILITY_MEMORY_BYTE_ARRAY_PATCH_H_

#include "sp/sp.h"
#include "sp/memory.h"
#include "sp/memory/patch.h"
#include "sp/memory/aob_scan.h"

#include <vector>


__SP_NAMESPACE
namespace mem {


class byte_patch : public sp::mem::patch_interface {

protected:

    void *_write_address = NULL;

    std::vector<uint8_t> _patch_data;
    std::vector<uint8_t> _saved_data;

    // Retrieve pointer to location in memory that will be patched
    virtual inline void* _resolve() const
    {
        return this->_write_address;
    }

public:

    // Constructors/destructors
    byte_patch() {}

    byte_patch(void *write_address, uint8_t* data, size_t size)
        : _write_address(write_address)
    {
        if (write_address != NULL)
        {
            sp::mem::set_protection(this->_write_address, size, MEM_PROTECT_RWX);
        }

        if (data != NULL)
        {
            this->_patch_data.assign(data, data+size);
        }
        else
        {
            this->_patch_data = std::vector<uint8_t>();
        }

        this->save();
    }

    byte_patch(void *write_address, const std::vector<uint8_t>& data)
        : _write_address(write_address)
    {
        if (write_address != NULL)
        {
            sp::mem::set_protection(this->_write_address, data.size(), MEM_PROTECT_RWX);
        }

        this->_patch_data = data;

        this->save();
    }

    virtual inline ~byte_patch() {}



    // Retrieve pointer to location in memory that will be patched
    virtual inline uint8_t* resolve() const
    {
        return (uint8_t*)this->_resolve();
    }

    // Size of patch (in bytes)
    inline size_t size() const
    {
        return this->_patch_data.size();
    }

    // Patch data bytes
    inline const uint8_t* get_patch_data() const
    {
        return this->_patch_data.data();
    }

    // Vector holding patch data
    inline std::vector<uint8_t> get_patch_data_vector() const
    {
        return this->_patch_data;
    }

    // Original data bytes
    inline const uint8_t* get_saved_data() const
    {
        return this->_saved_data.data();
    }

    // Vector holding original data
    inline std::vector<uint8_t> get_saved_data_vector() const
    {
        return this->_saved_data;
    }

    // Apply patch data
    virtual inline bool apply() const
    {
        if (!this->is_applied())
        {
            return sp::mem::patch_bytes(this->_write_address, this->get_patch_data(), this->_patch_data.size());
        }
        return true;
    }

    // Restore original data
    virtual inline bool restore() const
    {
        return sp::mem::patch_bytes(this->_write_address, this->_saved_data.data(), this->_saved_data.size());
    }

    // Checks if patch is currently applied
    inline bool is_applied() const
    {
        if (this->_write_address != NULL)
        {
            return compare_byte_arrays((uint8_t*)this->get_patch_data(), (uint8_t*)this->_write_address, this->_patch_data.size());
        }
        return false;
    }

    // Back up current data at write address
    inline void save()
    {
        if (this->_write_address != NULL)
        {
            this->_saved_data.assign((uint8_t*)this->_write_address, ((uint8_t*)this->_write_address) + this->_patch_data.size());
        }
    }


    // Set new location in memory to patch when apply() is called
    inline void set_write_address(void* write_address)
    {
        if (write_address != this->_write_address && write_address != NULL)
        {
            this->_write_address = write_address;
            sp::mem::set_protection(write_address, this->_patch_data.size(), MEM_PROTECT_RWX);
        }
    }


    // Set new data to write when apply() is called
    inline void set_patch_data(uint8_t* data, size_t size)
    {
        if (data != NULL && size > this->_patch_data.size())
        {
            sp::mem::set_protection(this->_write_address, size, MEM_PROTECT_RWX);
        }

        if (data != NULL)
        {
            this->_patch_data.assign(data, data + size);
        }
    }

    // Set new data to write when apply() is called
    inline void set_patch_data(const std::vector<uint8_t>& data)
    {
        if (data.size() > this->_patch_data.size())
        {
            sp::mem::set_protection(this->_write_address, data.size(), MEM_PROTECT_RWX);
        }

        this->_patch_data = data;
    }

}; // class byte_patch


} // namespace mem
__SP_NAMESPACE_CLOSE // namespace sp

#endif // SP_UTILITY_MEMORY_BYTE_ARRAY_PATCH_H_
