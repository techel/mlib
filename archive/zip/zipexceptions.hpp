#pragma once

#include <stdexcept>

namespace mlib::archive::zip
{

//
// various exceptions
//

struct Error : public std::runtime_error
{
	Error(const std::string &e) : runtime_error("stream.zip" + e) {}
};

struct GeneralError : public Error
{
	GeneralError(const std::string &msg) : Error(" (" + msg + ")") {}
};

struct RemoveError : public Error
{
	RemoveError() : Error(".cannotremove") {}
};

struct EofError : public Error
{
	EofError() : Error(".eof") {}
};

struct OpenError : public Error
{
	OpenError() : Error(".open") {}
};

struct NotFound : public Error
{
	NotFound() : Error(".notfound") {}
};

struct ReadError : public Error
{
	ReadError() : Error(".read") {}
};

struct WriteError : public Error
{
	WriteError() : Error(".write") {}
};

struct SeekError : public Error
{
	SeekError() : Error(".seek") {}
};

struct InvalidStat : public Error
{
	InvalidStat() : Error(".invstat") {}
};

struct ChecksumInvalid : public Error
{
	ChecksumInvalid() : Error(".checksum") {}
};

struct MultidiskUnsupported : public Error
{
	MultidiskUnsupported() : Error(".multidisk") {}
};

struct UnknownError : public Error
{
	UnknownError() : Error(".unknown") {}
};

struct UnknownCompression : public Error
{
	UnknownCompression() : Error(".compunknown") {}
};

struct UnknownFormat : public Error
{
	UnknownFormat() : Error(".formatunknown") {}
};

struct Inconsistent : public Error
{
	Inconsistent() : Error(".inconsistent") {}
};

struct UnknownEncryption : public Error
{
	UnknownEncryption() : Error(".cryptunknown") {}
};

struct PasswordRequired : public Error
{
	PasswordRequired() : Error(".needpassword") {}
};

struct PasswordWrong : public Error
{
	PasswordWrong() : Error(".passwordwrong") {}
};

}}}