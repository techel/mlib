#include "renderbuffer.hpp"

#include <GL/glew.h>

namespace mlib{namespace glender
{

RenderBuffer::RenderBuffer()
{
    GLuint h;
    glGenRenderbuffers(1, &h);
    Handle = static_cast<uintptr_t>(h);
}

RenderBuffer::~RenderBuffer()
{
    GLuint h = static_cast<GLuint>(Handle);
    glDeleteRenderbuffers(1, &h);
}

void RenderBuffer::setupStorage(const svec2 &s, Type t)
{
    glBindRenderbuffer(GL_RENDERBUFFER, static_cast<GLuint>(Handle));

    GLuint type;
    switch(t)
    {
        case Type::Color: type = GL_RGBA; break;
        case Type::Depth: type = GL_DEPTH_COMPONENT; break;
        case Type::Stencil: type = GL_STENCIL_COMPONENTS; break;
        default: assert(false);
    }
    glRenderbufferStorage(GL_RENDERBUFFER, type, s.x, s.y);
}

}}