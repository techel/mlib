#pragma once

#include "opengl/globject.hpp"
#include "../types.hpp"

namespace mlib{namespace glender
{

//
// texture
//
// Holds a 2-dimensional texture.
//

class Texture : public opengl::GLObjectBase<Texture>
{
public:
	Texture();
	~Texture();

	enum class Type { Color4, Depth, Stencil };

	struct Sampling
	{
		enum class FilterType { None, Linear } Filter = FilterType::None;
		enum class ClampType { Repeat, Clamp } ClampX = ClampType::Clamp, ClampY = ClampType::Clamp;
	};

	void sampling(const Sampling &s);

	void setupStorage(const svec2 &size, Type type);
	void loadPixels(const uint32_t *data, const svec2 &size);
	void loadSubpixels(const uint32_t *data, const svec2 &size, const svec2 &offset);

	const svec2 &size() const { return Size; }

    //retrieves width*heigth pixels
    void pullPixels(uint32_t *dest);

	Texture(Texture&&) = default;
	Texture &operator=(Texture&&) = default;

private:
	glm::ivec2 Size;
};

}}