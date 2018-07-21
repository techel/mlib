#pragma once

#include "zipfile.hpp"

#include <ios>
#include <string>
#include <cstddef>
#include <cstdint>
#include <utility>

struct zip_source;
struct zip;

namespace mlib::archive::zip
{

//
// archive entry properties
//

using EntryIndex = uint64_t;

struct EntryInfo
{
	std::string Name;
	std::streamsize Size;
	EntryIndex Index;
};

//
// handler for enumerating entries
//

struct IEntryHandler
{
	virtual ~IEntryHandler() = default;
	virtual void onZipEntry(EntryInfo e) = 0;
};

template<class F>
struct EntryHandler : public IEntryHandler
{
	EntryHandler(F f) : Handler(std::move(f)) {}
	F Handler;

	void onZipEntry(EntryInfo e) override
	{
		Handler(std::move(e));
	}
};

template<class F>
auto entryHandler(F func)
{
	return EntryHandler<F>(std::move(func));
}

//
// zip archive
//

class Archive
{
public:
	Archive() noexcept;
	Archive(zip_source *s);
	Archive(const std::string &path, std::ios::openmode mode);
	~Archive();

	void openStream(zip_source *s);
	bool openArchive(const std::string &path, std::ios::openmode mode);
	void close();
	void discard();

	EntryIndex numEntries() const;
	std::pair<EntryIndex, bool> find(const std::string &path) const;

	File open(EntryIndex idx, const std::string &passw = "");
	File open(const std::string &path, const std::string &passw = "");

	EntryInfo stat(EntryIndex idx) const;
	std::pair<EntryInfo, bool> stat(const std::string &path) const;

	void enumerate(IEntryHandler &e) const;
	
	Archive(const Archive&) = delete;
	Archive &operator=(const Archive&) = delete;

	Archive(Archive &&rhs) noexcept;
	Archive &operator=(Archive &&rhs) noexcept;

private:
	zip *Internal;
};

}}}