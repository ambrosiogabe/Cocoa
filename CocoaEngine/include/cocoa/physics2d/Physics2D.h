#ifndef COCOA_ENGINE_PHYSICS_2D_H
#define COCOA_ENGINE_PHYSICS_2D_H
#include "externalLibs.h"
#include "cocoa/core/Core.h"

#include "cocoa/physics2d/PhysicsComponents.h"
#include "cocoa/core/Entity.h"
#include "cocoa/scenes/SceneData.h"

namespace Cocoa
{
	namespace Physics2D
	{
		COCOA void init(const glm::vec2& gravity);
		COCOA void destroy(SceneData& scene);

		COCOA bool pointInBox(const glm::vec2& point, const glm::vec2& halfSize, const glm::vec2& position, float rotationDegrees);

		COCOA void addEntity(Entity entity);
        COCOA void deleteEntity(Entity entity);
		COCOA void update(SceneData& scene, float dt);

        COCOA void applyForceToCenter(const Rigidbody2D& rigidbody, const glm::vec2& force);
        COCOA void applyForce(const Rigidbody2D& rigidbody, const glm::vec2& force, const glm::vec2& point);
        COCOA void applyLinearImpulseToCenter(const Rigidbody2D& rigidbody, const glm::vec2& impulse);
        COCOA void applyLinearImpulse(const Rigidbody2D& rigidbody, const glm::vec2& impulse, const glm::vec2& point);

        // ----------------------------------------------------------------------------
        // Serialization
        // ----------------------------------------------------------------------------
        COCOA void serialize(json& j, Entity entity, const AABB& box);
        COCOA void deserializeAabb(const json& j, Entity entity);
        COCOA void serialize(json& j, Entity entity, const Box2D& box);
        COCOA void deserializeBox2D(const json& j, Entity entity);
        COCOA void serialize(json& j, Entity entity, const Rigidbody2D& rigidbody);
        COCOA void deserializeRigidbody2D(const json& j, Entity entity);
	};
}

#endif
