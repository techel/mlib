#pragma once

#include <exception>
#include <zip.h>

namespace mlib::archive::zip
{

//
// error code wrapper
//

class ErrorCode
{
public:
	ErrorCode();
	ErrorCode(int ziperror);
	~ErrorCode();

	bool none() const noexcept;
	int zip() const noexcept;
	int system() const noexcept;

	bool operator!() const noexcept;
	operator bool() const noexcept;

	const char *what() const;

	ErrorCode(const ErrorCode&) = default;
	ErrorCode &operator=(const ErrorCode&) = default;

	ErrorCode(ErrorCode &&rhs) noexcept;
	ErrorCode &operator=(ErrorCode &&rhs) noexcept;

	operator zip_error_t*() { return &Value; }
	operator const zip_error_t*() const { return &Value; }

private:
	zip_error_t Value;
};

// throws exception corresponding to error code
[[noreturn]] void throwError(const zip_error_t *e);

//
// exception management
//

//when a libzip-function fails and the system error code is set to this value, an exception was thrown which can be retrieved.
static constexpr int ExceptionSystemError = 0xFEAB;

//sets last exception. The stored exception is local to the current thread.
void setLastException(std::exception_ptr e) noexcept;

//retrieves last exception
std::exception_ptr lastException();
bool hasLastException() noexcept;

//sets the last exception to null and throws.
[[noreturn]] void rethrowLastException();

//checks whether the error indicates 'ExceptionSystemError' and calls 'rethrowLastException'
void checkException(const zip_error_t *ze);

}}}