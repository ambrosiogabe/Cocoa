#include "editorWindows/SceneHeirarchyWindow.h"
#include "editorWindows/InspectorWindow.h"
#include "gui/ImGuiExtended.h"
#include "gui/FontAwesome.h"
#include "util/Settings.h"

#include "cocoa/core/Entity.h"
#include "cocoa/components/Transform.h"
#include "cocoa/components/Tag.h"

namespace Cocoa
{
	namespace SceneHeirarchyWindow
	{
		static void DoTreeNode(Entity entity, TransformData& transform, Tag& entityTag, SceneData& scene);
		static bool IsDescendantOf(Entity childEntity, Entity parentEntity);

		void ImGui(SceneData& scene)
		{
			ImGui::Begin(ICON_FA_PROJECT_DIAGRAM " Scene");
			int index = 0;
			auto view = scene.Registry.view<TransformData, Tag>();
			for (entt::entity rawEntity : view)
			{
				Entity entity = NEntity::CreateEntity(rawEntity);
				TransformData& transform = NEntity::GetComponent<TransformData>(entity);
				Tag& tag = NEntity::GetComponent<Tag>(entity);
				if (NEntity::IsNull(transform.Parent))
				{
					DoTreeNode(entity, transform, tag, scene);
				}
			}
			ImGui::End();
		}

		static void DoTreeNode(Entity parentEntity, TransformData& parentTransform, Tag& parentTag, SceneData& scene)
		{
			static bool isDark = true;

			bool open = ImGui::TreeNodeEx(parentTag.Name,
					ImGuiTreeNodeFlags_FramePadding | 
					ImGuiTreeNodeFlags_DefaultOpen | 
					(parentTag.Selected ? ImGuiTreeNodeFlags_Selected : 0) | 
					(parentTag.HasChildren ? 0 : ImGuiTreeNodeFlags_Leaf) |
					ImGuiTreeNodeFlags_OpenOnArrow |
					ImGuiTreeNodeFlags_SpanFullWidth,
				"%s", parentTag.Name);
			bool clicked = ImGui::IsItemClicked();

			// We do drag drop right after the element we want it to effect, and this one will be a source and drag target
			if (ImGui::BeginDragDropSource())
			{
				// Set payload to carry the address of transform (could be anything)
				ImGui::SetDragDropPayload("SCENE_HEIRARCHY_ENTITY_TRANSFORM", &parentEntity, sizeof(Entity));
				ImGui::Text("%s", parentTag.Name);
				ImGui::EndDragDropSource();
			}

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCENE_HEIRARCHY_ENTITY_TRANSFORM"))
				{
					Log::Assert(payload->DataSize == sizeof(Entity), "Invalid payload.");
					Entity childEntity = *(Entity*)payload->Data;
					if (!NEntity::IsNull(childEntity) && !IsDescendantOf(parentEntity, childEntity))
					{
						TransformData& childTransform = NEntity::GetComponent<TransformData>(childEntity);
						childTransform.Parent = parentEntity;
						childTransform.RelativePosition = childTransform.Position - parentTransform.Position;
						parentTag.HasChildren = true;
					}
				}
				ImGui::EndDragDropTarget();
			}

			if (open)
			{
				auto view = scene.Registry.view<TransformData, Tag>();
				for (entt::entity rawChildEntity : view)
				{
					Entity childEntity = NEntity::CreateEntity(rawChildEntity);
					TransformData& childTransform = NEntity::GetComponent<TransformData>(childEntity);
					Tag& childTag = NEntity::GetComponent<Tag>(childEntity);
					if (childTransform.Parent == parentEntity)
					{
						DoTreeNode(childEntity, childTransform, childTag, scene);
					}
				}
				ImGui::TreePop();
			}

			if (clicked)
			{
				InspectorWindow::ClearAllEntities();
				InspectorWindow::AddEntity(parentEntity);
			}

			parentTag.Selected = InspectorWindow::GetActiveEntity() == parentEntity;
		}

		static bool IsDescendantOf(Entity childEntity, Entity parentEntity)
		{
			TransformData& childTransform = NEntity::GetComponent<TransformData>(childEntity);
			Tag& childTag = NEntity::GetComponent<Tag>(childEntity);
			if (childTransform.Parent == parentEntity || childEntity == parentEntity)
			{
				return true;
			}
			else if (!NEntity::IsNull(childTransform.Parent)) 
			{
				return IsDescendantOf(childTransform.Parent, parentEntity);
			}
			return false;
		}
	}
}