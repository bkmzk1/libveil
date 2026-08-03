
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
        GLCamera(const Vector3& initPos, const Vector3& up, float aspectRatio, float fovyDeg);
        ~GLCamera() = default;

        void calculateAttitude(double xpos, double ypos);
        void updateView();
        void updateProjection(float fovyDeg, float aspectRatio);
        void resyncMouse();
        void move(const Vector3& factor);

        const Matrix4& getView();
        inline const Matrix4& getProjection() const { return m_projection; }
        inline const Vector3& getPosition() const { return m_position; }
        inline const Vector3& getUp() const { return m_up; }
        inline const Vector3& getFront() const { return m_front; }

    private:
        Matrix4 m_projection{1.0f};
        Matrix4 m_view{1.0f};

        Vector3 m_position{0.0f, 0.0f, 0.0f};
        Vector3 m_up{0.0f, 0.0f, 0.0f};
        Vector3 m_front{0.0f, 0.0f, 0.0f};

        float m_yaw = -90.0f;
        float m_pitch = 0.0f;
        double m_lastx = 0.0f;
        double m_lasty = 0.0f;

        bool m_firstMovement = true;
        bool m_viewDirty = false;

}; //class GLCamera

}; //namespace veil