#pragma once

#include <string>

#include "../codec.hpp"

namespace mlib::codec::video::h264
{

//
// exceptions
//

template<class T>
struct GenericError : public GenericCodecError<T>
{
    GenericError(const std::string &e) : GenericCodecError<T>("H.264", e) {}
};

struct ErrorCode : public GenericError<CodecErrorcode>
{
    ErrorCode(long errcode) : GenericError(std::to_string(errcode)) {}
};


}