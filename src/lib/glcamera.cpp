
#include <veil/glcamera.hpp>

namespace veil {

GLCamera::GLCamera(const Vector3& initPos, const Vector3& up, float aspectRatio, float fovyDeg) {

    m_position = initPos;
    m_up = up;

    m_projection.makeProjection(fovyDeg, aspectRatio, 0.1f, 500.0f);
    m_view = Matrix4(1.0f);

    m_lastx = 0.0f;
    m_lasty = 0.0f;

    m_viewDirty = true;
}

const Matrix4& GLCamera::getView() {
    
    if (m_viewDirty) 
        updateView();  
    return m_view;
}

void GLCamera::calculateAttitude(double xpos, double ypos) {

    if (m_firstMovement) {

        m_lastx = xpos;
        m_lasty = ypos;
        m_firstMovement = false;
    }

    double xoff = (xpos - m_lastx) * 0.1f;
    double yoff = (m_lasty - ypos) * 0.1f;

    m_yaw += static_cast<float>(xoff);
    m_pitch += static_cast<float>(yoff);
    m_pitch = std::clamp(m_pitch, -89.0f, 89.0f);

    m_lastx = xpos;
    m_lasty = ypos;

    m_viewDirty = true;
}

void GLCamera::updateView() {

    float x = std::cos(glm::radians(m_yaw)) * std::cos(glm::radians(m_pitch));
    float y = std::sin(glm::radians(m_pitch));
    float z = std::sin(glm::radians(m_yaw)) * std::cos(glm::radians(m_pitch));
    Vector3 forward(x, y, z);

    m_front = forward.normalized();

    m_view.makeView(m_position, m_position + m_front, m_up);

    m_viewDirty = false;
}

void GLCamera::updateProjection(float fovyDeg, float aspectRatio) {

    m_projection.makeProjection(fovyDeg, aspectRatio, 0.1f, 500.0f);
}

void GLCamera::resyncMouse() {
    m_firstMovement = true;
}

void GLCamera::move(const Vector3& factor) {

    m_position += factor;
    m_viewDirty = true;
}

}; //namespace veil