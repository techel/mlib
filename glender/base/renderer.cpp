#include "renderer.hpp"

#include <GL/glew.h>
#include "opengl/glstate.hpp"

#include "texture.hpp"
#include "framebuffer.hpp"
#include "operators.hpp"
#include "shader.hpp"

namespace mlib{namespace glender
{

//
// renderer
//

Renderer::Renderer()
{
	glEnable(GL_SCISSOR_TEST);
	glEnable(GL_TEXTURE_2D);
	glClearDepth(1.0f);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	opengl::checkOpenGLError();

	uint32_t white[1] = { 0xFFFFFFFF };
	WhiteTex.loadPixels(white, ivec2(1, 1));
}
Renderer::~Renderer()
{
}

std::string Renderer::information() const
{
	std::string info = std::string("renderer: ") + (const char*)glGetString(GL_RENDERER) +
		"; vendor: " + (const char*)glGetString(GL_VENDOR) +
		"; version: " + (const char*)glGetString(GL_VERSION);

	return info;
}


Renderer::ViewportTicket Renderer::useViewport(const Viewport &v)
{
    ViewportTicket ticket(*this, View);
    viewport(&v);
    return ticket;
}
void Renderer::viewport(const Viewport *v)
{
    View = v;
    if(v)
    {
        glViewport(v->offset().x, v->offset().y, v->size().x, v->size().y);
        glScissor(v->offset().x, v->offset().y, v->size().x, v->size().y);
    }
}


Renderer::TargetTicket Renderer::useTarget(FrameBuffer &t)
{
    TargetTicket ticket(*this, Target);
    target(&t);
    return ticket;
}
void Renderer::target(FrameBuffer *t)
{
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, t ? static_cast<GLuint>(t->handle()) : 0);
    Target = t;
}


Renderer::BlendTicket Renderer::useBlending(const BlendFunction &f)
{
    BlendTicket ticket(*this, Blending);
    blending(&f);
    return ticket;
}
void Renderer::blending(const BlendFunction *f)
{
    Blending = f;
    if(f)
    {
        glEnable(GL_BLEND);
        auto params = f->openglParameters();
        glBlendEquationSeparate(params[0], params[3]);
        glBlendFuncSeparate(params[1], params[2], params[4], params[5]);
    }
    else
        glDisable(GL_BLEND);
}


Renderer::StencilTicket Renderer::useStencil(const StencilFunction &s)
{
    StencilTicket ticket(*this, Stencil);
    stencil(&s);
    return ticket;
}
void Renderer::stencil(const StencilFunction *s)
{
    Stencil = s;
    if(s)
    {
        glEnable(GL_STENCIL_TEST);
        auto values = s->openglValues();
        auto ops = s->openglOperations();
        glStencilMask(values[2]);
        glStencilFunc(ops[2], values[1], values[0]);
        glStencilOp(ops[0], ops[0], ops[1]);
    }
    else
        glDisable(GL_STENCIL_TEST);
}


Renderer::DepthTicket Renderer::useDepth(const DepthFunction &f)
{
    DepthTicket ticket(*this, Depth);
    depth(&f);
    return ticket;
}
void Renderer::depth(const DepthFunction *f)
{
    Depth = f;
    if(f && f->test())
    {
        glEnable(GL_DEPTH_TEST);
        glDepthMask(f->mask() ? GL_TRUE : GL_FALSE);
        glDepthFunc(GL_LESS);
    }
    else
        glDisable(GL_DEPTH_TEST);
}


Renderer::ColorTicket Renderer::useColor(const ColorFunction &cf)
{
    ColorTicket ticket(*this, Color);
    color(&cf);
    return ticket;
}
void Renderer::color(const ColorFunction *cf)
{
    if(cf)
    {
        auto mask = cf->mask();
        glColorMask(mask[0], mask[1], mask[2], mask[3]);
    }
    else
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}


void Renderer::bindShader(const Shader &s)
{
    glUseProgram(static_cast<GLuint>(s.handle()));
    CurrentShader = &s;
}


void Renderer::clear(const normvec4 &c)
{
    glClearColor(c.x, c.y, c.z, c.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}
void Renderer::clearStencil(uint8_t value)
{
    glStencilMask(0xFF);
    glClearStencil(value);
    glClear(GL_STENCIL_BUFFER_BIT);
}
void Renderer::clearDepth()
{
    glClear(GL_DEPTH_BUFFER_BIT);
}

void Renderer::bindTexture(size_t unit, const Texture &t)
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(t.handle()));
}


void Renderer::enableVertexAttribute(size_t index)
{
    glEnableVertexAttribArray(static_cast<GLuint>(index));
}
void Renderer::disableVertexAttribute(size_t index)
{
    glDisableVertexAttribArray(static_cast<GLuint>(index));
}
template<>
void Renderer::selectVertexAttributes(size_t index, const VertexAttributes<float> &t)
{
    enableVertexAttribute(index);
    glVertexAttribPointer(static_cast<GLuint>(index), 1, GL_FLOAT, GL_FALSE, 0, t.raw());
}
template<>
void Renderer::selectVertexAttributes(size_t index, const VertexAttributes<glm::vec<2, float>> &t)
{
    enableVertexAttribute(index);
    glVertexAttribPointer(static_cast<GLuint>(index), 2, GL_FLOAT, GL_FALSE, 0, t.raw());
}
template<>
void Renderer::selectVertexAttributes(size_t index, const VertexAttributes<glm::vec<3, float>> &t)
{
    enableVertexAttribute(index);
    glVertexAttribPointer(static_cast<GLuint>(index), 3, GL_FLOAT, GL_FALSE, 0, t.raw());
}
template<>
void Renderer::selectVertexAttributes(size_t index, const VertexAttributes<glm::vec<4, float>> &t)
{
    enableVertexAttribute(index);
    glVertexAttribPointer(static_cast<GLuint>(index), 4, GL_FLOAT, GL_FALSE, 0, t.raw());
}
void Renderer::renderPrimitives(PrimitiveType primtype, size_t num)
{
    glDrawArrays(static_cast<GLenum>(primtype), 0, num);
}

}}