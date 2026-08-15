
#pragma once 

#include <veil_export.h>

#include <initializer_list>
#include <glm/gtc/type_ptr.hpp>

#include "assets.hpp"
#include "math.hpp"

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

}; //namespace veil