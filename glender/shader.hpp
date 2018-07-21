#pragma once

#include "../types.hpp"
#include "../commonerror.hpp"
#include "opengl/globject.hpp"
#include "util/rawarray.hpp"

namespace mlib{namespace glender
{

//
// exceptions
//

struct FragShaderCompilationError : public CommonError
{
	FragShaderCompilationError(const std::string &s) : CommonError(".shadercomp.frag [" + s + "]") {}
};
struct VertShaderCompilationError : public CommonError
{
	VertShaderCompilationError(const std::string &s) : CommonError(".shadercomp.vert [" + s + "]") {}
};
struct ShaderLinkError : public CommonError
{
	ShaderLinkError(const std::string &s) : CommonError(".shadercomp.link [" + s + "]") {}
};
struct UnknownShaderAttribute : public CommonError
{
	UnknownShaderAttribute(const std::string &s) : CommonError(".shaderattr [" + s + "]") {}
};
struct UnknownShaderUniform : public CommonError
{
	UnknownShaderUniform(const std::string &s) : CommonError(".shaderunif [ " + s + "]") {}
};

//
// shader
//
// A shader, programmed with GLSL.
//

class Shader : public opengl::GLObjectBase<Shader>
{
public:
	Shader(const std::string &vertex, const std::string &fragment);
	~Shader();

	size_t uniformIndex(std::string name) const;
	size_t attributeIndex(std::string name) const;

	template<class Type>
	void uniformValue(size_t index, const Type &t);

	template<size_t ArraySize, class Type>
	void uniformArray(size_t index, const Type *begin)
	{
		uniformArray(index, begin, multipleObjectsToArray<ArraySize>(begin).data(), ArraySize);
	}

	Shader(Shader&&) = default;
	Shader &operator=(Shader&&) = default;

private:
	template<class T, class BasicType> void uniformArray(size_t index, const T *ar, const BasicType *begin, size_t num);

};

}}