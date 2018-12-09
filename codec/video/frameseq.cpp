#include "frameseq.hpp"

#include <utility>
#include <cassert>
#include <cstring>
#include <memory>

namespace mlib::codec::video
{

//
// frame sequence
//

static void fill(char *&ptr, const void *data, size_t n)
{
    std::memcpy(ptr, data, n);
    ptr += n;
}

void Frameseq::append(const Videoframe &f)
{
    Frame newf;
    newf.Info = f;

    if(f.Format == Pixelformat::YUV12P)
    {
        auto npxY = (f.Width + f.Linestrides[0]) * f.Height;
        auto npxU = ((f.Width + f.Linestrides[1]) * f.Height + 3) / 4;
        auto npxV = ((f.Height + f.Linestrides[2]) * f.Height + 3) / 4;
        newf.Pixels.resize(npxY + npxU + npxV);

        auto ptr = newf.Pixels.data();

        newf.Info.Planes[0] = ptr;
        fill(ptr, f.Planes[0], npxY);
        newf.Info.Planes[1] = ptr;
        fill(ptr, f.Planes[1], npxU);
        newf.Info.Planes[2] = ptr;
        fill(ptr, f.Planes[2], npxV);
    }
    else if(f.Format == Pixelformat::RGBA32I)
    {
        auto npx = (f.Width + f.Linestrides[0]) * f.Height * 4;
        newf.Pixels.resize(npx);

        auto ptr = newf.Pixels.data();
        newf.Info.Planes[0] = ptr;
        fill(ptr, f.Planes[0], npx);
    }
    else if(f.Format == Pixelformat::RGB24I)
    {
        auto npx = (f.Width + f.Linestrides[0]) * f.Height * 3;
        newf.Pixels.resize(npx);

        auto ptr = newf.Pixels.data();
        newf.Info.Planes[0] = ptr;
        fill(ptr, f.Planes[0], npx);
    }
    else
        assert(false);

    Frames.push_back(std::move(newf));
}

//
// frame sequence decoder
//

struct Seek : public ISourceSeek
{
    size_t Framepos;
};

FrameseqDecodec::FrameseqDecodec(const Frameseq &src) : Source(&src), Currframe(0)
{
}

void FrameseqDecodec::codecLogging(CodecLoglevel, CodecLogger)
{
}

void FrameseqDecodec::codecParameter(const std::string &parameter, const std::string &value)
{
    throw UnknownParameter(parameter);
}

bool FrameseqDecodec::fetchFrame(Videoframe &fr)
{
    if(Currframe >= Source->length())
        return false;

    fr = (*Source)[Currframe++];
    return true;
}

std::unique_ptr<ISourceSeek> FrameseqDecodec::tellFrame() const
{
    auto s = std::make_unique<Seek>();
    s->Framepos = Currframe;
    return s;
}

void FrameseqDecodec::seekFrame(const ISourceSeek &sk)
{
    const auto &s = static_cast<const Seek&>(sk);
    Currframe = s.Framepos;
}

}