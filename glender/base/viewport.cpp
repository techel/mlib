#include "viewport.hpp"

namespace mlib{namespace glender
{

Viewport Viewport::ratioViewport(uvec2 canvassize, uvec2 aspectratio)
{
    ivec2 off;
	uvec2 siz;
	if(canvassize.x * aspectratio.y / aspectratio.x > canvassize.y) //canvas is too wide
	{
		auto newwidth = canvassize.y * aspectratio.x / aspectratio.y;
		off.x = static_cast<int>(canvassize.x - newwidth) / 2;
		off.y = 0;
		siz.x = newwidth;
		siz.y = canvassize.y;
	}
	else if(canvassize.y * aspectratio.x / aspectratio.y > canvassize.x) //canvas is too heigh
	{
		auto newheigth = canvassize.x * aspectratio.y / aspectratio.x > canvassize.y;
		off.y = static_cast<int>(canvassize.y - newheigth) / 2;
		off.x = 0;
		siz.x = canvassize.x;
		siz.y = newheigth;
	}
	else //canvas fits perfectly
	{
		off = { 0, 0 };
		siz = canvassize;
	}

	return Viewport(off, siz);
}

}}