
#pragma once

#include <veil_export.h>

#include <vector>
#include <span>

#include "assets.hpp"
#include "shader.hpp"

namespace veil {

class VEIL_EXPORT Mesh : public IDrawable {
    public:
        Mesh(std::vector<Vertex>&& vertices, std::vector<unsigned int>&& indices, Material& material);
        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;
        Mesh(Mesh&& other) noexcept;
        Mesh& operator=(Mesh&& other) noexcept;
        ~Mesh();
        
        void render() const override;

        inline const std::span<const Vertex> getVertices() const { return m_vertices; }
        inline const std::span<const unsigned int> getIndices() const { return m_indices; }
        inline const Material& getMaterial() const { return m_material; }
        inline const GLuint getVAO() const { return m_vao; }

        inline DrawableType getType() const override { return DrawableType::MESH; }
    private:
        std::vector<Vertex> m_vertices;
        std::vector<unsigned int> m_indices;
        Material m_material;

        GLuint m_vao, m_vbo, m_ebo;
}; //class Mesh

}; //namespace veil