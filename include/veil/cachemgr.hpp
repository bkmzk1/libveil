
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

class VEIL_EXPORT TextureStorage : public util::Singleton<TextureStorage> {

    friend class util::Singleton<TextureStorage>;

    public:
        ~TextureStorage(); 
        const Texture& loadTexture(const std::string& path);
        
    private:
        TextureStorage() = default;
        std::unordered_map<std::string, Texture> m_cache;
        GLuint loadTextureFromFile(std::string_view path) const;
}; //class TextureStorage

class VEIL_EXPORT ModelStorage : public util::Singleton<ModelStorage> {

    friend class util::Singleton<ModelStorage>;

    public:
        ~ModelStorage() = default;

        ModelInstance instantiate(const std::string& path) const;
        ModelInstance loadModel(const std::string& path);

        void saveToBIN(const Model& model) const;
        void loadFromBIN(Model& model);

        const Model& getModel(const std::string& path) const;

    private:
        ModelStorage() = default;
        std::unordered_map<std::string, Model> m_cache;
}; //class ModelStorage

}; //namespace veil