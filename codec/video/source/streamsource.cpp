#include "streamsource.hpp"

#include <cassert>

namespace mlib::codec::video::streamsource
{

struct S : public ISourceSeek
{
    std::streampos Position;
};

StreamSource::StreamSource(std::istream &s) : Source(&s)
{
}

std::pair<const void*, size_t> StreamSource::extendBuffer(size_t amount)
{
    auto oldsize = Buffer.size();
    Buffer.resize(oldsize + amount);
    Source->read(Buffer.data() + oldsize, amount);

    size_t nread = static_cast<size_t>(Source->gcount());

    if(nread < amount)
        Buffer.resize(oldsize + nread);

    return { Buffer.data(), nread };
}
std::pair<const void*, size_t> StreamSource::advanceBuffer(size_t amount)
{
    assert(amount <= Buffer.size());

    auto remaining = Buffer.size() - amount;
    std::memcpy(Buffer.data(), Buffer.data() + amount, remaining);
    Source->read(Buffer.data() + remaining, amount);

    size_t nread = static_cast<size_t>(Source->gcount());

    if(nread < amount)
        Buffer.resize(remaining + nread);

    Position += amount;

    return { Buffer.data(), nread };
}
std::pair<const void*, size_t> StreamSource::bufferProperties() const
{
    return { Buffer.data(), Buffer.size() };
}
std::unique_ptr<ISourceSeek> StreamSource::sourceTell() const
{
    auto s = std::make_unique<S>();
    s->Position = Position;
    return s;
}
void StreamSource::sourceSeek(const ISourceSeek &s)
{
    const auto &k = static_cast<const S&>(s);
    Source->clear();
    Source->seekg(k.Position, std::ios::beg);
    if(Source->bad())
        throw SeekError(static_cast<std::streamoff>(k.Position));

    Position = k.Position;
    Buffer.clear();
}

}