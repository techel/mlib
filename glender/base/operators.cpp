#include "operators.hpp"

#include <GL/glew.h>

namespace mlib{namespace glender
{

//
// blending parameters
//

static unsigned int blendOperator(BlendFunction::Operator op)
{
	switch(op)
	{
		case BlendFunction::Operator::Add: return GL_FUNC_ADD;
		case BlendFunction::Operator::Subtract: return GL_FUNC_SUBTRACT;
		case BlendFunction::Operator::Less: return GL_MIN;
		case BlendFunction::Operator::Greater: return GL_MAX;
	}
	abort();
}
static unsigned blendKoefficient(BlendFunction::Koefficient k)
{
	switch(k)
	{
		case BlendFunction::Koefficient::One: return GL_ONE;
		case BlendFunction::Koefficient::Zero: return GL_ZERO;
		case BlendFunction::Koefficient::SrcAlpha: return GL_SRC_ALPHA;
		case BlendFunction::Koefficient::DstAlpha: return GL_DST_ALPHA;
		case BlendFunction::Koefficient::NegSrcAlpha: return GL_ONE_MINUS_SRC_ALPHA;
		case BlendFunction::Koefficient::NegDstAlpha: return GL_ONE_MINUS_DST_ALPHA;
	}
	abort();
}
BlendFunction::BlendFunction(Operator rgbOp, Koefficient rgbSrc, Koefficient rgbDst, Operator aOp, Koefficient aSrc, Koefficient aDst)
{
	RgbOp = blendOperator(rgbOp);
	RgbSrcK = blendKoefficient(rgbSrc);
	RgbDstK = blendKoefficient(rgbDst);
	AlphaOp = blendOperator(aOp);
	AlphaSrcK = blendKoefficient(aSrc);
	AlphaDstK = blendKoefficient(aDst);
}
BlendFunction::BlendFunction(Operator op, Koefficient src, Koefficient dst)
{
	*this = BlendFunction(op, src, dst, op, src, dst);
}
BlendFunction::BlendFunction(Template t)
{
	switch(t)
	{
		case Template::Translucency:
			*this = BlendFunction(Operator::Add, Koefficient::SrcAlpha, Koefficient::NegSrcAlpha);
			break;
		default:
			abort();
	}
}

//
// stencil parameters
//

static unsigned int stencilOperation(StencilFunction::Operation op)
{
	switch(op)
	{
		case StencilFunction::Operation::Keep: return GL_KEEP;
		case StencilFunction::Operation::Replace: return GL_REPLACE;
		case StencilFunction::Operation::Zero: return GL_ZERO;
		case StencilFunction::Operation::Incr: return GL_INCR;
	}
	abort();
}
static unsigned int stencilFunction(StencilFunction::Function f)
{
	switch(f)
	{
		case StencilFunction::Function::Always: return GL_ALWAYS;
		case StencilFunction::Function::Never: return GL_NEVER;
		case StencilFunction::Function::Less: return GL_LESS;
		case StencilFunction::Function::Greater: return GL_GREATER;
		case StencilFunction::Function::Grequal: return GL_GEQUAL;
		case StencilFunction::Function::Lequal: return GL_LEQUAL;
		case StencilFunction::Function::Equal: return GL_EQUAL;
		case StencilFunction::Function::NotEqual: return GL_NOTEQUAL;
	}
	abort();
}

StencilFunction::StencilFunction(Operation passop, Operation failop, Function f, uint8_t ref, uint8_t writemask, uint8_t mask)
{
	PassOp = stencilOperation(passop);
	FailOp = stencilOperation(failop);
	Func = stencilFunction(f);
	Reference = ref;
	WriteMask = writemask;
	ReadMask = mask;
}

//
// depth parameters
//

DepthFunction::DepthFunction(Template t)
{
	if(t == Template::Simple)
		*this = DepthFunction(true, true);

	abort();
}

//
// color parameters
//

}}