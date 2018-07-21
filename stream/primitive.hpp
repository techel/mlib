#pragma once

#include <ostream>
#include <istream>
#include <stdexcept>
#include <string>
#include <vector>
#include <cstdint>

namespace mlib::stream
{

//
// exceptions
//

struct ReadBlobError : public std::runtime_error
{
	ReadBlobError(std::streamsize expected, std::streamsize got) :
		runtime_error("stream.readblob (" + std::to_string(expected) + ") (" + std::to_string(got)) {}
};

struct WriteBlobError : public std::runtime_error
{
	WriteBlobError() : runtime_error("stream.writeblob") {}
}

//
// functions for reading binary data
//
	
template<class T>
std::istream &readBlob(std::istream &i, T *dest, size_t len)
{
	i.read(reinterpret_cast<char*>(&dest), len);
	if(i.gcount() != len)
		throw ReadBlobError(len, i.gcount());
	return i;
}

template<class T>
std::ostream &writeBlob(std::ostream &o, const T *src, size_t len)
{
	o.write(reinterpret_cast<const char*>(src), len);
	if(!o)
		throw WriteBlobError();
	return o;
}

//
// functions for reading primitive objects
//

//
// boolean
// Represented as uint8_t, either 0 or any other value.
//

std::ostream &serialize(std::ostream &o, bool b);
std::istream &deserialize(std::istream &i, bool &b);

//
// integers
// Represented in little endian.
//

std::ostream &serialize(std::ostream &o, uint8_t b);
std::istream &deserialize(std::istream &i, uint8_t &b);

std::ostream &serialize(std::ostream &o, uint32_t b);
std::istream &deserialize(std::istream &i, uint32_t &b);

std::ostream &serialize(std::ostream &o, int32_t b);
std::istream &deserialize(std::istream &i, int32_t &b);

std::ostream &serialize(std::ostream &o, uint64_t b);
std::istream &deserialize(std::istream &i, uint64_t &b);

//
// floating point
// Represented in IEEE 754.
//

std::ostream &serialize(std::ostream &o, float b);
std::istream &deserialize(std::istream &i, float &b);

//
// char-string
// uint32_t
// followed by blob of chars
//

std::ostream &serialize(std::ostream &o, const std::string &str);
std::ostream &serialize(std::ostream &o, const char *str);
std::istream &deserialize(std::istream &i, std::string &s);

//
// char-string
// uint32_t
// followed by blob of chars
//

std::ostream &serialize(std::ostream &o, const std::vector<char> &dat);
std::istream &deserialize(std::istream &i, std::vector<char> &v);

//
// generic deserialization
// Makes use of ADL.
//

template<class T>
T deserialize(std::istream &i)
{
	T t;
	deserialize(i, t);
	return t;
}

}}