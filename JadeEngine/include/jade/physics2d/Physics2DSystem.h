#pragma once
#include "externalLibs.h"
#include "jade/core/Core.h"

#include "jade/systems/System.h"
#include "jade/core/Entity.h"

namespace Jade
{
    // ----------------------------------------------------------------------------
    // Collider Components
    // ---------------------------------------------------------------------------- 
    enum class BodyType2D : uint8
    {
        Dynamic = 0,
        Kinematic = 1,
        Static = 2
    };

    struct Box2D
    {
        glm::vec2 m_Size = glm::vec2();
        glm::vec2 m_HalfSize = glm::vec2();
        glm::vec2 m_Offset = glm::vec2();
    };

    struct AABB
    {
        glm::vec2 m_Size = glm::vec2();
        glm::vec2 m_HalfSize = glm::vec2();
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
        Entity m_Ignore = Entity::Null;
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

    class Physics2DSystem : public System
    {
    public:
        Physics2DSystem(const char* name, Scene* scene)
            : System(name, scene) { }
         
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

        // ----------------------------------------------------------------------------
        // Serialization
        // ----------------------------------------------------------------------------
        static void Serialize(json& j, Entity entity, const AABB& box);
        static void DeserializeAABB(json& j, Entity entity);
        static void Serialize(json& j, Entity entity, const Box2D& box);
        static void DeserializeBox2D(json& j, Entity entity);
        static void Serialize(json& j, Entity entity, const Rigidbody2D& rigidbody);
        static void DeserializeRigidbody2D(json& j, Entity entity);
    };
}