#pragma once

#include "jade/physics2d/rigidbody/Rigidbody2D.h"
#include "jade/util/Log.h"
#include "jade/util/JMath.h"

#include <glm/vec2.hpp>
#include <array>

namespace Jade
{
	struct Box2D
	{
		glm::vec2 m_Size = glm::vec2();
		glm::vec2 m_HalfSize = glm::vec2();
		Rigidbody2D* rigidbody = nullptr;

		Box2D(glm::vec2 min, glm::vec2 max)
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

		std::array<glm::vec2, 4> GetVertices() const
		{
			Log::Assert(rigidbody != nullptr, "Collider must have a reference to a rigidbody.");

			glm::vec2 min = GetMin();
			glm::vec2 max = GetMax();

			std::array<glm::vec2, 4> vertices = {
				glm::vec2(min.x, min.y), glm::vec2(min.x, max.y),
				glm::vec2(max.x, max.y), glm::vec2(max.x, min.y)
			};

			if (!JMath::Compare(rigidbody->m_Rotation, 0.0f))
			{
				for (auto& vec : vertices)
				{
					//JMath::Rotate(vec, rigidbody->m_Rotation, rigidbody->m_Position);
				}
			}

			return vertices;
		}
	};
}