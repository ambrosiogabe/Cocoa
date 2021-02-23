#include "editorWindows/SceneHeirarchyWindow.h"
#include "editorWindows/InspectorWindow.h"
#include "gui/ImGuiExtended.h"
#include "gui/FontAwesome.h"
#include "util/Settings.h"

#include "cocoa/core/Entity.h"
#include "cocoa/components/Transform.h"
#include "cocoa/components/Tag.h"
#include "cocoa/util/DynamicArray.h"

namespace Cocoa
{
	namespace SceneHeirarchyWindow
	{
		// Internal variables
		// This is the in between spaces for all the different elements in the
		// scene heirarchy tree
		static DynamicArray<ImRect> inBetweenBuffer;
		static DynamicArray<Entity> orderedEntities;

		// Private functions
		static void DoTreeNode(Entity entity, TransformData& transform, Tag& entityTag, SceneData& scene);
		static bool IsDescendantOf(Entity childEntity, Entity parentEntity);
		static bool ImGuiSceneHeirarchyWindow(SceneData& scene, int* inBetweenIndex);
		static void PopulateOrderedEntities(SceneData& scene);

		void Init()
		{
			inBetweenBuffer = NDynamicArray::Create<ImRect>();
			orderedEntities = NDynamicArray::Create<Entity>();
		}

		void Destroy()
		{
			NDynamicArray::Free<ImRect>(inBetweenBuffer);
			NDynamicArray::Free<Entity>(orderedEntities);
		}

		void AddNewEntity(Entity entity)
		{
			// TODO: Consider making entity creation a message then subscribing to this message type
			NDynamicArray::Add<Entity>(orderedEntities, entity);
		}

		void ImGui(SceneData& scene)
		{
			ImGui::Begin(ICON_FA_PROJECT_DIAGRAM " Scene");
			int index = 0;
			NDynamicArray::Clear<ImRect>(inBetweenBuffer, false);

			if (orderedEntities.m_NumElements == 0)
			{
				PopulateOrderedEntities(scene);
			}

			// Now recurse through all the entities
			for (int i = 0; i < orderedEntities.m_NumElements; i++)
			{
				Entity entity = orderedEntities.m_Data[i];
				TransformData& transform = NEntity::GetComponent<TransformData>(entity);
				Tag& tag = NEntity::GetComponent<Tag>(entity);
				if (NEntity::IsNull(transform.Parent))
				{
					DoTreeNode(entity, transform, tag, scene);
				}
			}

			// We do this after drawing all the elements so that we can loop through all
			// the rects in the window
			static int inBetweenIndex = 0;
			if (ImGuiSceneHeirarchyWindow(scene, &inBetweenIndex))
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCENE_HEIRARCHY_ENTITY_TRANSFORM"))
				{
					Log::Assert(payload->DataSize == sizeof(Entity), "Invalid payload.");
					Entity childEntity = *(Entity*)payload->Data;
					if (!NEntity::IsNull(childEntity))
					{
						TransformData& childTransform = NEntity::GetComponent<TransformData>(childEntity);
						childTransform.Parent = NEntity::CreateNull();
						childTransform.LocalPosition = glm::vec3();
						int indexOfDraggedElement = NDynamicArray::FindIndexOf<Entity>(orderedEntities, childEntity);
						Log::Assert(indexOfDraggedElement != -1, "We should never not find an entity in the ordered entities.");

						Entity draggedElement = orderedEntities.m_Data[indexOfDraggedElement];
						bool movingElementUp = indexOfDraggedElement > inBetweenIndex;
						bool movingElementDown = indexOfDraggedElement < inBetweenIndex;
						if (movingElementUp)
						{
							for (int i = indexOfDraggedElement; i > inBetweenIndex; i--)
							{
								orderedEntities.m_Data[i] = orderedEntities.m_Data[i - 1];
							}
							orderedEntities.m_Data[inBetweenIndex] = draggedElement;
						}
						else if (movingElementDown)
						{
							for (int i = indexOfDraggedElement; i < inBetweenIndex; i++)
							{
								orderedEntities.m_Data[i] = orderedEntities.m_Data[i + 1];
							}
							orderedEntities.m_Data[inBetweenIndex] = draggedElement;
						}
					}
				}
				ImGui::EndDragDropTarget();
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

			// Track the in-between size for this tree node
			ImVec2 cursorPos = ImGui::GetCursorPos();
			ImVec2 elementSize = ImGui::GetItemRectSize();
			elementSize.x -= ImGui::GetStyle().FramePadding.x;
			elementSize.y = ImGui::GetStyle().FramePadding.y;
			cursorPos.y -= ImGui::GetStyle().FramePadding.y;
			ImVec2 windowPos = ImGui::GetCurrentWindow()->Pos;
			NDynamicArray::Add<ImRect>(
				inBetweenBuffer,
				ImRect(windowPos.x + cursorPos.x, windowPos.y + cursorPos.y, windowPos.x + cursorPos.x + elementSize.x, windowPos.y + cursorPos.y + elementSize.y));

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
						childTransform.LocalPosition = childTransform.Position - parentTransform.Position;
						parentTag.HasChildren = true;
					}
				}
				ImGui::EndDragDropTarget();
			}

			if (open)
			{
				auto view = scene.Registry.view<TransformData, Tag>();
				bool parentHasChildren = false;
				for (entt::entity rawChildEntity : view)
				{
					Entity childEntity = NEntity::CreateEntity(rawChildEntity);
					TransformData& childTransform = NEntity::GetComponent<TransformData>(childEntity);
					Tag& childTag = NEntity::GetComponent<Tag>(childEntity);
					if (childTransform.Parent == parentEntity)
					{
						DoTreeNode(childEntity, childTransform, childTag, scene);
						parentHasChildren = true;
					}
				}
				ImGui::TreePop();
				parentTag.HasChildren = parentHasChildren;
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

		static bool ImGuiSceneHeirarchyWindow(SceneData& scene, int* inBetweenIndex)
		{
			ImGuiContext& g = *GImGui;
			if (!g.DragDropActive)
				return false;

			ImGuiWindow* window = g.CurrentWindow;
			if (window == NULL)
				return false;

			ImRect windowRect = window->Rect();
			if (!ImGui::IsMouseHoveringRect(windowRect.Min, windowRect.Max))
				return false;
			if (window->SkipItems)
				return false;

			// We need to find the in-between spaces of two tree nodes in the scene heirarchy tree.
			// So, if we just loop through all the objects in this window we should be able to 
			// maybe find out which in-between we are in
			bool hoveringBetween = false;
			for (int i = 0; i < inBetweenBuffer.m_NumElements; i++)
			{
				ImRect& rect = inBetweenBuffer.m_Data[i];
				if (ImGui::IsMouseHoveringRect(rect.Min, rect.Max))
				{
					windowRect = rect;
					// I just tweaked these values until I got a line of about 1 pixel.
					// TODO: Test this on different resolution sized screens and make sure it's 1 pixel there as well
					windowRect.Min.y += 4;
					windowRect.Max.y = windowRect.Min.y - 4;
					*inBetweenIndex = i;
					hoveringBetween = true;
					break;
				}
			}

			Log::Assert(inBetweenBuffer.m_NumElements > 0, "No tree elements, impossible to be dragging them...");
			ImVec2 mousePos = ImGui::GetMousePos();
			if (mousePos.y > inBetweenBuffer.m_Data[inBetweenBuffer.m_NumElements - 1].Max.y)
			{
				// If we are below all elements default to showing a place at the bottom
				// of the elements as where it will be added
				windowRect = inBetweenBuffer.m_Data[inBetweenBuffer.m_NumElements - 1];
				windowRect.Min.y += 4;
				windowRect.Max.y = windowRect.Min.y - 4;
				hoveringBetween = true;
				*inBetweenIndex = inBetweenBuffer.m_NumElements - 1;
			}

			if (!hoveringBetween)
			{
				return false;
			}

			IM_ASSERT(g.DragDropWithinTarget == false);
			g.DragDropTargetRect = windowRect;
			g.DragDropTargetId = window->ID;
			g.DragDropWithinTarget = true;
			return true;
		}

		static void PopulateOrderedEntities(SceneData& scene)
		{
			// TODO: Have this load in a project's serialized entity order in the future so that
			// TODO: user does not lose their work
			auto view = scene.Registry.view<TransformData, Tag>();
			for (entt::entity rawEntity : view)
			{
				Entity entity = NEntity::CreateEntity(rawEntity);
				TransformData& transform = NEntity::GetComponent<TransformData>(entity);
				if (NEntity::IsNull(transform.Parent))
				{
					NDynamicArray::Add<Entity>(orderedEntities, entity);
				}
			}
		}
	}
}