
#pragma once

#include <veil_export.h>

#include <string>
#include <glad/glad.h>

#include "math.hpp"

namespace veil {

class Shader;

enum class DrawableType : uint8_t {
    MODEL_SINGULAR,
    MODEL_INSTANCED,
    UNKNOWN
}; //enum class DrawableType

struct VEIL_EXPORT Vertex {
    Vector3 position = { 0.0f, 0.0f, 0.0f };
    Vector3 normal = { 0.0f, 0.0f, 0.0f };
    Vector2 texuv = { 0.0f, 0.0f };

}; //struct Vertex

struct VEIL_EXPORT Texture {
    GLuint id = 0u;
    std::string path = "\0";

}; //struct Texture

struct VEIL_EXPORT Material {
    Texture diffuse;
    Texture specular;

}; //struct Material

class VEIL_EXPORT IDrawable {
    public:
        virtual ~IDrawable() = default;

        void setCurrentShader(const Shader* shader) const { m_currentShader = shader; }

        virtual void render() const = 0;

        virtual DrawableType getType() const = 0;
        const Shader* getCurrentShader() const { return m_currentShader; }
    protected:    
        mutable const Shader* m_currentShader = 0;
}; //class IDrawable

namespace util {

    struct VEIL_EXPORT BINCacheHeader {

        unsigned int diffLen;
        unsigned int specLen;
        unsigned int vertCount;
        unsigned int indCount;

    }; //struct BINCacheHeader

    template<typename T>
    class Singleton {
        public:
            Singleton(const Singleton&) = delete;
            Singleton& operator=(const Singleton&) = delete;
            Singleton(Singleton&&) = delete;
            Singleton& operator=(Singleton&&) = delete;
        
            static T& getInstance() {
                static T instance;
                return instance;
            }
        protected:
            Singleton() = default;
            ~Singleton() = default;

    }; //class Singleton

}; //namespace util

}; //namespace veil