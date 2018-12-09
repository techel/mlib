#include "pxconv.hpp"

#include <utility>
#include <libyuv.h>

namespace mlib::codec::video
{

Pxconv::Pxconv(IVideoDecodec &source, Pixelformat destfmt) : Source(&source), Format(destfmt)
{
}

void Pxconv::codecLogging(CodecLoglevel lvl, CodecLogger lg)
{
    Source->codecLogging(lvl, std::move(lg));
}

void Pxconv::codecParameter(const std::string &parameter, const std::string &value)
{
    Source->codecParameter(parameter, value);
}

bool Pxconv::fetchFrame(Videoframe &f)
{
    Videoframe fr;
    if(!Source->fetchFrame(fr))
        return false;

    f = fr;
    f.Format = Format;

    if(Format == Pixelformat::RGB24I && fr.Format == Pixelformat::YUV12P)
    {
        if(fr.Width * fr.Height * 3 > Buffer.size())
            Buffer.resize(fr.Width * fr.Height * 3);

        libyuv::I420ToRGB24((uint8_t*)fr.Planes[0], (int)fr.Linestrides[0],
            (uint8_t*)fr.Planes[1], (int)fr.Linestrides[1],
            (uint8_t*)fr.Planes[2], (int)fr.Linestrides[2],
            (uint8_t*)Buffer.data(), (int)fr.Width * 3, (int)fr.Width, (int)fr.Height);

        f.Linestrides[0] = 0;
        f.Planes[0] = Buffer.data();
    }
    if(Format == Pixelformat::RGBA32I && fr.Format == Pixelformat::YUV12P)
    {
        if(fr.Width * fr.Height * 4 > Buffer.size())
            Buffer.resize(fr.Width * fr.Height * 4);

        libyuv::I420ToRGBA((uint8_t*)fr.Planes[0], (int)fr.Linestrides[0],
            (uint8_t*)fr.Planes[1], (int)fr.Linestrides[1],
            (uint8_t*)fr.Planes[2], (int)fr.Linestrides[2],
            (uint8_t*)Buffer.data(), (int)fr.Width * 4, (int)fr.Width, (int)fr.Height);

        f.Linestrides[0] = 0;
        f.Planes[0] = Buffer.data();
    }
    else if(Format == fr.Format)
    {
        f = fr;
    }
    else
    {
        throw InfeasibleConversion();
    }

    return true;
}

std::unique_ptr<ISourceSeek> Pxconv::tellFrame() const
{
    return Source->tellFrame();
}

void Pxconv::seekFrame(const ISourceSeek &s)
{
    Source->seekFrame(s);
}

}