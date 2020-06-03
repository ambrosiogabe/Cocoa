#pragma once

#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>

struct SpriteRenderer {
    glm::vec4 m_Color;
};

class Transform {

public:
    Transform(glm::vec3 position, glm::vec3 scale, glm::vec3 eulerRotation)
        : m_Position(position), m_Scale(scale), m_EulerRotation(eulerRotation) {
            m_Orientation = glm::toQuat(glm::orientate3(m_EulerRotation));

            m_Forward = glm::vec3(0, 0, 1) * m_Orientation;
            m_Up = glm::vec3(0, 1, 0) * m_Orientation;
            m_Right = glm::vec3(1, 0, 0) * m_Orientation;

            this->Update();
        }

    
    void Update() {
        
        m_ModelMatrix = glm::translate(glm::mat4(1.0f), m_Position);
        m_ModelMatrix = m_ModelMatrix * glm::toMat4(m_Orientation);
        m_ModelMatrix = glm::scale(m_ModelMatrix, m_Scale);
    }

public:
    glm::vec3 m_Position;
    glm::vec3 m_Scale;
    glm::vec3 m_EulerRotation;
    glm::quat m_Orientation;

    glm::vec3 m_Forward;
    glm::vec3 m_Up;
    glm::vec3 m_Right;

    glm::mat4 m_ModelMatrix;
    glm::mat4 m_InverseModelMatrix;
};