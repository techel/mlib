#include "decodepiq.hpp"

#include <SFML/Graphics/Image.hpp>

namespace mlib::codec::video::piq
{

struct Decoder::ImplData
{
    sf::Image CurrentFrame;
};

static uint32_t read32(const void *ptr)
{
    const uint8_t *p = static_cast<const uint8_t*>(ptr);
    return (uint32_t)p[0] | (uint32_t)p[1] << 8 | (uint32_t)p[2] << 16 | (uint32_t)p[3] << 24;
}

Decoder::Decoder(ICodecSourcebuffer &source, const std::vector<std::pair<std::string, std::string>> &opts) : Source(&source)
{
    if(!opts.empty())
        throw GenericError<UnknownParameter>(opts[0].first);

    auto[ptr, len] = Source->minimalBuffer(8);
    if(len < 9)
        throw GenericError<StreamUnexpectedEnd>("header expected");

    const uint8_t *p = static_cast<const uint8_t*>(ptr);
    if(p[0] != 'P' || p[1] != 'I' || p[2] != 'Q' || p[3] != '!' || p[4] != 1)
        throw GenericError<StreamMalformed>("invalid signature");

    Source->advanceBuffer(9);

    Impl = std::make_unique<ImplData>();
}

Decoder::~Decoder()
{
}

void Decoder::codecLogging(CodecLoglevel, CodecLogger)
{
    //TODO
}

void Decoder::codecParameter(const std::string &parameter, const std::string & value)
{
    throw GenericError<UnknownParameter>(parameter);
}

bool Decoder::fetchFrame(Videoframe &f)
{
    auto[ptr, len] = Source->minimalBuffer(4);

    if(len == 0)
        return false;

    if(len < 4)
        throw GenericError<StreamUnexpectedEnd>("invalid frame header");

    size_t framelen = static_cast<size_t>(read32(ptr));

    Source->advanceBuffer(4);
    auto[ptr2, len2] = Source->minimalBuffer(framelen);

    if(len2 < framelen)
        throw GenericError<StreamUnexpectedEnd>("image data too short");
    
    if(!Impl->CurrentFrame.loadFromMemory(ptr2, len))
        throw GenericError<StreamMalformed>("malformed image");

    Source->advanceBuffer(framelen);

    f.Format = Pixelformat::RGBA;
    f.Layout = Pixellayout::Interleaved;
    f.Width = Impl->CurrentFrame.getSize().x;
    f.Height = Impl->CurrentFrame.getSize().y;
    f.Planes[0] = Impl->CurrentFrame.getPixelsPtr();
    f.Linestrides[0] = 0;

    return true;
}

std::unique_ptr<ISourceSeek> Decoder::tellFrame() const
{
    return Source->sourceTell();
}

void Decoder::seekFrame(const ISourceSeek &s)
{
    Source->sourceSeek(s);
}

}