
#pragma once

#include <veil_export.h>

#include <unordered_map>
#include <string_view>

#include "assets.hpp"
#include "logmgr.hpp"

namespace veil {

constexpr std::string g_cacheFile = "/cache.bin";
constexpr std::string g_cacheDir  = "/.cache";

class Model;
class ModelInstance;

class VEIL_EXPORT TextureStorage {
    public:
        TextureStorage() = default;
        TextureStorage(const TextureStorage&) = delete;
        TextureStorage& operator=(const TextureStorage&) = delete;
        TextureStorage(TextureStorage&&) = delete;
        TextureStorage& operator=(TextureStorage&&) = delete;
        ~TextureStorage() = default; 

        Texture loadTexture(const std::string& path);
        static void shutdown();

        static TextureStorage& getInstance();

    private:
        std::unordered_map<std::string, Texture> m_cache;

        GLuint loadTextureFromFile(std::string_view path) const;
        
}; //class TextureStorage

class VEIL_EXPORT ModelStorage {
    public:
        ModelStorage() = default;
        ModelStorage(const ModelStorage&) = delete;
        ModelStorage& operator=(const ModelStorage&) = delete;
        ModelStorage(ModelStorage&&) = delete;
        ModelStorage& operator=(ModelStorage&&) = delete;
        ~ModelStorage() = default;

        static ModelStorage& getInstance();

        ModelInstance loadModel(const std::string& path);
        ModelInstance getExisting(const std::string& path) const;
        static void shutdown();

        void saveToBIN(const Model& model) const;
        void loadFromBIN(Model& model);

    private:
        std::unordered_map<std::string, Model> m_cache;
}; //class ModelStorage

}; //namespace veil