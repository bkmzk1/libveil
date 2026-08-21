
#pragma once

#include <veil_export.h>

#include "assets.hpp"
#include "shader.hpp"
#include "model.hpp"

namespace veil {

enum class DrawableType : uint8_t {

    UNKNOWN,
    MESH_SINGULAR,
    MESH_INSTANCED,
    MODEL_SINGULAR,
    MODEL_INSTANCED
}; //enum class DrawableType

template<typename T> 
struct DrawableTraits {
    static constexpr DrawableType singularType = DrawableType::UNKNOWN;
    static constexpr DrawableType instancedType = DrawableType::UNKNOWN;
};
template<>
struct DrawableTraits<Mesh> {
    static constexpr DrawableType singularType = DrawableType::MESH_SINGULAR;
    static constexpr DrawableType instancedType = DrawableType::MESH_INSTANCED;
};
template<>
struct DrawableTraits<Model> {
    static constexpr DrawableType singularType = DrawableType::MODEL_SINGULAR;
    static constexpr DrawableType instancedType = DrawableType::MODEL_INSTANCED;
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
concept cRenderable = requires(T x, int mode) {
    x.render(mode);
    x.getMeshesRead();
};

template<cRenderable T>
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
        inline DrawableType getType() const override { return DrawableTraits<T>::singularType; }
    
    private:
        const T& m_base;
        Matrix4 m_modelMatrix{1.0f};
}; //class Instance

using ModelInstance = Instance<Model>;
using MeshInstance = Instance<Mesh>;

template<cRenderable T>
class InstanceRendered : public Drawable {
    public:
        InstanceRendered() = delete;
        InstanceRendered(const T& base, size_t maxInstances) : m_base(base) {
            m_maxInstances = maxInstances;

            glCreateBuffers(1, &m_instancesVBO);
            glNamedBufferData(m_instancesVBO, maxInstances * sizeof(Matrix4), nullptr, GL_STATIC_DRAW);
        }

        InstanceRendered(const InstanceRendered&) = delete;
        InstanceRendered& operator=(const InstanceRendered&) = delete;

        InstanceRendered(InstanceRendered&& other) noexcept : m_base(std::move(other.m_base)) {

            m_instancesVBO = other.m_instancesVBO;
            m_maxInstances = other.m_maxInstances;
            m_instancesCount = other.m_instancesCount;

            other.m_instancesVBO = 0;
        }
        InstanceRendered& operator=(InstanceRendered&& other) noexcept  {
            
            if (this != &other) {
                if (m_instancesVBO) 
                    glDeleteBuffers(1, &m_instancesVBO);

                m_instancesVBO = other.m_instancesVBO;
                m_maxInstances = other.m_maxInstances;
                m_instancesCount = other.m_instancesCount;

                other.m_instancesVBO = 0;
            }
            return *this;
        }

        virtual ~InstanceRendered() {
            if (m_instancesVBO)
                glDeleteBuffers(1, &m_instancesVBO);
        }

        void setInstances(std::span<const Matrix4> instances) {
            if (instances.size() > m_maxInstances)
                throw veil::Exception(Log::message(LogType::CRITICAL, "Too many instances '{}' ", instances.size()));

            m_instancesCount = instances.size();
            glNamedBufferSubData(m_instancesVBO, 0, m_instancesCount * sizeof(Matrix4), &instances[0]);
        }
        void setInstanceAttribute(const ShaderProgram& shader, std::string_view attribName) {

            GLint modelAttribLocation = glGetAttribLocation(shader.getID(), attribName.data());

            if (modelAttribLocation < 0)
                throw veil::Exception(Log::message(LogType::CRITICAL, "No uniform attribute found '{}'", attribName));

            for (const auto& mesh : m_base.getMeshesRead()) {

                GLuint vao = mesh.getVAO();

                glVertexArrayVertexBuffer(vao, 1, m_instancesVBO, 0, sizeof(Matrix4));
                glVertexArrayBindingDivisor(vao, 1, 1);

                for (int i = 0; i < 4; ++i) {

                    GLuint loc = modelAttribLocation + i;
                    glEnableVertexArrayAttrib(vao, loc);
                    glVertexArrayAttribBinding(vao, loc, 1);
                    glVertexArrayAttribFormat(vao, loc, 4, GL_FLOAT, GL_FALSE, i * sizeof(Vector4));
                }
            }
        }
        
        void render() const override {

            for (const auto& mesh : m_base.getMeshesRead()) {

                const Material& material = mesh.getMaterial();

                glBindVertexArray(mesh.getVAO());

                if (material.diffuse)
                    glBindTextureUnit(0, material.diffuse->id);
                if (material.specular)
                    glBindTextureUnit(1, material.specular->id);

                glDrawElementsInstanced(m_drawingMode, mesh.getIndices().size(), GL_UNSIGNED_INT, nullptr, m_instancesCount);
            }
        }

        inline const T& getBase() const { return m_base; }
        inline GLuint getInstancesVBO() const { return m_instancesVBO; }
        inline size_t getMaxInstances() const { return m_maxInstances; }
        inline size_t getInstancesCount() const { return m_instancesCount; }
        inline DrawableType getType() const override { return DrawableTraits<T>::instancedType; }

    private:
        const T& m_base; 
        GLuint m_instancesVBO = 0;
        size_t m_maxInstances = 0;
        size_t m_instancesCount = 0;
}; //class InstanceRendered

using InstancedModel = InstanceRendered<Model>;
using InstancedMesh = InstanceRendered<Mesh>;
    
} //namespace veil