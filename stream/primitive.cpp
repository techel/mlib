#include "primitive.hpp"
#include <cstring>

namespace mlib::stream
{

#ifndef MLIB_BYTEORDER_LE
	#error endiannes is not implemented yet
#endif

std::ostream &serialize(std::ostream &o, bool b)
{
	uint8_t v = b ? 1 : 0;
	return serialize(o, v);
}
std::istream &deserialize(std::istream &i, bool &b)
{
	b = deserialize<uint8_t>(i) != 0;
	return i;
}

std::ostream &serialize(std::ostream &o, uint8_t b)
{
	return writeBlob(o, &b, sizeof(b));
}
std::istream &deserialize(std::istream &i, uint8_t &b)
{
	return readBlob(i, &b, sizeof(b));
}

std::ostream &serialize(std::ostream &o, uint32_t b)
{
	return writeBlob(o, &b, sizeof(b));
}
std::istream &deserialize(std::istream &i, uint32_t &b)
{
	return readBlob(i, &b, sizeof(b));
}

std::ostream &serialize(std::ostream &o, int32_t b)
{
	return writeBlob(o, &b, sizeof(b));
}
std::istream &deserialize(std::istream &i, int32_t &b)
{
	return readBlob(i, &b, sizeof(b));
}

std::ostream &serialize(std::ostream &o, uint64_t b)
{
    return writeBlob(o, &b, sizeof(b));
}
std::istream &deserialize(std::istream &i, uint64_t &b)
{
    return readBlob(i, &b, sizeof(b));
}

std::ostream &serialize(std::ostream &o, float b)
{
    return writeBlob(o, &b, sizeof(b));
}
std::istream &deserialize(std::istream &i, float &b)
{
    return readBlob(i, &b, sizeof(b));
}

std::ostream &serialize(std::ostream &o, const std::string &str)
{
	serialize(o, static_cast<uint32_t>(str.size()));
	writeBlob(o, str.data(), str.size());
	return o;
}
std::ostream &serialize(std::ostream &o, const char *str)
{
	size_t len = strlen(str);
	serialize(o, static_cast<uint32_t>(len));
	writeBlob(o, str, len);
	return o;
}
std::istream &deserialize(std::istream &i, std::string &s)
{
	size_t len = static_cast<size_t>(deserialize<uint32_t>(i));
	s.resize(len);
	readBlob(i, &s[0], len);
	return i;
}

std::ostream &serialize(std::ostream &o, const std::vector<char> &dat)
{
	serialize(o, static_cast<uint32_t>(dat.size()));
	writeBlob(o, dat.data(), dat.size());
	return o;
}
std::istream &deserialize(std::istream &i, std::vector<char> &v)
{
	size_t len = static_cast<size_t>(deserialize<uint32_t>(i));
	v.resize(len);
	readBlob(i, &v[0], len);
	return i;
}

}}