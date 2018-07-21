#include "ziperror.hpp"

#include <utility>

#include "zipexceptions.hpp"

namespace mlib::archive::zip
{

//
// error code wrapper
//

ErrorCode::ErrorCode()
{
	zip_error_init(&Value);
}
ErrorCode::ErrorCode(int ziperror) : ErrorCode()
{
	zip_error_set(&Value, ziperror, 0);
}
ErrorCode::~ErrorCode()
{
	zip_error_fini(&Value);
}

bool ErrorCode::none() const noexcept
{
	return zip() == ZIP_ER_OK;
}

int ErrorCode::zip() const noexcept
{
	return zip_error_code_zip(&Value);
}

int ErrorCode::system() const noexcept
{
	return zip_error_code_system(&Value);
}

bool ErrorCode::operator!() const noexcept
{
	return none();
}

ErrorCode::operator bool() const noexcept
{
	return !none();
}

const char *ErrorCode::what() const
{
	return zip_error_strerror(const_cast<zip_error_t*>(&Value));
}

ErrorCode::ErrorCode(ErrorCode &&rhs) noexcept
{
	std::swap(Value, rhs.Value);
}

ErrorCode &ErrorCode::operator=(ErrorCode &&rhs) noexcept
{
	std::swap(Value, rhs.Value);
	return *this;
}

[[noreturn]] void throwError(const zip_error_t *e)
{
	const auto ze = zip_error_code_zip(e);
	if(ze == ZIP_ER_MULTIDISK)
		throw MultidiskUnsupported();
	else if(ze == ZIP_ER_SEEK)
		throw SeekError();
	else if(ze == ZIP_ER_READ)
		throw ReadError();
	else if(ze == ZIP_ER_WRITE)
		throw WriteError();
	else if(ze == ZIP_ER_CRC)
		throw ChecksumInvalid();
	else if(ze == ZIP_ER_OPEN)
		throw OpenError();
	else if(ze == ZIP_ER_ZLIB)
		throw UnknownError();
	else if(ze == ZIP_ER_MEMORY)
		throw std::bad_alloc();
	else if(ze == ZIP_ER_COMPNOTSUPP)
		throw UnknownCompression();
	else if(ze == ZIP_ER_EOF)
		throw EofError();
	else if(ze == ZIP_ER_NOZIP)
		throw UnknownFormat();
	else if(ze == ZIP_ER_INCONS)
		throw Inconsistent();
	else if(ze == ZIP_ER_EOF)
		throw EofError();
	else if(ze == ZIP_ER_REMOVE)
		throw RemoveError();
	else if(ze == ZIP_ER_NOPASSWD)
		throw PasswordRequired();
	else if(ze == ZIP_ER_WRONGPASSWD)
		throw PasswordWrong();
	else if(ze == ZIP_ER_NOENT)
		throw NotFound();
	else
		throw GeneralError(zip_error_strerror(const_cast<zip_error_t*>(e)));
}

//
// exception management
//

static thread_local std::exception_ptr LastException;

void setLastException(std::exception_ptr e) noexcept
{
	LastException = std::move(e);
}

std::exception_ptr lastException()
{
	return LastException;
}

bool hasLastException() noexcept
{
	return !!LastException;
}

[[noreturn]] void rethrowLastException()
{
	auto e = std::move(LastException);
	LastException = nullptr;
	std::rethrow_exception(e);
}

void checkException(const zip_error_t *ze)
{
	if(zip_error_code_system(ze) == ExceptionSystemError)
		rethrowLastException();
}

}}}