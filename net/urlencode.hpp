#pragma once

#include <charconv>
#include <string_view>
#include <string>

#include <mlib/unicode/texttransform.hpp>

namespace mlib::net
{

//
// Percent-encodes non-alphanumeric chars except '-'.
// Space is encoded as '+'.
//
// Extractor: bool(char&) - extracts single character, returns false on EOF
// Inserter: void(char) - inserts single characters
//
//

template<class Extractor, class Inserter>
void urlencode(Extractor extract, Inserter insert)
{
    char c;
    while(extract(c))
    {
        if(c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c >= '0' && c <= '9' || c == '-')
        {
            insert(c);
        }
        else if(c == ' ')
        {
            insert('+');
        }
        else
        {
            char digits[2];
            std::to_chars(digits, digits + 2, (unsigned char)c, 16);
            digits[0] = unicode::transform::upperLatin(digits[0]);
            digits[1] = unicode::transform::upperLatin(digits[1]);

            insert('%');
            insert(digits[0]);
            insert(digits[1]);
        }
    }
}

inline std::string urlencode(std::string_view s)
{
    std::string res;
    res.reserve(s.size() * 2);

    urlencode([&s](char &c)
    {
        if(s.empty())
            return false;

        c = s.front();
        s.remove_prefix(1);
        return true;
    }, [&res](char c)
    {
        res += c;
    });

    return res;
}

//
// Percent-decoder expands percents
//
// Extractor: bool(char&) - extracts single character, returns false on EOF
// Inserter: void(char) - inserts single characters
//

template<class Extractor, class Inserter>
bool urldecode(Extractor extract, Inserter insert)
{
    char c;
    while(extract(c))
    {
        if(c == '%')
        {
            char digits[2];
            if(!extract(digits[0]) || !extract(digits[1]))
                return false;

            unsigned char val;
            if(auto res = std::from_chars(digits, digits + 2, val, 16); res.ptr != digits + 2)
                return false;

            insert((char)val);
        }
        else if(c == '+')
        {
            insert(' ');
        }
        else
        {
            insert(c);
        }
    }
    return true;
}

inline std::pair<std::string, bool> urldecode(std::string_view s)
{
    std::string res;
    res.reserve(s.size() / 2);

    bool valid = urldecode([&s](char &c)
    {
        if(s.empty())
            return false;

        c = s.front();
        s.remove_prefix(1);
        return true;
    }, [&res](char c)
    {
        res += c;
    });

    return std::make_pair(std::move(res), valid);
}

}