
#pragma once

#include <veil_export.h>

#include <unordered_map>
#include <filesystem>

#include "assets.hpp"
#include "model.hpp"
#include "drawable.hpp"

namespace veil {

inline const std::filesystem::path g_cacheFile = "cache.bin";
inline const std::filesystem::path g_cacheDir  = ".cache";

struct BINCacheHeader {

    size_t diffLen = 0;
    size_t specLen = 0;
    size_t vertCount = 0;
    size_t indCount = 0;

}; //struct BINCacheHeader

class VEIL_EXPORT TextureStorage : public util::Singleton<TextureStorage> {

    friend class util::Singleton<TextureStorage>;

    public:
        ~TextureStorage(); 
        void shutdown();
        
        const Texture& loadTexture(const std::filesystem::path& path);
    private:
        TextureStorage() = default;
        std::unordered_map<std::filesystem::path, Texture> m_cache;
        static GLuint loadTextureFromFile(const std::filesystem::path& path);
}; //class TextureStorage

class VEIL_EXPORT ModelStorage : public util::Singleton<ModelStorage> {

    friend class util::Singleton<ModelStorage>;

    public:
        ~ModelStorage();
        void shutdown();

        ModelInstance instantiate(const std::filesystem::path& path) const;
        void loadModel(const std::filesystem::path& path);

        void saveToBIN(const Model& model) const;
        void loadFromBIN(Model& model);

        const Model& getModel(const std::filesystem::path& path) const;
    private:
        ModelStorage() = default;
        std::unordered_map<std::filesystem::path, Model> m_cache;
}; //class ModelStorage

class VEIL_EXPORT ShaderStorage : public util::Singleton<ShaderStorage> {

    friend class util::Singleton<ShaderStorage>;

    public:
        ~ShaderStorage();
        void shutdown();

        void loadShader(const std::string& name, std::initializer_list<std::pair<std::string_view, GLenum>> sources);

        const ShaderProgram* getShader(const std::string& name) const;
    private:
        ShaderStorage() = default;
        std::unordered_map<std::string, ShaderProgram> m_cache;
}; //class ShaderStorage

class VEIL_EXPORT UniformBufferStorage : public util::Singleton<UniformBufferStorage> {

    friend class util::Singleton<UniformBufferStorage>;

    public:
        ~UniformBufferStorage();
        void shutdown();

        template<typename T>
        void loadUBO(GLint bindingPoint) {

            auto it = m_cache.find(bindingPoint);
            if (it != m_cache.end())
                return;

            m_cache.try_emplace(bindingPoint, UniformBuffer(bindingPoint, std::type_identity<T>()));
        }

        const UniformBuffer* getUBO(GLint bindingPoint);
    private:
        UniformBufferStorage() = default;
        std::unordered_map<GLint, UniformBuffer> m_cache;
}; //class UniformBufferStorage

template<typename T>
inline auto& Storage() {
    return T::getInstance();
}

}; //namespace veil