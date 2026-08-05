
#include <veil/model.hpp>

namespace veil {

Mesh::Mesh(std::vector<Vertex>&& vertices, std::vector<unsigned int>&& indices, Material& material) {

    m_vertices = std::move(vertices);
    m_indices = std::move(indices);
    m_material = material;

    glCreateVertexArrays(1, &m_vao);
    glCreateBuffers(1, &m_vbo);
    glCreateBuffers(1, &m_ebo);

    glNamedBufferStorage(m_vbo, m_vertices.size()*sizeof(Vertex), &m_vertices[0], 0);
    glVertexArrayVertexBuffer(m_vao, 0, m_vbo, 0, sizeof(Vertex));

    glNamedBufferStorage(m_ebo, m_indices.size()*sizeof(unsigned int), &m_indices[0], 0);
    glVertexArrayElementBuffer(m_vao, m_ebo);

    glEnableVertexArrayAttrib(m_vao, 0);
    glVertexArrayAttribBinding(m_vao, 0, 0);
    glVertexArrayAttribFormat(m_vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));

    glEnableVertexArrayAttrib(m_vao, 1);
    glVertexArrayAttribBinding(m_vao, 1, 0);
    glVertexArrayAttribFormat(m_vao, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
    
    glEnableVertexArrayAttrib(m_vao, 2);
    glVertexArrayAttribBinding(m_vao, 2, 0);
    glVertexArrayAttribFormat(m_vao, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, texuv));
}

Mesh::Mesh(Mesh&& other) noexcept {

    m_vertices = std::move(other.m_vertices);
    m_indices = std::move(other.m_indices);
    m_material = other.m_material;
    m_vao = other.m_vao;
    m_vbo = other.m_vbo;
    m_ebo = other.m_ebo;

    other.m_vao = 0;
    other.m_vbo = 0;
    other.m_ebo = 0;
}

Mesh& Mesh::operator=(Mesh&& other) noexcept {

    if (this != &other) {

        if (m_vao) glDeleteVertexArrays(1, &m_vao);
        if (m_vbo) glDeleteBuffers(1, &m_vbo);
        if (m_ebo) glDeleteBuffers(1, &m_ebo);

        m_vertices = std::move(other.m_vertices);
        m_indices = std::move(other.m_indices);
        m_material = other.m_material;
        m_vao = other.m_vao;
        m_vbo = other.m_vbo;
        m_ebo = other.m_ebo;

        other.m_vao = 0;
        other.m_vbo = 0;
        other.m_ebo = 0;

    }
    return *this;
}

Mesh::~Mesh() {
    
    if (m_vao) glDeleteVertexArrays(1, &m_vao);
    if (m_vbo) glDeleteBuffers(1, &m_vbo);
    if (m_ebo) glDeleteBuffers(1, &m_ebo);
}

void Mesh::render() const {

    if (m_material.diffuse)
        glBindTextureUnit(0, m_material.diffuse->id);
    if (m_material.specular)
        glBindTextureUnit(1, m_material.specular->id);

    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
}


Model::Model(std::string_view path) {

    LogTimer lt(path);

    m_directory = std::filesystem::path(path).parent_path().string();
    std::string cacheFile = m_directory + g_cacheDir + g_cacheFile;

    if (std::filesystem::exists(cacheFile)) 
        ModelStorage::getInstance().loadFromBIN(*this);
    else
        loadModel(path);
}

void Model::render() const {

    for (const auto& mesh : m_meshes) 
        mesh.render();
}

void Model::loadModel(std::string_view path) {

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(
        path.data(), 
        aiProcess_Triangulate | 
        aiProcess_FlipUVs | 
        aiProcess_GenSmoothNormals |
        aiProcess_JoinIdenticalVertices |
        aiProcess_ImproveCacheLocality
    );

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        throw veil::Exception(Log::message(LogType::CRITICAL, "{}", importer.GetErrorString()));
    }

    processNode(scene->mRootNode, scene);

    ModelStorage::getInstance().saveToBIN(*this);
}

void Model::processNode(aiNode* node, const aiScene* scene) {

    for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        m_meshes.emplace_back(processMesh(mesh, scene));
    }
    for (unsigned int i = 0; i < node->mNumChildren; ++i)
        processNode(node->mChildren[i], scene);
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    Material material;

    vertices.reserve(mesh->mNumVertices);
    indices.reserve(mesh->mNumFaces * 3);

    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {

        Vertex vertex;
        Vector3 vector;

        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.position = vector;

        vector.x = mesh->mNormals[i].x;
        vector.y = mesh->mNormals[i].y;
        vector.z = mesh->mNormals[i].z;
        vertex.normal = vector;

        if (mesh->mTextureCoords[0]) {

            Vector2 uv;
            uv.x = mesh->mTextureCoords[0][i].x;
            uv.y = mesh->mTextureCoords[0][i].y;
            vertex.texuv = uv;
        }
        else 
            vertex.texuv = Vector2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }
    for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {

        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; ++j) 
            indices.push_back(face.mIndices[j]);
    }

    if (mesh->mMaterialIndex >= 0) {

        aiMaterial* aiMat = scene->mMaterials[mesh->mMaterialIndex];

        if (aiMat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
            aiString relativePath;
            aiMat->GetTexture(aiTextureType_DIFFUSE, 0, &relativePath);
            material.diffuse = &TextureStorage::getInstance().loadTexture(m_directory + '/' + relativePath.C_Str());
        }
        if (aiMat->GetTextureCount(aiTextureType_SPECULAR) > 0) {
            aiString relativePath;
            aiMat->GetTexture(aiTextureType_SPECULAR, 0, &relativePath);
            material.specular = &TextureStorage::getInstance().loadTexture(m_directory + '/' + relativePath.C_Str());
        }
    }

    return Mesh(std::move(vertices), std::move(indices), material);
}

}; //namespace veil