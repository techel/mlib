#pragma once

#include <tuple>
#include <utility>
#include "../base/shader.hpp"
#include "../base/renderer.hpp"

namespace mlib{namespace glender
{

//
// concept ShaderUnitMesh
//
// Represents a class whose objects hold vertices of particular attributes.
//
//
// concept ShaderUnit
//
// Represents a class whose objects are responsible for binding the vertex attributes of a corresponding
// ShaderUnitMesh before rendering.
//
// types:
//  using MeshType = <ShaderUnitMesh>
//      Corresponding Mesh type.
//
// methods:
//  constructor(const Shader&, ...)
//      Constructor taking a reference to a shader object and other arbitrary arguments.
//
//  <T> prepareRendering(<ShaderUnitMesh>)
//      Binds vertices of supplied mesh and returns object of type T whose destructor unbinds the vertices.
//


//
// composite mesh
//
// Contains ShaderUnitMeshes of various ShaderUnits.
//

template<class FirstMesh, class... RestMeshes>
class CompositeMesh
{
public:
    CompositeMesh() = default;

    template<size_t Index>
    auto &unit() { return std::get<Index>(Meshes); }

    template<size_t Index>
    const auto &unit() const { return std::get<Index>(Meshes); }

    void primitiveType(Renderer::PrimitiveType t) { PrimType = t; }
    auto primitiveType() const { return PrimType; }

private:
    std::tuple<FirstMesh, RestMeshes...> Meshes;
    Renderer::PrimitiveType PrimType = Renderer::PrimitiveType::Quads;
};

//
// composite shader
//
// Contains various ShaderUnits. Constructor takes one argument per shader unit.
//

template<class FirstShader, class... RestShaders>
class CompositeShader
{
public:
    template<class... Args>
    CompositeShader(Shader &&shader, Args &&...args) : TheShader(std::move(shader)), ShaderUnits(std::forward<Args>(args)...)
    {
    }

    template<size_t Index>
    auto &unit() { return std::get<Index>(ShaderUnits); }

    template<size_t Index>
    const auto &unit() const { return std::get<Index>(ShaderUnits); }

    template<size_t... ShaderIndices, class FirstMesh, class... RestMeshes>
    void render(Renderer &r, const CompositeMesh<FirstMesh, RestMeshes...> &m) const
    {
        r.bindShader(TheShader);
        prepareRendering<0, ShaderIndices...>(r, m, Tag1());
    }

private:
    Shader TheShader;
    std::tuple<FirstShader, RestShaders...> ShaderUnits;

    struct Tag1 {};
    struct Tag2 {};

    template<size_t Index, size_t FirstShaderIndex, size_t... RestShaderIndices, class FirstMesh, class... RestMeshes>
    void prepareRendering(Renderer &r, const CompositeMesh<FirstMesh, RestMeshes...> &m, Tag1) const
    {
        auto ticket = std::get<FirstShaderIndex>(ShaderUnits).prepareRendering(r, m.unit<Index>());
        prepareRendering<Index + 1, RestShaderIndices...>(r, m, std::conditional_t<Index == sizeof...(RestMeshes), Tag2, Tag1>());
    }
    template<size_t Index, size_t... ShaderIndices, class FirstMesh, class... RestMeshes>
    void prepareRendering(Renderer &r, const CompositeMesh<FirstMesh, RestMeshes...> &m, Tag2) const
    {
        r.renderPrimitives(m.primitiveType(), m.unit<0>().size());
    }
};

}}