/*
    CONTRIBUTORS:
        Sean Pesce

*/
#pragma once

#ifndef SP_USER_SETTING_CLASS_H_
#define SP_USER_SETTING_CLASS_H_


#include "sp/sp.h"
#include "sp/file.h"

#include "INIReader.h"

#include <string>



class user_setting_base_internal {
public:
    static const INIReader& reader();
protected:
    user_setting_base_internal() {};
};



template <typename T>
class user_setting : protected user_setting_base_internal {
public:
    const std::string section;
    const std::string name;
    const T default_value;
    void(*action) (T&);
    T value;

    inline const T& read()
    {
        this->value = user_setting_base_internal::reader().Get<T>(this->section, this->name, this->default_value);
        return this->value;
    }

    inline const user_setting& perform_task()
    {
        if (this->action != NULL)
        {
            this->action(value);
        }
        return *this;
    }

    inline const user_setting& load(bool _perform_task = true, bool reload_from_file = true)
    {
        if (reload_from_file)
        {
            this->read();
        }
        if (_perform_task)
        {
            this->perform_task();
        }
        return *this;
    }

    user_setting(const std::string& _section, const std::string& _name) : section(_section), name(_name), action(NULL) {}
    user_setting(const std::string& _section, const std::string& _name, const T& _default) : section(_section), name(_name), default_value(_default), action(NULL) {}

    user_setting(void(*_action)(T&), const std::string& _section, const std::string& _name) : section(_section), name(_name), action(_action) {}
    user_setting(void(*_action)(T&), const std::string& _section, const std::string& _name, const T& _default) : section(_section), name(_name), default_value(_default), action(_action) {}

private:
    user_setting() = delete;
};



#endif // SP_USER_SETTING_CLASS_H_
