#pragma once

#include <string>
#include <string_view>
#include <utility>
#include <stdexcept>

namespace mlib::net
{

//
// exceptions
//

struct URIMalformed : std::runtime_error
{
    URIMalformed(const std::string &url) : runtime_error("net.uri.malformed (" + url + ")") {}
};

//
// URI
//
// foo://example.com:8042/over/there?name=ferret#nose
// \_ / \______________ / \_________ / \_________ / \__/
// |           |            |            |        |
// scheme     authority       path        query   fragment
// | _____________________ | __
// / \ / \
// urn:example:animal:ferret:nose
//

class URI
{
public:
    URI(std::string uri);
    URI() = default;

    std::string_view string() const { return Content; };
    std::string_view scheme() const { return { Content.data(), SchemeLength }; }
    std::string_view authority() const { return { Content.data() + AuthorityBegin, AuthorityLength }; }
    std::string_view path() const { return { Content.data() + PathBegin, PathLength }; }
    std::string_view query() const { return { Content.data() + QueryBegin, QueryLength }; }
    std::string_view fragment() const { return { Content.data() + FragmentBegin, FragmentLength }; }

    static URI construct(std::string_view scheme, std::string_view auth, std::string_view path, std::string_view query, std::string_view frag);

    enum class ParseMode { Strict, Relaxed }; //strict: requires scheme and path; relaxed: nothing required
    static std::pair<URI, bool> parse(std::string url, ParseMode m = ParseMode::Strict);

    //returns slug of path or entire of path if no '/' present
    std::pair<std::string_view, std::string_view> splitPath() const;

    //returns 'rel' as URI if strictly parsable or replaces slug of this URI with "rel", removes query and fragment and returns new URI
    std::pair<URI, bool> absolute(const std::string &rel) const;

private:
    std::string Content;
    size_t SchemeLength = 0;
    size_t AuthorityBegin = 0, AuthorityLength = 0;
    size_t PathBegin = 0, PathLength = 0;
    size_t QueryBegin = 0, QueryLength = 0;
    size_t FragmentBegin = 0, FragmentLength = 0;
};

}