
#pragma once

#include <veil_export.h>

#include "assets.hpp"
#include "shader.hpp"
#include "model.hpp"

namespace veil {

enum class DrawableType : uint8_t {

    UNKNOWN,
    MODEL_SINGULAR,
    MODEL_INSTANCED
}; //enum class DrawableType

class VEIL_EXPORT IDrawable {
    public:
        virtual ~IDrawable() = default;

        void setCurrentShader(const Shader* shader) const { m_currentShader = shader; }
        virtual void render() const = 0;

        virtual DrawableType getType() const = 0;
        const Shader* getCurrentShader() const { return m_currentShader; }

    protected:    
        mutable const Shader* m_currentShader = nullptr;
}; //class IDrawable

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
        InstancedModels() = delete;
        InstancedModels(const Model& base, size_t maxInstances);

        InstancedModels(const InstancedModels&) = delete;
        InstancedModels& operator=(const InstancedModels&) = delete;

        InstancedModels(InstancedModels&&) noexcept;
        InstancedModels& operator=(InstancedModels&&) noexcept;

        virtual ~InstancedModels();

        void setInstances(std::span<const Matrix4> instances);
        void setInstanceAttribute(const Shader& shader, std::string_view attribName);
        void render() const override;

        inline const Model& getBase() const { return m_base; }
        inline size_t getMaxInstances() const { return m_maxInstances; }
        inline size_t getInstancesCount() const { return m_instancesCount; }
        inline DrawableType getType() const override { return DrawableType::MODEL_INSTANCED; }

    private:
        const Model& m_base; 

        GLuint m_instancesVBO = 0;
        size_t m_maxInstances = 0;
        size_t m_instancesCount = 0;
}; //class InstancedModels
    
} //namespace veil