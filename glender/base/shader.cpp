#include "shader.hpp"

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

namespace mlib{namespace glender
{

//
//	OpenGL object wrapper with deleter
//

struct ObjectWrapper
{
	GLuint Handle;
	void(*Deleter)(GLuint);

	ObjectWrapper(GLuint handle, void(*del)(GLuint)) : Handle(handle), Deleter(del) {}
	~ObjectWrapper()
	{
		if(Handle)
			Deleter(Handle);
	}

	void release() { Handle = 0; }
};

//
// object deleters
//

static void deleteShader(GLuint handle)
{
	glDeleteShader(handle);
}
static void deleteProgram(GLuint handle)
{
	glDeleteProgram(handle);
}

//
// object validators

template<class ExceptionClass>
static void validateShader(GLuint hShader)
{
	GLint compiled;
	glGetShaderiv(hShader, GL_COMPILE_STATUS, &compiled);
	if(!compiled)
	{
		GLint loglen = 0;
		glGetShaderiv(hShader, GL_INFO_LOG_LENGTH, &loglen);
		std::string log;
		if(loglen > 0)
		{
			log.resize(loglen);
			glGetShaderInfoLog(hShader, loglen, nullptr, &log[0]);
			throw ExceptionClass(log);
		}
	}
}
static void validateProgram(GLuint hProgram)
{
	GLint linked;
	glGetProgramiv(hProgram, GL_LINK_STATUS, &linked);
	if(!linked)
	{
		GLint loglen = 0;
		glGetProgramiv(hProgram, GL_INFO_LOG_LENGTH, &loglen);
		std::string log;
		if(loglen > 0)
		{
			log.resize(loglen);
			glGetShaderInfoLog(hProgram, loglen, nullptr, &log[0]);
			throw ShaderLinkError(log);
		}
	}
}

//
// shader
//

static void useShader(GLuint handle)
{
	glUseProgram(handle);
}

Shader::Shader(const std::string &vertex, const std::string &fragment)
{
	const char *vertsrc = vertex.c_str();
	const char *fragsrc = fragment.c_str();
	GLint vertlen = static_cast<GLint>(vertex.size());
	GLint fraglen = static_cast<GLint>(fragment.size());

	glUseProgram(0);

	ObjectWrapper vert(glCreateShader(GL_VERTEX_SHADER), &deleteShader);
	glShaderSource(vert.Handle, 1, &vertsrc, &vertlen);
	glCompileShader(vert.Handle);
	validateShader<VertShaderCompilationError>(vert.Handle);

	ObjectWrapper frag(glCreateShader(GL_FRAGMENT_SHADER), &deleteShader);
	glShaderSource(frag.Handle, 1, &fragsrc, &fraglen);
	glCompileShader(frag.Handle);
	validateShader<FragShaderCompilationError>(frag.Handle);

	ObjectWrapper prog(glCreateProgram(), &deleteProgram);
	glAttachShader(prog.Handle, vert.Handle);
	glAttachShader(prog.Handle, frag.Handle);
	glLinkProgram(prog.Handle);
	validateProgram(prog.Handle);

	glDetachShader(prog.Handle, vert.Handle);
	glDetachShader(prog.Handle, frag.Handle);

	glUseProgram(0);

	Handle = prog.Handle;
	prog.release();
}
Shader::~Shader()
{
	glUseProgram(0);
	glDeleteProgram(Handle);
}
size_t Shader::attributeIndex(std::string name) const
{
	useShader(static_cast<GLuint>(Handle));
	auto idx = glGetAttribLocation(static_cast<GLuint>(Handle), name.c_str());
	if(idx < 0)
		throw UnknownShaderAttribute(name);
	return static_cast<size_t>(idx);
}
size_t Shader::uniformIndex(std::string name) const
{
	useShader(static_cast<GLuint>(Handle));
	auto idx = glGetUniformLocation(static_cast<GLuint>(Handle), name.c_str());
	if(idx < 0)
		throw UnknownShaderUniform(name);
	return static_cast<size_t>(idx);
}

//
// uniform values
//

// TODO: support more uniform types

template<>
void Shader::uniformValue(size_t index, const int &t)
{
	useShader(static_cast<GLuint>(Handle));
	glUniform1i(static_cast<GLuint>(index), t);
}

template<>
void Shader::uniformValue(size_t index, const float &t)
{
	useShader(static_cast<GLuint>(Handle));
	glUniform1f(static_cast<GLuint>(index), t);
}

template<>
void Shader::uniformValue(size_t index, const double &t)
{
	useShader(static_cast<GLuint>(Handle));
	glUniform1d(static_cast<GLuint>(index), t);
}

template<>
void Shader::uniformValue(size_t index, const glm::vec<2, float> &t)
{
	useShader(static_cast<GLuint>(Handle));
	glUniform2f(static_cast<GLuint>(index), t.x, t.y);
}

template<>
void Shader::uniformValue(size_t index, const glm::vec<3, float> &t)
{
	useShader(static_cast<GLuint>(Handle));
	glUniform3f(static_cast<GLuint>(index), t.x, t.y, t.z);
}

template<>
void Shader::uniformValue(size_t index, const glm::vec<4, float> &t)
{
	useShader(static_cast<GLuint>(Handle));
	glUniform4f(static_cast<GLuint>(index), t.x, t.y, t.z, t.w);
}

template<>
void Shader::uniformValue(size_t index, const glm::mat<4, 4, float> &t)
{
	useShader(static_cast<GLuint>(Handle));
	glUniformMatrix4fv(static_cast<GLuint>(index), 1, GL_FALSE, glm::value_ptr(t));
}

//
// uniform array
//

template<>
void Shader::uniformArray(size_t index, const float *ar, const float *begin, size_t num)
{
	useShader(static_cast<GLuint>(Handle));
	glUniform1fv(static_cast<GLuint>(index), static_cast<GLsizei>(num), begin);
}

template<>
void Shader::uniformArray(size_t index, const vec2 *ar, const float *begin, size_t num)
{
	useShader(static_cast<GLuint>(Handle));
	glUniform2fv(static_cast<GLuint>(index), static_cast<GLsizei>(num), begin);
}

template<>
void Shader::uniformArray(size_t index, const vec3 *ar, const float *begin, size_t num)
{
	useShader(static_cast<GLuint>(Handle));
	glUniform3fv(static_cast<GLuint>(index), static_cast<GLsizei>(num), begin);
}

template<>
void Shader::uniformArray(size_t index, const vec4 *ar, const float *begin, size_t num)
{
	useShader(static_cast<GLuint>(Handle));
	glUniform4fv(static_cast<GLuint>(index), static_cast<GLsizei>(num), begin);
}

}}