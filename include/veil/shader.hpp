
#pragma once 

#include <veil_export.h>

#include <initializer_list>
#include <utility>
#include <type_traits>
#include <typeindex>

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

        void setUniform(int location, const Vector3& vec) const;
        void setUniform(int location, const Vector2& vec) const;
        void setUniform(int location, const Matrix4& mat) const;

        inline GLuint getID() const { return m_shaderID; }

    private:
        GLuint m_shaderID = 0;
}; //class ShaderProgram

template<typename T>
concept cUniform = std::is_convertible_v<std::decay_t<T>, Vector3> ||
                   std::is_convertible_v<std::decay_t<T>, Vector2> ||
                   std::is_convertible_v<std::decay_t<T>, Matrix4>;

class UniformBuffer {
    public:
        UniformBuffer() = delete;

        template<typename T>
        UniformBuffer(unsigned int location, std::type_identity<T>) : m_type(typeid(T)) {

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

        template<typename T>
        void setValue(const T& value) const {

            std::type_index typeIDT = typeid(T);

            if (m_type != typeIDT)
            throw veil::Exception(Log::message(LogType::CRITICAL, "Buffer type '{}' does not match value type '{}'", 
                                      m_type.name(), typeIDT.name()));

            glNamedBufferSubData(m_ubo, 0, sizeof(value), &value);
        }

        inline GLuint getID() { return m_ubo; }
        inline unsigned int getLocation() { return m_location; }

    private:
        GLuint m_ubo = 0;
        unsigned int m_location = 0;
        std::type_index m_type;
}; //class UniformBuffer

}; //namespace veil