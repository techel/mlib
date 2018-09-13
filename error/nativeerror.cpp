#include "nativeerror.hpp"

#include <mlib/platform.hpp>

#ifdef YFW_PLATFORM_WIN32
#include <Windows.h>
#endif

#include <string>
#include <cstring>
#include <mlib/unicode/unicodecvt.hpp>

namespace mlib::error
{

//
// error message formatting
//

std::string formatError(ErrorCode c)
{
#ifdef YFW_PLATFORM_WIN32
	wchar_t buf[400];
	
	auto len = FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				   nullptr, c, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), &buf[prefix], 470, nullptr);
				   
	std::wsprintf(&buf[len], " (%u)", c);
				   				   
	return unicode::fromNative(std::wstring(buf, std::wstrlen(buf)));
#else
	return std::strerror(c) + (" (" + std::to_string(c) + ")");
#endif
}

}