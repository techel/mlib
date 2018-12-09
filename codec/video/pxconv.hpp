#pragma once

#include <vector>
#include <stdexcept>

#include "codec.hpp"

namespace mlib::codec::video
{

//
// converts between pixel formats
//

class Pxconv : public IVideoDecodec
{
public:
    Pxconv(IVideoDecodec &source, Pixelformat destfmt);

    void codecLogging(CodecLoglevel, CodecLogger) override;
    void codecParameter(const std::string &parameter, const std::string &value) override;

    bool fetchFrame(Videoframe&) override;

    std::unique_ptr<ISourceSeek> tellFrame() const override;
    void seekFrame(const ISourceSeek&) override;

private:
    IVideoDecodec *Source;
    Pixelformat Format;
    std::vector<char> Buffer;
};

//
// errors
//

struct InfeasibleConversion : public std::runtime_error
{
    InfeasibleConversion() : runtime_error("animutil.videoplayer.infconv") {}
};

}