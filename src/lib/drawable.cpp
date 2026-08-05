
#include <veil/drawable.hpp>

namespace veil {

ModelInstance::ModelInstance(const Model& base) : m_base(base) {
    
    m_modelMatrix = Matrix4(1.0f);
}

void ModelInstance::render() const {

    m_base.render();
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

InstancedModels::InstancedModels(const Model& base, size_t maxInstances) : m_base(base) {

    m_maxInstances = maxInstances;

    glCreateBuffers(1, &m_instancesVBO);
    glNamedBufferData(m_instancesVBO, maxInstances * sizeof(Matrix4), nullptr, GL_STATIC_DRAW);
}

InstancedModels::InstancedModels(InstancedModels&& other) noexcept : m_base(std::move(other.m_base)) {

    m_instancesVBO = other.m_instancesVBO;
    m_maxInstances = other.m_maxInstances;
    m_instancesCount = other.m_instancesCount;

    other.m_instancesVBO = 0;
}

InstancedModels& InstancedModels::operator=(InstancedModels&& other) noexcept {

    if (this != &other) {

        if (m_instancesVBO) 
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
        throw veil::Exception(Log::message(LogType::CRITICAL, "Too many instances '{}' ", instances.size()));

    m_instancesCount = instances.size();
    glNamedBufferSubData(m_instancesVBO, 0, m_instancesCount * sizeof(Matrix4), &instances[0]);
}

void InstancedModels::setInstanceAttribute(const Shader& shader, std::string_view name) {

    GLint modelAttribLocation = glGetAttribLocation(shader.getID(), name.data());

    if (modelAttribLocation < 0)
        throw veil::Exception(Log::message(LogType::CRITICAL, "No uniform attribute found '{}'", name));

    for (const auto& mesh : m_base.getMeshesRead()) {

        GLuint vao = mesh.getVAO();

        glVertexArrayVertexBuffer(vao, 1, m_instancesVBO, 0, sizeof(Matrix4));
        glVertexArrayBindingDivisor(vao, 1, 1);

        for (int i = 0; i < 4; ++i) {

            GLuint loc = modelAttribLocation + i;
            glEnableVertexArrayAttrib(vao, loc);
            glVertexArrayAttribBinding(vao, loc, 1);
            glVertexArrayAttribFormat(vao, loc, 4, GL_FLOAT, GL_FALSE, i * sizeof(Vector4));
        }
    }
}

void InstancedModels::render() const {

    for (const auto& mesh : m_base.getMeshesRead()) {

        const Material& material = mesh.getMaterial();

        glBindVertexArray(mesh.getVAO());

        if (material.diffuse)
            glBindTextureUnit(0, material.diffuse->id);
        if (material.specular)
            glBindTextureUnit(1, material.specular->id);

        glDrawElementsInstanced(GL_TRIANGLES, mesh.getIndices().size(), GL_UNSIGNED_INT, nullptr, m_instancesCount);
    }
}

}; //namespace veil