#pragma once

#include "../codec.hpp"

namespace mlib::codec::video::memorysource
{

//
// memory sourcebuffer
//

class MemorySource : public ICodecSourcebuffer
{
public:
    MemorySource(const void *begin, size_t length);

    std::pair<const void*, size_t> extendBuffer(size_t length) override;
    std::pair<const void*, size_t> advanceBuffer(size_t amount) override;
    std::pair<const void*, size_t> bufferProperties() const override;

    std::unique_ptr<ISourceSeek> sourceTell() const override;
    void sourceSeek(const ISourceSeek &) override;

private:
    const char *Begin;
    size_t Length;
};

}