
#pragma once

#include <veil_export.h>

#include <unordered_map>
#include <filesystem>

#include "assets.hpp"

namespace veil {

inline const std::filesystem::path g_cacheFile = "cache.bin";
inline const std::filesystem::path g_cacheDir  = ".cache";

class Model;

template<typename T>
class Instance;

using ModelInstance = Instance<Model>;

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
        GLuint loadTextureFromFile(const std::filesystem::path& path) const;
}; //class TextureStorage

class VEIL_EXPORT ModelStorage : public util::Singleton<ModelStorage> {

    friend class util::Singleton<ModelStorage>;

    public:
        ~ModelStorage();
        void shutdown();

        ModelInstance instantiate(const std::filesystem::path& path) const;
        ModelInstance loadModel(const std::filesystem::path& path);

        void saveToBIN(const Model& model) const;
        void loadFromBIN(Model& model);

        const Model& getModel(const std::filesystem::path& path) const;

    private:
        ModelStorage() = default;
        std::unordered_map<std::filesystem::path, Model> m_cache;
}; //class ModelStorage

}; //namespace veil