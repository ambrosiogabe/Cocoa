#include "jade/physics2d/Physics2DSystem.h"
#include "jade/components/Transform.h"
#include "jade/core/Application.h"
#include "jade/util/JMath.h"
#include "jade/commands/CommandHistory.h"
#include "jade/commands/ChangeVec2Command.h"
#include "jade/commands/ChangeFloatCommand.h"
#include "jade/commands/ChangeEnumCommand.h"
#include "jade/core/ImGuiExtended.h"

#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

namespace Jade
{
	void Physics2DSystem::ImGui(entt::registry& registry)
	{
		entt::entity activeEntity = Application::Get()->GetScene()->GetActiveEntity();

		if (registry.has<Rigidbody2D>(activeEntity))
		{
			static bool treeNodeOpen = true;
			ImGui::SetNextTreeNodeOpen(treeNodeOpen);
			if (ImGui::CollapsingHeader("Rigidbody 2D"))
			{
				Rigidbody2D& rb = registry.get<Rigidbody2D>(activeEntity);
				int currentItem = static_cast<int>(rb.m_BodyType);
				std::array<const char*, 3> items = { "Dynamic", "Kinematic", "Static" };
				if (ImGui::Combo("Body Type:", &currentItem, &items[0], items.size()))
				{
					CommandHistory::AddCommand(new ChangeEnumCommand<BodyType2D>(rb.m_BodyType, static_cast<BodyType2D>(currentItem)));
					CommandHistory::SetNoMergeMostRecent();
				}

				ImGui::Checkbox("Continous: ##0", &rb.m_ContinuousCollision);
				ImGui::Checkbox("Fixed Rotation##1", &rb.m_FixedRotation);
				ImGui::UndoableDragFloat("Linear Damping: ##2", rb.m_LinearDamping);
				ImGui::UndoableDragFloat("Angular Damping: ##3", rb.m_AngularDamping);
				ImGui::UndoableDragFloat("Mass: ##4", rb.m_Mass);
				ImGui::UndoableDragFloat2("Velocity: ##5", rb.m_Velocity);
			}
		}

		if (registry.has<AABB>(activeEntity))
		{
			static bool treeNodeOpen = true;
			ImGui::SetNextTreeNodeOpen(treeNodeOpen);
			if (ImGui::CollapsingHeader("AABB"))
			{
				AABB& box = registry.get<AABB>(activeEntity);
				ImGui::UndoableDragFloat2("Offset: ##6", box.m_Offset);
				ImGui::UndoableDragFloat2("Size: ##7", box.m_Size);
			}
		}

		if (registry.has<Box2D>(activeEntity))
		{
			static bool treeNodeOpen = true;
			ImGui::SetNextTreeNodeOpen(treeNodeOpen);
			if (ImGui::CollapsingHeader("Box2D"))
			{
				Box2D& box = registry.get<Box2D>(activeEntity);
				//ImGui::UndoableDragFloat2("Offset: ", box.m_Offset);
				ImGui::UndoableDragFloat2("Size: ##8", box.m_HalfSize);
			}
		}

		if (registry.has<Circle>(activeEntity))
		{
			static bool treeNodeOpen = true;
			ImGui::SetNextTreeNodeOpen(treeNodeOpen);
			if (ImGui::CollapsingHeader("Circle"))
			{
				Circle& circle = registry.get<Circle>(activeEntity);
				//ImGui::UndoableDragFloat2("Offset: ", box.m_Offset);
				ImGui::UndoableDragFloat("Radius: ##9", circle.m_Radius);
			}
		}
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
		auto result = Application::Get()->GetScene()->GetEntity(box);
		Transform& transform = result.first.get<Transform>(result.second);

		glm::vec2 boxScale = transform.m_Scale;
		glm::vec2 boxCenter = JMath::Vector2From3(transform.m_Position);// +(box.m_Offset * boxScale);
		glm::vec2 boxHalfSize = box.m_HalfSize * boxScale;

		return boxCenter - boxHalfSize;
	}

	glm::vec2 Physics2DSystem::GetMax(const Box2D& box)
	{
		auto result = Application::Get()->GetScene()->GetEntity(box);
		Transform& transform = result.first.get<Transform>(result.second);

		glm::vec2 boxScale = transform.m_Scale;
		glm::vec2 boxCenter = JMath::Vector2From3(transform.m_Position);// +(box.m_Offset * boxScale);
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

		if (!JMath::Compare(GetRotation(box), 0.0f))
		{
			for (auto& vec : vertices)
			{
				JMath::Rotate(vec, rotation, center);
			}
		}

		return vertices;
	}

	glm::vec2 Physics2DSystem::GetCenter(const Box2D& box)
	{
		auto result = Application::Get()->GetScene()->GetEntity(box);
		auto& transform = result.first.get<Transform>(result.second);
		return JMath::Vector2From3(transform.m_Position);
	}

	float Physics2DSystem::GetRotation(const Box2D& box)
	{
		auto result = Application::Get()->GetScene()->GetEntity(box);
		auto& transform = result.first.get<Transform>(result.second);
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
		auto result = Application::Get()->GetScene()->GetEntity(box);
		Transform& transform = result.first.get<Transform>(result.second);

		glm::vec2 boxScale = transform.m_Scale;
		glm::vec2 boxCenter = JMath::Vector2From3(transform.m_Position) + (box.m_Offset * boxScale);
		glm::vec2 boxHalfSize = box.m_HalfSize * boxScale;

		return boxCenter + boxHalfSize;
	}

	glm::vec2 Physics2DSystem::GetMin(const AABB& box)
	{
		auto result = Application::Get()->GetScene()->GetEntity(box);
		Transform& transform = result.first.get<Transform>(result.second);

		glm::vec2 boxScale = transform.m_Scale;
		glm::vec2 boxCenter = JMath::Vector2From3(transform.m_Position) + (box.m_Offset * boxScale);
		glm::vec2 boxHalfSize = box.m_HalfSize * boxScale;

		return boxCenter - boxHalfSize;
	}

	glm::vec2 Physics2DSystem::GetCenter(const AABB& box)
	{
		auto result = Application::Get()->GetScene()->GetEntity(box);
		auto& transform = result.first.get<Transform>(result.second);
		return JMath::Vector2From3(transform.m_Position);
	}

	// ----------------------------------------------------------------------------
	// Circle Helpers
	// ----------------------------------------------------------------------------
	glm::vec2 Physics2DSystem::GetCenter(const Circle& circle)
	{
		auto result = Application::Get()->GetScene()->GetEntity(circle);
		auto& transform = result.first.get<Transform>(result.second);
		return JMath::Vector2From3(transform.m_Position);
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