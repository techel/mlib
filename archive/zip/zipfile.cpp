#include "zipfile.hpp"

#include <utility>
#include <cassert>
#include <zip.h>

#include "ziperror.hpp"

namespace mlib::archive::zip
{

File::File(zip_file *f) noexcept : Internal(f)
{
}

File::File() noexcept : Internal(nullptr)
{
}

File::~File()
{
	if(Internal)
		zip_fclose(Internal);
}

size_t File::read(char *dst, size_t len)
{
	assert(Internal);

	const auto ret = zip_fread(Internal, dst, len);

	if(ret < 0)
	{
		auto err = zip_file_get_error(Internal);
		checkException(err);
		throwError(err);
	}

	return static_cast<size_t>(ret);
}

File::File(File &&rhs) noexcept : Internal(nullptr)
{
	std::swap(Internal, rhs.Internal);
}

File &File::operator=(File &&rhs) noexcept
{
	std::swap(Internal, rhs.Internal);
	return *this;
}


}}}