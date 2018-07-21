#include "exception.hpp"

#include <regex>

namespace mlib::error
{

Info info(const std::string &what)
{
	static const std::regex reg("(\\S+)\\s*[\\[(]([^\\])]*)[\\])]");

	Info r;

	std::smatch m;
	if(std::regex_match(what, m, reg))
	{
		r.Type = m[1].str();
		for(size_t i = 2; i < m.size(); ++i)
			r.Arguments.push_back(m[i].str());
	}
	else
		r.Type = what;

	return r;
}

ErrorInfo info(const std::exception &e)
{
	return info(e.what());
}

}