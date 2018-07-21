#include "ziparchive.hpp"

#include <zip.h>
#include <cassert>
#include <utility>

#include "ziperror.hpp"
#include "zipexceptions.hpp"

namespace mlib::archive::zip
{

Archive::Archive() noexcept : Internal(nullptr)
{
}

Archive::Archive(zip_source *s)
{
	openStream(s);
}

Archive::Archive(const std::string &path, std::ios::openmode mode)
{
	if(!openArchive(path, mode))
		throw NotFound();
}

Archive::~Archive()
{
	close();
}

void Archive::close()
{
	if(Internal)
		zip_close(Internal);

	Internal = nullptr;
}

void Archive::discard()
{
	assert(Internal);

	zip_discard(Internal);
	Internal = nullptr;
}

void Archive::openStream(zip_source *s)
{
	assert(s);

	close();

	ErrorCode err;
	Internal = zip_open_from_source(s, 0, err);
	if(!Internal)
	{
		checkException(err);
		throwError(err);
	}
}

bool Archive::openArchive(const std::string &path, std::ios::openmode mode)
{
	close();

	int flags = 0;
	if(!(mode & std::ios::out))
		flags |= ZIP_RDONLY;
	if(mode & std::ios::trunc)
		flags |= ZIP_TRUNCATE;

	int err;
	Internal = zip_open(path.c_str(), flags, &err);
	if(!Internal)
	{
		if(err == ZIP_ER_NOENT)
			return false;

		throwError(ErrorCode(err));
	}

	return true;
}

EntryIndex Archive::numEntries() const
{
	assert(Internal);

	const auto n = zip_get_num_entries(Internal, 0);
	if(n < 0)
		throwError(zip_get_error(Internal));

	return static_cast<EntryIndex>(n);
}

std::pair<EntryIndex, bool> Archive::find(const std::string &path) const
{
	assert(Internal);
	const auto idx = zip_name_locate(Internal, path.c_str(), 0);
	if(idx < 0)
		return { 0, false };
	else
		return {static_cast<EntryIndex>(idx), true};
}

auto Archive::open(EntryIndex idx, const std::string &passw) -> File
{
	assert(Internal);

	auto file = zip_fopen_index_encrypted(Internal, idx, 0, passw.c_str());
	if(!file)
	{
		const auto err = zip_get_error(Internal);
		checkException(err);
		throwError(err);
	}

	return File(file);
}

File Archive::open(const std::string &path, const std::string &passw)
{
	const auto idx = find(path);
	if(!idx.second)
		return File(nullptr);

	return open(idx.first, passw);
}

EntryInfo Archive::stat(EntryIndex idx) const
{
	assert(Internal);

	zip_stat_t stat;
	const auto ret = zip_stat_index(Internal, idx, 0, &stat);
	if(ret)
		throwError(zip_get_error(Internal));

	if((stat.valid & ZIP_STAT_NAME) == 0 || (stat.valid & ZIP_STAT_SIZE) == 0)
		throw InvalidStat();

	EntryInfo e;
	e.Name = stat.name;
	e.Size = static_cast<std::streamsize>(stat.size);
	e.Index = idx;
	return e;
}
std::pair<EntryInfo, bool> Archive::stat(const std::string &path) const
{
	const auto idx = find(path);
	if(!idx.second)
		return { {}, false };

	return { stat(idx.first), true };
}
void Archive::enumerate(IEntryHandler &e) const
{
	const auto n = numEntries();
	for(EntryIndex i = 0; i < n; ++i)
		e.onZipEntry(stat(i));
}
Archive::Archive(Archive &&rhs) noexcept
{
	std::swap(Internal, rhs.Internal);
}

Archive &Archive::operator=(Archive &&rhs) noexcept
{
	std::swap(Internal, rhs.Internal);
	return *this;
}

}}}