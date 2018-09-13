#pragma once

namespace mlib::error
{

//
// error message formatting
//

#ifdef YFW_PLATFORM_WIN32
using ErrorCode = unsigned int;
#else
using ErrorCode = int;
#endif

std::string formatError(ErrorCode c);

}