#include <iosfwd>
#include <vector>

#include "../codec.hpp"

namespace mlib::codec::video::streamsource
{

//
// exceptions
//

struct StreamError : public CodecError
{
    StreamError(const std::string &e) : CodecError(".streamsource" + e) {}
};

struct SeekError : public StreamError
{
    SeekError(std::streamoff seekpos) : StreamError(".seek (" + std::to_string(seekpos) + ")") {}
};

//
// std::istream sourcebuffer
//

class StreamSource : public ICodecSourcebuffer
{
public:
    StreamSource(std::istream &i);

    std::pair<const void*, size_t> extendBuffer(size_t length) override;
    std::pair<const void*, size_t> advanceBuffer(size_t amount) override;
    std::pair<const void*, size_t> bufferProperties() const override;

    std::unique_ptr<ISourceSeek> sourceTell() const override;
    void sourceSeek(const ISourceSeek&) override;

private:
    std::istream *Source;
    std::vector<char> Buffer;
    std::streampos Position;
};

}