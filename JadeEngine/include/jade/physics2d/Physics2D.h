#pragma once

#include "jade/physics2d/Physics2DSystem.h"
#include "jade/core/Core.h"

#include <entt/entt.h>
#include <box2d/b2_world.h>

namespace Jade
{
	class Physics2D
	{
	public:
		static AABB GetBoundingBoxForPixels(uint8* pixels, int width, int height, int channels);
		static std::pair<entt::registry&, entt::entity> OverlapPoint(const glm::vec2& point);

		static Physics2D* Get();
		static void Init(entt::registry& registry);

		void AddEntity(entt::entity entity);
		void Update(float dt);
		void Destroy();

	private:
		Physics2D(entt::registry& registry)
			: m_Registry(registry) {}

		entt::registry& m_Registry;
		
		// Box2D Stuff
		b2Vec2 m_Gravity { 0.0f, -10.0f };
		b2World m_World { m_Gravity };
		float m_PhysicsTime = 0.0f;

		static std::unique_ptr<Physics2D> s_Instance;
	};
}