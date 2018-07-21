#pragma once

#include <utility>
#include <cstdint>

namespace mlib{namespace glender{namespace opengl
{

//
// Base class for OpenGL objects.
//
// Holds a handle to an OpenGL object. May be moved but not copied.
//

template<class T>
class GLObjectBase
{
public:
	GLObjectBase() = default;
	GLObjectBase(GLObjectBase &&rhs)
	{
		std::swap(Handle, rhs.Handle);
	}
	GLObjectBase &operator=(GLObjectBase &&rhs) noexcept
	{
		std::swap(Handle, rhs.Handle);
		return *this;
	}
	GLObjectBase(const GLObjectBase&) = delete;
	GLObjectBase &operator=(const GLObjectBase&) = delete;

	uintptr_t handle() const { return Handle; }

protected:
	uintptr_t Handle;
};

}}}