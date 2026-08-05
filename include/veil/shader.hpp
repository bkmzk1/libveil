
#pragma once 

#include <veil_export.h>

#include <fstream>
#include <initializer_list>
#include <concepts>
#include <glm/gtc/type_ptr.hpp>

#include "assets.hpp"
#include "log.hpp"
#include "math.hpp"

namespace veil {

class VEIL_EXPORT Shader {
    public:
        Shader() = delete;
        Shader(std::initializer_list<std::pair<std::string_view, GLenum>> sources);

        Shader(const Shader&) = delete;
        Shader& operator=(const Shader&) = delete;
        
        Shader(Shader&&) noexcept;
        Shader& operator=(Shader&&) noexcept;

        ~Shader();

        inline void useProgram() const { glUseProgram(m_shaderProgram); }

        void setUniform(int location, float x, float y, float z) const;
        void setUniform(int location, float x, float y) const;
        void setUniform(int location, const glm::mat4& mat) const;
        void setUniform(int location, const Vector3& vec) const;
        void setUniform(int location, const Vector2& vec) const;
        void setUniform(int location, const Matrix4& mat) const;

        inline GLuint getID() const { return m_shaderProgram; }

    private:
        GLuint m_shaderProgram;
}; //class Shader

}; //namespace veil