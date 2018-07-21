#pragma once

#include <string>
#include <utility>
#include <stdexcept>

namespace mlib{namespace net
{

struct UrlMalformed : std::runtime_error
{
	UrlMalformed(const std::string &url) : runtime_error("net.url.malformed (" + url + ")")) {}
};

struct Url
{
	Url(const std::string &url);
	Url() = default;

	std::pair<std::string, std::string> pathFile() const;

	std::string Protocol, Host, Path;

	static Url parse(const std::string &url);
	static Url absolute(Url url, const std::string &other);
};

}}