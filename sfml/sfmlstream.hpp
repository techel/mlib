#pragma once

#include <iosfwd>
#include <stdexcept>
#include <SFML/System/InputStream.hpp>

namespace mlib{namespace sfml
{

//
// exceptions
//

struct RetrieveLengthError : public std::runtime_error
{
	RetrieveLengthError() : runtime_error(".sfml.retlength") {}
};

//
// SfmlStreamAdapter
//
// Exposes a std::istream as a sf::InputStream for use with SFML.
//

class SfmlStreamAdapter : public sf::InputStream
{
	std::istream &Stream;
	sf::Int64 Length;

public:
	SfmlStreamAdapter(std::istream &in);
	~SfmlStreamAdapter() = default;

	sf::Int64 read(void *data, sf::Int64 size) override;
	sf::Int64 seek(sf::Int64 pos) override;
	sf::Int64 tell() override;
	sf::Int64 getSize() override;
};

}}

