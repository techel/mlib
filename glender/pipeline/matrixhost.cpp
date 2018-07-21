#include "matrixhost.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace mlib{namespace glender
{

auto MatrixHost::scale(const vec3 &s) -> ModifyTicket
{
    return modify(glm::scale(value(), s));
}

auto MatrixHost::translate(const vec3 &t) -> ModifyTicket
{
    return modify(glm::translate(value(), t));
}

auto MatrixHost::rotate(scalar rad, const vec3 &axis) -> ModifyTicket
{
    return modify(glm::rotate(value(), rad, axis));
}

}}