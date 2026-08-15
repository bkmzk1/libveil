
#include "../../include/veil/shader.hpp"
#include "../../include/veil/log.hpp"

#include <fstream>

namespace veil {

static std::string readFile(const std::string& filename) {

    std::ifstream file(filename, std::ios::in | std::ios::binary);

    if (!file.is_open())
        throw veil::Exception(Log::message(LogType::CRITICAL, "Failed to read {}", filename));
    
    return std::string{
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    };
}

ShaderProgram::ShaderProgram(std::initializer_list<std::pair<std::string_view, GLenum>> sources) {

    if (sources.size() == 0)
        throw veil::Exception(Log::message(LogType::CRITICAL, "Failed to create an empty shader"));

    m_shaderID = glCreateProgram();

    std::vector<GLuint> shaderIDs;
    shaderIDs.reserve(sources.size());

    try {
        std::vector<std::string> sourceFiles;
        sourceFiles.reserve(sources.size());

        std::vector<GLenum> sourceTypes;
        sourceTypes.reserve(sources.size());

        for (const auto& source : sources) {

            sourceFiles.push_back(readFile(static_cast<std::string>(source.first)));
            sourceTypes.push_back(source.second);
        }

        for (size_t i = 0; i < sources.size(); ++i) {

            GLuint shader = glCreateShader(sourceTypes[i]);
            shaderIDs.push_back(shader);

            const char* srcPtr = sourceFiles[i].c_str();

            glShaderSource(shader, 1, &srcPtr, nullptr);
            glCompileShader(shader);

            GLint success;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {

                GLchar infoLog[512];
                glGetShaderInfoLog(shader, 512, nullptr, infoLog);
                throw veil::Exception(Log::message(LogType::CRITICAL, "Failed to compile shader: {}", infoLog));
            }

            glAttachShader(m_shaderID, shader);
        }

        glLinkProgram(m_shaderID);

        GLint success;
        glGetProgramiv(m_shaderID, GL_LINK_STATUS, &success);
        if (!success) {
            
            GLchar infoLog[512];
            glGetProgramInfoLog(m_shaderID, 512, nullptr, infoLog);
            throw veil::Exception(Log::message(LogType::CRITICAL, "Failed to link shader program: {}", infoLog));
        }
    }
    catch(...) {
        for (const auto& shader : shaderIDs)  {

            glDetachShader(m_shaderID, shader);
            glDeleteShader(shader);
        }
        glDeleteProgram(m_shaderID);
        throw;
    }

    for (const auto& shader : shaderIDs) {

        glDetachShader(m_shaderID, shader);
        glDeleteShader(shader);
    }
}
ShaderProgram::ShaderProgram(ShaderProgram&& other) noexcept {

    if (m_shaderID) glDeleteProgram(m_shaderID);

    m_shaderID = other.m_shaderID;

    other.m_shaderID = 0;    
}
ShaderProgram& ShaderProgram::operator=(ShaderProgram&& other) noexcept {

    if (this != &other) {

        if (m_shaderID) glDeleteProgram(m_shaderID);

        m_shaderID = other.m_shaderID;

        other.m_shaderID = 0;
    }
    return *this;
}
ShaderProgram::~ShaderProgram() {

    if (m_shaderID) glDeleteProgram(m_shaderID);
}

void ShaderProgram::setUniform(int location, float x, float y, float z) const {
    glProgramUniform3f(m_shaderID, location, x, y, z);
}
void ShaderProgram::setUniform(int location, float x, float y) const {
    glProgramUniform2f(m_shaderID, location, x, y);
}
void ShaderProgram::setUniform(int location, const glm::mat4& mat) const {
    glProgramUniformMatrix4fv(m_shaderID, location, 1, GL_FALSE, glm::value_ptr(mat));
}
void ShaderProgram::setUniform(int location, const Vector3& vec) const {
    glProgramUniform3f(m_shaderID, location, vec.x, vec.y, vec.z);
}
void ShaderProgram::setUniform(int location, const Vector2& vec) const {
    glProgramUniform2f(m_shaderID, location, vec.x, vec.y);
}
void ShaderProgram::setUniform(int location, const Matrix4& mat) const {
    glProgramUniformMatrix4fv(m_shaderID, location, 1, GL_FALSE, glm::value_ptr(static_cast<Matrix4::Base>(mat)));
}

UniformBuffer::UniformBuffer(UniformBuffer&& other) noexcept {

    if (m_ubo) glDeleteBuffers(1, &m_ubo);

    m_ubo = other.m_ubo;
    other.m_ubo = 0;
}
UniformBuffer& UniformBuffer::operator=(UniformBuffer&& other) noexcept {

    if (this != &other) {

        if (m_ubo) glDeleteBuffers(1, &m_ubo);

        m_ubo = other.m_ubo;
        other.m_ubo = 0;
    }   
    return *this;
}
UniformBuffer::~UniformBuffer() {

    if (m_ubo) glDeleteBuffers(1, &m_ubo);
}

} //namespace veil