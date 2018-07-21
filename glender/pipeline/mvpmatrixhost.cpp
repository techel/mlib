#include "mvpmatrixhost.hpp"


namespace mlib{namespace glender
{

MVPHost::MVPHost() : Model(mat4(1)), View(mat4(1)), Projection(mat4(1)), CompositeVP(mat4(1))
{
    ModelTicket = Model.observe([this](const mat4 &m)
    {
        Composite.value(m * CompositeVP);
    });

    ViewTicket = View.observe([this](const mat4 &v)
    {
        CompositeVP = v * Projection.value();
        Composite.value(Model.value() * CompositeVP);
    });

    ProjectionTicket = Projection.observe([this](const mat4 &p)
    {
        CompositeVP = View.value() * p;
        Composite.value(Model.value() * CompositeVP);
    });
}

}}