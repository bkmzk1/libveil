
#pragma once

#include <veil_export.h>

#include <vector>
#include <fstream>
#include <filesystem>
#include <span>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "assets.hpp"
#include "shader.hpp"
#include "cache.hpp"
#include "log.hpp"
#include "math.hpp"

namespace veil {

class VEIL_EXPORT Mesh {
    public:
        Mesh() = delete;
        Mesh(std::vector<Vertex>&& vertices, std::vector<unsigned int>&& indices, Material& material);

        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;
        
        Mesh(Mesh&& other) noexcept;
        Mesh& operator=(Mesh&& other) noexcept;
        
        ~Mesh();
        
        void render() const;

        inline const std::span<const Vertex> getVertices() const { return m_vertices; }
        inline const std::span<const unsigned int> getIndices() const { return m_indices; }
        inline const Material& getMaterial() const { return m_material; }
        inline const GLuint getVAO() const { return m_vao; }
        
    private:
        std::vector<Vertex> m_vertices;
        std::vector<unsigned int> m_indices;
        Material m_material;

        GLuint m_vao, m_vbo, m_ebo;
}; //class Mesh

class VEIL_EXPORT Model {
    public:
        Model() = delete;
        explicit Model(std::string_view path);

        Model(const Model&) = delete;
        Model& operator=(const Model&) = delete;

        Model(Model&&) noexcept = default;
        Model& operator=(Model&&) noexcept = default;
        
        void render() const; 

        inline std::string_view getDirectory() const { return m_directory; }
        inline std::span<const Mesh> getMeshesRead() const { return m_meshes; }
        inline std::vector<Mesh>& getMeshesWrite() { return m_meshes; }
        
    private:
        std::vector<Mesh> m_meshes;
        std::string m_directory;

        void loadModel(std::string_view path);
        void processNode(aiNode* node, const aiScene* scene);
        Mesh processMesh(aiMesh* mesh, const aiScene* scene);
}; //class Model

}; //namespace veil