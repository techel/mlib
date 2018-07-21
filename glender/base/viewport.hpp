#pragma once

#include "../types.hpp"

namespace mlib{namespace glender
{

//
// viewport
//

class Viewport
{
public:
    Viewport() = default;
    Viewport(ivec2 off, uvec2 size) : Offset(off), Size(size) {}

	void offset(ivec2 v) { Offset = v; }
	void resize(uvec2 s) { Size = s; }
	const auto &offset() const { return Offset; }
	const auto &size() const { return Size; }

    uvec2 fromCanvas(ivec2 point) { return point - Offset; }
    uvec2 toCanvas(ivec2 point) { return point + Offset; }

	//computes viewport from canvas and ratio to eventually add horizontal or vertical black bars later
    static Viewport ratioViewport(uvec2 canvassize, uvec2 aspectratio);

private:
    ivec2 Offset;
    uvec2 Size;
};

}}