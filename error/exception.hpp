#pragma once

#include <exception>
#include <string>
#include <vector>

namespace mlib::error
{

//
// error strings
//
// An exception string may be formatted as follows:
// ".namespace1.namespace2.etc (argument1) (argument2)"
//
// where ".namespace1.namespace2.etc" is the error type.
// Brackets can be ( or [, the preceding . may be omitted.
//
// In case an exception string is not formatted like this,
// the Type field is set to that string, the Argument field stays empty.
//

struct Info
{
	std::string Type;
	std::vector<std::string> Arguments;
};

//
// functions for extracting error information
//

Info info(const std::exception &e);
Info info(const std::string &what);

}