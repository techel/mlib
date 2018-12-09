#pragma once

#include <vector>
#include <utility>
#include <memory>

#include "commonpiq.hpp"
#include "../codec.hpp"

namespace mlib::codec::video::piq
{

//
// picture sequence decoder (using stb_image)
//
// format: (little endian)
// 'PIQ!' signature
// [uint8] version (currently 0x01)
// [uint32] number of frames
// 
// for all frames:
// [uint32] image size
// ... image data
//
// plane0 points to image RGBA pixels.
//

class Decoder : public IVideoDecodec
{
public:
    Decoder(ICodecSourcebuffer &source, const std::vector<std::pair<std::string, std::string>> &opts = {});
    ~Decoder();

    void codecLogging(CodecLoglevel, CodecLogger) override;
    void codecParameter(const std::string &parameter, const std::string &value) override;

    bool fetchFrame(Videoframe &) override;

    std::unique_ptr<ISourceSeek> tellFrame() const override;
    void seekFrame(const ISourceSeek &) override;

private:
    ICodecSourcebuffer *Source;
    
    struct ImplData;
    std::unique_ptr<ImplData> Impl;
};

}