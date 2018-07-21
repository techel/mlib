#pragma once

#include "opengl/globject.hpp"
#include "../types.hpp"

namespace mlib{namespace glender
{

//
// render buffer
//
// A auxilliary buffer (depth, stencil, color) to render to.
//

class RenderBuffer : public opengl::GLObjectBase<RenderBuffer>
{
public:
	RenderBuffer();
	~RenderBuffer();

	enum class Type { Color, Depth, Stencil };
	void setupStorage(const svec2 &s, Type t);
};

}}