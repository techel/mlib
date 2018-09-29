#pragma once

#include <stdexcept>
#include <cstddef>

#include "modinstance.hpp"

namespace mlib::process::module
{

//
// exceptions
//

struct ResourceNotFound : public std::invalid_argument
{
	ResourceNotFound(const std::string &name) : invalid_argument("module.noresource (" +  name +")") {}
};

//
// embedded resource file
//
// A resource if identifie by a path, separated by .
//
//

class Resource
{
public:
	Resource() noexcept;
	explicit Resource(const Instance &inst, const std::string &resname);
	explicit Resource(void *native, const std::string &resname);
	~Resource();

	bool open(const Instance &inst, const std::string &resname);
	bool open(void *native, const std::string &resname);
	void close();

	const void *data() const;

	template<class T>
	const T *data() const { return static_cast<const T*>(data()); }

	size_t size() const;

	Resource(const Resource&) = delete;
	Resource &operator=(const Resource&) = delete;

	Resource(Resource &&rhs) noexcept;
	Resource &operator=(Resource &&rhs) noexcept;

private:
	void *Handle;
	size_t Size;
};

}