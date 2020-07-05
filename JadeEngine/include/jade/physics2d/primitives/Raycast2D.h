#pragma once

#include <glm/vec2.hpp>
#include <entt.h>

namespace Jade
{
	struct Raycast2D
	{
		glm::vec2 m_Origin;
		glm::vec2 m_Direction;
		float m_MaxDistance;
		entt::entity m_Ignore;

		Raycast2D(glm::vec2 origin, glm::vec2 direction)
		{
			m_Origin = origin;
			m_Direction = direction;
			m_MaxDistance = std::numeric_limits<float>::max();
			m_Ignore = entt::null;
		}

		Raycast2D(glm::vec2 origin, glm::vec2 direction, float maxDistance, entt::entity ignore)
		{
			m_Origin = origin;
			m_Direction = direction;
			m_MaxDistance = maxDistance;
			m_Ignore = ignore;
		}
	};
}