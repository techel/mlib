#pragma once
#include "iostreams.hpp"

#include <iterator>
#include <cassert>

namespace mlib::stream
{

//
// memory streambuffer
//
// Streams data directly from a given range using random access iterators, which can be pointers to a block of memory as well.
// The object type the iterators point to is determined by the decayed result of operator*().
//

template<class Iter>
class MemoryInputStreambuf : public IBasicStreambuf<std::decay_t<decltype(*std::declval<Iter>())>>
{
    Iter Current;
    size_t Remaining;

public:
    MemoryInputStreambuf(Iter begin, Iter end) : Current(begin)
    {
        auto dist = std::distance(begin, end);
        assert(dist >= 0);
        Remaining = static_cast<size_t>(dist);
    }

    using Type = std::decay_t<decltype(*std::declval<Iter>())>;

    size_t readBuffer(Type *dst, size_t size) override
    {
		size = std::min(size, Remaining);
        std::copy(Current, Current + size, dst);
        Remaining -= size;
        Current += size;
        return size;
    }
};


}}