#pragma once

#include <string>
#include <stdexcept>
#include <cstddef>
#include <memory>
#include <functional>

#include "pixelformats.hpp"

namespace mlib::codec::video
{

//
// generic exceptions
//
// Codec exception strings should contain the codec's name.
//

struct CodecError : public std::runtime_error
{
    CodecError(const std::string &e) : runtime_error(".codec.video" + e) {}
};

struct UnknownParameter : public CodecError
{
    UnknownParameter(const std::string &e) : CodecError(".unknownparam" + e) {}
};

struct WrongParameter : public CodecError
{
    WrongParameter(const std::string &e) : CodecError(".wrongparam" + e) {}
};

struct CodecErrorcode : public CodecError
{
    CodecErrorcode(const std::string &e) : CodecError(".errcode" + e) {}
};

struct StreamMalformed : public CodecError
{
    StreamMalformed(const std::string &e) : CodecError(".malformed" + e) {}
};

struct StreamUnexpectedEnd : public StreamMalformed
{
    StreamUnexpectedEnd(const std::string &e) : StreamMalformed(".end" + e) {}
};

template<class ErrorClass>
struct GenericCodecError : public ErrorClass
{
    GenericCodecError(const std::string &codecname, const std::string &message)
        : ErrorClass(" (" + codecname + ") (" + message + ")") {}
};

//
// seek information base class
//
// Objects hold information for seeking within a source.
//

struct ISourceSeek
{
    virtual ~ISourceSeek() = default;
};

//
// codec source
//
// An implementation holds a buffer containing the source data.
// - extendWindow appends more data to the buffer.
//   Returns a pointer to the buffer's begin and amount window
//   is extended (=amount of new data read).
// - advanceWindow advances the virtual buffer mapping and reads n more bytes.
//   n must be not greater than the buffer's size.
//   Returns a pointer to the buffer's begin and amout m of new data read.
//   In case there is less data to read, the buffer shrinks by n-m.
// - bufferProperties retrieves a pointer to the buffer's begin and it's size
// - sourceTell returns an object holding the current offset of the mapped buffer
//   into the stream which can be passed to sourceSeek later.
// - minimalBuffer calls extendBuffer if the buffer is not large enough.
//

struct ICodecSourcebuffer
{
    virtual ~ICodecSourcebuffer() = default;

    virtual std::pair<const void*, size_t> extendBuffer(size_t length) = 0;
    virtual std::pair<const void*, size_t> advanceBuffer(size_t amount) = 0;
    virtual std::pair<const void*, size_t> bufferProperties() const = 0;

    virtual std::unique_ptr<ISourceSeek> sourceTell() const = 0;
    virtual void sourceSeek(const ISourceSeek&) = 0;

    virtual std::pair<const void*, size_t> minimalBuffer(size_t minimumlength)
    {
        auto[ptr, len] = bufferProperties();
        if(len < minimumlength)
            std::tie(ptr, len) = extendBuffer(minimumlength - len);

        return { ptr, len };
    }
};

//
// video frame
//
// Holds pointers to up to four planes and their line strides.
//

struct Videoframe
{
    Pixelformat Format;
    Pixellayout Layout;
    unsigned int Width, Height;
    const void *Planes[4];
    size_t Linestrides[4];
};

//
// log callback
//

using CodecLogger = std::function<void(std::string)>;

enum class CodecLoglevel { None, Info, Debug };

//
// video decoder
//
// - codecParameeter sets codec-specific options.
// - tellFrame returns an object that can be used to continue from a previous frame.
// - fetchFrame returns a Videoframe. The codec itself manages the buffer of pixels, so
//   the user can only rely on the pointers to be valid until another call to the codec's methods.
//   Returns false in case of end of stream.
//

struct IVideoDecodec
{
    virtual ~IVideoDecodec() = default;

    virtual void codecLogging(CodecLoglevel, CodecLogger) = 0;
    virtual void codecParameter(const std::string &parameter, const std::string &value) = 0;

    virtual bool fetchFrame(Videoframe&) = 0;

    virtual std::unique_ptr<ISourceSeek> tellFrame() const = 0;
    virtual void seekFrame(const ISourceSeek&) = 0;
};

}