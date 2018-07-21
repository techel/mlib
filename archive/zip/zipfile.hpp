#pragma once

#include <ios>

struct zip_file;

namespace mlib::archive::zip
{

class File
{
public:
	File(zip_file *f) noexcept;
	File() noexcept;
	~File();

	size_t read(char *dst, size_t len);

	operator zip_file*() { return Internal; }
	operator const zip_file*() const { return Internal; }

	operator bool() const { return Internal != nullptr; }
	bool operator!() const { return Internal == nullptr; }

	File(const File&) = delete;
	File &operator=(const File&) = delete;

	File(File &&rhs) noexcept;
	File &operator=(File &&rhs) noexcept;

private:
	zip_file *Internal;
};

}}}