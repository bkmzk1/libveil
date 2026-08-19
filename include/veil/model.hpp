
#pragma once

#include <veil_export.h>

#include <vector>
#include <filesystem>
#include <span>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "assets.hpp"

namespace veil {

class VEIL_EXPORT Mesh {
    public:
        Mesh() = delete;
        Mesh(std::vector<Vertex>&& vertices, std::vector<unsigned int>&& indices, const Material& material);

        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;

        Mesh(Mesh&& other) noexcept;
        Mesh& operator=(Mesh&& other) noexcept;
        
        ~Mesh();
        
        void render(int mode) const;

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
        Model() = default;
        explicit Model(const std::filesystem::path& path);

        Model(const Model&) = delete;
        Model& operator=(const Model&) = delete;

        Model(Model&&) noexcept = default;
        Model& operator=(Model&&) noexcept = default;

        inline void setDirectory(const std::filesystem::path& path) { m_directory = path; }
        
        void render(int mode) const; 
        void loadModel(const std::filesystem::path& path);

        inline const std::filesystem::path& getDirectory() const { return m_directory; }
        inline std::span<const Mesh> getMeshesRead() const { return m_meshes; }
        inline std::vector<Mesh>& getMeshesWrite() { return m_meshes; }
        
    private:
        std::vector<Mesh> m_meshes;
        std::filesystem::path m_directory;

        void processNode(aiNode* node, const aiScene* scene);
        Mesh processMesh(aiMesh* mesh, const aiScene* scene);
}; //class Model

}; //namespace veil