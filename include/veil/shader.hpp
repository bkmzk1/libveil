
#pragma once 

#include <veil_export.h>

#include <initializer_list>
#include <utility>
#include <type_traits>

#include <glm/gtc/type_ptr.hpp>

#include "assets.hpp"
#include "math.hpp"
#include "log.hpp"

namespace veil {

class VEIL_EXPORT ShaderProgram {
    public:
        ShaderProgram() = default;
        ShaderProgram(std::initializer_list<std::pair<std::string_view, GLenum>> sources);

        ShaderProgram(const ShaderProgram&) = delete;
        ShaderProgram& operator=(const ShaderProgram&) = delete;
        
        ShaderProgram(ShaderProgram&&) noexcept;
        ShaderProgram& operator=(ShaderProgram&&) noexcept;

        ~ShaderProgram();

        inline void useProgram() const { glUseProgram(m_shaderID); }

        void setUniform(int location, float x, float y, float z) const;
        void setUniform(int location, float x, float y) const;
        void setUniform(int location, const glm::mat4& mat) const;
        void setUniform(int location, const Vector3& vec) const;
        void setUniform(int location, const Vector2& vec) const;
        void setUniform(int location, const Matrix4& mat) const;

        inline GLuint getID() const { return m_shaderID; }

    private:
        GLuint m_shaderID = 0;
}; //class ShaderProgram

class UniformBuffer {
    public:
        UniformBuffer() = default;

        template<typename T>
        UniformBuffer(unsigned int location, std::type_identity<T>) : m_size(sizeof(T)) {

            GLuint ubo;
            glCreateBuffers(1, &ubo);
            glNamedBufferStorage(ubo, sizeof(T), nullptr, GL_DYNAMIC_STORAGE_BIT);
            glBindBufferBase(GL_UNIFORM_BUFFER, location, ubo);

            m_ubo = ubo;
            m_location = location;
        }

        UniformBuffer(const UniformBuffer&) = delete;
        UniformBuffer& operator=(const UniformBuffer&) = delete;

        UniformBuffer(UniformBuffer&&) noexcept;
        UniformBuffer& operator=(UniformBuffer&&) noexcept;

        ~UniformBuffer();

        //FIXME: Catch the type error, not the size error 
        template<typename T>
        void setValue(const T& value) const {

            if (m_size != sizeof(value))
                throw veil::Exception(Log::message(LogType::CRITICAL, "Buffer size '{}' does not match value size '{}'", 
                                      m_size, sizeof(value)));

            glNamedBufferSubData(m_ubo, 0, sizeof(value), &value);
        }

        inline GLuint getID() { return m_ubo; }
        inline unsigned int getLocation() { return m_location; }

    private:
        GLuint m_ubo = 0;
        unsigned int m_location = 0;
        const GLint64 m_size = 0;
}; //class UniformBuffer

}; //namespace veil