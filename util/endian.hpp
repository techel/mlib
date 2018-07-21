#pragma once
#include <array>
#include "../platform.hpp"

namespace mlib{namespace util{namespace endian
{

inline uint16_t reverse(uint16_t v)
{
	return (v << 8) | (v >> 8);
}

inline uint32_t reverse(uint32_t v)
{
	return (v << 24) | (v >> 24) | ((v & 0xFF00) << 8) | ((v & 0xFF0000) >> 8);
}

inline uint64_t reverse(uint64_t x)
{
	x = (x & 0x00000000FFFFFFFF) << 32 | (x & 0xFFFFFFFF00000000) >> 32;
	x = (x & 0x0000FFFF0000FFFF) << 16 | (x & 0xFFFF0000FFFF0000) >> 16;
	x = (x & 0x00FF00FF00FF00FF) << 8  | (x & 0xFF00FF00FF00FF00) >> 8;
	return x;
}

template<class T>
T toBigEndian(T v)
{
#ifdef MLIB_BYTEORDER_BE
	return v;
#else
	return reverse(v);
#endif
}

template<class T>
T fromBigEndian(T v)
{
#ifdef MLIB_BYTEORDER_BE
	return v;
#else
	return reverse(v);
#endif
}

template<class T>
T toLittleEndian(T v)
{
#ifdef MLIB_BYTEORDER_LE
	return v;
#else
	return reverse(v);
#endif
}

template<class T>
T fromLittleEndian(T v)
{
#ifdef MLIB_BYTEORDER_LE
	return v;
#else
	return reverse(v);
#endif
}

template<class T>
T toNetwork(T v)
{
	return toBigEndian(v);
}
template<class T>
T fromNetwork(T v)
{
	return fromBigEndian(v);
}

}}}
