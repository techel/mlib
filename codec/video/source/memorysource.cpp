#include "memorysource.hpp"

#include <cassert>

namespace mlib::codec::video::memorysource
{

struct S : public ISourceSeek
{
    const char *Begin;
    size_t Length;
};

MemorySource::MemorySource(const void *begin, size_t length) : Begin(static_cast<const char*>(begin)), Length(length)
{
}

std::pair<const void*, size_t> MemorySource::extendBuffer(size_t length)
{
    return { Begin, 0 };
}
std::pair<const void*, size_t> MemorySource::advanceBuffer(size_t amount)
{
    assert(amount <= Length);
    Begin += amount;
    Length -= amount;
    return { Begin, 0 };
}
std::pair<const void*, size_t> MemorySource::bufferProperties() const
{
    return { Begin, Length };
}

std::unique_ptr<ISourceSeek> MemorySource::sourceTell() const
{
    auto s = std::make_unique<S>();
    s->Begin = Begin;
    s->Length = Length;
    return s;
}
void MemorySource::sourceSeek(const ISourceSeek &s)
{
    auto &k = static_cast<const S&>(s);
    Begin = k.Begin;
    Length = k.Length;
}

}