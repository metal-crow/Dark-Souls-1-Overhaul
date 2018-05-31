/*
    CONTRIBUTORS:
        Sean Pesce

    Console text output interface
*/

#pragma once

#ifndef SP_UTILITIES_HISTORY_CLASS_H_
#define SP_UTILITIES_HISTORY_CLASS_H_

#include "sp/sp.h"

#include <deque>
#include <string>


__SP_NAMESPACE
namespace util {


template <typename T>
class history {

protected:

    std::deque<T> _history = std::deque<T>();
    size_t _max = 0; // If zero, no history limit
    bool   _allow_repeat_items = true; // Allow consecutive, identical entries

public:

    // Constructors/destructors
    history() {}

    history(size_t max_entries, bool allow_repeat_items = true) : _max(max_entries), _allow_repeat_items(allow_repeat_items)
    {}

    history(bool allow_repeat_items) : _allow_repeat_items(allow_repeat_items)
    {}

    inline ~history() {}

    
    ////// ACCESSORS //////
    inline size_t max_entries()        const { return this->_max; }
    inline bool   allow_repeat_items() const { return _allow_repeat_items; }
    inline bool   empty() const { return _history.empty(); }
    inline bool   full()  const { return this->_max > 0 && (_history.size() >= this->_max); }
    inline size_t size()  const { return _history.size(); }

    inline T& last(size_t index = 0)
    {
        return this->_history[index];
    }

    inline T& operator[](size_t index)
    {
        return this->_history[index];
    }

    inline const T& last(size_t index = 0) const
    {
        return this->_history[index];
    }

    inline const T& operator[](size_t index) const
    {
        return this->_history[index];
    }

    ////// MUTATORS //////

    void set_max_entries(size_t max_entries)
    {
        this->_max = max_entries;
        if (this->_max > 0)
        {
            while (_history.size() > this->_max)
            {
                _history.pop_back();
            }
        }
    }

    inline void set_allow_repeat_items(bool allow)
    {
        // Note: Existing repeat items are not removed
        _allow_repeat_items = allow;
    }


    ////// ACTIONS //////

    inline void clear()
    {
        _history.clear();
    }

    inline void add(const T& item)
    {
        if (_allow_repeat_items || _history.empty() || this->last() != item)
        {
            this->_history.push_front(item);
        }

        if (this->_max > 0)
        {
            while (_history.size() > this->_max)
            {
                _history.pop_back();
            }
        }
    }

    inline void operator+=(const T& item)
    {
        this->add(item);
    }

}; // class history


} // namespace util
__SP_NAMESPACE_CLOSE // namespace sp

#endif // SP_UTILITIES_HISTORY_CLASS_H_
