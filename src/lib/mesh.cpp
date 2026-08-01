
#include <veil/mesh.hpp>

namespace veil {

Mesh::Mesh(std::vector<Vertex>&& vertices, std::vector<unsigned int>&& indices, Material& material) {

    m_vertices = std::move(vertices);
    m_indices = std::move(indices);
    m_material = material;

    glCreateVertexArrays(1, &m_vao);
    glCreateBuffers(1, &m_vbo);
    glCreateBuffers(1, &m_ebo);

    glNamedBufferStorage(m_vbo, m_vertices.size()*sizeof(Vertex), &m_vertices[0], 0);
    glVertexArrayVertexBuffer(m_vao, 0, m_vbo, 0, sizeof(Vertex));

    glNamedBufferStorage(m_ebo, m_indices.size()*sizeof(unsigned int), &m_indices[0], 0);
    glVertexArrayElementBuffer(m_vao, m_ebo);

    glEnableVertexArrayAttrib(m_vao, 0);
    glVertexArrayAttribBinding(m_vao, 0, 0);
    glVertexArrayAttribFormat(m_vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));

    glEnableVertexArrayAttrib(m_vao, 1);
    glVertexArrayAttribBinding(m_vao, 1, 0);
    glVertexArrayAttribFormat(m_vao, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
    
    glEnableVertexArrayAttrib(m_vao, 2);
    glVertexArrayAttribBinding(m_vao, 2, 0);
    glVertexArrayAttribFormat(m_vao, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, texuv));
}

Mesh::Mesh(Mesh&& other) noexcept {

    m_vertices = std::move(other.m_vertices);
    m_indices = std::move(other.m_indices);
    m_material = other.m_material;
    m_vao = other.m_vao;
    m_vbo = other.m_vbo;
    m_ebo = other.m_ebo;

    other.m_vao = 0;
    other.m_vbo = 0;
    other.m_ebo = 0;
}

Mesh& Mesh::operator=(Mesh&& other) noexcept {

    if (this != &other) {

        glDeleteVertexArrays(1, &m_vao);
        glDeleteBuffers(1, &m_vbo);
        glDeleteBuffers(1, &m_ebo);

        m_vertices = std::move(other.m_vertices);
        m_indices = std::move(other.m_indices);
        m_material = other.m_material;
        m_vao = other.m_vao;
        m_vbo = other.m_vbo;
        m_ebo = other.m_ebo;

        other.m_vao = 0;
        other.m_vbo = 0;
        other.m_ebo = 0;

    }
    return *this;
}

Mesh::~Mesh() {
    
    if (m_vao) glDeleteVertexArrays(1, &m_vao);
    if (m_vbo) glDeleteBuffers(1, &m_vbo);
    if (m_ebo) glDeleteBuffers(1, &m_ebo);
}

void Mesh::render() const {

    glBindTextureUnit(0, m_material.diffuse.id);
    glBindTextureUnit(1, m_material.specular.id);

    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
}

}; //namespace veil