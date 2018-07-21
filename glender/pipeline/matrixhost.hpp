#pragma once

#include "../types.hpp"
#include "valuehost.hpp"

namespace mlib{namespace glender
{

//
// matrix host
//
// Value host for a 4x4-matrix providing additional operations for transformation.
//

class MatrixHost : public ValueHost<mat4>
{
public:
    using ValueHost<mat4>::ValueHost;

    ModifyTicket scale(const vec3 &s);
    ModifyTicket translate(const vec3 &off);
    ModifyTicket rotate(scalar rad, const vec3 &axis);
};

}}