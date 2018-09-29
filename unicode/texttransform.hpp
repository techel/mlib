#pragma once

namespace mlib::unicode::transform
{

//
// converts a-z to A-Z
//

inline char upperLatin(char c)
{
    return c >= U'a' && c <= U'z' ? c - 32 : c;
}

//
// converts A-Z to a-z
//

inline char lowerLatin(char c)
{
    return c >= U'A' && c <= U'Z' ? c + 32 : c;
}

}