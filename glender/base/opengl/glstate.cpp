#include "glstate.hpp"

#include <string>
#include <GL/glew.h>

namespace mlib{namespace glender{namespace opengl
{

void checkOpenGLError()
{
    auto err = glGetError();
    if(err != GL_NO_ERROR)
        throw OpenGLError(std::to_string(static_cast<unsigned int>(err)));
}

}}}