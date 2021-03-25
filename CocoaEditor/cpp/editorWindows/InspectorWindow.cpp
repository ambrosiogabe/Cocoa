#include "editorWindows/InspectorWindow.h"
#include "gui/ImGuiExtended.h"
#include "gui/FontAwesome.h"
#include "gui/ImGuiExtended.h"
#include "nativeScripting/SourceFileWatcher.h"

#include "cocoa/components/Tag.h"
#include "cocoa/components/Transform.h"
#include "cocoa/components/SpriteRenderer.h"
#include "cocoa/components/FontRenderer.h"
#include "cocoa/physics2d/Physics2D.h"
#include "cocoa/util/CMath.h"
#include "cocoa/components/Transform.h"
#include "cocoa/physics2d/Physics2D.h"
#include "cocoa/renderer/DebugDraw.h"
#include "cocoa/systems/ScriptSystem.h"
#include "cocoa/core/AssetManager.h"
#include "cocoa/core/Memory.h"
#include "cocoa/scenes/Scene.h"

namespace Cocoa
{
	namespace InspectorWindow
	{
		// Internal Variables
		std::vector<Entity> ActiveEntities = std::vector<Entity>();
		static const char* StringPointerBuffer[100];
		static const int STRING_BUFFER_MAX = 100;
		static char StringBuffer[STRING_BUFFER_MAX];

		// Forward declarations
		// =====================================================================
		// Basic components
		// =====================================================================
		static void ImGuiTag(Tag& tag);
		static void ImGuiTransform(TransformData& transform);

		// =====================================================================
		// Renderer components
		// =====================================================================
		static void ImGuiSpriteRenderer(SpriteRenderer& spr);
		static void ImGuiFontRenderer(FontRenderer& fontRenderer);
		static void ImGuiCamera(Camera& camera);

		// =====================================================================
		// Physics components
		// =====================================================================
		static void ImGuiRigidbody2D(Rigidbody2D& rb);
		static void ImGuiAABB(AABB& box);
		static void ImGuiBox2D(Box2D& box);
		static void ImGuiCircle(Circle& circle);

		static void ImGuiAddComponentButton();

		void ImGui(SceneData& scene)
		{
			ImGui::Begin(ICON_FA_CUBE "Inspector");
			if (ActiveEntities.size() == 0)
			{
				ImGui::End();
				return;
			}

			bool doTag = true;
			bool doTransform = true;
			bool doSpriteRenderer = true;
			bool doFontRenderer = true;
			bool doRigidbody2D = true;
			bool doAABB = true;
			bool doBox2D = true;
			bool doCircle = true;
			bool doCamera = true;

			for (auto& entity: ActiveEntities)
			{
				Log::Assert(Scene::IsValid(scene, entity), "Invalid active entity in inspector window");
				doTag &= NEntity::HasComponent<Tag>(entity);
				doTransform &= NEntity::HasComponent<TransformData>(entity);
				doSpriteRenderer &= NEntity::HasComponent<SpriteRenderer>(entity);
				doFontRenderer &= NEntity::HasComponent<FontRenderer>(entity);
				doRigidbody2D &= NEntity::HasComponent<Rigidbody2D>(entity);
				doAABB &= NEntity::HasComponent<AABB>(entity);
				doBox2D &= NEntity::HasComponent<Box2D>(entity);
				doCircle &= NEntity::HasComponent<Circle>(entity);
				doCamera &= NEntity::HasComponent<Camera>(entity);
			}

			if (doTag)
				ImGuiTag(NEntity::GetComponent<Tag>(ActiveEntities[0]));
			if (doTransform)
				ImGuiTransform(NEntity::GetComponent<TransformData>(ActiveEntities[0]));
			if (doSpriteRenderer)
				ImGuiSpriteRenderer(NEntity::GetComponent<SpriteRenderer>(ActiveEntities[0]));
			if (doFontRenderer)
				ImGuiFontRenderer(NEntity::GetComponent<FontRenderer>(ActiveEntities[0]));
			if (doRigidbody2D)
				ImGuiRigidbody2D(NEntity::GetComponent<Rigidbody2D>(ActiveEntities[0]));
			if (doBox2D)
				ImGuiBox2D(NEntity::GetComponent<Box2D>(ActiveEntities[0]));
			if (doAABB)
				ImGuiAABB(NEntity::GetComponent<AABB>(ActiveEntities[0]));
			if (doCircle)
				ImGuiCircle(NEntity::GetComponent<Circle>(ActiveEntities[0]));
			if (doCamera)
				ImGuiCamera(NEntity::GetComponent<Camera>(ActiveEntities[0]));
			ScriptSystem::ImGui(scene, ActiveEntities[0]);

			ImGuiAddComponentButton();
			ImGui::End();
		}

		void AddEntity(Entity entity)
		{
			if (!NEntity::IsNull(entity) && std::find(ActiveEntities.begin(), ActiveEntities.end(), entity) == ActiveEntities.end())
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
				return NEntity::CreateNull();
			return ActiveEntities[0];
		}

		// =====================================================================
		// Internal Functions
		// =====================================================================
		static void ImGuiAddComponentButton()
		{
			const std::vector<UClass> classes = SourceFileWatcher::GetClasses();
			int defaultComponentSize = 6;
			int size = classes.size() + defaultComponentSize;
			auto classIter = classes.begin();
			for (int i = 0; i < classes.size(); i++)
			{
				StringPointerBuffer[i + defaultComponentSize] = classIter->m_ClassName.c_str();
				classIter++;
			}

			StringPointerBuffer[0] = "Sprite Renderer";
			StringPointerBuffer[1] = "Font Renderer";
			StringPointerBuffer[2] = "Rigidbody2D";
			StringPointerBuffer[3] = "Box Collider2D";
			StringPointerBuffer[4] = "Circle Collider2D";
			StringPointerBuffer[5] = "Camera";

			Entity activeEntity = ActiveEntities[0];
			int itemPressed = 0;
			if (CImGui::ButtonDropdown(ICON_FA_PLUS " Add Component", StringPointerBuffer, size, itemPressed))
			{
				switch (itemPressed)
				{
				case 0:
					NEntity::AddComponent<SpriteRenderer>(activeEntity);
					break;
				case 1:
					NEntity::AddComponent<FontRenderer>(activeEntity);
					break;
				case 2:
					NEntity::AddComponent<Rigidbody2D>(activeEntity);
					break;
				case 3:
					NEntity::AddComponent<Box2D>(activeEntity);
					break;
				case 4:
					NEntity::AddComponent<Circle>(activeEntity);
					break;
				case 5:
				{
					Camera camera = NCamera::CreateCamera();
					NEntity::AddComponent<Camera>(activeEntity, camera);
					break;
				}
				default:
					Log::Info("Adding component %s from inspector to %d", StringPointerBuffer[itemPressed], entt::to_integral(activeEntity.Handle));
					ScriptSystem::AddComponentFromString(StringPointerBuffer[itemPressed], activeEntity.Handle, NEntity::GetScene()->Registry);
					break;
				}
			}
		}

		// =====================================================================
		// Basic components
		// =====================================================================
		static void ImGuiTag(Tag& tag)
		{
			if (ImGui::CollapsingHeader("Tag"))
			{
				CImGui::BeginCollapsingHeaderGroup();
				Log::Assert(tag.Size < STRING_BUFFER_MAX, "Entity Name only supports text sizes up to 100 characters.");
				strcpy(StringBuffer, tag.Name);
				if (CImGui::InputText("Entity Name: ", StringBuffer, sizeof(StringBuffer)))
				{
					int newTextSize = strlen(StringBuffer);
					int newTextSizeWithNullChar = newTextSize + 1;
					// We keep it as char* until we the new string over the pointer, then we assign a new immutable 
					// const char* to the tag
					char* newTagName = nullptr;
					if (tag.IsHeapAllocated)
					{
						newTagName = (char*)ReallocMem((void*)tag.Name, sizeof(char) * newTextSizeWithNullChar);
					}
					else
					{
						newTagName = (char*)AllocMem(sizeof(char) * newTextSizeWithNullChar);
						tag.IsHeapAllocated = true;
					}
					strcpy(newTagName, StringBuffer);
					tag.Name = newTagName;
				}
				CImGui::EndCollapsingHeaderGroup();
			}
		}

		static void ImGuiTransform(TransformData& transform)
		{
			static bool collapsingHeaderOpen = true;
			if (ImGui::CollapsingHeader(ICON_FA_STAMP " Transform"))
			{
				CImGui::BeginCollapsingHeaderGroup();
				CImGui::UndoableDragFloat3("Position: ", transform.Position);
				CImGui::UndoableDragFloat3("Scale: ", transform.Scale);
				CImGui::UndoableDragFloat3("Rotation: ", transform.EulerRotation);
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
					CImGui::InputText("##SpriteRendererTexture", (char*)NCPath::Filename(tex.Path),
						NCPath::FilenameSize(tex.Path), ImGuiInputTextFlags_ReadOnly);
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

				Log::Assert(fontRenderer.text.size() < STRING_BUFFER_MAX, "Font Renderer only supports text sizes up to 100 characters.");
				strcpy(StringBuffer, fontRenderer.text.c_str());
				if (CImGui::InputText("Text: ", StringBuffer, sizeof(StringBuffer)))
				{
					fontRenderer.text = StringBuffer;
				}

				if (fontRenderer.m_Font)
				{
					const Font& font = AssetManager::GetFont(fontRenderer.m_Font.m_AssetId);
					CImGui::InputText("##FontRendererTexture", (char*)NCPath::Filename(font.m_Path),
						NCPath::FilenameSize(font.m_Path), ImGuiInputTextFlags_ReadOnly);
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

		static void ImGuiCamera(Camera& camera)
		{
			static bool collapsingHeaderOpen = true;
			if (ImGui::CollapsingHeader("Camera"))
			{
				CImGui::BeginCollapsingHeaderGroup();
				CImGui::UndoableDragFloat("Zoom: ", camera.Zoom);
				CImGui::UndoableDragFloat2("Projection Size: ", camera.ProjectionSize);
				CImGui::UndoableDragFloat("Near Plane: ", camera.ProjectionNearPlane);
				CImGui::UndoableDragFloat("Far Plane: ", camera.ProjectionFarPlane);
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
			}

			// Draw box highlight
			const TransformData& transform = NEntity::GetComponent<TransformData>(ActiveEntities[0]);
			DebugDraw::AddBox2D(CMath::Vector2From3(transform.Position), box.m_HalfSize * 2.0f * CMath::Vector2From3(transform.Scale), transform.EulerRotation.z);
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