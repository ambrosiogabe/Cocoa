#pragma once
#include "externalLibs.h"
#include "cocoa/core/Core.h"

#include "cocoa/physics2d/PhysicsComponents.h"
#include "cocoa/core/Entity.h"
#include "cocoa/scenes/SceneData.h"

namespace Cocoa
{
	namespace Physics2D
	{
		COCOA void Init(const glm::vec2& gravity);
		COCOA void Destroy(SceneData& scene);

		COCOA bool PointInBox(const glm::vec2& point, const glm::vec2& halfSize, const glm::vec2& position, float rotationDegrees);

		COCOA void AddEntity(Entity entity);
        COCOA void DeleteEntity(Entity entity);
		COCOA void Update(SceneData& scene, float dt);

        COCOA void ApplyForceToCenter(const Rigidbody2D& rigidbody, const glm::vec2& force);
        COCOA void ApplyForce(const Rigidbody2D& rigidbody, const glm::vec2& force, const glm::vec2& point);
        COCOA void ApplyLinearImpulseToCenter(const Rigidbody2D& rigidbody, const glm::vec2& impulse);
        COCOA void ApplyLinearImpulse(const Rigidbody2D& rigidbody, const glm::vec2& impulse, const glm::vec2& point);

        // ----------------------------------------------------------------------------
        // Serialization
        // ----------------------------------------------------------------------------
        COCOA void Serialize(json& j, Entity entity, const AABB& box);
        COCOA void DeserializeAABB(json& j, Entity entity);
        COCOA void Serialize(json& j, Entity entity, const Box2D& box);
        COCOA void DeserializeBox2D(json& j, Entity entity);
        COCOA void Serialize(json& j, Entity entity, const Rigidbody2D& rigidbody);
        COCOA void DeserializeRigidbody2D(json& j, Entity entity);
	};
}