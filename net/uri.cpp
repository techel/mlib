#include "uri.hpp"

#include <regex>

#include "urlencode.hpp"

namespace mlib::net
{

URI::URI(std::string uri)
{
	auto [uriobj, valid] = parse(uri);
    if(!valid)
        throw URIMalformed(uri);

    *this = uriobj;
}

URI URI::construct(std::string_view scheme, std::string_view auth, std::string_view path, std::string_view query, std::string_view frag)
{
    URI uriobj;

    std::string uri(scheme);
    uriobj.SchemeLength = scheme.size();

    if(scheme.size() != 0)
        uri += ':';

    if(auth.size() != 0)
    {
        if(scheme.size() != 0)
            uri += "//";

        uriobj.AuthorityBegin = uri.size();


        uri += std::string(auth) + (path.size() > 0 && path[1] != '/' ? "/" : "");
        uriobj.AuthorityLength = auth.size();
    }

    if(path.size() != 0)
    {
        uriobj.PathBegin = uri.size();
        uri += std::string(path);
        uriobj.PathLength = path.size();
    }

    if(query.size() != 0)
    {
        uriobj.QueryBegin = uri.size();
        uri += '?' + std::string(query);
        uriobj.QueryLength = query.size();
    }

    if(frag.size() != 0)
    {
        uriobj.FragmentBegin = uri.size();
        uri += '#' + std::string(frag);
        uriobj.FragmentLength = frag.size();
    }

    uriobj.Content = std::move(uri);
    return uriobj;
}

std::pair<std::string_view, std::string_view> URI::splitPath() const
{
    auto p = path();
	const auto idx = p.find_last_of("/");
	if(idx == p.npos)
        return { "", p };
	else
		return { p.substr(0, idx), p.substr(idx + 1) };
}

std::pair<URI, bool> URI::parse(std::string uri, ParseMode mode)
{
	URI uriobj;

    static const std::regex strict("([^:]+):(?://([^/?#]+))?(/)?([^?#]+)(?:\\?([^#]+))?(?:#(.+))?");
    static const std::regex relaxed("(?:([^:]+):)?(?://([^/?#]+))?(/)?([^?#]*)(?:\\?([^#]+))?(?:#(.+))?");

    const auto &r = mode == ParseMode::Strict ? strict : relaxed;

    std::smatch m;
    if(!std::regex_match(uri, m, r))
        return { URI(), false };

    uriobj.SchemeLength = m[1].length();

    uriobj.AuthorityBegin = m[2].first - uri.begin();
    uriobj.AuthorityLength = m[2].length();

    uriobj.PathBegin = m[4].first - uri.begin();
    uriobj.PathLength = m[4].length();

    if(uriobj.AuthorityLength != 0 && m[3].length() != 0) //treat / after non-empty authority as part of path
    {
        uriobj.PathBegin--;
        uriobj.PathLength++;
    }

    uriobj.QueryBegin = m[5].first - uri.begin();
    uriobj.QueryLength = m[5].length();

    uriobj.FragmentBegin = m[6].first - uri.begin();
    uriobj.FragmentLength = m[6].length();

    uriobj.Content = std::move(uri);
	return std::make_pair(std::move(uriobj), true);
}

std::pair<URI, bool> URI::absolute(const std::string &rel) const
{
    if(auto[uri2, valid] = parse(rel, ParseMode::Strict); valid)
        return { uri2, true };

    auto newuri = construct(scheme(), authority(), std::string(splitPath().first) + "/" + rel, {}, {});
    return std::make_pair(std::move(newuri), true);
}

}