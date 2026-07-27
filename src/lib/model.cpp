
#include <veil/model.hpp>

namespace veil {

Model::Model(std::string_view path) {

    LogTimer lt(path);

    m_directory = std::filesystem::path(path).parent_path().string();

    std::string cacheFile = m_directory + g_cacheDir + g_cacheFile;

    if (std::filesystem::exists(cacheFile)) 
        ModelStorage::getInstance().loadFromBIN(*this);
    else
        loadModel(path);
}

void Model::render(const Shader& shader) const {

    for (const auto& mesh : m_meshes) 
        mesh.render(shader);
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

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        throw veil::Exception(importer.GetErrorString());

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
            material.diffuse = TextureStorage::getInstance().loadTexture(m_directory + '/' + relativePath.C_Str());
        }
        if (aiMat->GetTextureCount(aiTextureType_SPECULAR) > 0) {
            aiString relativePath;
            aiMat->GetTexture(aiTextureType_SPECULAR, 0, &relativePath);
            material.specular = TextureStorage::getInstance().loadTexture(m_directory + '/' + relativePath.C_Str());
        }
    }

    return Mesh(std::move(vertices), std::move(indices), material);
}

ModelInstance::ModelInstance(const Model& base) : m_base(base) {
    
    m_modelMatrix = Matrix4(1.0f);
}

void ModelInstance::render(const Shader& shader) const {

    m_base.render(shader);
}

void ModelInstance::translate(const Vector3& translationVec) {
    m_modelMatrix.translate(translationVec);
}
void ModelInstance::rotate(float deg, const Vector3& rotateDir) {
    m_modelMatrix.rotate(deg, rotateDir);
}
void ModelInstance::scale(const Vector3& scaleRatio) {
    m_modelMatrix.scale(scaleRatio);
}

InstancedModels::InstancedModels(const Model& base, size_t maxInstances, GLuint modelUniformLocation) : m_base(base) {

    m_maxInstances = maxInstances;

    glCreateBuffers(1, &m_instancesVBO);
    glNamedBufferData(m_instancesVBO, maxInstances * sizeof(Matrix4), nullptr, GL_STATIC_DRAW);

    for (const auto& mesh : base.getMeshesRead()) {

        GLuint vao = mesh.getVAO();

        glVertexArrayVertexBuffer(vao, 1, m_instancesVBO, 0, sizeof(Matrix4));
        glVertexArrayBindingDivisor(vao, 1, 1);

        for (int i = 0; i < 4; ++i) {

            GLuint loc = modelUniformLocation + i;
            glEnableVertexArrayAttrib(vao, loc);
            glVertexArrayAttribBinding(vao, loc, 1);
            glVertexArrayAttribFormat(vao, loc, 4, GL_FLOAT, GL_FALSE, i * sizeof(Vector4));
        }
    }
}

InstancedModels::InstancedModels(InstancedModels&& other) noexcept : m_base(std::move(other.m_base)) {

    m_instancesVBO = other.m_instancesVBO;
    m_maxInstances = other.m_maxInstances;
    m_instancesCount = other.m_instancesCount;

    other.m_instancesVBO = 0;
}

InstancedModels& InstancedModels::operator=(InstancedModels&& other) noexcept {

    if (this != &other) {

        glDeleteBuffers(1, &m_instancesVBO);

        m_instancesVBO = other.m_instancesVBO;
        m_maxInstances = other.m_maxInstances;
        m_instancesCount = other.m_instancesCount;

        other.m_instancesVBO = 0;
    }
    return *this;
}

InstancedModels::~InstancedModels() {

    if (m_instancesVBO)
        glDeleteBuffers(1, &m_instancesVBO);
}

void InstancedModels::setInstances(std::span<const Matrix4> instances) {

    if (instances.size() > m_maxInstances)
        throw veil::Exception(std::format("Too many instances '{}' ", instances.size()));

    m_instancesCount = instances.size();
    glNamedBufferSubData(m_instancesVBO, 0, m_instancesCount * sizeof(Matrix4), &instances[0]);
}

void InstancedModels::render(const Shader& shader) const {

    for (const auto& mesh : m_base.getMeshesRead()) {

        const Material& material = mesh.getMaterial();

        glBindVertexArray(mesh.getVAO());
        glBindTextureUnit(0, material.diffuse.id);
        glBindTextureUnit(1, material.specular.id);

        glDrawElementsInstanced(GL_TRIANGLES, mesh.getIndices().size(), GL_UNSIGNED_INT, nullptr, m_instancesCount);
    }
}

}; //namespace veil