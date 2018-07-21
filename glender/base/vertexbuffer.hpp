#pragma once

#include <vector>
#include "../util/rawarray.hpp"

namespace mlib{namespace glender
{

//
// vertex attribute list
//
// Contains a single type of attributes of vertices.
//

template<class TheType>
class VertexAttributes
{
	std::vector<typename util::ArrayMapper<TheType>::ArrayType> Vertices;

public:
	VertexAttributes() = default;

	size_t size() const { return Vertices.size(); }
	void resize(size_t s) { Vertices.resize(s); }

	void set(size_t i, const TheType &t) { Vertices[i] = ArrayMapper<TheType>::from(t); }
	TheType get(size_t i) const { return ArrayMapper<TheType>::to(Vertices[i]); }

	const void *raw() const { return Vertices.data(); }
};

}}