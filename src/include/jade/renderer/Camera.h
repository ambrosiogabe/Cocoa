#pragma once

#include "jade/components/components.h"

#include <glm/matrix.hpp>

class Camera {
public:
    Camera(glm::vec3& position);

    void AdjustPerspective();
    glm::mat4& GetViewMatrix();
    glm::mat4& GetOrthoView();

    Transform& GetTransform() { return m_Transform; }
    const glm::vec3& CameraForward() const { return m_CameraForward; }
    const glm::vec3& CameraUp() const { return m_CameraUp; }
    const glm::vec3& CameraRight() const { return m_CameraRight; }
    const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
    const glm::mat4& GetOrthoProjection() const { return m_OrthoProjection; }
    const glm::mat4& GetInverseProjection() const { return m_InverseProjection; }
    const glm::mat4& GetOrthoInverseProjection() const { return m_OrthoInverseProjection; }

private:
    void CalculateAspect();

private:
    // Projection/View matrices for ortho and perspective
    glm::mat4 m_ProjectionMatrix, m_ViewMatrix, m_InverseProjection, m_OrthoProjection, m_OrthoView, m_OrthoInverseProjection;

    Transform m_Transform;
    glm::vec3 m_CameraForward, m_CameraUp, m_CameraRight;

    float m_Fov = 45.0f;
    float m_Aspect = 1920.0f / 1080.0f;
};