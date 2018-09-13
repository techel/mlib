#include "modinstance.hpp"

#include <cassert>
#include <utility>
#include <yfw/platform.hpp>

#ifdef YFW_PLATFORM_WIN32
#include <Windows.h>
#include <mlib/unicode/unicodecvt.hpp>
#else
#include <errno.h>
#include <dlfcn.h>
#endif

namespace mlib::process::module
{

//
// platform specific
//

static void throwLastError(const char *e)
{
#ifdef YFW_PLATFORM_WIN32
	auto c = GetLastError();
#else
	auto c = errno;
	throw GenericError(e + (" (" + error::formatError(c) + ")"), c);
#endif
}

//
// instance
//

Instance::Instance() noexcept : Handle(nullptr), Dynamic(false)
{
}
Instance::Instance(const std::string &name)
{
	open(name);
}
Instance::~Instance()
{
	close();
}

bool Instance::open(const std::string &name)
{
	close();

#ifdef YFW_PLATFORM_WIN32
	const auto h = GetModuleHandleW(unicode::toNative(name).c_str());
	if(!h)
		throwLastError(".instance.open");

	Handle = reinterpret_cast<void*>(h);
	Dynamic = false;
#else 
#endif

	return true;
}

void Instance::close()
{
	if(Dynamic && Handle)
	{
#ifdef YFW_PLATFORM_WIN32
		FreeLibrary(reinterpret_cast<HMODULE>(Handle));
#else
#endif
	}
	Handle = nullptr;
}

Symbol Instance::symbol(const std::string &name)
{
	assert(Handle);

#ifdef YFW_PLATFORM_WIN32
	return reinterpret_cast<Symbol>(GetProcAddress(reinterpret_cast<HMODULE>(Handle), name.c_str()));
#endif
}

Procedure Instance::procedure(const std::string &name)
{
	return reinterpret_cast<Procedure>(symbol(name));
}

Instance Instance::current()
{
	Instance in;

#ifdef YFW_PLATFORM_WIN32
	const auto h = GetModuleHandleW(nullptr);
	in.Handle = reinterpret_cast<void*>(h);
	in.Dynamic = false;
#endif

	return in;
}

Instance::Instance(Instance &&rhs) noexcept : Handle(nullptr), Dynamic(false)
{
	std::swap(Handle, rhs.Handle);
	std::swap(Dynamic, rhs.Dynamic);
}

Instance &Instance::operator=(Instance &&rhs) noexcept
{
	std::swap(Handle, rhs.Handle);
	std::swap(Dynamic, rhs.Dynamic);
	return *this;
}

}