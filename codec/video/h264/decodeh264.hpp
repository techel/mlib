#pragma once

#include <string>
#include <vector>

#include "../codec.hpp"
#include "common264.hpp"

namespace mlib::codec::video::h264
{

//
// H264 decoder
//
// Decodes H264 stream in AnnexB format.
//

//
// parameters
//
// - bufcapacity: the internal's buffer size, default: 1MiB
//

class Decoder : public virtual IVideoDecodec
{
public:
    Decoder(ICodecSourcebuffer &source, std::vector<std::pair<std::string, std::string>> parameters);
    ~Decoder();

    void codecLogging(CodecLoglevel, CodecLogger);
    void codecParameter(const std::string &parameter, const std::string &value) override;

    bool fetchFrame(Videoframe&) override;

    std::unique_ptr<ISourceSeek> tellFrame() const override;
    void seekFrame(const ISourceSeek&) override;

    Decoder(Decoder&&) = delete;
    Decoder(const Decoder&) = delete;
    Decoder &operator=(Decoder&&) = delete;
    Decoder &operator=(const Decoder&) = delete;

private:
    struct ImplData;
    std::unique_ptr<ImplData> Impl;
};

}