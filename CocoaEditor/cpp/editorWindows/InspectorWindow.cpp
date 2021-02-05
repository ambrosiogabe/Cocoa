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
	namespace InspectorWindowUtil
	{
		// Internal Variables
		std::vector<Entity> ActiveEntities = std::vector<Entity>();
		static const char* StringBuffer[100];

		// Forward declarations
		// =====================================================================
		// Basic components
		// =====================================================================
		static void ImGuiTransform(Transform& transform);

		// =====================================================================
		// Renderer components
		// =====================================================================
		static void ImGuiSpriteRenderer(SpriteRenderer& spr);
		static void ImGuiFontRenderer(FontRenderer& fontRenderer);

		// =====================================================================
		// Physics components
		// =====================================================================
		static void ImGuiRigidbody2D(Rigidbody2D& rb);
		static void ImGuiAABB(AABB& box);
		static void ImGuiBox2D(Box2D& box);
		static void ImGuiCircle(Circle& circle);

		static void ImGuiAddComponentButton(const InspectorWindow& inspectorWindow);

		void ImGui(const InspectorWindow& inspectorWindow)
		{
			ImGui::Begin(ICON_FA_CUBE "Inspector");
			if (ActiveEntities.size() == 0)
			{
				ImGui::End();
				return;
			}

			bool doTransform = true;
			bool doSpriteRenderer = true;
			bool doFontRenderer = true;
			bool doRigidbody2D = true;
			bool doAABB = true;
			bool doBox2D = true;
			bool doCircle = true;

			for (auto& entity : ActiveEntities)
			{
				doTransform &= entity.HasComponent<Transform>();
				doSpriteRenderer &= entity.HasComponent<SpriteRenderer>();
				doFontRenderer &= entity.HasComponent<FontRenderer>();
				doRigidbody2D &= entity.HasComponent<Rigidbody2D>();
				doAABB &= entity.HasComponent<AABB>();
				doBox2D &= entity.HasComponent<Box2D>();
				doCircle &= entity.HasComponent<Circle>();
			}

			if (doTransform)
				ImGuiTransform(ActiveEntities[0].GetComponent<Transform>());
			if (doSpriteRenderer)
				ImGuiSpriteRenderer(ActiveEntities[0].GetComponent<SpriteRenderer>());
			if (doFontRenderer)
				ImGuiFontRenderer(ActiveEntities[0].GetComponent<FontRenderer>());
			if (doRigidbody2D)
				ImGuiRigidbody2D(ActiveEntities[0].GetComponent<Rigidbody2D>());
			if (doBox2D)
				ImGuiBox2D(ActiveEntities[0].GetComponent<Box2D>());
			if (doAABB)
				ImGuiAABB(ActiveEntities[0].GetComponent<AABB>());
			if (doCircle)
				ImGuiCircle(ActiveEntities[0].GetComponent<Circle>());
			// TODO Fix this by resolving depenedency on imguilayer somehow
			//if (inspectorWindow.ScriptSystemPtr)
				//inspectorWindow.ScriptSystemPtr->ImGui(ActiveEntities[0]);

			ImGuiAddComponentButton(inspectorWindow);
			ImGui::End();
		}

		void AddEntity(Entity entity)
		{
			if (std::find(ActiveEntities.begin(), ActiveEntities.end(), entity) == ActiveEntities.end())
				ActiveEntities.push_back(entity);
		}

		void RemoveEntity(Entity entity)
		{
			auto iter = std::find(ActiveEntities.begin(), ActiveEntities.end(), entity);
			if (iter != ActiveEntities.end())
				ActiveEntities.erase(iter);
		}

		void ClearAllEntities()
		{
			ActiveEntities.clear();
		}

		Entity GetActiveEntity()
		{
			if (ActiveEntities.size() == 0)
				return Entity::Null;
			return ActiveEntities[0];
		}

		// =====================================================================
		// Internal Functions
		// =====================================================================
		static void ImGuiAddComponentButton(const InspectorWindow& inspectorWindow)
		{
			const std::vector<UClass> classes = SourceFileWatcher::GetClasses();
			int size = classes.size() + 4;
			auto classIter = classes.begin();
			for (int i = 5; i < classes.size() + 5; i++)
			{
				StringBuffer[i] = classIter->m_ClassName.c_str();
				classIter++;
			}

			StringBuffer[0] = "Sprite Renderer";
			StringBuffer[1] = "Font Renderer";
			StringBuffer[2] = "Rigidbody2D";
			StringBuffer[3] = "Box Collider2D";
			StringBuffer[4] = "Circle Collider2D";

			Entity activeEntity = ActiveEntities[0];
			int itemPressed = 0;
			if (CImGui::ButtonDropdown(ICON_FA_PLUS " Add Component", StringBuffer, size, itemPressed))
			{
				switch (itemPressed)
				{
				case 0:
					activeEntity.AddComponent<SpriteRenderer>();
					break;
				case 1:
					activeEntity.AddComponent<FontRenderer>();
					break;
				case 2:
					activeEntity.AddComponent<Rigidbody2D>();
					break;
				case 3:
					activeEntity.AddComponent<Box2D>();
					break;
				case 4:
					activeEntity.AddComponent<Circle>();
					break;
				default:
					if (inspectorWindow.ScriptSystemPtr)
					{
						Log::Info("Adding component %s from inspector tp %d", StringBuffer[itemPressed], entt::to_integral(activeEntity.GetRawEntity()));
						inspectorWindow.ScriptSystemPtr->AddComponentFromString(StringBuffer[itemPressed], activeEntity.GetRawEntity(), activeEntity.GetRegistry());
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
		static void ImGuiTransform(Transform& transform)
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
		static void ImGuiSpriteRenderer(SpriteRenderer& spr)
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
					CImGui::InputText("##SpriteRendererTexture", (char*)tex.Path.Filename(),
						tex.Path.FilenameSize(), ImGuiInputTextFlags_ReadOnly);
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

		static void ImGuiFontRenderer(FontRenderer& fontRenderer)
		{
			static bool collapsingHeaderOpen = true;
			if (ImGui::CollapsingHeader("Font Renderer"))
			{
				CImGui::BeginCollapsingHeaderGroup();
				CImGui::UndoableDragInt("Z-Index: ##fonts", fontRenderer.m_ZIndex);
				CImGui::UndoableColorEdit4("Font Color: ", fontRenderer.m_Color);
				CImGui::UndoableDragInt("Font Size: ", fontRenderer.fontSize);

				static char textBuffer[100];
				Log::Assert(fontRenderer.text.size() < 100, "Font Renderer only supports text sizes up to 100 characters.");
				strcpy(textBuffer, fontRenderer.text.c_str());
				if (CImGui::InputText("Text: ", textBuffer, 100))
				{
					fontRenderer.text = textBuffer;
				}

				if (fontRenderer.m_Font)
				{
					const Font& font = AssetManager::GetFont(fontRenderer.m_Font.m_AssetId);
					CImGui::InputText("##FontRendererTexture", (char*)font.m_Path.Filename(),
						font.m_Path.FilenameSize(), ImGuiInputTextFlags_ReadOnly);
				}
				else
				{
					CImGui::InputText("##FontRendererTexture", "Default Font", 12, ImGuiInputTextFlags_ReadOnly);
				}
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FONT_HANDLE_ID"))
					{
						IM_ASSERT(payload->DataSize == sizeof(int));
						int fontResourceId = *(const int*)payload->Data;
						fontRenderer.m_Font = fontResourceId;
					}
					ImGui::EndDragDropTarget();
				}

				CImGui::EndCollapsingHeaderGroup();
			}
		}


		// =====================================================================
		// Physics components
		// =====================================================================
		static void ImGuiRigidbody2D(Rigidbody2D& rb)
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

		static void ImGuiAABB(AABB& box)
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

		static void ImGuiBox2D(Box2D& box)
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
				const Transform& transform = ActiveEntities[0].GetComponent<Transform>();
				DebugDraw::AddBox2D(CMath::Vector2From3(transform.m_Position), box.m_HalfSize * 2.0f * CMath::Vector2From3(transform.m_Scale), transform.m_EulerRotation.z);
			}
		}

		static void ImGuiCircle(Circle& circle)
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
}