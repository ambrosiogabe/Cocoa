#ifndef COCOA_ENGINE_PHYSICS_COMPONENTS_H
#define COCOA_ENGINE_PHYSICS_COMPONENTS_H
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
        glm::vec2 size = glm::vec2(1.0f, 1.0f);
        glm::vec2 halfSize = glm::vec2(0.5f, 0.5f);
        glm::vec2 offset = glm::vec2();
    };

    struct AABB
    {
        glm::vec2 size = glm::vec2(1.0f, 1.0f);
        glm::vec2 halfSize = glm::vec2(0.5f, 0.5f);
        glm::vec2 offset = glm::vec2();
    };

    struct Circle
    {
        float radius = 1.0f;
        float inertiaTensor = 0.0f;
    };

    struct Ray2D
    {
        glm::vec2 origin = glm::vec2();
        glm::vec2 direction = glm::vec2();
        float maxDistance = 0.0f;
        Entity ignore = NEntity::createNull();
    };

    struct Rigidbody2D
    {
        glm::vec2 velocity = glm::vec2();
        float angularDamping = 0.0f;
        float linearDamping = 0.5f;
        float mass = 0.0f;
        BodyType2D bodyType = BodyType2D::Dynamic;

        bool fixedRotation = false;
        bool continuousCollision = false;

        void* rawRigidbody = nullptr;
    };
}

#endif
