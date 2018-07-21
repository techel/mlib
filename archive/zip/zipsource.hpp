#pragma once

#include <ios>
#include <memory>
#include <cstdint>

struct zip_source;

namespace mlib::archive::zip
{

//
// source callback
//

struct ISource
{
	virtual ~ISource() = default;

	virtual void zipPrepareWrite() {}
	virtual void zipPrepareRead() {}
	virtual void zipReadingDone() {}
	virtual void zipFinish() {}

	virtual void zipSeekRead(std::streamsize off, std::ios::seekdir dir) = 0;
	virtual void zipSeekWrite(std::streamsize off, std::ios::seekdir dir) = 0;
	virtual std::streamsize zipTellRead() = 0;
	virtual std::streamsize zipTellWrite() = 0;
	virtual std::streamsize zipRead(char *dst, std::streamsize len) = 0;
	virtual void zipWrite(const char *src, std::streamsize len) = 0;

	virtual void zipRollbackWrite() {}
	virtual void zipCommitWrite() {}

	virtual void zipAchiveEmpty() {}
};

//
// zip source
//

class Source
{
public:
	Source() noexcept;
	Source(ISource &s);
	Source(const std::string &path, std::streamsize off = 0, std::streamsize len = 0);
	Source(const char *buffer, size_t len);
	~Source();

	void openStream(ISource &s);
	bool openFile(const std::string &path, std::streamsize off = 0, std::streamsize len = 0);
	void openBuffer(const char *buffer, size_t len);
	void close() noexcept;

	operator zip_source*() { return Internal; }
	operator const zip_source*() const { return Internal; }

	Source(const Source&) = delete;
	Source &operator=(const Source&) = delete;

	Source(Source &&rhs) noexcept;
	Source &operator=(Source &&rhs) noexcept;

private:
	struct ImplCallbackData;
	std::unique_ptr<ImplCallbackData> ImplCallback;

	zip_source *Internal;
};

}}}