#include "zipsource.hpp"

#include <zip.h>
#include <cstdio>
#include <utility>

#include "ziperror.hpp"

namespace mlib::archive::zip
{

struct Source::ImplCallbackData
{
	ISource *Src;

	static zip_int64_t callback(void *userdata, void *data, zip_uint64_t len, zip_source_cmd_t cmd)
	{
		auto me = reinterpret_cast<ImplCallbackData*>(userdata);
		try
		{
			return me->thiscallback(data, len, cmd);
		}
		catch(...)
		{
			setLastException(std::current_exception());
			return -1;
		}
	}

	zip_int64_t thiscallback(void *data, zip_uint64_t len, zip_source_cmd_t cmd)
	{
		if(cmd == ZIP_SOURCE_BEGIN_WRITE)
		{
			Src->zipPrepareWrite();
		}
		else if(cmd == ZIP_SOURCE_CLOSE)
		{
			Src->zipReadingDone();
		}
		else if(cmd == ZIP_SOURCE_COMMIT_WRITE)
		{
			Src->zipCommitWrite();
		}
		else if(cmd == ZIP_SOURCE_FREE)
		{
			Src->zipFinish();
		}
		else if(cmd == ZIP_SOURCE_OPEN)
		{
			Src->zipPrepareRead();
		}
		else if(cmd == ZIP_SOURCE_READ)
		{
			return static_cast<zip_int64_t>(Src->zipRead(reinterpret_cast<char*>(data), static_cast<std::streamsize>(len)));
		}
		else if(cmd == ZIP_SOURCE_REMOVE)
		{
			Src->zipAchiveEmpty();
		}
		else if(cmd == ZIP_SOURCE_ROLLBACK_WRITE)
		{
			Src->zipRollbackWrite();
		}
		else if(cmd == ZIP_SOURCE_SEEK)
		{
			const auto seek = reinterpret_cast<const zip_source_args_seek*>(data);
			const auto dir = seek->whence == SEEK_CUR ? std::ios::cur : seek->whence == SEEK_END ? std::ios::end : std::ios::beg;
			Src->zipSeekRead(static_cast<std::streamsize>(seek->offset), dir);
		}
		else if(cmd == ZIP_SOURCE_SEEK_WRITE)
		{
			const auto seek = reinterpret_cast<const zip_source_args_seek*>(data);
			const auto dir = seek->whence == SEEK_CUR ? std::ios::cur : seek->whence == SEEK_END ? std::ios::end : std::ios::beg;
			Src->zipSeekWrite(static_cast<std::streamsize>(seek->offset), dir);
		}
		else if(cmd == ZIP_SOURCE_TELL)
		{
			return static_cast<zip_int64_t>(Src->zipTellRead());
		}
		else if(cmd == ZIP_SOURCE_TELL_WRITE)
		{
			return static_cast<zip_int64_t>(Src->zipTellWrite());
		}
		else if(cmd == ZIP_SOURCE_ERROR)
		{
			int *errcodes = reinterpret_cast<int*>(data);
			if(hasLastException())
			{
				errcodes[0] = ZIP_ER_INTERNAL;
				errcodes[1] = ExceptionSystemError;
			}
			else
			{
				errcodes[0] = ZIP_ER_OK;
				errcodes[1] = 0;
			}
			return sizeof(int) * 2;
		}
		else if(cmd == ZIP_SOURCE_SUPPORTS)
		{
			zip_source_make_command_bitmap(ZIP_SOURCE_BEGIN_WRITE, ZIP_SOURCE_CLOSE, ZIP_SOURCE_COMMIT_WRITE, ZIP_SOURCE_ERROR,
				ZIP_SOURCE_FREE, ZIP_SOURCE_OPEN, ZIP_SOURCE_READ, ZIP_SOURCE_REMOVE, ZIP_SOURCE_ROLLBACK_WRITE, ZIP_SOURCE_SEEK,
				ZIP_SOURCE_SEEK_WRITE, ZIP_SOURCE_SUPPORTS, ZIP_SOURCE_TELL, ZIP_SOURCE_TELL_WRITE);
		}

		return 0;
	}
};

Source::Source() noexcept : Internal(nullptr)
{
}
Source::Source(ISource &s)
{
	openStream(s);
}
Source::Source(const std::string &path, std::streamsize off, std::streamsize len)
{
	openFile(path, off, len);
}
Source::Source(const char *buffer, size_t len)
{
	openBuffer(buffer, len);
}
Source::~Source()
{
	close();
}

void Source::close() noexcept
{
	if(Internal)
		zip_source_free(Internal);

	Internal = nullptr;
}

void Source::openStream(ISource &s)
{
	close();

	ImplCallback = std::make_unique<ImplCallbackData>();
	ImplCallback->Src = &s;

	zip_source_function_create(&ImplCallbackData::callback, ImplCallback.get(), nullptr);
	zip_source_keep(Internal);
}
bool Source::openFile(const std::string &path, std::streamsize off, std::streamsize len)
{
	ErrorCode err;
	Internal = zip_source_file_create(path.c_str(), static_cast<zip_uint64_t>(off), static_cast<zip_uint64_t>(len), err);
	if(!Internal)
	{
		if(err.zip() == ZIP_ER_NOENT)
			return false;

		throwError(ErrorCode(err));
	}
	zip_source_keep(Internal);
	return true;
}
void Source::openBuffer(const char *buffer, size_t len)
{
	ErrorCode err;
	Internal = zip_source_buffer_create(buffer, len, 0, err);
	if(!Internal)
		throwError(err);

	zip_source_keep(Internal);
}

Source::Source(Source &&rhs) noexcept : Internal(nullptr)
{
	std::swap(Internal, rhs.Internal);
}

Source &Source::operator=(Source &&rhs) noexcept
{
	std::swap(Internal, rhs.Internal);
	return *this;
}

}}}