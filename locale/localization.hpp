#pragma once

#include <string>
#include <string_view>

namespace mlib::locale
{

struct ILocale
{
    virtual ~ILocale() = default;

    virtual std::string translate(std::string_view text) const = 0;
};

}