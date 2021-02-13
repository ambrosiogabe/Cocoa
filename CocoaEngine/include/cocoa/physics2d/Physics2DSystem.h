#pragma once
#include "externalLibs.h"
#include "cocoa/core/Core.h"

#include "cocoa/physics2d/Physics2DSystem.h"
#include "cocoa/physics2d/PhysicsComponents.h"
#include "cocoa/core/Entity.h"
#include "cocoa/scenes/SceneData.h"

namespace Cocoa
{
	namespace Physics2DSystem
	{
		COCOA void Init(const glm::vec2& gravity);
		COCOA void Destroy(SceneData& scene);

		//COCOA Entity OverlapPoint(Scene* scene, const glm::vec2& point);
		COCOA bool PointInBox(const glm::vec2& point, const glm::vec2& halfSize, const glm::vec2& position, float rotationDegrees);

		COCOA void AddEntity(Entity entity);
		COCOA void Update(SceneData& scene, float dt);

        // ----------------------------------------------------------------------------
        // Box2D Helpers
        // ----------------------------------------------------------------------------
        //COCOA Box2D Box2DFrom(glm::vec2 size);
        //COCOA Box2D Box2DFrom(glm::vec2 min, glm::vec2 max);
        //COCOA glm::vec2 GetMin(const Box2D& box);
        //COCOA glm::vec2 GetMax(const Box2D& box);
        //COCOA std::array<glm::vec2, 4> GetVertices(const Box2D& box);
        //COCOA glm::vec2 GetCenter(const Box2D& box);
        //COCOA float GetRotation(const Box2D& box);

        // ----------------------------------------------------------------------------
        // AABB Helpers
        // ----------------------------------------------------------------------------
        //COCOA AABB AABBFrom(glm::vec2 min, glm::vec2 max);
        //COCOA AABB AABBFrom(glm::vec2 min, glm::vec2 max, glm::vec2 offset);
        //COCOA glm::vec2 GetMin(const AABB& box);
        //COCOA glm::vec2 GetMax(const AABB& box);
        //COCOA glm::vec2 GetCenter(const AABB& box);

        // ----------------------------------------------------------------------------
        // Circle Helpers
        // ----------------------------------------------------------------------------
        //COCOA glm::vec2 GetCenter(const Circle& circle);

        // ----------------------------------------------------------------------------
        // Ray2D Helpers
        // ----------------------------------------------------------------------------
        //COCOA Ray2D Ray2DFrom(glm::vec2 origin, glm::vec2 direction);
        //COCOA Ray2D Ray2DFrom(glm::vec2 origin, glm::vec2 direction, float maxDistance, entt::entity ignore);

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