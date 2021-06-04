#pragma once
#include "externalLibs.h"
#include "cocoa/core/Core.h"
#include "cocoa/core/Entity.h"

namespace Cocoa
{
    enum class BodyType2D : uint8
    {
        Dynamic = 0,
        Kinematic = 1,
        Static = 2
    };

    struct Box2D
    {
        glm::vec2 m_Size = glm::vec2(1.0f, 1.0f);
        glm::vec2 m_HalfSize = glm::vec2(0.5f, 0.5f);
        glm::vec2 m_Offset = glm::vec2();
    };

    struct AABB
    {
        glm::vec2 m_Size = glm::vec2(1.0f, 1.0f);
        glm::vec2 m_HalfSize = glm::vec2(0.5f, 0.5f);
        glm::vec2 m_Offset = glm::vec2();
    };

    struct Circle
    {
        float m_Radius = 1.0f;
        float m_InertiaTensor = 0.0f;
    };

    struct Ray2D
    {
        glm::vec2 m_Origin = glm::vec2();
        glm::vec2 m_Direction = glm::vec2();
        float m_MaxDistance = 0.0f;
        Entity m_Ignore = NEntity::createNull();
    };

    struct Rigidbody2D
    {
        glm::vec2 m_Velocity = glm::vec2();
        float m_AngularDamping = 0.0f;
        float m_LinearDamping = 0.5f;
        float m_Mass = 0.0f;
        BodyType2D m_BodyType = BodyType2D::Dynamic;

        bool m_FixedRotation = false;
        bool m_ContinuousCollision = false;

        void* m_RawRigidbody = nullptr;
    };
}