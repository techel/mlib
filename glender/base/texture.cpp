#include "texture.hpp"

#include <GL/glew.h>
#include "opengl/glstate.hpp"

namespace mlib{namespace glender
{

Texture::Texture()
{
	GLuint h;
	glGenTextures(1, &h);
	opengl::checkOpenGLError();

	Handle = static_cast<uintptr_t>(h);

	sampling(Sampling());
}

Texture::~Texture()
{
	GLuint h = static_cast<GLuint>(Handle);
	glDeleteTextures(1, &h);
}

void Texture::sampling(const Sampling &s)
{
	glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(Handle));

	GLint filter = s.Filter == Sampling::FilterType::Linear ? GL_LINEAR : GL_NEAREST;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

	GLint repx = s.ClampX == Sampling::ClampType::Clamp ? GL_CLAMP : GL_REPEAT;
    GLint repy = s.ClampY == Sampling::ClampType::Clamp ? GL_CLAMP : GL_REPEAT;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, repx);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, repy);
}

void Texture::setupStorage(const svec2 &s, Type t)
{
	glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(Handle));

	GLint format, intformat;
	switch(t)
	{
		case Type::Color4: intformat = format = GL_RGBA; break;
		case Type::Depth: format = GL_DEPTH_COMPONENT; intformat = GL_DEPTH_COMPONENT; break;
		case Type::Stencil: format = intformat = GL_DEPTH_STENCIL; break;
		default: assert(false);
	}

	glTexImage2D(GL_TEXTURE_2D, 0, intformat, s.x, s.y, 0, format, GL_FLOAT, nullptr);
	opengl::checkOpenGLError();

	Size = s;
}

void Texture::loadPixels(const uint32_t *data, const svec2 &s)
{
	glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(Handle));
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, s.x, s.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	opengl::checkOpenGLError();

	Size = s;
}
void Texture::loadSubpixels(const uint32_t *data, const svec2 &s, const svec2 &off)
{
	glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(Handle));
	glTexSubImage2D(GL_TEXTURE_2D, 0, off.x, off.y, s.x, s.y, GL_RGBA, GL_UNSIGNED_BYTE, data);
    opengl::checkOpenGLError();
}

void Texture::pullPixels(uint32_t *dest)
{
    if(Size.x == 0 || Size.y == 0)
        return;

    glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(Handle));
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, dest);
}

}}