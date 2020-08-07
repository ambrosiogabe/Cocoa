#pragma once
#include "externalLibs.h"

#include "EditorWindows/InspectorWindow.h"
#include "Gui/ImGuiExtended.h"
#include "FontAwesome.h"

#include "jade/components/components.h"
#include "jade/components/Transform.h"
#include "jade/physics2d/Physics2DSystem.h"
#include "jade/renderer/DebugDraw.h"
#include "jade/util/JMath.h"

namespace Jade
{
	std::vector<Entity> InspectorWindow::s_ActiveEntities = std::vector<Entity>();

	void InspectorWindow::ImGui()
	{
		ImGui::Begin(ICON_FA_CUBE "Inspector");
		if (s_ActiveEntities.size() == 0)
		{
			ImGui::End();
			return;
		}

		bool doTransform = true;
		bool doSpriteRenderer = true;
		bool doRigidbody2D = true;
		bool doAABB = true;
		bool doBox2D = true;
		bool doCircle = true;

		for (auto& entity : s_ActiveEntities)
		{
			doTransform &= entity.HasComponent<Transform>();
			doSpriteRenderer &= entity.HasComponent<SpriteRenderer>();
			doRigidbody2D &= entity.HasComponent<Rigidbody2D>();
			doAABB &= entity.HasComponent<AABB>();
			doBox2D &= entity.HasComponent<Box2D>();
			doCircle &= entity.HasComponent<Circle>();
		}

		if (doTransform)
			ImGuiTransform(s_ActiveEntities[0].GetComponent<Transform>());
		if (doSpriteRenderer)
			ImGuiSpriteRenderer(s_ActiveEntities[0].GetComponent<SpriteRenderer>());
		if (doRigidbody2D)
			ImGuiRigidbody2D(s_ActiveEntities[0].GetComponent<Rigidbody2D>());
		if (doBox2D)
			ImGuiBox2D(s_ActiveEntities[0].GetComponent<Box2D>());
		if (doAABB)
			ImGuiAABB(s_ActiveEntities[0].GetComponent<AABB>());
		if (doCircle)
			ImGuiCircle(s_ActiveEntities[0].GetComponent<Circle>());

		ImGuiAddComponentButton();
		ImGui::End();
	}

	void InspectorWindow::AddEntity(Entity entity)
	{
		if (std::find(s_ActiveEntities.begin(), s_ActiveEntities.end(), entity) == s_ActiveEntities.end())
			s_ActiveEntities.push_back(entity);
	}

	void InspectorWindow::RemoveEntity(Entity entity)
	{
		auto iter = std::find(s_ActiveEntities.begin(), s_ActiveEntities.end(), entity);
		if (iter != s_ActiveEntities.end())
			s_ActiveEntities.erase(iter);
	}

	void InspectorWindow::ClearAllEntities()
	{
		s_ActiveEntities.clear();
	}

	Entity InspectorWindow::GetActiveEntity()
	{
		if (s_ActiveEntities.size() == 0)
			return Entity::Null;
		return s_ActiveEntities[0];
	}

	// =====================================================================
	// Helper ImGui functions
	// =====================================================================
	void InspectorWindow::ImGuiAddComponentButton()
	{
		Entity activeEntity = s_ActiveEntities[0];
		int itemPressed = 0;
		std::array<const char*, 4> components = { "Sprite Renderer", "Rigidbody2D", "BoxCollider2D", "CircleCollider2D" };
		if (JImGui::ButtonDropdown(ICON_FA_PLUS " Add Component", components.data(), (int)components.size(), itemPressed))
		{
			switch (itemPressed)
			{
			case 0:
				activeEntity.AddComponent<SpriteRenderer>();
				break;
			case 1:
				activeEntity.AddComponent<Rigidbody2D>();
				break;
			case 2:
				activeEntity.AddComponent<Box2D>();
				break;
			case 3:
				activeEntity.AddComponent<Circle>();
				break;
			}
		}
	}

	// =====================================================================
	// Basic components
	// =====================================================================
	void InspectorWindow::ImGuiTransform(Transform& transform)
	{
		static bool collapsingHeaderOpen = true;
		ImGui::SetNextTreeNodeOpen(collapsingHeaderOpen);
		if (ImGui::CollapsingHeader(ICON_FA_STAMP " Transform"))
		{
			JImGui::BeginCollapsingHeaderGroup();
			JImGui::UndoableDragFloat3("Position: ", transform.m_Position);
			JImGui::UndoableDragFloat3("Scale: ", transform.m_Scale);
			JImGui::UndoableDragFloat3("Rotation: ", transform.m_EulerRotation);
			JImGui::EndCollapsingHeaderGroup();
		}
	}


	// =====================================================================
	// Renderer components
	// =====================================================================
	void InspectorWindow::ImGuiSpriteRenderer(SpriteRenderer& spr)
	{
		static bool collapsingHeaderOpen = true;
		ImGui::SetNextTreeNodeOpen(collapsingHeaderOpen);
		if (ImGui::CollapsingHeader("Sprite Renderer"))
		{
			JImGui::BeginCollapsingHeaderGroup();
			JImGui::UndoableDragInt("Z-Index: ", spr.m_ZIndex);
			JImGui::UndoableColorEdit4("Sprite Color: ", spr.m_Color);

			if (spr.m_Sprite.m_Texture)
			{
				JImGui::InputText("##SpriteRendererTexture", (char*)spr.m_Sprite.m_Texture->GetFilepath().Filename(),
					spr.m_Sprite.m_Texture->GetFilepath().FilenameSize(), ImGuiInputTextFlags_ReadOnly);
			}
			else
			{
				JImGui::InputText("##SpriteRendererTexture", "Default Sprite", 14, ImGuiInputTextFlags_ReadOnly);
			}
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TEXTURE_HANDLE_ID"))
				{
					IM_ASSERT(payload->DataSize == sizeof(int));
					int textureResourceId = *(const int*)payload->Data;
					spr.m_Sprite.m_Texture = std::static_pointer_cast<Texture>(AssetManager::GetAsset(textureResourceId));
				}
				ImGui::EndDragDropTarget();
			}

			JImGui::EndCollapsingHeaderGroup();
		}
	}


	// =====================================================================
	// Physics components
	// =====================================================================
	void InspectorWindow::ImGuiRigidbody2D(Rigidbody2D& rb)
	{
		static bool treeNodeOpen = true;
		ImGui::SetNextTreeNodeOpen(treeNodeOpen);
		if (ImGui::CollapsingHeader("Rigidbody 2D"))
		{
			JImGui::BeginCollapsingHeaderGroup();

			int currentItem = static_cast<int>(rb.m_BodyType);
			std::array<const char*, 3> items = { "Dynamic", "Kinematic", "Static" };
			JImGui::UndoableCombo<BodyType2D>(rb.m_BodyType, "Body Type:", &items[0], (int)items.size());
			if (JImGui::UndoableCombo<BodyType2D>(rb.m_BodyType, "Body Type:", &items[0], (int)items.size()))
			{
				CommandHistory::AddCommand(new ChangeEnumCommand<BodyType2D>(rb.m_BodyType, static_cast<BodyType2D>(currentItem)));
				CommandHistory::SetNoMergeMostRecent();
			}

			JImGui::Checkbox("Continous: ##0", &rb.m_ContinuousCollision);
			JImGui::Checkbox("Fixed Rotation##1", &rb.m_FixedRotation);
			JImGui::UndoableDragFloat("Linear Damping: ##2", rb.m_LinearDamping);
			JImGui::UndoableDragFloat("Angular Damping: ##3", rb.m_AngularDamping);
			JImGui::UndoableDragFloat("Mass: ##4", rb.m_Mass);
			JImGui::UndoableDragFloat2("Velocity: ##5", rb.m_Velocity);

			JImGui::EndCollapsingHeaderGroup();
		}
	}

	void InspectorWindow::ImGuiAABB(AABB& box)
	{
		static bool treeNodeOpen = true;
		ImGui::SetNextTreeNodeOpen(treeNodeOpen);
		if (ImGui::CollapsingHeader("AABB"))
		{
			JImGui::BeginCollapsingHeaderGroup();

			JImGui::UndoableDragFloat2("Offset: ##6", box.m_Offset);
			JImGui::UndoableDragFloat2("Size: ##7", box.m_Size);

			JImGui::EndCollapsingHeaderGroup();
		}
	}

	void InspectorWindow::ImGuiBox2D(Box2D& box)
	{
		static bool treeNodeOpen = true;
		ImGui::SetNextTreeNodeOpen(treeNodeOpen);
		if (ImGui::CollapsingHeader("Box2D"))
		{
			JImGui::BeginCollapsingHeaderGroup();

			//ImGui::UndoableDragFloat2("Offset: ", box.m_Offset);
			JImGui::UndoableDragFloat2("Size: ##8", box.m_HalfSize);

			JImGui::EndCollapsingHeaderGroup();

			// Draw box highlight
			const Transform& transform = s_ActiveEntities[0].GetComponent<Transform>();
			DebugDraw::AddBox2D(JMath::Vector2From3(transform.m_Position), box.m_HalfSize * 2.0f * JMath::Vector2From3(transform.m_Scale), transform.m_EulerRotation.z);
		}
	}

	void InspectorWindow::ImGuiCircle(Circle& circle)
	{
		static bool treeNodeOpen = true;
		ImGui::SetNextTreeNodeOpen(treeNodeOpen);
		if (ImGui::CollapsingHeader("Circle"))
		{
			JImGui::BeginCollapsingHeaderGroup();

			//ImGui::UndoableDragFloat2("Offset: ", box.m_Offset);
			JImGui::UndoableDragFloat("Radius: ##9", circle.m_Radius);

			JImGui::EndCollapsingHeaderGroup();
		}
	}
}