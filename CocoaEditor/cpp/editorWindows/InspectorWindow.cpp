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

		void imgui(SceneData& scene)
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
				Logger::Assert(Scene::isValid(scene, entity), "Invalid active entity in inspector window");
				doTag &= NEntity::hasComponent<Tag>(entity);
				doTransform &= NEntity::hasComponent<TransformData>(entity);
				doSpriteRenderer &= NEntity::hasComponent<SpriteRenderer>(entity);
				doFontRenderer &= NEntity::hasComponent<FontRenderer>(entity);
				doRigidbody2D &= NEntity::hasComponent<Rigidbody2D>(entity);
				doAABB &= NEntity::hasComponent<AABB>(entity);
				doBox2D &= NEntity::hasComponent<Box2D>(entity);
				doCircle &= NEntity::hasComponent<Circle>(entity);
				doCamera &= NEntity::hasComponent<Camera>(entity);
			}

			if (doTag)
				ImGuiTag(NEntity::getComponent<Tag>(ActiveEntities[0]));
			if (doTransform)
				ImGuiTransform(NEntity::getComponent<TransformData>(ActiveEntities[0]));
			if (doSpriteRenderer)
				ImGuiSpriteRenderer(NEntity::getComponent<SpriteRenderer>(ActiveEntities[0]));
			if (doFontRenderer)
				ImGuiFontRenderer(NEntity::getComponent<FontRenderer>(ActiveEntities[0]));
			if (doRigidbody2D)
				ImGuiRigidbody2D(NEntity::getComponent<Rigidbody2D>(ActiveEntities[0]));
			if (doBox2D)
				ImGuiBox2D(NEntity::getComponent<Box2D>(ActiveEntities[0]));
			if (doAABB)
				ImGuiAABB(NEntity::getComponent<AABB>(ActiveEntities[0]));
			if (doCircle)
				ImGuiCircle(NEntity::getComponent<Circle>(ActiveEntities[0]));
			if (doCamera)
				ImGuiCamera(NEntity::getComponent<Camera>(ActiveEntities[0]));
			ScriptSystem::imGui(scene, ActiveEntities[0]);

			ImGuiAddComponentButton();
			ImGui::End();
		}

		void addEntity(Entity entity)
		{
			if (!NEntity::isNull(entity) && std::find(ActiveEntities.begin(), ActiveEntities.end(), entity) == ActiveEntities.end())
				ActiveEntities.push_back(entity);
		}

		void removeEntity(Entity entity)
		{
			auto iter = std::find(ActiveEntities.begin(), ActiveEntities.end(), entity);
			if (iter != ActiveEntities.end())
				ActiveEntities.erase(iter);
		}

		void clearAllEntities()
		{
			ActiveEntities.clear();
		}

		Entity getActiveEntity()
		{
			if (ActiveEntities.size() == 0)
				return NEntity::createNull();
			return ActiveEntities[0];
		}

		// =====================================================================
		// Internal Functions
		// =====================================================================
		static void ImGuiAddComponentButton()
		{
			//const std::vector<UClass> classes = SourceFileWatcher::GetClasses();
			int defaultComponentSize = 6;
			int size = defaultComponentSize;// +classes.size();
			////auto classIter = classes.begin();
			//for (int i = 0; i < classes.size(); i++)
			//{
			//	StringPointerBuffer[i + defaultComponentSize] = classIter->m_ClassName.c_str();
			//	classIter++;
			//}

			StringPointerBuffer[0] = "Sprite Renderer";
			StringPointerBuffer[1] = "Font Renderer";
			StringPointerBuffer[2] = "Rigidbody2D";
			StringPointerBuffer[3] = "Box Collider2D";
			StringPointerBuffer[4] = "Circle Collider2D";
			StringPointerBuffer[5] = "Camera";

			Entity activeEntity = ActiveEntities[0];
			int itemPressed = 0;
			if (CImGui::buttonDropdown(ICON_FA_PLUS " Add Component", StringPointerBuffer, size, itemPressed))
			{
				switch (itemPressed)
				{
				case 0:
					NEntity::addComponent<SpriteRenderer>(activeEntity);
					break;
				case 1:
					NEntity::addComponent<FontRenderer>(activeEntity);
					break;
				case 2:
					NEntity::addComponent<Rigidbody2D>(activeEntity);
					break;
				case 3:
					NEntity::addComponent<Box2D>(activeEntity);
					break;
				case 4:
					NEntity::addComponent<Circle>(activeEntity);
					break;
				case 5:
				{
					Camera camera = NCamera::createCamera();
					NEntity::addComponent<Camera>(activeEntity, camera);
					break;
				}
				default:
					Logger::Info("Adding component %s from inspector to %d", StringPointerBuffer[itemPressed], entt::to_integral(activeEntity.handle));
					ScriptSystem::addComponentFromString(StringPointerBuffer[itemPressed], activeEntity.handle, NEntity::getScene()->registry);
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
				CImGui::beginCollapsingHeaderGroup();
				Logger::Assert(tag.size < STRING_BUFFER_MAX, "Entity Name only supports text sizes up to 100 characters.");
				strcpy(StringBuffer, tag.name);
				if (CImGui::inputText("Entity Name: ", StringBuffer, sizeof(StringBuffer)))
				{
					int newTextSize = strlen(StringBuffer);
					int newTextSizeWithNullChar = newTextSize + 1;
					// We keep it as char* until we the new string over the pointer, then we assign a new immutable 
					// const char* to the tag
					char* newTagName = nullptr;
					if (tag.isHeapAllocated)
					{
						newTagName = (char*)ReallocMem((void*)tag.name, sizeof(char) * newTextSizeWithNullChar);
					}
					else
					{
						newTagName = (char*)AllocMem(sizeof(char) * newTextSizeWithNullChar);
						tag.isHeapAllocated = true;
					}
					strcpy(newTagName, StringBuffer);
					tag.name = newTagName;
				}
				CImGui::endCollapsingHeaderGroup();
			}
		}

		static void ImGuiTransform(TransformData& transform)
		{
			static bool collapsingHeaderOpen = true;
			if (ImGui::CollapsingHeader(ICON_FA_STAMP " Transform"))
			{
				CImGui::beginCollapsingHeaderGroup();
				CImGui::undoableDragFloat3("Position: ", transform.position);
				CImGui::undoableDragFloat3("Scale: ", transform.scale);
				CImGui::undoableDragFloat3("Rotation: ", transform.eulerRotation);
				CImGui::endCollapsingHeaderGroup();
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
				CImGui::beginCollapsingHeaderGroup();
				CImGui::undoableDragInt("Z-Index: ", spr.zIndex);
				CImGui::undoableColorEdit4("Sprite Color: ", spr.color);

				if (spr.sprite.texture)
				{
					const Texture& tex = AssetManager::getTexture(spr.sprite.texture.assetId);
					CImGui::inputText("##SpriteRendererTexture", (char*)tex.path.filename().string().c_str(),
						tex.path.string().length(), ImGuiInputTextFlags_ReadOnly);
				}
				else
				{
					CImGui::inputText("##SpriteRendererTexture", "Default Sprite", 14, ImGuiInputTextFlags_ReadOnly);
				}
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TEXTURE_HANDLE_ID"))
					{
						IM_ASSERT(payload->DataSize == sizeof(int));
						int textureResourceId = *(const int*)payload->Data;
						spr.sprite.texture = NHandle::createHandle<Texture>(textureResourceId);
					}
					ImGui::EndDragDropTarget();
				}

				CImGui::endCollapsingHeaderGroup();
			}
		}

		static void ImGuiFontRenderer(FontRenderer& fontRenderer)
		{
			static bool collapsingHeaderOpen = true;
			if (ImGui::CollapsingHeader("Font Renderer"))
			{
				CImGui::beginCollapsingHeaderGroup();
				CImGui::undoableDragInt("Z-Index: ##fonts", fontRenderer.zIndex);
				CImGui::undoableColorEdit4("Font Color: ", fontRenderer.color);
				CImGui::undoableDragInt("Font Size: ", fontRenderer.fontSize);

				Logger::Assert(fontRenderer.text.size() < STRING_BUFFER_MAX, "Font Renderer only supports text sizes up to 100 characters.");
				strcpy(StringBuffer, fontRenderer.text.c_str());
				if (CImGui::inputText("Text: ", StringBuffer, sizeof(StringBuffer)))
				{
					fontRenderer.text = StringBuffer;
				}

				if (fontRenderer.font)
				{
					const Font& font = AssetManager::getFont(fontRenderer.font.assetId);
					CImGui::inputText("##FontRendererTexture", (char*)font.path.filename().string().c_str(),
						font.path.filename().string().length(), ImGuiInputTextFlags_ReadOnly);
				}
				else
				{
					CImGui::inputText("##FontRendererTexture", "Default Font", 12, ImGuiInputTextFlags_ReadOnly);
				}
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FONT_HANDLE_ID"))
					{
						IM_ASSERT(payload->DataSize == sizeof(int));
						int fontResourceId = *(const int*)payload->Data;
						fontRenderer.font = NHandle::createHandle<Font>(fontResourceId);
					}
					ImGui::EndDragDropTarget();
				}

				CImGui::endCollapsingHeaderGroup();
			}
		}

		static void ImGuiCamera(Camera& camera)
		{
			static bool collapsingHeaderOpen = true;
			if (ImGui::CollapsingHeader("Camera"))
			{
				CImGui::beginCollapsingHeaderGroup();
				CImGui::undoableDragFloat("Zoom: ", camera.zoom);
				CImGui::undoableDragFloat2("Projection Size: ", camera.projectionSize);
				CImGui::undoableDragFloat("Near Plane: ", camera.projectionNearPlane);
				CImGui::undoableDragFloat("Far Plane: ", camera.projectionFarPlane);
				CImGui::undoableColorEdit3("Clear Color: ", camera.clearColor);
				CImGui::endCollapsingHeaderGroup();
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
				CImGui::beginCollapsingHeaderGroup();

				int currentItem = static_cast<int>(rb.bodyType);
				std::array<const char*, 3> items = { "Dynamic", "Kinematic", "Static" };
				CImGui::undoableCombo<BodyType2D>(rb.bodyType, "Body Type:", &items[0], (int)items.size());

				CImGui::checkbox("Continous: ##0", &rb.continuousCollision);
				CImGui::checkbox("Fixed Rotation##1", &rb.fixedRotation);
				CImGui::undoableDragFloat("Linear Damping: ##2", rb.linearDamping);
				CImGui::undoableDragFloat("Angular Damping: ##3", rb.angularDamping);
				CImGui::undoableDragFloat("Mass: ##4", rb.mass);
				CImGui::undoableDragFloat2("Velocity: ##5", rb.velocity);

				CImGui::endCollapsingHeaderGroup();
			}
		}

		static void ImGuiAABB(AABB& box)
		{
			static bool treeNodeOpen = true;
			ImGui::SetNextTreeNodeOpen(treeNodeOpen);
			if (ImGui::CollapsingHeader("AABB"))
			{
				CImGui::beginCollapsingHeaderGroup();

				CImGui::undoableDragFloat2("Offset: ##6", box.offset);
				CImGui::undoableDragFloat2("Size: ##7", box.size);

				CImGui::endCollapsingHeaderGroup();
			}
		}

		static void ImGuiBox2D(Box2D& box)
		{
			static bool treeNodeOpen = true;
			ImGui::SetNextTreeNodeOpen(treeNodeOpen);
			if (ImGui::CollapsingHeader("Box2D"))
			{
				CImGui::beginCollapsingHeaderGroup();

				//imgui::UndoableDragFloat2("Offset: ", box.m_Offset);
				CImGui::undoableDragFloat2("Size: ##8", box.halfSize);

				CImGui::endCollapsingHeaderGroup();
			}

			// Draw box highlight
			const TransformData& transform = NEntity::getComponent<TransformData>(ActiveEntities[0]);
			DebugDraw::addBox2D(CMath::vector2From3(transform.position), box.halfSize * 2.0f * CMath::vector2From3(transform.scale), transform.eulerRotation.z);
		}

		static void ImGuiCircle(Circle& circle)
		{
			static bool treeNodeOpen = true;
			ImGui::SetNextTreeNodeOpen(treeNodeOpen);
			if (ImGui::CollapsingHeader("Circle"))
			{
				CImGui::beginCollapsingHeaderGroup();

				//imgui::UndoableDragFloat2("Offset: ", box.m_Offset);
				CImGui::undoableDragFloat("Radius: ##9", circle.radius);

				CImGui::endCollapsingHeaderGroup();
			}
		}
	}
}