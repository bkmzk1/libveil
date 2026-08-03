
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
#include "mesh.hpp"
#include "cachemgr.hpp"
#include "logmgr.hpp"
#include "math.hpp"

namespace veil {

class VEIL_EXPORT Model {
    public:
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

class VEIL_EXPORT ModelInstance : public IDrawable {
    public:
        ModelInstance() = delete;
        ModelInstance(const Model& base);

        ModelInstance(const ModelInstance&) = delete;
        ModelInstance& operator=(const ModelInstance&) = delete;
        
        ModelInstance(ModelInstance&&) noexcept = default;
        ModelInstance& operator=(ModelInstance&&) noexcept = default;

        virtual ~ModelInstance() = default;

        void render() const override;

        void translate(const Vector3& translationVec);
        void rotate(float deg, const Vector3& rotateDir);
        void scale(const Vector3& scaleRatio);

        inline const Model& getBase() const { return m_base; }
        inline const Matrix4& getModelMat() const { return m_modelMatrix; }

        inline DrawableType getType() const override { return DrawableType::MODEL_SINGULAR; }

    private:
        const Model& m_base;
        Matrix4 m_modelMatrix;
}; //class ModelInstance

class VEIL_EXPORT InstancedModels : public IDrawable {
    public:
        InstancedModels(const Model& base, size_t maxInstances);

        InstancedModels(const InstancedModels&) = delete;
        InstancedModels& operator=(const InstancedModels&) = delete;

        InstancedModels(InstancedModels&&) noexcept;
        InstancedModels& operator=(InstancedModels&&) noexcept;

        virtual ~InstancedModels();

        void setInstances(std::span<const Matrix4> instances);
        void setInstanceAttribute(const Shader& shader, std::string_view attribName);

        void render() const override;

        inline DrawableType getType() const override { return DrawableType::MODEL_INSTANCED; }

    private:
        const Model& m_base; 

        GLuint m_instancesVBO = 0;
        size_t m_maxInstances = 0;
        size_t m_instancesCount = 0;
}; //class InstancedModels

}; //namespace veil