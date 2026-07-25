
#pragma once

#include <veil_export.h>

#include <string>
#include <glad/glad.h>

#include "math.hpp"

namespace veil {

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

    struct BINCacheHeader {

        unsigned int diffLen;
        unsigned int specLen;
        unsigned int vertCount;
        unsigned int indCount;

    }; //struct BINCacheHeader


}; //namespace util

}; //namespace veil