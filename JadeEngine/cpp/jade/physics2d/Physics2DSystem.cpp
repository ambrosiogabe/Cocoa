#include "jade/physics2d/Physics2DSystem.h"
#include "jade/components/Transform.h"

namespace Jade
{
	void Physics2DSystem::Update(entt::registry& registry, float dt)
	{
		registry.group<BoundingBox>(entt::get<Transform>).each([](auto entity, BoundingBox& aabb, Transform& transform)
		{
				//aabb.SyncTransform(transform);
		});
	}


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
		// TODO: IMPLEMENT ME
		return glm::vec2();
		//return rigidbody->m_Position - m_HalfSize;
	}

	glm::vec2 Physics2DSystem::GetMax(const Box2D& box)
	{
		// TODO: IMPLEMENT ME
		return glm::vec2();
		//return rigidbody->m_Position + m_HalfSize;
	}

	std::array<glm::vec2, 4> Physics2DSystem::GetVertices(const Box2D& box)
	{
		glm::vec2 min = GetMin(box);
		glm::vec2 max = GetMax(box);

		std::array<glm::vec2, 4> vertices = {
			glm::vec2(min.x, min.y), glm::vec2(min.x, max.y),
			glm::vec2(max.x, max.y), glm::vec2(max.x, min.y)
		};

		//if (!JMath::Compare(rigidbody->m_Rotation, 0.0f))
		//{
		//	for (auto& vec : vertices)
		//	{
		//		JMath::Rotate(vec, rigidbody->m_Rotation, rigidbody->m_Position);
		//	}
		//}

		return vertices;
	}

	glm::vec2 Physics2DSystem::GetCenter(const Box2D& box)
	{
		// TODO: IMPLEMENT ME
		return glm::vec2();
	}

	float Physics2DSystem::GetRotation(const Box2D& box)
	{
		// TODO: IMPLEMENT ME
		return 0.0f;
	}

	// ----------------------------------------------------------------------------
	// AABB Helpers
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
		// TODO: IMPLEMENT ME
		return glm::vec2();
		//return m_Center - m_HalfSize;
	}

	glm::vec2 Physics2DSystem::GetMin(const AABB& box)
	{
		// TODO: IMPLEMENT ME
		return glm::vec2();
		//return m_Center + m_HalfSize;	
	}

	glm::vec2 Physics2DSystem::GetCenter(const AABB& box)
	{
		// TODO: IMPLEMENT ME
		return glm::vec2();
	}

	// TODO: COME UP WITH BETTER SOLUTION
	//void SyncTransform(const Transform& transform)
	//{
	//	m_Center.x = transform.m_Position.x;
	//	m_Center.y = transform.m_Position.y;
	//}

	// ----------------------------------------------------------------------------
	// Circle Helpers
	// ----------------------------------------------------------------------------
	glm::vec2 Physics2DSystem::GetCenter(const Circle& circle)
	{
		// TODO: IMPLEMENT ME
		return glm::vec2();
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
		ray.m_Ignore = entt::null;
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
}