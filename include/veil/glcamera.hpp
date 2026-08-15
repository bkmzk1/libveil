
#pragma once 

#include <veil_export.h>

#include <glm/gtc/matrix_transform.hpp>

#include "math.hpp"

namespace veil {

class VEIL_EXPORT GLCamera {

    private:
        struct {
            Matrix4 projection{1.0f};
            Matrix4 view{1.0f};
        } m_attitude;

        Vector3 m_position{0.0f, 0.0f, 0.0f};
        Vector3 m_up{0.0f, 0.0f, 0.0f};
        Vector3 m_front{0.0f, 0.0f, 0.0f};

        float m_yaw = -90.0f;
        float m_pitch = 0.0f;
        double m_lastx = 0.0f;
        double m_lasty = 0.0f;

        bool m_firstMovement = true;
        bool m_viewDirty = false;

    public: 
        GLCamera() = default;
        GLCamera(const Vector3& initPos, const Vector3& up, float aspectRatio, float fovyDeg);

        void calculateAttitude(double xpos, double ypos);
        void updateView();
        void updateProjection(float fovyDeg, float aspectRatio);
        void resyncMouse();
        void move(const Vector3& factor);

        inline const decltype(m_attitude)& getAttitude() {
            
            if (m_viewDirty) {
                m_viewDirty = false;
                updateView();
            }
            return m_attitude;
        }
        inline const Vector3& getPosition() const { return m_position; }
        inline const Vector3& getUp() const { return m_up; }
        inline const Vector3& getFront() const { return m_front; }

}; //class GLCamera

}; //namespace veil