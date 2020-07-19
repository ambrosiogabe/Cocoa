#pragma once

#include "externalLibs.h"

namespace Jade
{
    struct Transform
    {
        Transform()
        {
            Init(glm::vec3(0), glm::vec3(1), glm::vec3(0));
        }

        Transform(glm::vec3 position, glm::vec3 scale, glm::vec3 eulerRotation, const char* name = "New GameObject",
            entt::entity parent = entt::null, entt::entity previous = entt::null)
        {
            Init(position, scale, eulerRotation, name, parent, previous);
        }

        void UpdateMatrices()
        {
            m_ModelMatrix = glm::translate(glm::mat4(1.0f), m_Position);
            m_ModelMatrix = m_ModelMatrix * glm::toMat4(m_Orientation);
            m_ModelMatrix = glm::scale(m_ModelMatrix, m_Scale);
        }

        static void Serialize(entt::entity entity, const Transform& transform);
        static void Deserialize(json& j, entt::registry& registry, entt::entity entity);

        entt::entity m_Parent;
        entt::entity m_Previous;
        entt::entity m_Next;
        entt::entity m_First;

        glm::vec3 m_Position;
        glm::vec3 m_Scale;
        glm::vec3 m_EulerRotation;
        glm::quat m_Orientation;

        glm::vec3 m_Forward;
        glm::vec3 m_Up;
        glm::vec3 m_Right;

        glm::mat4 m_ModelMatrix;
        glm::mat4 m_InverseModelMatrix;
        const char* m_Name;

    private:
        void Init(glm::vec3 position, glm::vec3 scale, glm::vec3 eulerRotation, const char* name = "New GameObject",
            entt::entity parent = entt::null, entt::entity previous = entt::null)
        {
            m_Position = position;
            m_Scale = scale;
            m_EulerRotation = eulerRotation;
            m_Orientation = glm::toQuat(glm::orientate3(m_EulerRotation));

            m_Forward = glm::vec3(0, 0, 1) * m_Orientation;
            m_Up = glm::vec3(0, 1, 0) * m_Orientation;
            m_Right = glm::vec3(1, 0, 0) * m_Orientation;
            m_Name = name;
            m_Parent = parent;
            m_Previous = previous;
            m_First = entt::null;
            m_Next = entt::null;

            this->UpdateMatrices();
        }
    };
}