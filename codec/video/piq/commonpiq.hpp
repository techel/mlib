#pragma once

#include "../codec.hpp"

namespace mlib::codec::video::piq
{

//
// exceptions
//

template<class T>
struct GenericError : public GenericCodecError<T>
{
    GenericError(const std::string &e) : GenericCodecError<T>("PIQ", e) {}
};

struct ErrorCode : public GenericError<CodecErrorcode>
{
    ErrorCode(long errcode) : GenericError(std::to_string(errcode)) {}
};


}