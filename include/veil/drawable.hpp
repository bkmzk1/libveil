
#pragma once

#include <veil_export.h>

#include "assets.hpp"
#include "shader.hpp"
#include "model.hpp"

namespace veil {

enum class DrawableType : uint8_t {

    UNKNOWN,
    MESH_SINGULAR,
    MODEL_SINGULAR,
    MODEL_INSTANCED
}; //enum class DrawableType

template <typename T>
struct DrawableTraits {
    static constexpr DrawableType type = DrawableType::UNKNOWN;
};
template <>
struct DrawableTraits<Model> {
    static constexpr DrawableType type = DrawableType::MODEL_SINGULAR;
};
template <>
struct DrawableTraits<Mesh> {
    static constexpr DrawableType type = DrawableType::MESH_SINGULAR;
};

class VEIL_EXPORT Drawable {
    public:
        virtual ~Drawable() = default;

        void setDrawingMode(int mode) { m_drawingMode = mode; }
        void setCurrentShader(const ShaderProgram* shader) const { m_currentShader = shader; }

        virtual void render() const = 0;

        virtual DrawableType getType() const = 0;
        const ShaderProgram* getCurrentShader() const { return m_currentShader; }

    protected:    
        mutable const ShaderProgram* m_currentShader = nullptr;
        mutable int m_drawingMode = GL_TRIANGLES;
}; //class Drawable

template<typename T>
class Instance : public Drawable {
    public:
        Instance() = delete;
        inline Instance(const T& base) : m_base(base) { }

        Instance(const Instance&) = delete;
        Instance& operator=(const Instance&) = delete;

        virtual ~Instance() = default;

        inline void render() const override { m_base.render(m_drawingMode); }
        inline void translate(const Vector3& translationVec) { m_modelMatrix.translate(translationVec); }
        inline void rotate(float deg, const Vector3& rotateDir) { m_modelMatrix.rotate(deg, rotateDir); }
        inline void scale(const Vector3& scaleRatio) { m_modelMatrix.scale(scaleRatio); }

        inline const T& getBase() const { return m_base; }
        inline const Matrix4& getModelMat() const { return m_modelMatrix; }
        inline DrawableType getType() const override { return DrawableTraits<T>::type; }
    
    private:
        const T& m_base;
        Matrix4 m_modelMatrix{1.0f};
}; //class Instance

using ModelInstance = Instance<Model>;
using MeshInstance = Instance<Mesh>;

class VEIL_EXPORT InstancedModels : public Drawable {
    public:
        InstancedModels() = delete;
        InstancedModels(const Model& base, size_t maxInstances);

        InstancedModels(const InstancedModels&) = delete;
        InstancedModels& operator=(const InstancedModels&) = delete;

        InstancedModels(InstancedModels&&) noexcept;
        InstancedModels& operator=(InstancedModels&&) noexcept;

        virtual ~InstancedModels();

        void setInstances(std::span<const Matrix4> instances);
        void setInstanceAttribute(const ShaderProgram& shader, std::string_view attribName);
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