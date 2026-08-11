
#pragma once

#include <veil_export.h>

#include <filesystem>
#include <glad/glad.h>

#include "math.hpp"

namespace veil {

struct VEIL_EXPORT Vertex {

    Vector3 position = { 0.0f, 0.0f, 0.0f };
    Vector3 normal = { 0.0f, 0.0f, 0.0f };
    Vector2 texuv = { 0.0f, 0.0f };
}; //struct Vertex

struct VEIL_EXPORT Texture {

    Texture() = default;
    Texture(GLuint texID, const std::filesystem::path& texPath) : id(texID), path(texPath) {}

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    Texture(Texture&& other) {

        id = other.id;
        path = std::move(other.path);
        other.id = 0;
    };
    Texture& operator=(Texture&& other) {

        if (this != &other) {

            if (id) 
                glDeleteTextures(1, &id);

            path = std::move(other.path);
            id = other.id;
            other.id = 0;
        }
        return *this;
    }

    ~Texture() {
        if (id)
            glDeleteTextures(1, &id);
    }

    GLuint id = 0;
    std::filesystem::path path;

}; //struct Texture

struct VEIL_EXPORT Material {
    const Texture* diffuse = nullptr;
    const Texture* specular = nullptr;

}; //struct Material

namespace util {

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