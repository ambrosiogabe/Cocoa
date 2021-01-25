#include "externalLibs.h"

#include "cocoa/physics2d/Physics2DSystem.h"
#include "cocoa/components/Transform.h"
#include "cocoa/core/Application.h"
#include "cocoa/util/CMath.h"
#include "cocoa/commands/ICommand.h"
#include "cocoa/renderer/DebugDraw.h"

namespace Cocoa
{
	// ----------------------------------------------------------------------------
	// Box2D Helpers
	// ----------------------------------------------------------------------------
	Box2D Physics2DSystem::Box2DFrom(glm::vec2 size)
	{
		Box2D box;
		box.m_Size = size;
		box.m_HalfSize = box.m_Size / 2.0f;
		return box;
	}

	Box2D Physics2DSystem::Box2DFrom(glm::vec2 min, glm::vec2 max)
	{
		Box2D box;
		box.m_Size = max - min;
		box.m_HalfSize = box.m_Size / 2.0f;
		return box;
	}

	glm::vec2 Physics2DSystem::GetMin(const Box2D& box)
	{
		Entity entity = Entity::FromComponent(box);
		Transform& transform = entity.GetComponent<Transform>();

		glm::vec2 boxScale = transform.m_Scale;
		glm::vec2 boxCenter = CMath::Vector2From3(transform.m_Position);// +(box.m_Offset * boxScale);
		glm::vec2 boxHalfSize = box.m_HalfSize * boxScale;

		return boxCenter - boxHalfSize;
	}

	glm::vec2 Physics2DSystem::GetMax(const Box2D& box)
	{
		Entity entity = Entity::FromComponent(box);
		Transform& transform = entity.GetComponent<Transform>();

		glm::vec2 boxScale = transform.m_Scale;
		glm::vec2 boxCenter = CMath::Vector2From3(transform.m_Position);// +(box.m_Offset * boxScale);
		glm::vec2 boxHalfSize = box.m_HalfSize * boxScale;

		return boxCenter + boxHalfSize;
	}

	std::array<glm::vec2, 4> Physics2DSystem::GetVertices(const Box2D& box)
	{
		glm::vec center = GetCenter(box);
		float rotation = GetRotation(box);
		glm::vec2 min = GetMin(box);
		glm::vec2 max = GetMax(box);

		std::array<glm::vec2, 4> vertices = {
			glm::vec2(min.x, min.y), glm::vec2(min.x, max.y),
			glm::vec2(max.x, max.y), glm::vec2(max.x, min.y)
		};

		if (!CMath::Compare(GetRotation(box), 0.0f))
		{
			for (auto& vec : vertices)
			{
				CMath::Rotate(vec, rotation, center);
			}
		}

		return vertices;
	}

	glm::vec2 Physics2DSystem::GetCenter(const Box2D& box)
	{
		Entity entity = Entity::FromComponent(box);
		Transform& transform = entity.GetComponent<Transform>();
		return CMath::Vector2From3(transform.m_Position);
	}

	float Physics2DSystem::GetRotation(const Box2D& box)
	{
		Entity entity = Entity::FromComponent(box);
		Transform& transform = entity.GetComponent<Transform>();
		return transform.m_EulerRotation.z;
	}

	// ----------------------------------------------------------------------------
	// AABB Helpers and BoundingBox Helpers
	// ----------------------------------------------------------------------------
	AABB Physics2DSystem::AABBFrom(glm::vec2 min, glm::vec2 max)
	{
		AABB box;
		box.m_Size = max - min;
		box.m_HalfSize = box.m_Size / 2.0f;
		return box;
	}

	AABB Physics2DSystem::AABBFrom(glm::vec2 min, glm::vec2 max, glm::vec2 offset)
	{
		AABB box;
		box.m_Offset = offset;
		box.m_Size = max - min;
		box.m_HalfSize = box.m_Size / 2.0f;
		return box;
	}

	glm::vec2 Physics2DSystem::GetMax(const AABB& box)
	{
		Entity entity = Entity::FromComponent(box);
		Transform& transform = entity.GetComponent<Transform>();

		glm::vec2 boxScale = transform.m_Scale;
		glm::vec2 boxCenter = CMath::Vector2From3(transform.m_Position) + (box.m_Offset * boxScale);
		glm::vec2 boxHalfSize = box.m_HalfSize * boxScale;

		return boxCenter + boxHalfSize;
	}

	glm::vec2 Physics2DSystem::GetMin(const AABB& box)
	{
		Entity entity = Entity::FromComponent(box);
		Transform& transform = entity.GetComponent<Transform>();

		glm::vec2 boxScale = transform.m_Scale;
		glm::vec2 boxCenter = CMath::Vector2From3(transform.m_Position) + (box.m_Offset * boxScale);
		glm::vec2 boxHalfSize = box.m_HalfSize * boxScale;

		return boxCenter - boxHalfSize;
	}

	glm::vec2 Physics2DSystem::GetCenter(const AABB& box)
	{
		Entity entity = Entity::FromComponent(box);
		Transform& transform = entity.GetComponent<Transform>();
		return CMath::Vector2From3(transform.m_Position);
	}

	// ----------------------------------------------------------------------------
	// Circle Helpers
	// ----------------------------------------------------------------------------
	glm::vec2 Physics2DSystem::GetCenter(const Circle& circle)
	{
		Entity entity = Entity::FromComponent(circle);
		Transform& transform = entity.GetComponent<Transform>();
		return CMath::Vector2From3(transform.m_Position);
	}

	// ----------------------------------------------------------------------------
	// Ray2D Helpers
	// ----------------------------------------------------------------------------
	Ray2D Physics2DSystem::Ray2DFrom(glm::vec2 origin, glm::vec2 direction)
	{
		Ray2D ray;
		ray.m_Origin = origin;
		ray.m_Direction = direction;
		ray.m_MaxDistance = std::numeric_limits<float>::max();
		ray.m_Ignore = Entity::Null;
		return ray;
	}

	Ray2D Physics2DSystem::Ray2DFrom(glm::vec2 origin, glm::vec2 direction, float maxDistance, entt::entity ignore)
	{
		Ray2D ray;
		ray.m_Origin = origin;
		ray.m_Direction = direction;
		ray.m_MaxDistance = maxDistance;
		ray.m_Ignore = ignore;
		return ray;
	}

	void Physics2DSystem::Serialize(json& j, Entity entity, const AABB& box)
	{
		json halfSize = CMath::Serialize("HalfSize", box.m_HalfSize);
		json offset = CMath::Serialize("Offset", box.m_Offset);
		int size = j["Components"].size();
		j["Components"][size] = {
			{"AABB", {
				{"Entity", entity.GetID()},
				halfSize,
				offset
			}}
		};
	}

	void Physics2DSystem::DeserializeAABB(json& j, Entity entity)
	{
		AABB box;
		box.m_HalfSize = CMath::DeserializeVec2(j["AABB"]["HalfSize"]);
		box.m_Size = box.m_HalfSize * 2.0f;
		box.m_Offset = CMath::DeserializeVec2(j["AABB"]["Offset"]);
		entity.AddComponent<AABB>(box);
	}

	void Physics2DSystem::Serialize(json& j, Entity entity, const Box2D& box)
	{
		json halfSize = CMath::Serialize("HalfSize", box.m_HalfSize);
		int size = j["Components"].size();
		j["Components"][size] = {
			{"Box2D", {
				{"Entity", entity.GetID()},
				halfSize,
			}}
		};
	}

	void Physics2DSystem::DeserializeBox2D(json& j, Entity entity)
	{
		Box2D box;
		box.m_HalfSize = CMath::DeserializeVec2(j["Box2D"]["HalfSize"]);
		box.m_Size = box.m_HalfSize * 2.0f;
		entity.AddComponent<Box2D>(box);
	}

	void Physics2DSystem::Serialize(json& j, Entity entity, const Rigidbody2D& rb)
	{
		json angularDamping = { "AngularDamping", rb.m_AngularDamping };
		json linearDamping = { "LinearDamping", rb.m_LinearDamping };
		json mass = { "Mass", rb.m_Mass };
		json velocity = CMath::Serialize("Velocity", rb.m_Velocity);
		json continousCollision = { "ContinousCollision", rb.m_ContinuousCollision };
		json fixedRotation = { "FixedRotation", rb.m_FixedRotation };
		int size = j["Components"].size();
		j["Components"][size] = {
			{"Rigidbody2D", {
				{"Entity", entity.GetID()},
				angularDamping,
				linearDamping,
				mass,
				velocity,
				continousCollision,
				fixedRotation
			}}
		};
	}

	void Physics2DSystem::DeserializeRigidbody2D(json& j, Entity entity)
	{
		Rigidbody2D rb;
		rb.m_AngularDamping = j["Rigidbody2D"]["AngularDamping"];
		rb.m_LinearDamping = j["Rigidbody2D"]["LinearDamping"];
		rb.m_Mass = j["Rigidbody2D"]["Mass"];
		rb.m_Velocity = CMath::DeserializeVec2(j["Rigidbody2D"]["Velocity"]);
		rb.m_ContinuousCollision = j["Rigidbody2D"]["ContinousCollision"];
		rb.m_FixedRotation = j["Rigidbody2D"]["FixedRotation"];
		entity.AddComponent<Rigidbody2D>(rb);
	}
}