#include "decodeh264.hpp"

#include <wels/codec_api.h>
#include <wels/codec_def.h>
#include <wels/codec_ver.h>

#include <utility>
#include <cassert>

namespace mlib::codec::video::h264
{

//
// impl
//

struct Decoder::ImplData
{    
    struct FrameSeekinfo : public ISourceSeek
    {
        std::unique_ptr<ISourceSeek> BufferSeekinfo;
        size_t BufferOffset;
        size_t BufferSize;
    };

    CodecLogger Logger;
    ICodecSourcebuffer *Source;
    ISVCDecoder *Decoder;

    const void *SourcebufferBegin;
    size_t SourcebufferLength, CurrentOffset;

    size_t BufferCapacity;

    ImplData(ICodecSourcebuffer &source, size_t capacity) : Source(&source), BufferCapacity(capacity)
    {
        if(long e = WelsCreateDecoder(&Decoder); e != 0)
            throw ErrorCode(e);

        SDecodingParam d = { 0 };
        d.bParseOnly = false;
        d.eEcActiveIdc = ERROR_CON_DISABLE;
        d.sVideoProperty.eVideoBsType = VIDEO_BITSTREAM_DEFAULT;
        if(long e = Decoder->Initialize(&d); e != 0)
            throw ErrorCode(e);

        size_t curlen;
        std::tie(std::ignore, curlen) = Source->bufferProperties();

        if(curlen < BufferCapacity)
            Source->extendBuffer(BufferCapacity - curlen);

        std::tie(SourcebufferBegin, SourcebufferLength) = Source->bufferProperties();
        CurrentOffset = 0;
    }
    ~ImplData()
    {
        Decoder->Uninitialize();
        WelsDestroyDecoder(Decoder);
    }

    static void tracecb(void *ctx, int level, const char *msg)
    {
        static_cast<ImplData*>(ctx)->Logger(msg);
    }

    static bool isMarker(const void *beg, size_t off)
    {
        auto buf = static_cast<const unsigned char*>(beg);
        return buf[off] == 0 && buf[off + 1] == 0 && buf[off + 2] == 0 && buf[off + 3] == 1; //signature: 00 00 00 01
    }

    std::tuple<bool, size_t, size_t> nextNALUnit(size_t off)
    {
        if(SourcebufferLength < 4 || off >= SourcebufferLength)
            return { false, 0, 0 };

        size_t startoff = off; //begin at position where we stopped last time
        off += 4; //jump over start bytes of current NAL unit

        for(; off < SourcebufferLength - 3; ++off) //search signature of next unit until end of buffer
        {
            if(isMarker(SourcebufferBegin, off)) 
                return { true, startoff, off - startoff }; //return success, start offset into buffer, length of found unit
        }

        auto remaininglen = SourcebufferLength - startoff; //signature not found -> pull more bytes
        auto advance = startoff;
        size_t nread;
        std::tie(SourcebufferBegin, nread) = Source->advanceBuffer(advance);

        SourcebufferLength -= (advance - nread); //less data might have been read

        off -= advance; //adjust offset to where we stopped

        for(;;)
        {
            for(; off < SourcebufferLength - 3; ++off) //search signature of next unit until end of buffer
            {
                if(isMarker(SourcebufferBegin, off))
                    return { true, 0, off }; //return success, start offset into buffer, length of found unit
            }

            //not found: unit might be longer than buffer -> extend buffer
            std::tie(SourcebufferBegin, nread) = Source->extendBuffer(BufferCapacity);
            SourcebufferLength += nread;

            if(nread == 0) //no new data has been appended -> stream is empty, return last unit
                return { true, 0, SourcebufferLength }; //no new data has been appendend -> stream is empty, return last unit
        }
    }

    bool fetchFrame(Videoframe &f)
    {
        SBufferInfo bufinfo = { 0 };
        unsigned char *picptrs[3];

        while(bufinfo.iBufferStatus != 1)
        {
            const auto[success, start, len] = nextNALUnit(CurrentOffset);
            CurrentOffset = start + len;

            if(success)
                Decoder->DecodeFrameNoDelay(&static_cast<const unsigned char*>(SourcebufferBegin)[start], static_cast<int>(len), picptrs, &bufinfo);
            else
                return false;
        }

        f.Format = Pixelformat::YUV12P;
        f.Width = bufinfo.UsrData.sSystemBuffer.iWidth;
        f.Height = bufinfo.UsrData.sSystemBuffer.iHeight;
        f.Planes[0] = picptrs[0];
        f.Planes[1] = picptrs[1];
        f.Planes[2] = picptrs[2];
        f.Linestrides[0] = bufinfo.UsrData.sSystemBuffer.iStride[0];
        f.Linestrides[1] = bufinfo.UsrData.sSystemBuffer.iStride[1];
        f.Linestrides[2] = bufinfo.UsrData.sSystemBuffer.iStride[1];

        return true;
    }
};

//
// decoder
//


Decoder::Decoder(ICodecSourcebuffer &source, std::vector<std::pair<std::string, std::string>> parameters)
{
    size_t capacity = 1024 * 1024;

    for(const auto &[p, v] : parameters)
    {
        if(p == "bufcapacity")
        {
            if(sscanf(v.c_str(), "%zu", &capacity) == 0 || capacity <= 4)
                throw GenericError<WrongParameter>(p);
        }
        else
            throw GenericError<UnknownParameter>(p);
    }

    Impl = std::make_unique<ImplData>(source, capacity);
}
Decoder::~Decoder()
{
}

void Decoder::codecLogging(CodecLoglevel l, CodecLogger h)
{
    Impl->Logger = std::move(h);
    int loglevel = l == CodecLoglevel::None ? WELS_LOG_QUIET : l == CodecLoglevel::Info ? WELS_LOG_DEFAULT : WELS_LOG_DEBUG;
    Impl->Decoder->SetOption(DECODER_OPTION_TRACE_LEVEL, &loglevel);

    void *ctx = Impl.get();
    Impl->Decoder->SetOption(DECODER_OPTION_TRACE_CALLBACK_CONTEXT, &ctx);

    WelsTraceCallback cb = ImplData::tracecb;
    Impl->Decoder->SetOption(DECODER_OPTION_TRACE_CALLBACK, &cb);
}

void Decoder::codecParameter(const std::string &p, const std::string &v)
{
    if(p == "bufcapacity")
    {
        size_t capacity;
        if(sscanf(p.c_str(), "%zu", &capacity) == 0 || capacity <= 4)
            throw GenericError<WrongParameter>(p);

        Impl->BufferCapacity = capacity;
    }
    else
        throw GenericError<UnknownParameter>(p);
}

bool Decoder::fetchFrame(Videoframe &f)
{
    return Impl->fetchFrame(f);
}

std::unique_ptr<ISourceSeek> Decoder::tellFrame() const
{
    auto inf = std::make_unique<ImplData::FrameSeekinfo>();
    inf->BufferSeekinfo = Impl->Source->sourceTell();
    inf->BufferOffset = Impl->CurrentOffset;
    inf->BufferSize = Impl->SourcebufferLength;
    return inf;
}

void Decoder::seekFrame(const ISourceSeek &s)
{
    const ImplData::FrameSeekinfo &inf = static_cast<const ImplData::FrameSeekinfo&>(s);
    Impl->Source->sourceSeek(*inf.BufferSeekinfo);

    if(auto siz = Impl->Source->bufferProperties().second; siz < inf.BufferSize)
        Impl->Source->extendBuffer(inf.BufferSize - siz);

    std::tie(Impl->SourcebufferBegin, Impl->SourcebufferLength) = Impl->Source->bufferProperties();

    if(Impl->SourcebufferLength != inf.BufferSize)
    {
        Impl->SourcebufferLength = 0;
        throw StreamUnexpectedEnd("seek frame");
    }

    Impl->CurrentOffset = inf.BufferOffset;
}

}