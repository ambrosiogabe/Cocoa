#pragma once

#include "jade/physics2d/Physics2DSystem.h"
#include "jade/core/Core.h"

#include <entt/entt.h>

namespace Jade
{
	class Physics2D
	{
	public:
		static BoundingBox GetBoundingBoxForPixels(uint8* pixels, int width, int height, int channels);
		static std::pair<entt::registry&, entt::entity> OverlapPoint(const glm::vec2& point);

		static Physics2D* Get();
		static void Init(entt::registry& registry);

	private:
		Physics2D(entt::registry& registry)
			: m_Registry(registry) { }

		entt::registry& m_Registry;

		static std::unique_ptr<Physics2D> s_Instance;
	};
}