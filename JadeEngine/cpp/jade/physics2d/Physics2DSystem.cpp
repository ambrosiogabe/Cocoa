#include "externalLibs.h"

#include "jade/physics2d/Physics2DSystem.h"
#include "jade/components/Transform.h"
#include "jade/core/Application.h"
#include "jade/util/JMath.h"
#include "jade/commands/ICommand.h"
#include "jade/renderer/DebugDraw.h"
#include "jade/core/ImGuiExtended.h"

namespace Jade
{
	void Physics2DSystem::Render(entt::registry& registry)
	{
		entt::entity activeEntity = Application::Get()->GetScene()->GetActiveEntity();

		if (entt::to_integral(activeEntity) != entt::to_integral(entt::null))
		{
			if (registry.has<Box2D, Transform>(activeEntity))
			{
				const Box2D& box = registry.get<Box2D>(activeEntity);
				const Transform& transform = registry.get<Transform>(activeEntity);
				DebugDraw::AddBox2D(JMath::Vector2From3(transform.m_Position), box.m_HalfSize * 2.0f * JMath::Vector2From3(transform.m_Scale), transform.m_EulerRotation.z);
			}
		}
	}

	void Physics2DSystem::ImGui(entt::registry& registry)
	{
		entt::entity activeEntity = Application::Get()->GetScene()->GetActiveEntity();

		if (registry.has<Rigidbody2D>(activeEntity))
		{
			static bool treeNodeOpen = true;
			ImGui::SetNextTreeNodeOpen(treeNodeOpen);
			if (ImGui::CollapsingHeader("Rigidbody 2D"))
			{
				JImGui::BeginCollapsingHeaderGroup();

				Rigidbody2D& rb = registry.get<Rigidbody2D>(activeEntity);
				int currentItem = static_cast<int>(rb.m_BodyType);
				std::array<const char*, 3> items = { "Dynamic", "Kinematic", "Static" };
				JImGui::UndoableCombo<BodyType2D>(rb.m_BodyType, "Body Type:", &items[0], (int)items.size());
				//if (ImGui::UndoableCombo<BodyType2D>(rb.m_BodyType, "Body Type:", &items[0], (int)items.size()))
				//{
				//	CommandHistory::AddCommand(new ChangeEnumCommand<BodyType2D>(rb.m_BodyType, static_cast<BodyType2D>(currentItem)));
				//	CommandHistory::SetNoMergeMostRecent();
				//}

				JImGui::Checkbox("Continous: ##0", &rb.m_ContinuousCollision);
				JImGui::Checkbox("Fixed Rotation##1", &rb.m_FixedRotation);
				JImGui::UndoableDragFloat("Linear Damping: ##2", rb.m_LinearDamping);
				JImGui::UndoableDragFloat("Angular Damping: ##3", rb.m_AngularDamping);
				JImGui::UndoableDragFloat("Mass: ##4", rb.m_Mass);
				JImGui::UndoableDragFloat2("Velocity: ##5", rb.m_Velocity);

				JImGui::EndCollapsingHeaderGroup();
			}
		}

		if (registry.has<AABB>(activeEntity))
		{
			static bool treeNodeOpen = true;
			ImGui::SetNextTreeNodeOpen(treeNodeOpen);
			if (ImGui::CollapsingHeader("AABB"))
			{
				JImGui::BeginCollapsingHeaderGroup();

				AABB& box = registry.get<AABB>(activeEntity);
				JImGui::UndoableDragFloat2("Offset: ##6", box.m_Offset);
				JImGui::UndoableDragFloat2("Size: ##7", box.m_Size);

				JImGui::EndCollapsingHeaderGroup();
			}
		}

		if (registry.has<Box2D>(activeEntity))
		{
			static bool treeNodeOpen = true;
			ImGui::SetNextTreeNodeOpen(treeNodeOpen);
			if (ImGui::CollapsingHeader("Box2D"))
			{
				JImGui::BeginCollapsingHeaderGroup();

				Box2D& box = registry.get<Box2D>(activeEntity);
				//ImGui::UndoableDragFloat2("Offset: ", box.m_Offset);
				JImGui::UndoableDragFloat2("Size: ##8", box.m_HalfSize);

				JImGui::EndCollapsingHeaderGroup();
			}
		}

		if (registry.has<Circle>(activeEntity))
		{
			static bool treeNodeOpen = true;
			ImGui::SetNextTreeNodeOpen(treeNodeOpen);
			if (ImGui::CollapsingHeader("Circle"))
			{
				JImGui::BeginCollapsingHeaderGroup();

				Circle& circle = registry.get<Circle>(activeEntity);
				//ImGui::UndoableDragFloat2("Offset: ", box.m_Offset);
				JImGui::UndoableDragFloat("Radius: ##9", circle.m_Radius);

				JImGui::EndCollapsingHeaderGroup();
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

	void Physics2DSystem::Serialize(entt::entity& entity, const AABB& box)
	{
		json& j = Application::Get()->GetScene()->GetSaveDataJson();

		json halfSize = JMath::Serialize("HalfSize", box.m_HalfSize);
		json offset = JMath::Serialize("Offset", box.m_Offset);
		int size = j["Size"];
		j["Components"][size] = {
			{"AABB", {
				{"Entity", entt::to_integral(entity)},
				halfSize,
				offset
			}}
		};

		j["Size"] = size + 1;
	}

	void Physics2DSystem::DeserializeAABB(json& j, entt::registry& registry, entt::entity entity)
	{
		AABB box;
		box.m_HalfSize = JMath::DeserializeVec2(j["AABB"]["HalfSize"]);
		box.m_Size = box.m_HalfSize * 2.0f;
		box.m_Offset = JMath::DeserializeVec2(j["AABB"]["Offset"]);
		registry.emplace<AABB>(entity, box);
	}

	void Physics2DSystem::Serialize(entt::entity& entity, const Box2D& box)
	{
		json& j = Application::Get()->GetScene()->GetSaveDataJson();

		json halfSize = JMath::Serialize("HalfSize", box.m_HalfSize);
		int size = j["Size"];
		j["Components"][size] = {
			{"Box2D", {
				{"Entity", entt::to_integral(entity)},
				halfSize,
			}}
		};

		j["Size"] = size + 1;
	}

	void Physics2DSystem::DeserializeBox2D(json& j, entt::registry& registry, entt::entity entity)
	{
		Box2D box;
		box.m_HalfSize = JMath::DeserializeVec2(j["Box2D"]["HalfSize"]);
		box.m_Size = box.m_HalfSize * 2.0f;
		registry.emplace<Box2D>(entity, box);
	}

	void Physics2DSystem::Serialize(entt::entity& entity, const Rigidbody2D& rb)
	{
		json& j = Application::Get()->GetScene()->GetSaveDataJson();

		json angularDamping = { "AngularDamping", rb.m_AngularDamping };
		json linearDamping = { "LinearDamping", rb.m_LinearDamping };
		json mass = { "Mass", rb.m_Mass };
		json velocity = JMath::Serialize("Velocity", rb.m_Velocity);
		json continousCollision = { "ContinousCollision", rb.m_ContinuousCollision };
		json fixedRotation = { "FixedRotation", rb.m_FixedRotation };
		int size = j["Size"];
		j["Components"][size] = {
			{"Rigidbody2D", {
				{"Entity", entt::to_integral(entity)},
				angularDamping,
				linearDamping,
				mass,
				velocity,
				continousCollision,
				fixedRotation
			}}
		};

		j["Size"] = size + 1;
	}

	void Physics2DSystem::DeserializeRigidbody2D(json& j, entt::registry& registry, entt::entity entity)
	{
		Rigidbody2D rb;
		rb.m_AngularDamping = j["Rigidbody2D"]["AngularDamping"];
		rb.m_LinearDamping = j["Rigidbody2D"]["LinearDamping"];
		rb.m_Mass = j["Rigidbody2D"]["Mass"];
		rb.m_Velocity = JMath::DeserializeVec2(j["Rigidbody2D"]["Velocity"]);
		rb.m_ContinuousCollision = j["Rigidbody2D"]["ContinousCollision"];
		rb.m_FixedRotation = j["Rigidbody2D"]["FixedRotation"];
		registry.emplace<Rigidbody2D>(entity, rb);
	}
}