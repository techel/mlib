#pragma once

#include <vector>

#include "codec.hpp"

namespace mlib::codec::video
{

//
// frame sequence
//

class Frameseq
{
public:
    Frameseq() = default;
    void append(const Videoframe &f);

    size_t length() const { return Frames.size(); }

    const Videoframe &operator[](size_t idx) const { return Frames[idx].Info; }

    Frameseq(Frameseq&&) = delete;
    Frameseq(const Frameseq&) = delete;
    Frameseq &operator=(Frameseq&&) = delete;
    Frameseq &operator=(const Frameseq&) = delete;
    
private:
    struct Frame
    {
        Videoframe Info;
        std::vector<char> Pixels;
    };
    std::vector<Frame> Frames;
};

//
// frame sequence decoder
//

class FrameseqDecodec : public IVideoDecodec
{
public:
    FrameseqDecodec(const Frameseq &src);

    void codecLogging(CodecLoglevel, CodecLogger) override;
    void codecParameter(const std::string &parameter, const std::string &value) override;

    bool fetchFrame(Videoframe&) override;

    std::unique_ptr<ISourceSeek> tellFrame() const override;
    void seekFrame(const ISourceSeek&) override;

private:
    const Frameseq *Source;
    size_t Currframe;
};

}