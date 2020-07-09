#pragma once

#include "jade/systems/System.h"
#include "jade/core/Core.h"

#include <vector>
#include <entt.h>
#include <glm/vec2.hpp>

namespace Jade
{
    // ----------------------------------------------------------------------------
    // Collider Components
    // ---------------------------------------------------------------------------- 
    struct Box2D
    {
        glm::vec2 m_Size = glm::vec2();
        glm::vec2 m_HalfSize = glm::vec2();
    };

    struct AABB
    {
        glm::vec2 m_Size = glm::vec2();
        glm::vec2 m_HalfSize = glm::vec2();
        glm::vec2 m_Offset = glm::vec2();
        glm::vec2 m_Center = glm::vec2();
    };
    using BoundingBox = AABB;

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
        entt::entity m_Ignore = entt::null;
    };

    struct Rigidbody2D
    {
        glm::vec2 m_Velocity = glm::vec2();
        float m_Friction = 0.0f;
    };

    class Physics2DSystem : public System
    {
    public:
        Physics2DSystem(const char* name)
            : System(name) { }
         
        //virtual void Render(entt::registry& registry) override;
        //virtual void ImGui(entt::registry& registry) override;
        virtual void Update(entt::registry& registry, float dt) override;

        // ----------------------------------------------------------------------------
        // Box2D Helpers
        // ----------------------------------------------------------------------------
        static Box2D Box2DFrom(glm::vec2 size);
        static Box2D Box2DFrom(glm::vec2 min, glm::vec2 max);
        static glm::vec2 GetMin(const Box2D& box);
        static glm::vec2 GetMax(const Box2D& box);
        static std::array<glm::vec2, 4> GetVertices(const Box2D& box);
        static glm::vec2 GetCenter(const Box2D& box);
        static float GetRotation(const Box2D& box);

        // ----------------------------------------------------------------------------
        // AABB Helpers
        // ----------------------------------------------------------------------------
        static AABB AABBFrom(glm::vec2 min, glm::vec2 max);
        static AABB AABBFrom(glm::vec2 min, glm::vec2 max, glm::vec2 offset);
        static glm::vec2 GetMin(const AABB& box);
        static glm::vec2 GetMax(const AABB& box);
        static glm::vec2 GetCenter(const AABB& box);

        // ----------------------------------------------------------------------------
        // Circle Helpers
        // ----------------------------------------------------------------------------
        static glm::vec2 GetCenter(const Circle& circle);

        // ----------------------------------------------------------------------------
        // Ray2D Helpers
        // ----------------------------------------------------------------------------
        static Ray2D Ray2DFrom(glm::vec2 origin, glm::vec2 direction);
        static Ray2D Ray2DFrom(glm::vec2 origin, glm::vec2 direction, float maxDistance, entt::entity ignore);
    };
}