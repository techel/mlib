#pragma once

#include <stdexcept>

namespace mlib{namespace glender
{

struct CommonError : public std::runtime_error
{
	CommonError(const std::string &msg) : runtime_error(".glender." + msg) {}
};

}}