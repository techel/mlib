#pragma once

#include <string>
#include <stdexcept>
#include <mlib/error/nativeerror.hpp>

namespace mlib::process::module
{

//
// module instance are instances of process images and dynamic libraries
//

//
// excptions
//

struct Error : public std::runtime_error
{
	Error(const std::string &msg) : runtime_error("process.module" + msg) {}
}

struct GenericError : public Error
{
	error::ErrorCode Code;
	Error(const std::string &e, error::ErrorCode c) : Code(c), Error(e) {}
}

struct InstanceNotFound : public Error
{
	InstanceNotFound(const std::string &name) : Error(".noinstance (" + name + ")") {}
};

//
// generic symbol types
//

using Symbol = void*;
using Procedure = void(*)();

//
// instance
//

class Instance
{
public:
	Instance() noexcept;
	explicit Instance(const std::string &name);
	~Instance();

	void open(const std::string &name);
	void close();

	Symbol symbol(const std::string &name);
	Procedure procedure(const std::string &name);

	void *native() const { return Handle; }

	static Instance current();

	Instance(const Instance&) = delete;
	Instance &operator=(const Instance&) = delete;

	Instance(Instance &&rhs) noexcept;
	Instance &operator=(Instance &&rhs) noexcept;

private:
	bool Dynamic;
	void *Handle;
};

}