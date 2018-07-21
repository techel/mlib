#include "framebuffer.hpp"

#include <GL/glew.h>
#include "opengl/glstate.hpp"

namespace mlib{namespace glender
{

FrameBuffer::FrameBuffer()
{
	GLuint h;
	glGenFramebuffers(1, &h);
	Handle = static_cast<uintptr_t>(h);
}
FrameBuffer::~FrameBuffer()
{
	GLuint h = static_cast<GLuint>(Handle);
	glDeleteFramebuffers(1, &h);
}
static GLenum attachmentToGL(FrameBuffer::Attachment a)
{
	GLenum e;
	switch(a)
	{
		case FrameBuffer::Attachment::Color: e = GL_COLOR_ATTACHMENT0; break;
		case FrameBuffer::Attachment::Depth: e = GL_DEPTH_ATTACHMENT; break;
		case FrameBuffer::Attachment::Stencil: e = GL_STENCIL_ATTACHMENT; break;
		default: assert(false);
	}
	return e;
}

struct FramebufferTicket
{
	GLint CurrentFBO;

	FramebufferTicket(GLuint newFBO)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &CurrentFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, newFBO);
	}
	~FramebufferTicket()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, CurrentFBO);
	}
};

void FrameBuffer::attachTexture(Texture &t, Attachment a)
{
	FramebufferTicket ticket(static_cast<GLuint>(Handle));
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentToGL(a), GL_TEXTURE_2D, static_cast<GLuint>(Handle), 0);
	validate();
}
void FrameBuffer::attachBuffer(RenderBuffer &t, Attachment a)
{
	FramebufferTicket ticket(static_cast<GLuint>(Handle));
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachmentToGL(a), GL_RENDERBUFFER, static_cast<GLuint>(Handle));
	validate();
}
void FrameBuffer::validate()
{
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		opengl::checkOpenGLError();
}

}}