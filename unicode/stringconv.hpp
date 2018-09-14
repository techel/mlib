#pragma once

#include <charconv>
#include <type_traits>
#include <array>
#include <tuple>
#include <cstddef>
#include <cstdlib>
#include <sstream>

namespace mlib::unicode::stringconv
{

//
// Locale-independent stringify functions for built-in types
//
// - take a buffer tu write to, the value and optional arguments.
// - return the number of chars written to the buffer. 0 indicates failure.
//

template<class Integer>
std::enable_if_t<std::is_integral_v<Integer>, size_t> stringify(char *begin, char *end, Integer i, int base=10)
{
    auto res = std::to_chars(begin, end, i, base);
    return res.ec == std::errc() ? res.ptr - begin : 0;
}

template<class Float>
std::enable_if_t<std::is_floating_point_v<Float>, size_t>stringify(char *begin, char *end, Float f)
{
    std::stringstream s;
    s.imbue(std::locale("C"));
    s << f;
    s.read(begin, end - begin);
    return static_cast<size_t>(s.gcount());
}

//
// stringify utilites
//

// forwards to a stringify function

template<class Value, class... Args>
size_t toRange(char *begin, char *end, Value v, Args&&... args)
{
    return stringify(begin, end, v, std::forward<Args>(args)...);
}

// returns an array of size N of chars and number of valid chars in the array

template<class Value, size_t N=100, class... Args>
std::pair<std::array<char, N>, size_t> toArray(Value v, Args&&... args)
{
    std::array<char, N> chars;
    size_t written = stringify(chars.data(), chars.data() + chars.size(), v, std::forward<Args>(args)...);
    return { chars, written };
}

// returns a string of N chars at maximum

template<class Value, size_t N=100, class... Args>
std::string toString(Value v, Args&&... args)
{
    auto[arr, siz] = toArray<Value, N>(v, std::forward<Args>(args)...);
    return std::string(arr.data(), siz);
}

//
// locale-independent extraction functions for built-in types
//
// - take a buffer to read from, the value to write to and optional arguments
// - return the number of chars read from the buffer and success bit.
//

template<class Integer>
std::enable_if_t<std::is_integral_v<Integer>, std::pair<size_t, bool>> extract(const char *begin, const char *end, Integer &i, int base=10)
{
    auto res = std::from_chars(begin, end, i, base);
    return { res.ptr - begin, res.ec != std::errc::invalid_argument };
}

template<class Float>
std::enable_if_t<std::is_floating_point_v<Float>, std::pair<size_t, bool>> extract(const char *begin, const char *end, Float &f)
{
    std::stringstream s;
    s.imbue(std::locale("C"));
    s.write(begin, end - begin);
    s >> f;

    return { static_cast<size_t>(s.gcount()), !s.fail() };
}

//
// extraction utilites
//

// forwards to a extraction function

template<class Value, class... Args>
std::tuple<Value, size_t, bool> fromRange(const char *begin, const char *end, Args&&... args)
{
    Value v;
    auto [read, success] = extract(begin, end, v, std::forward<Args>(args)...);
    return { std::move(v), read, success };
}

// extracts from a null-terminated string

template<class Value, class... Args>
std::pair<Value, bool> fromString(const char *str, Args&&... args)
{
    Value v;
    auto len = std::strlen(str);
    auto res = extract(str, str + len, v, std::forward<Args>(args)...);
    return { std::move(v), res.first == len };
}

// extracts from a std::string

template<class Value, class... Args>
std::pair<Value, bool> fromString(const std::string &str, Args&&... args)
{
    Value v;
    auto res = extract(str.data(), str.data() + str.size(), v, std::forward<Args>(args)...);
    return { std::move(v), res.first == str.size() };
}

}