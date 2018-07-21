#pragma once

#include <array>
#include "../types.hpp"

namespace mlib{namespace glender{namespace util
{

//
// maps vectors, matrices and primitive types to arrays
//
// For instance, a vec2 is converted to an array of 3 floats.
//

template<class T>
class ArrayMapper;

template<>
struct ArrayMapper<float>
{
	static constexpr size_t Size = 1;
	typedef float UnderlyingType;
	typedef std::array<UnderlyingType, Size> ArrayType;

	static float to(const ArrayType &t) { return t[0]; }
	static ArrayType from(const scalar &t) { return{ t }; }
};

template<>
struct ArrayMapper<vec2>
{
	static constexpr size_t Size = 2;
	typedef float UnderlyingType;
	typedef std::array<UnderlyingType, Size> ArrayType;

	static vec2 to(const ArrayType &t) { return vec2(t[0], t[1]); }
	static ArrayType from(const vec2 &v) { return {v.x, v.y}; }
};

template<>
struct ArrayMapper<vec3>
{
	static constexpr size_t Size = 3;
	typedef float UnderlyingType;
	typedef std::array<UnderlyingType, Size> ArrayType;

	static auto to(const ArrayType &t) { return vec3(t[0], t[1], t[2]); }
	static ArrayType from(const vec3 &v) { return{ v.x, v.y, v.z }; }
};

template<>
struct ArrayMapper<vec4>
{
	static constexpr size_t Size = 4;
	typedef float UnderlyingType;
	typedef std::array<UnderlyingType, Size> ArrayType;

	static auto to(const ArrayType &t) { return vec4(t[0], t[1], t[2], t[3]); }
	static ArrayType from(const vec4 &v) { return{ v.x, v.y, v.z, v.w }; }
};

//
// multipleObjectsToArray
//
// For instance, an array of 4 Vector3f is converted to an array of 12 floats.
//

template<size_t NumElements, class Iter>
auto multipleObjectsToArray(Iter begin)
{
	typedef ArrayMapper<std::decay_t<decltype(*begin)>> Mapper;
	std::array<Mapper::UnderlyingType, NumElements * Mapper::Size> result;

	size_t idx = 0;
	for(size_t i = 0; i < NumElements; ++i)
	{
		auto ar = Mapper::from(*begin++);
		for(size_t j = 0; j < ar.size(); ++j)
			result[idx++] = std::move(ar[j]);
	}
	return result;
}

}}}