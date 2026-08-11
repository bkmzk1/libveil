
#include <veil/shader.hpp>
#include <veil/log.hpp>

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

Shader::Shader(std::initializer_list<std::pair<std::string_view, GLenum>> sources) {

    LogTimer shader("shader");

    if (sources.size() == 0)
        throw veil::Exception(Log::message(LogType::CRITICAL, "Failed to create an empty shader"));

    m_shaderProgram = glCreateProgram();

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

            glAttachShader(m_shaderProgram, shader);
        }

        glLinkProgram(m_shaderProgram);

        GLint success;
        glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            
            GLchar infoLog[512];
            glGetProgramInfoLog(m_shaderProgram, 512, nullptr, infoLog);
            throw veil::Exception(Log::message(LogType::CRITICAL, "Failed to link shader program: {}", infoLog));
        }
    }
    catch(...) {
        for (const auto& shader : shaderIDs)  {

            glDetachShader(m_shaderProgram, shader);
            glDeleteShader(shader);
        }
        glDeleteProgram(m_shaderProgram);
        throw;
    }

    for (const auto& shader : shaderIDs) {

        glDetachShader(m_shaderProgram, shader);
        glDeleteShader(shader);
    }
}
Shader::Shader(Shader&& other) noexcept {

    m_shaderProgram = other.m_shaderProgram;
    other.m_shaderProgram = 0;
}
Shader& Shader::operator=(Shader&& other) noexcept {

    if (this != &other) {

        glDeleteProgram(other.m_shaderProgram);

        m_shaderProgram = other.m_shaderProgram;

        other.m_shaderProgram = 0;
    }
    return *this;
}
Shader::~Shader() {

    glDeleteProgram(m_shaderProgram);
}

void Shader::setUniform(int location, float x, float y, float z) const {
    glProgramUniform3f(m_shaderProgram, location, x, y, z);
}
void Shader::setUniform(int location, float x, float y) const {
    glProgramUniform2f(m_shaderProgram, location, x, y);
}
void Shader::setUniform(int location, const glm::mat4& mat) const {
    glProgramUniformMatrix4fv(m_shaderProgram, location, 1, GL_FALSE, glm::value_ptr(mat));
}
void Shader::setUniform(int location, const Vector3& vec) const {
    glProgramUniform3f(m_shaderProgram, location, vec.x, vec.y, vec.z);
}
void Shader::setUniform(int location, const Vector2& vec) const {
    glProgramUniform2f(m_shaderProgram, location, vec.x, vec.y);
}
void Shader::setUniform(int location, const Matrix4& mat) const {
    glProgramUniformMatrix4fv(m_shaderProgram, location, 1, GL_FALSE, glm::value_ptr(static_cast<Matrix4::Base>(mat)));
}

} //namespace veil