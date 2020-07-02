#pragma once

#include "jade/physics2d/rigidbody/Rigidbody2D.h"
#include "jade/util/Log.h"

#include <glm/vec2.hpp>
#include <array>

namespace Jade
{
	struct AABB
	{
		glm::vec2 m_Size = glm::vec2();
		glm::vec2 m_HalfSize = glm::vec2();
		Rigidbody2D* rigidbody = nullptr;

		AABB(glm::vec2 min, glm::vec2 max)
		{
			m_Size = max - min;
			m_HalfSize = m_Size / 2.0f;
		}

		glm::vec2 GetMin() const
		{
			Log::Assert(rigidbody != nullptr, "Collider must have a reference to a rigidbody.");

			return rigidbody->m_Position - m_HalfSize;
		}

		glm::vec2 GetMax() const
		{
			Log::Assert(rigidbody != nullptr, "Collider must have a reference to a rigidbody.");

			return rigidbody->m_Position + m_HalfSize;
		}
	};
}