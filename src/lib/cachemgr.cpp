
#include <veil/cachemgr.hpp>
#include <veil/model.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace veil {

Texture TextureStorage::loadTexture(const std::string& path) {

    if (path.empty() || path == "\0") {

        std::cout << Log::message(LogType::WARNING, "Tried to load a texture with no path") << std::endl;
        return Texture{0, "\0"};
    }

    auto it = m_cache.find(path);
    if (it != m_cache.end())
        return it->second;

    GLuint textureID = loadTextureFromFile(path);
    Texture texture{textureID, path};

    m_cache[path] = texture;

    return texture;
} 

void TextureStorage::shutdown() {

    auto& instance = getInstance();
    for (const auto& [path, texture] : instance.m_cache) {
        if (texture.id) {
            glDeleteTextures(1, &texture.id);
        }
    }
    instance.m_cache.clear();
} 

TextureStorage& TextureStorage::getInstance() {

    static TextureStorage ts;
    return ts;
} 

GLuint TextureStorage::loadTextureFromFile(std::string_view path) const {

    stbi_set_flip_vertically_on_load(false);

    int width, height, channels;
    unsigned char* data = stbi_load(path.data(), &width, &height, &channels, 0);

    if (!data) {

        stbi_image_free(data);
        throw veil::Exception(Log::message(LogType::CRITICAL, "Failed to load texture at {}", path));
        return 0;
    }

    GLenum internalFormat = 0;
    GLenum dataFormat = 0;

    if (channels == 1) {
        internalFormat = GL_R8;
        dataFormat = GL_RED;
    } 
    else if (channels == 3) {
        internalFormat = GL_RGB8;
        dataFormat = GL_RGB;
    } 
    else if (channels == 4) {
        internalFormat = GL_RGBA8;
        dataFormat = GL_RGBA;
    }

    GLuint textureID;
 
    glCreateTextures(GL_TEXTURE_2D, 1, &textureID);
    glTextureStorage2D(textureID, 1, internalFormat, width, height);
    glTextureSubImage2D(textureID, 0, 0, 0, width, height, dataFormat, GL_UNSIGNED_BYTE, data);

    glTextureParameteri(textureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(textureID, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);

    return textureID;
}

ModelStorage& ModelStorage::getInstance() {

    static ModelStorage ms;
    return ms;
} 

ModelInstance ModelStorage::loadModel(const std::string& path) {
    
    auto it = m_cache.find(path);
    if (it != m_cache.end())
        return ModelInstance(this->getExisting(path));
    
    m_cache.try_emplace(path, Model(path));
    return ModelInstance(this->getExisting(path));
} 

ModelInstance ModelStorage::getExisting(const std::string& path) const {

    auto it = m_cache.find(path);

    if (it == m_cache.end())
        throw veil::Exception(Log::message(LogType::CRITICAL, "Failed to load model at {}", path));

    return ModelInstance(it->second);
}

const Model& ModelStorage::getReference(const std::string& path) const {

    auto it = m_cache.find(path);

    if (it == m_cache.end())
        throw veil::Exception(Log::message(LogType::CRITICAL, "No existing model found '{}'", path));

    return it->second;
}

void ModelStorage::shutdown() {

    auto& instance = getInstance();
    instance.m_cache.clear();
}

void ModelStorage::saveToBIN(const Model& model) const {

    const auto& meshes = model.getMeshesRead();

    std::string localCacheDir = std::format("{}{}", model.getDirectory(), g_cacheDir);
    std::filesystem::create_directories(localCacheDir);

    std::string cacheFile = localCacheDir + g_cacheFile;

    std::ofstream out(cacheFile, std::ios::binary);
    if (!out.is_open())
        throw veil::Exception(Log::message(LogType::CRITICAL, "Unable to open {}", cacheFile));

    size_t totalBytes = sizeof(unsigned int);

    for (const auto& mesh : meshes) {

        totalBytes += sizeof(util::BINCacheHeader);
        totalBytes += mesh.getMaterial().diffuse.path.length();
        totalBytes += mesh.getMaterial().specular.path.length();
        totalBytes += mesh.getVertices().size() * sizeof(Vertex);
        totalBytes += mesh.getIndices().size() * sizeof(unsigned int);
    }

    std::vector<char> fileBuffer(totalBytes);
    char* bufferPtr = fileBuffer.data();

    unsigned int meshNum = static_cast<unsigned int>(meshes.size());
    std::memcpy(bufferPtr, &meshNum, sizeof(meshNum));
    bufferPtr += sizeof(meshNum);

    util::BINCacheHeader header;

    for (const auto& mesh : meshes) {

        const auto& diffPath = mesh.getMaterial().diffuse.path;
        const auto& specPath = mesh.getMaterial().specular.path;

        header.diffLen = static_cast<unsigned int>(diffPath.length());
        header.specLen = static_cast<unsigned int>(specPath.length());
        header.vertCount = static_cast<unsigned int>(mesh.getVertices().size());
        header.indCount = static_cast<unsigned int>(mesh.getIndices().size());
        std::memcpy(bufferPtr, &header, sizeof(header));
        bufferPtr += sizeof(header);

        if (header.diffLen > 0) {
            std::memcpy(bufferPtr, diffPath.data(), header.diffLen);
            bufferPtr += header.diffLen;
        }
        if (header.specLen > 0) {
            std::memcpy(bufferPtr, specPath.data(), header.specLen);
            bufferPtr += header.specLen;
        }

        size_t vertBytes = header.vertCount * sizeof(Vertex);
        std::memcpy(bufferPtr, mesh.getVertices().data(), vertBytes);
        bufferPtr += vertBytes;

        size_t indBytes = header.indCount * sizeof(unsigned int);
        std::memcpy(bufferPtr, mesh.getIndices().data(), indBytes);
        bufferPtr += indBytes;
    }

    out.write(fileBuffer.data(), totalBytes);
}

void ModelStorage::loadFromBIN(Model& model) {

    auto& meshes = model.getMeshesWrite();

    std::string cacheFile = std::format("{}{}{}", model.getDirectory(), g_cacheDir, g_cacheFile);

    std::ifstream in(cacheFile, std::ios::binary | std::ios::ate);
    if (!in.is_open()) 
        throw veil::Exception(Log::message(LogType::CRITICAL, "Unable to open {}", cacheFile));

    std::streamsize fileSize = in.tellg();
    in.seekg(0, std::ios::beg);

    std::vector<char> fileBuffer(fileSize);
    in.read(fileBuffer.data(), fileSize);
    
    const char* bufferPtr = fileBuffer.data();

    unsigned int meshNum = 0;
    std::memcpy(&meshNum, bufferPtr, sizeof(meshNum));
    bufferPtr += sizeof(meshNum);

    meshes.reserve(meshNum);

    for (unsigned int i = 0; i < meshNum; ++i) {

        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        util::BINCacheHeader header;

        std::memcpy(&header, bufferPtr, sizeof(header));
        bufferPtr += sizeof(header);

        Texture diffTexture{0, "\0"};
        if (header.diffLen > 0) {

            std::string diffPath(bufferPtr, header.diffLen);
            bufferPtr += header.diffLen;
            diffTexture = TextureStorage::getInstance().loadTexture(diffPath);
        }
        Texture specTexture{0, "\0"};
        if (header.specLen > 0) {

            std::string specPath(bufferPtr, header.specLen);
            bufferPtr += header.specLen;
            specTexture = TextureStorage::getInstance().loadTexture(specPath);
        }
        Material material{diffTexture, specTexture};

        vertices.resize(header.vertCount);
        indices.resize(header.indCount);

        size_t vertBytes = sizeof(Vertex) * header.vertCount;
        size_t indBytes = sizeof(unsigned int) * header.indCount;

        std::memcpy(vertices.data(), bufferPtr, vertBytes);
        bufferPtr += vertBytes;
        std::memcpy(indices.data(), bufferPtr, indBytes);
        bufferPtr += indBytes;

        meshes.emplace_back(std::move(vertices), std::move(indices), material);
    }
}

}; //namespace veil