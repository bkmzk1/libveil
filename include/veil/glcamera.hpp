
#pragma once 

#include <veil_export.h>

#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>

#include "window.hpp"
#include "math.hpp"

namespace veil {

class VEIL_EXPORT GLCamera {
    public: 
        GLCamera() = delete;
        GLCamera(const Vector2& size, const Vector3& initPos, const Vector3& up, float aspectRatio, float fovyDeg);
        ~GLCamera() = default;

        Vector3 m_position;
        Vector3 m_up;
        Vector3 m_front;

        void calculateAttitude(double xpos, double ypos);
        void updateView();
        void updateProjection(float fovyDeg, float aspectRatio);

        const Matrix4& getProjection() const { return m_projection; }
        const Matrix4& getView() const { return m_view; }

    private:
        Matrix4 m_projection;
        Matrix4 m_view;

        float m_yaw = -90.0f;
        float m_pitch = 0.0f;
        double m_lastx;
        double m_lasty;

        bool firstMovement = true;

}; //class GLCamera

}; //namespace veil