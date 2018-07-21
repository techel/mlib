#include "uri.hpp"

#include <regex>

namespace mlib{namespace net
{

Url::Url(const std::string &url)
{
	*this = parse(url);
}

std::pair<std::string, std::string> Url::pathFile() const
{
	const auto idx = Path.find_last_of("/");
	if(idx == Path.npos)
		return { "", Path };
	else
		return { Path.substr(0, idx), Path.substr(idx + 1) };
}

Url Url::parse(const std::string &url)
{
	std::regex reg("([\\w-]+)://([\\w\\.-]+)([\\w%\\?\\.#/-]*)");
	std::smatch m;
	if(!std::regex_match(url, m, reg))
		throw UrlMalformed(url);

	Url urlobj;
	urlobj.Protocol = m[1].str();
	urlobj.Host = m[2].str();
	urlobj.Path = m[3].str().empty() ? "/" : m[3].str();

	return urlobj;
}

Url Url::absolute(Url url, const std::string &other)
{
	try
	{
		return parse(other);
	}
	catch(const UrlMalformed&)
	{
		url.Path = url.pathFile().first + "/" + other;
		return url;
	}
}

}}