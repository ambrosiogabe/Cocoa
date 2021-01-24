#include "editorWindows/InspectorWindow.h"
#include "gui/ImGuiExtended.h"
#include "gui/FontAwesome.h"
#include "nativeScripting/SourceFileWatcher.h"

#include "cocoa/util/CMath.h"
#include "cocoa/components/components.h"
#include "cocoa/components/Transform.h"
#include "cocoa/physics2d/Physics2DSystem.h"
#include "cocoa/renderer/DebugDraw.h"
#include "cocoa/systems/ScriptSystem.h"
#include "cocoa/core/AssetManager.h"

namespace Cocoa
{
	std::vector<Entity> InspectorWindow::s_ActiveEntities = std::vector<Entity>();
	ScriptSystem* InspectorWindow::s_ScriptSystem = nullptr;

	static const char* stringBuffer[100];

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
		if (s_ScriptSystem)
			s_ScriptSystem->ImGui(s_ActiveEntities[0]);

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
		const std::vector<UClass> classes = SourceFileWatcher::GetClasses();
		int size = classes.size() + 4;
		auto classIter = classes.begin();
		for (int i = 4; i < classes.size() + 4; i++)
		{
			stringBuffer[i] = classIter->m_ClassName.c_str();
			classIter++;
		}

		stringBuffer[0] = "Sprite Renderer";
		stringBuffer[1] = "Rigidbody2D";
		stringBuffer[2] = "Box Collider2D";
		stringBuffer[3] = "Circle Collider2D";

		Entity activeEntity = s_ActiveEntities[0];
		int itemPressed = 0;
		if (CImGui::ButtonDropdown(ICON_FA_PLUS " Add Component", stringBuffer, size, itemPressed))
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
			default:
				if (s_ScriptSystem != nullptr)
				{
					Log::Info("Adding component %s from inspector tp %d", stringBuffer[itemPressed], entt::to_integral(activeEntity.GetRawEntity()));
					s_ScriptSystem->AddComponentFromString(stringBuffer[itemPressed], activeEntity.GetRawEntity(), activeEntity.GetRegistry());
				}
				else
				{
					Log::Error("Trying to add script as component, but script system not set in inspector.");
				}
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
		if (ImGui::CollapsingHeader(ICON_FA_STAMP " Transform"))
		{
			CImGui::BeginCollapsingHeaderGroup();
			CImGui::UndoableDragFloat3("Position: ", transform.m_Position);
			CImGui::UndoableDragFloat3("Scale: ", transform.m_Scale);
			CImGui::UndoableDragFloat3("Rotation: ", transform.m_EulerRotation);
			CImGui::EndCollapsingHeaderGroup();
		}
	}


	// =====================================================================
	// Renderer components
	// =====================================================================
	void InspectorWindow::ImGuiSpriteRenderer(SpriteRenderer& spr)
	{
		static bool collapsingHeaderOpen = true;
		if (ImGui::CollapsingHeader("Sprite Renderer"))
		{
			CImGui::BeginCollapsingHeaderGroup();
			CImGui::UndoableDragInt("Z-Index: ", spr.m_ZIndex);
			CImGui::UndoableColorEdit4("Sprite Color: ", spr.m_Color);

			if (spr.m_Sprite.m_Texture)
			{
				const Texture& tex = AssetManager::GetTexture(spr.m_Sprite.m_Texture.m_AssetId);
				CImGui::InputText("##SpriteRendererTexture", (char*)tex.GetFilepath().Filename(),
					tex.GetFilepath().FilenameSize(), ImGuiInputTextFlags_ReadOnly);
			}
			else
			{
				CImGui::InputText("##SpriteRendererTexture", "Default Sprite", 14, ImGuiInputTextFlags_ReadOnly);
			}
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TEXTURE_HANDLE_ID"))
				{
					IM_ASSERT(payload->DataSize == sizeof(int));
					int textureResourceId = *(const int*)payload->Data;
					spr.m_Sprite.m_Texture = textureResourceId;
				}
				ImGui::EndDragDropTarget();
			}

			CImGui::EndCollapsingHeaderGroup();
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
			CImGui::BeginCollapsingHeaderGroup();

			int currentItem = static_cast<int>(rb.m_BodyType);
			std::array<const char*, 3> items = { "Dynamic", "Kinematic", "Static" };
			CImGui::UndoableCombo<BodyType2D>(rb.m_BodyType, "Body Type:", &items[0], (int)items.size());

			CImGui::Checkbox("Continous: ##0", &rb.m_ContinuousCollision);
			CImGui::Checkbox("Fixed Rotation##1", &rb.m_FixedRotation);
			CImGui::UndoableDragFloat("Linear Damping: ##2", rb.m_LinearDamping);
			CImGui::UndoableDragFloat("Angular Damping: ##3", rb.m_AngularDamping);
			CImGui::UndoableDragFloat("Mass: ##4", rb.m_Mass);
			CImGui::UndoableDragFloat2("Velocity: ##5", rb.m_Velocity);

			CImGui::EndCollapsingHeaderGroup();
		}
	}

	void InspectorWindow::ImGuiAABB(AABB& box)
	{
		static bool treeNodeOpen = true;
		ImGui::SetNextTreeNodeOpen(treeNodeOpen);
		if (ImGui::CollapsingHeader("AABB"))
		{
			CImGui::BeginCollapsingHeaderGroup();

			CImGui::UndoableDragFloat2("Offset: ##6", box.m_Offset);
			CImGui::UndoableDragFloat2("Size: ##7", box.m_Size);

			CImGui::EndCollapsingHeaderGroup();
		}
	}

	void InspectorWindow::ImGuiBox2D(Box2D& box)
	{
		static bool treeNodeOpen = true;
		ImGui::SetNextTreeNodeOpen(treeNodeOpen);
		if (ImGui::CollapsingHeader("Box2D"))
		{
			CImGui::BeginCollapsingHeaderGroup();

			//ImGui::UndoableDragFloat2("Offset: ", box.m_Offset);
			CImGui::UndoableDragFloat2("Size: ##8", box.m_HalfSize);

			CImGui::EndCollapsingHeaderGroup();

			// Draw box highlight
			const Transform& transform = s_ActiveEntities[0].GetComponent<Transform>();
			DebugDraw::AddBox2D(CMath::Vector2From3(transform.m_Position), box.m_HalfSize * 2.0f * CMath::Vector2From3(transform.m_Scale), transform.m_EulerRotation.z);
		}
	}

	void InspectorWindow::ImGuiCircle(Circle& circle)
	{
		static bool treeNodeOpen = true;
		ImGui::SetNextTreeNodeOpen(treeNodeOpen);
		if (ImGui::CollapsingHeader("Circle"))
		{
			CImGui::BeginCollapsingHeaderGroup();

			//ImGui::UndoableDragFloat2("Offset: ", box.m_Offset);
			CImGui::UndoableDragFloat("Radius: ##9", circle.m_Radius);

			CImGui::EndCollapsingHeaderGroup();
		}
	}
}