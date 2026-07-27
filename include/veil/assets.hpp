
#pragma once

#include <veil_export.h>

#include <string>
#include <glad/glad.h>

#include "math.hpp"

namespace veil {

class Shader;

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

namespace util {

    struct VEIL_EXPORT BINCacheHeader {

        unsigned int diffLen;
        unsigned int specLen;
        unsigned int vertCount;
        unsigned int indCount;

    }; //struct BINCacheHeader

    enum class DrawableType : uint8_t {
        MODEL_SINGULAR,
        MODEL_INSTANCED
    };

    class VEIL_EXPORT IDrawable {
        public:
            virtual ~IDrawable() = default;
            
            virtual void render(const Shader&) const = 0;
            virtual DrawableType getType() const = 0;
    }; //class IDrawable


}; //namespace util

}; //namespace veil