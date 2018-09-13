#pragma once

#include <Unknwn.h>
#include <memory>

namespace mlib::windows::com
{

//
// initializes/deinitializes COM for current thread
// see CoInitializeEx
//

enum class Model { Apartment, Multithreaded, Disable_OLE1DDE, SpeedOverMemory };

struct Ticket
{
	Ticket(Model model = Model::Apartment);
	~Ticket();
};

//
// COM object smartpointer
//

template<class ComT>
class com_ptr
{
public:
	com_ptr() noexcept : Impl(nullptr) {}
	explicit com_ptr(ComT *u) noexcept : Impl(u) {}

	ComT *get() { return Impl; }
	const ComT *get() const { return Impl; }

	ComT *operator->() { return Impl; }
	const ComT *operator->() const { return Impl; }

	ComT &operator*() { return *Impl; }
	const ComT &operator*() const { return *Impl; }

	~com_ptr()
	{
		if(Impl)
			Impl->Release();
	}

	com_ptr(const com_ptr &rhs) noexcept
	{
		Impl = rhs.Impl;
		Impl->AddRef();
	}

	com_ptr &operator=(const com_ptr &rhs) noexcept
	{
		Impl = rhs.Impl;
		Impl->AddRef();
		return *this;
	}

	com_ptr(com_ptr &&rhs) noexcept
	{
		Impl = rhs.Impl;
		rhs.Impl = nullptr;
	}

	com_ptr &operator=(com_ptr &&rhs) noexcept
	{
		Impl = rhs.Impl;
		rhs.Impl = nullptr;
		return *this;
	}

private:
	ComT *Impl;
};

//
// CoTaskMemFree smartpointer
//

struct CoTaskMemFreeHandler
{
	void operator()(void *p);
};

template<class T>
using commem_ptr = std::unique_ptr<T, CoTaskMemFreeHandler>;

}