
#pragma once 

#include <veil_export.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace veil {

template<int N>
struct VEIL_EXPORT Vector : glm::vec<N, float> {

    using Base = glm::vec<N, float>;

    using Base::Base;
    inline Vector() : Base() {}
    inline Vector(const Base& v) : Base(v) {}

    Vector normalized() const { 
        return Vector(glm::normalize(static_cast<Base>(*this))); 
    }
};
using Vector2 = Vector<2>;
using Vector3 = Vector<3>;
using Vector4 = Vector<4>;

template<int C, int R>
struct VEIL_EXPORT Matrix : glm::mat<C, R, float, glm::packed_highp> {

    using Base = glm::mat<C, R, float, glm::packed_highp>;

    using Base::Base;
    inline Matrix() : Base() {}
    inline Matrix(const Base& v) : Base(v) {}

    inline Matrix& operator=(const Base& m) {
        Base::operator=(m);
        return *this;
    }

    void makeProjection(float fovyDeg, float aspectRatio, float zNear, float zFar) {
        *this = glm::perspective(glm::radians(fovyDeg), aspectRatio, zNear, zFar);
    }
    void makeView(const Vector3& camPos, const Vector3& camCenter, const Vector3& camUp) {
        *this = glm::lookAt(camPos, camCenter, camUp);
    }

    void translate(const Vector3& transv) {
        *this = glm::translate(*this, transv);
    }
    void rotate(float angleDeg, const Vector3& rotv) {
        *this = glm::rotate(*this, glm::radians(angleDeg), rotv);
    }
    void scale(const Vector3& scalev) {
        *this = glm::scale(*this, scalev);
    }
};
using Matrix3 = Matrix<3, 3>;
using Matrix4 = Matrix<4, 4>;

}; //namespace veil