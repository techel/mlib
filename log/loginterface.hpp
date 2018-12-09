#pragma once

#include <string_view>

namespace mlib::log
{

//
// logging interface
//

enum Category { Debug, Error, Warning, Info };

struct ILog
{
    virtual ~ILog() = default;
    virtual void log(Category t, std::string_view modulename, std::string_view message) = 0;
};

}