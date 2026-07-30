
#pragma once

#include <veil_export.h>

#include <unordered_map>
#include <string_view>

#include "assets.hpp"
#include "logmgr.hpp"

namespace veil {

constexpr const char g_cacheFile[11] = "/cache.bin";
constexpr const char g_cacheDir[8]  = "/.cache";

class Model;
class ModelInstance;

class VEIL_EXPORT TextureStorage {
    public:
        TextureStorage() = default;
        TextureStorage(const TextureStorage&) = delete;
        TextureStorage& operator=(const TextureStorage&) = delete;
        TextureStorage(TextureStorage&&) = delete;
        TextureStorage& operator=(TextureStorage&&) = delete;
        ~TextureStorage(); 

        static TextureStorage& getInstance();

        Texture loadTexture(const std::string& path);

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
    
        const Model& getModel(const std::string& path) const;

        ModelInstance instantiate(const std::string& path) const;
        ModelInstance loadModel(const std::string& path);

        void saveToBIN(const Model& model) const;
        void loadFromBIN(Model& model);

    private:
        std::unordered_map<std::string, Model> m_cache;
}; //class ModelStorage

}; //namespace veil