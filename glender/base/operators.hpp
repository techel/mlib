#pragma once

#include <array>

namespace mlib{namespace glender
{

//
// blending parameters
//

class BlendFunction
{
public:
	enum class Operator { Add, Subtract, Less, Greater };
	enum class Koefficient { One, Zero, SrcAlpha, DstAlpha, NegSrcAlpha, NegDstAlpha };
	enum class Template { Translucency };

	BlendFunction(Operator rgbOp, Koefficient rgbSrc, Koefficient rgbDst, Operator aOp, Koefficient aSrc, Koefficient aDst);
	BlendFunction(Operator op, Koefficient src, Koefficient dst);
	BlendFunction(Template t);

	std::array<unsigned int, 6> openglParameters() const { return { RgbOp, RgbSrcK, AlphaDstK, AlphaOp, AlphaSrcK, AlphaDstK }; }

private:
	unsigned int RgbOp, RgbSrcK, RgbDstK, AlphaOp, AlphaSrcK, AlphaDstK;
};

//
// stencil parameters
//

class StencilFunction
{
public:
	enum class Operation { Keep, Zero, Replace, Incr };
	enum class Function { Never, Always, Less, Greater, Lequal, Grequal, Equal, NotEqual };

	StencilFunction(Operation passop, Operation failop, Function f, uint8_t ref, uint8_t writemask, uint8_t readmask);

	std::array<uint8_t, 3> openglValues() const { return { ReadMask, Reference, WriteMask }; }
	std::array<unsigned int, 3> openglOperations() const { return { FailOp, PassOp, Func }; }
	uint8_t openglReference() const { return Reference; }

private:
	uint8_t ReadMask, WriteMask, Reference;
	unsigned int FailOp, PassOp, Func;
};

//
// depth parameters
//

class DepthFunction
{
public:
	enum class Template { Simple };

	DepthFunction(bool write, bool test) : Write(true), Test(true) {}
	DepthFunction(Template t);

    bool test() const { return Test; }
	bool mask() const { return Write; }

private:
	bool Write = true, Test = true;
};

//
// color parameters
//

class ColorFunction
{
public:
	ColorFunction(bool r, bool g, bool b, bool a) : Red(r), Green(g), Blue(b), Alpha(a) {}

	std::array<bool, 4> mask() const { return { Red, Green, Blue, Alpha }; }

private:
	bool Red = true, Green = true, Blue = true, Alpha = true;
};

}}