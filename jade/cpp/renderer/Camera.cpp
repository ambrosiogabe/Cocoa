#include "renderer/Camera.h"
#include "core/JWindow.h"

namespace Jade {
    Camera::Camera(glm::vec3& position) {
        this->m_Transform = Transform(position, glm::vec3(1.0f), glm::vec3(0.0f));
        this->m_CameraForward = glm::vec3();
        this->m_CameraUp = glm::vec3();
        this->m_CameraRight = glm::vec3();

        this->m_InverseProjection = glm::mat4();
        this->m_OrthoInverseProjection = glm::mat4();
        this->m_OrthoView = glm::mat4();
        this->m_OrthoInverseView = glm::mat4();

        this->AdjustPerspective();
    }

    void Camera::AdjustPerspective() {
        this->CalculateAspect();

        this->m_ProjectionMatrix = glm::perspective(m_Fov, m_Aspect, 0.5f, 10000.0f);
        this->m_OrthoProjection = glm::ortho(-1920.0f * m_Zoom / 2.0f, 1920.0f * m_Zoom / 2.0f, -1080.0f * m_Zoom / 2.0f, 1080.0f * m_Zoom / 2.0f, 0.5f, 100.0f);

        this->m_InverseProjection = glm::inverse(m_ProjectionMatrix);
        this->m_OrthoInverseProjection = glm::inverse(m_OrthoProjection);
    }

    glm::mat4& Camera::GetViewMatrix() {
        this->m_CameraForward.x = glm::cos(glm::radians(m_Transform.m_EulerRotation.y)) * glm::cos(glm::radians(m_Transform.m_EulerRotation.x));
        this->m_CameraForward.y = glm::sin(glm::radians(m_Transform.m_EulerRotation.x));
        this->m_CameraForward.z = glm::sin(glm::radians(m_Transform.m_EulerRotation.y)) * glm::cos(glm::radians(m_Transform.m_EulerRotation.x));
        this->m_CameraForward = glm::normalize(m_CameraForward);

        this->m_CameraRight = glm::cross(m_CameraUp, glm::vec3(0, 1, 0));
        this->m_CameraUp = glm::cross(m_CameraRight, m_CameraForward);

        glm::vec3 front = glm::vec3(m_Transform.m_Position.x, m_Transform.m_Position.y, m_Transform.m_Position.z) + m_CameraForward;

        m_ViewMatrix = glm::lookAt(m_Transform.m_Position, front, m_CameraUp);
        return m_ViewMatrix;
    }

    glm::mat4& Camera::GetOrthoView() {
        glm::vec3 cameraFront = glm::vec3(0, 0, -1) + glm::vec3(m_Transform.m_Position.x, m_Transform.m_Position.y, 0.0f);
        glm::vec3 cameraUp = glm::vec3(0, 1.0f, 0);
        glm::normalize(cameraUp);

        this->m_OrthoView = glm::lookAt(glm::vec3(m_Transform.m_Position.x, m_Transform.m_Position.y, 20), cameraFront, cameraUp);
        this->m_OrthoInverseView = glm::inverse(m_OrthoView);

        return this->m_OrthoView;
    }

    void Camera::CalculateAspect() {
        // TODO: actually make this calculate window's current aspect
        this->m_Aspect = JWindow::Get()->GetTargetAspectRatio();
    }
}