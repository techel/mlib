#pragma once

#include <stdexcept>

namespace mlib{namespace glender{namespace opengl
{

//
// exceptions
//

struct OpenGLError : public std::runtime_error
{
    OpenGLError(const std::string &msg) : runtime_error(".glender.opengl [" + msg + "]") {}
};

void checkOpenGLError();

}}}