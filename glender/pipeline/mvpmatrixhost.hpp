#pragma once

#include "matrixhost.hpp"

namespace mlib{namespace glender
{

//
// model-view-projection matrix host
//

class MVPHost
{
public:
    MVPHost();

    MatrixHost &model() { return Model; }
    const MatrixHost &model() const { return Model; }

    MatrixHost &view() { return View; }
    const MatrixHost &view() const { return View; }

    MatrixHost &projection() { return Projection; }
    const MatrixHost &projection() const { return Projection; }

    MatrixHost &composite() { return Composite; }
    const MatrixHost &composite() const { return Composite; }

    MVPHost(MVPHost&&) = delete;
    MVPHost &operator=(MVPHost&&) = delete;

private:
    MatrixHost Model, View, Projection, Composite;
    mat4 CompositeVP;
    MatrixHost::ListenerTicket ModelTicket, ViewTicket, ProjectionTicket;
};

}}