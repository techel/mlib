#include "modresources.hpp"

#include <utility>
#include <cassert>
#include <mlib/platform.hpp>

#ifdef MLIB_PLATFORM_WIN32
#include <Windows.h>
#include <mlib/unicode/unicodecvt.hpp>
#endif

namespace mlib::process::module
{

Resource::Resource() noexcept : Handle(nullptr)
{
}

Resource::Resource(const Instance &inst, const std::string &resname) : Resource(inst.native(), resname)
{
}

Resource::Resource(void *native, const std::string &resname)
{
	if(!open(native, resname))
		throw ResourceNotFound(resname);
}

Resource::~Resource()
{
	close();
}

bool Resource::open(const Instance &inst, const std::string &resname)
{
	return open(inst.native(), resname);
}

bool Resource::open(void *native, const std::string &resname)
{
	assert(native);

	close();

#ifdef MLIB_PLATFORM_WIN32
	std::string type, name;
	const auto i = resname.find_last_of(".");
	if(i == resname.npos)
	{
		name = resname;
	}
	else
	{
		name = resname.substr(0, i);
		type = resname.substr(i + 1);
	}

	auto hmodule = reinterpret_cast<HMODULE>(native);
	auto hres = FindResourceA(hmodule, name.c_str(), type.c_str());
	if(!hres)
		return false;

	Handle = reinterpret_cast<void*>(LoadResource(hmodule, hres));
	Size = static_cast<size_t>(SizeofResource(hmodule, hres));
#else
#error not implemented yet
#endif

	return true;
}

void Resource::close()
{
	Handle = nullptr;
}

const void *Resource::data() const
{
	assert(Handle);
	return LockResource(reinterpret_cast<HGLOBAL>(Handle));
}

size_t Resource::size() const
{
	assert(Handle);
	return Size;
}

Resource::Resource(Resource &&rhs) noexcept : Handle(nullptr)
{
	std::swap(Handle, rhs.Handle);
	std::swap(Size, rhs.Size);
}

Resource &Resource::operator=(Resource &&rhs) noexcept
{
	std::swap(Handle, rhs.Handle);
	std::swap(Size, rhs.Size);
	return *this;
}

}