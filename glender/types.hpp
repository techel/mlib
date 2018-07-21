#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

namespace mlib{namespace glender
{

using scalar = float;

using vec2 = glm::vec<2, scalar>;
using vec3 = glm::vec<3, scalar>;
using vec4 = glm::vec<4, scalar>;

//coordinates ranging from (0,0) to (1,1)
using normvec2 = vec2;
using normvec4 = vec4;

using ivec2 = glm::vec<2, int>;
using uvec2 = glm::vec<2, unsigned int>;
using svec2 = glm::vec<2, size_t>;

using mat2 = glm::mat<2, 2, scalar>;
using mat3 = glm::mat<3, 3, scalar>;
using mat4 = glm::mat<4, 4, scalar>;

}}