#pragma once

#include "opengl/globject.hpp"
#include "../types.hpp"
#include "texture.hpp"
#include "renderbuffer.hpp"

namespace mlib{namespace glender
{

//
// frame buffer
//
// Contains various renderbuffers or rendertextures.
//

class FrameBuffer : public opengl::GLObjectBase<FrameBuffer>
{
public:
	FrameBuffer();
	~FrameBuffer();

	enum class Attachment { Color, Depth, Stencil };
	void attachTexture(Texture &t, Attachment a);
	void attachBuffer(RenderBuffer &t, Attachment a);

	void resize(const svec2 &s) { Size = s; }
	const svec2 &size() const { return Size; }

private:
    uvec2 Size;
    void validate();
};

}}