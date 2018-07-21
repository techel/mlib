#pragma once

#include <string>
#include "../types.hpp"
#include "../util/minticket.hpp"

#include "texture.hpp"
#include "viewport.hpp"
#include "vertexbuffer.hpp"

namespace mlib{namespace glender
{

class FrameBuffer;
class BlendFunction;
class StencilFunction;
class DepthFunction;
class ColorFunction;
class Shader;

//
// renderer
//
// Holds a set of states and renders primitives by binding attributes and subsequently rendering them.
//

class Renderer
{
public:
	Renderer();
	~Renderer();

	Renderer(const Renderer&) = delete;
	Renderer(Renderer&&) = delete;
	Renderer operator=(const Renderer&) = delete;
	Renderer operator=(Renderer&&) = delete;

	std::string information() const;

    const Texture &whiteTexture() { return WhiteTex; }

	void clear(const normvec4 &color);
	void clearStencil(uint8_t value);
	void clearDepth();

    void viewport(const Viewport *v);
    void target(FrameBuffer *t);
    void blending(const BlendFunction *f);
    void stencil(const StencilFunction *s);
    void depth(const DepthFunction *f);
    void color(const ColorFunction *c);

    const StencilFunction *currentStencilFunc() const { return Stencil; }

	using ViewportTicket = util::MinimalTicket<Renderer, const Viewport*, &Renderer::viewport>;
	using TargetTicket = util::MinimalTicket<Renderer, FrameBuffer*, &Renderer::target>;
	using BlendTicket = util::MinimalTicket<Renderer, const BlendFunction*, &Renderer::blending>;
	using StencilTicket = util::MinimalTicket<Renderer, const StencilFunction*, &Renderer::stencil>;
    using DepthTicket = util::MinimalTicket<Renderer, const DepthFunction*, &Renderer::depth>;
    using ColorTicket = util::MinimalTicket<Renderer, const ColorFunction*, &Renderer::color>;

	ViewportTicket useViewport(const Viewport &v);
	TargetTicket useTarget(FrameBuffer &t);
	BlendTicket useBlending(const BlendFunction &f);
	StencilTicket useStencil(const StencilFunction &s);
    DepthTicket useDepth(const DepthFunction &d);
    ColorTicket useColor(const ColorFunction &cf);

	void bindTexture(size_t unit, const Texture &t);
	void bindShader(const Shader &s);

	template<class T>
	auto bindAttributes(size_t attribindex, const VertexAttributes<T> &attribs)
	{
		selectVertexAttributes(attribindex, attribs);
		return util:::MinimalTicket<Renderer, size_t, &Renderer::disableVertexAttribute>(*this, attribindex);
	}

	enum class PrimitiveType { Triangles = 4, Quads = 7 };
	void renderPrimitives(PrimitiveType type, size_t numvertices);

private:
    const Viewport *View = nullptr;
    FrameBuffer *Target = nullptr;
    const BlendFunction *Blending = nullptr;
    const StencilFunction *Stencil = nullptr;
    const DepthFunction *Depth = nullptr;
    const ColorFunction *Color = nullptr;
    const Shader *CurrentShader = nullptr;

    Texture WhiteTex;

    template<class Type>
    void selectVertexAttributes(size_t index, const VertexAttributes<Type> &t);
    void enableVertexAttribute(size_t index);
    void disableVertexAttribute(size_t index);
};

}}