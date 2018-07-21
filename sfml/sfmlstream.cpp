#include "sfmlstream.hpp"
#include <cassert>
#include <istream>

namespace mlib{namespace sfml
{

SfmlStreamAdapter::SfmlStreamAdapter(std::istream &in) : Stream(in)
{
	auto cur = Stream.tellg();

	Stream.seekg(0, std::ios::end);
	Length = static_cast<sf::Int64>(Stream.tellg() - cur);
	Stream.seekg(cur, std::ios::beg);

	if(!Stream.good())
		throw RetrieveLengthError();

	Stream.clear();
}

sf::Int64 SfmlStreamAdapter::read(void *data, sf::Int64 size)
{
	Stream.read((char*)data, static_cast<std::streamsize>(size));
	return static_cast<sf::Int64>(Stream.gcount());
}
sf::Int64 SfmlStreamAdapter::seek(sf::Int64 pos)
{
    if(Stream.eof())
        Stream.clear();

	Stream.seekg(static_cast<std::streamsize>(pos), std::ios::beg);
	return Stream.good() ? tell() : -1;
}
sf::Int64 SfmlStreamAdapter::tell()
{
	auto pos = Stream.tellg();
	return static_cast<sf::Int64>(Stream.tellg());
}
sf::Int64 SfmlStreamAdapter::getSize()
{
	return Length;
}

}}