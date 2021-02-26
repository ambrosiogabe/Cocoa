#include "editorWindows/SceneHeirarchyWindow.h"
#include "editorWindows/InspectorWindow.h"
#include "gui/ImGuiExtended.h"
#include "gui/FontAwesome.h"
#include "util/Settings.h"

#include "cocoa/scenes/Scene.h"
#include "cocoa/core/Entity.h"
#include "cocoa/components/Transform.h"
#include "cocoa/components/Tag.h"
#include "cocoa/util/DynamicArray.h"
#include "cocoa/util/JsonExtended.h"
#include "cocoa/util/CMath.h"

namespace Cocoa
{
	namespace SceneHeirarchyWindow
	{
		static const char* SCENE_HEIRARCHY_PAYLOAD = "SCENE_HEIRARCHY_PAYLOAD";

		struct SceneTreeMetadata
		{
			Entity entity;
			int level;
			int index;
			bool selected;
		};

		// Internal variables
		// This is the in between spaces for all the different elements in the
		// scene heirarchy tree
		static DynamicArray<ImRect> inBetweenBuffer;
		static DynamicArray<SceneTreeMetadata> orderedEntities;
		static DynamicArray<SceneTreeMetadata> orderedEntitiesCopy;

		// Private functions
		static bool DoTreeNode(SceneTreeMetadata& element, TransformData& transform, Tag& entityTag, SceneData& scene, SceneTreeMetadata& nextElement);
		static bool IsDescendantOf(Entity childEntity, Entity parentEntity);
		static bool ImGuiSceneHeirarchyWindow(SceneData& scene, int* inBetweenIndex);
		static void AddElementAsChild(int parentIndex, int newChildIndex);
		static void MoveTreeTo(int treeToMoveIndex, int placeToMoveToIndex, bool reparent = true);
		static void UpdateLevel(int parentIndex, int newLevel);
		static int GetNumChildren(int parentIndex);

		void Init()
		{
			inBetweenBuffer = NDynamicArray::Create<ImRect>();
			orderedEntities = NDynamicArray::Create<SceneTreeMetadata>();
			orderedEntitiesCopy = NDynamicArray::Create<SceneTreeMetadata>();
		}

		void Destroy()
		{
			NDynamicArray::Free<ImRect>(inBetweenBuffer);
			NDynamicArray::Free<SceneTreeMetadata>(orderedEntities);
			NDynamicArray::Free<SceneTreeMetadata>(orderedEntitiesCopy);
		}

		void AddNewEntity(Entity entity)
		{
			// TODO: Consider making entity creation a message then subscribing to this message type
			int newIndex = orderedEntities.m_NumElements;
			NDynamicArray::Add<SceneTreeMetadata>(orderedEntities, SceneTreeMetadata{ entity, 0, newIndex, false });
			NDynamicArray::Add<SceneTreeMetadata>(orderedEntitiesCopy, SceneTreeMetadata{ entity, 0, newIndex, false });
		}

		void ImGui(SceneData& scene)
		{
			ImGui::Begin(ICON_FA_PROJECT_DIAGRAM " Scene");
			int index = 0;
			NDynamicArray::Clear<ImRect>(inBetweenBuffer, false);

			// Now iterate through all the entities
			for (int i = 0; i < orderedEntities.m_NumElements; i++)
			{
				SceneTreeMetadata& element = orderedEntities.m_Data[i];
				TransformData& transform = NEntity::GetComponent<TransformData>(element.entity);
				Tag& tag = NEntity::GetComponent<Tag>(element.entity);

				// Next element wraps around to 0, which plays nice with all of our sorting logic
				SceneTreeMetadata& nextElement = orderedEntities.m_Data[(i + 1) % orderedEntities.m_NumElements];
				int isOpen = 1;
				if (!DoTreeNode(element, transform, tag, scene, nextElement))
				{
					// If the tree node is not open, skip all the children
					int lastIndex = orderedEntities.m_NumElements - 1;
					for (int j = i + 1; j < orderedEntities.m_NumElements; j++)
					{
						if (orderedEntities.m_Data[j].level <= element.level)
						{
							lastIndex = j - 1;
							break;
						}
					}
					i = lastIndex;
					nextElement = orderedEntities.m_Data[(i + 1) % orderedEntities.m_NumElements];
					isOpen = 0;
				}

				if (nextElement.level <= element.level)
				{
					int numPops = element.level - nextElement.level + isOpen;
					for (int treePops = 0; treePops < numPops; treePops++)
					{
						ImGui::TreePop();
					}
				}
			}

			// We do this after drawing all the elements so that we can loop through all
			// the rects in the window
			static int inBetweenIndex = 0;
			if (ImGuiSceneHeirarchyWindow(scene, &inBetweenIndex))
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(SCENE_HEIRARCHY_PAYLOAD))
				{
					Log::Assert(payload->DataSize == sizeof(int), "Invalid payload.");
					int childIndex = *(int*)payload->Data;
					Log::Assert(childIndex >= 0 && childIndex < orderedEntities.m_NumElements, "Invalid payload.");
					SceneTreeMetadata& childMetadata = orderedEntitiesCopy.m_Data[childIndex];
					Log::Assert(!NEntity::IsNull(childMetadata.entity), "Invalid payload.");
					MoveTreeTo(childIndex, inBetweenIndex);
				}
				ImGui::EndDragDropTarget();
			}

			// Synchronize the copies
			for (int i = 0; i < orderedEntitiesCopy.m_NumElements; i++)
			{
				orderedEntities.m_Data[i] = orderedEntitiesCopy.m_Data[i];
			}

			ImGui::End();
		}

		static bool DoTreeNode(SceneTreeMetadata& element, TransformData& parentTransform, Tag& parentTag, SceneData& scene, SceneTreeMetadata& nextElement)
		{
			static bool isDark = true;

			TransformData& nextTransform = NEntity::GetComponent<TransformData>(nextElement.entity);
			ImGui::PushID(element.index);
			bool open = ImGui::TreeNodeEx(parentTag.Name,
				ImGuiTreeNodeFlags_FramePadding |
				ImGuiTreeNodeFlags_DefaultOpen |
				(element.selected ? ImGuiTreeNodeFlags_Selected : 0) |
				(nextTransform.Parent == element.entity ? 0 : ImGuiTreeNodeFlags_Leaf) |
				ImGuiTreeNodeFlags_OpenOnArrow |
				ImGuiTreeNodeFlags_SpanFullWidth,
				"%s", parentTag.Name);
			ImGui::PopID();

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
				ImGui::SetDragDropPayload(SCENE_HEIRARCHY_PAYLOAD, &element.index, sizeof(int));
				ImGui::Text("%s", parentTag.Name);
				ImGui::EndDragDropSource();
			}

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(SCENE_HEIRARCHY_PAYLOAD))
				{
					Log::Assert(payload->DataSize == sizeof(int), "Invalid payload.");
					int childIndex = *(int*)payload->Data;
					Log::Assert(childIndex >= 0 && childIndex < orderedEntities.m_NumElements, "Invalid payload.");
					SceneTreeMetadata& childMetadata = orderedEntitiesCopy.m_Data[childIndex];
					if (!NEntity::IsNull(childMetadata.entity) && !IsDescendantOf(element.entity, childMetadata.entity))
					{
						AddElementAsChild(element.index, childIndex);
					}
				}
				ImGui::EndDragDropTarget();
			}

			if (clicked)
			{
				InspectorWindow::ClearAllEntities();
				InspectorWindow::AddEntity(element.entity);
			}

			element.selected = InspectorWindow::GetActiveEntity() == element.entity;
			return open;
		}

		static void AddElementAsChild(int parentIndex, int newChildIndex)
		{
			Log::Assert(parentIndex != newChildIndex, "Tried to child a parent to itself, not possible.");

			SceneTreeMetadata& parent = orderedEntitiesCopy.m_Data[parentIndex];
			SceneTreeMetadata& newChild = orderedEntitiesCopy.m_Data[newChildIndex];
			TransformData& childTransform = NEntity::GetComponent<TransformData>(newChild.entity);
			TransformData& parentTransform = NEntity::GetComponent<TransformData>(parent.entity);

			childTransform.Parent = parent.entity;
			childTransform.LocalPosition = childTransform.Position - parentTransform.Position;
			UpdateLevel(newChildIndex, parent.level + 1);
			int placeToMoveToIndex = parent.index < newChild.index ?
				parent.index + 1 :
				parent.index;

			MoveTreeTo(newChildIndex, placeToMoveToIndex, false);
		}

		static void MoveTreeTo(int treeToMoveIndex, int placeToMoveToIndex, bool reparent)
		{
			if (placeToMoveToIndex == treeToMoveIndex)
			{
				// We're in the right place already, exit early
				return;
			}

			SceneTreeMetadata& placeToMoveTo = orderedEntitiesCopy.m_Data[placeToMoveToIndex];
			SceneTreeMetadata& treeToMove = orderedEntitiesCopy.m_Data[treeToMoveIndex];
			if (IsDescendantOf(placeToMoveTo.entity, treeToMove.entity))
			{
				return;
			}

			// We have to move this element and all it's children right into the parent's slot
			// so first find how many children this element has
			int numChildren = GetNumChildren(treeToMoveIndex);
			int numItemsToCopy = numChildren + 1;

			if (reparent)
			{
				TransformData& treeToMoveTransform = NEntity::GetComponent<TransformData>(treeToMove.entity);
				TransformData& placeToMoveToTransform = NEntity::GetComponent<TransformData>(placeToMoveTo.entity);
				TransformData& newParentTransform = !NEntity::IsNull(placeToMoveToTransform.Parent) ?
					NEntity::GetComponent<TransformData>(placeToMoveToTransform.Parent) :
					Transform::CreateTransform();
				treeToMove.level = placeToMoveTo.level;
				treeToMoveTransform.Parent = placeToMoveToTransform.Parent;
				treeToMoveTransform.LocalPosition = treeToMoveTransform.Position - newParentTransform.Position;
			}

			// Temporarily copy the tree we are about to move
			SceneTreeMetadata* copyOfTreeToMove = (SceneTreeMetadata*)AllocMem(sizeof(SceneTreeMetadata) * numItemsToCopy);
			for (int i = 0; i < numItemsToCopy; i++)
			{
				copyOfTreeToMove[i] = orderedEntitiesCopy.m_Data[i + treeToMove.index];
			}

			if (placeToMoveTo.index < treeToMove.index)
			{
				// Child is lower in the tree than parent
				// This makes room at the first slot of the parent for this new child and all his children
				for (int i = treeToMove.index + numChildren; i > placeToMoveTo.index; i--)
				{
					orderedEntitiesCopy.m_Data[i] = orderedEntitiesCopy.m_Data[i - numItemsToCopy];
					orderedEntitiesCopy.m_Data[i].index = i;
				}

				// Now copy new child and all children into that slot
				int copyIndex = 0;
				for (int i = placeToMoveTo.index; i < placeToMoveTo.index + numItemsToCopy; i++)
				{
					orderedEntitiesCopy.m_Data[i] = copyOfTreeToMove[copyIndex];
					orderedEntitiesCopy.m_Data[i].index = i;
					copyIndex++;
				}
			}
			else
			{
				// Child is higher in the tree than the parent
				// This makes room at the first slot of the parent for this new child and all his children
				int lastIndex = placeToMoveToIndex + numItemsToCopy;
				lastIndex = CMath::Min(lastIndex, orderedEntitiesCopy.m_NumElements - numItemsToCopy);
				for (int i = treeToMove.index; i < lastIndex; i++)
				{
					orderedEntitiesCopy.m_Data[i] = orderedEntitiesCopy.m_Data[i + numItemsToCopy];
					orderedEntitiesCopy.m_Data[i].index = i;
				}

				// Now copy new child and all children into that slot
				int copyIndex = 0;
				for (int i = placeToMoveToIndex - numChildren; i < placeToMoveToIndex + 1; i++)
				{
					orderedEntitiesCopy.m_Data[i] = copyOfTreeToMove[copyIndex];
					orderedEntitiesCopy.m_Data[i].index = i;
					copyIndex++;
				}
			}

			FreeMem(copyOfTreeToMove);
		}

		void DeleteEntity(Entity entity)
		{
			NDynamicArray::Remove<SceneTreeMetadata>(
				orderedEntities,
				SceneTreeMetadata{ entity, false, 0 },
				[](SceneTreeMetadata& e1, SceneTreeMetadata& e2) -> bool
				{
					// Custom compare function as a lambda
					return e1.entity == e2.entity;
				});

			NDynamicArray::Remove<SceneTreeMetadata>(
				orderedEntitiesCopy,
				SceneTreeMetadata{ entity, false, 0 },
				[](SceneTreeMetadata& e1, SceneTreeMetadata& e2) -> bool
				{
					// Custom compare function as a lambda
					return e1.entity == e2.entity;
				});
		}

		void Serialize(json& j)
		{
			json orderedEntitiesJson = {};
			for (int i = 0; i < orderedEntities.m_NumElements; i++)
			{
				SceneTreeMetadata metadata = orderedEntities.m_Data[i];
				json entityId = {
					{ "Id", NEntity::GetID(metadata.entity) },
					{ "Level", metadata.level },
					{ "Index", metadata.index },
					{ "Selected", metadata.selected }
				};
				orderedEntitiesJson.push_back(entityId);
			}
			j["SceneHeirarchyOrder"] = orderedEntitiesJson;
		}

		void Deserialize(json& j, SceneData& scene)
		{
			if (j.contains("SceneHeirarchyOrder"))
			{
				for (auto& entityJson : j["SceneHeirarchyOrder"])
				{
					if (entityJson.is_null()) continue;

					uint32 entityId = -1;
					JsonExtended::AssignIfNotNull(entityJson, "Id", entityId);
					int level = -1;
					JsonExtended::AssignIfNotNull(entityJson, "Level", level);
					Log::Assert(level != -1, "Invalid entity level serialized for scene heirarchy tree.");
					int index = -1;
					JsonExtended::AssignIfNotNull(entityJson, "Index", index);
					Log::Assert(index == orderedEntities.m_NumElements, "Scene tree was not serialized in sorted order, this will cause problems.");
					bool selected = false;
					JsonExtended::AssignIfNotNull(entityJson, "Selected", selected);

					Log::Assert(entt::entity(entityId) != entt::null, "Somehow a null entity got serialized in the scene heirarchy panel.");
					Log::Assert(Scene::IsValid(scene, entityId), "Somehow an invalid entity id got serialized in the scene heirarchy panel.");
					Entity entity = Entity{ entt::entity(entityId) };
					NDynamicArray::Add<SceneTreeMetadata>(orderedEntities, { entity, level, index, selected });
					NDynamicArray::Add<SceneTreeMetadata>(orderedEntitiesCopy, { entity, level, index, selected });
				}
			}
		}

		static void UpdateLevel(int parentIndex, int newParentLevel)
		{
			Log::Assert(parentIndex >= 0 && parentIndex < orderedEntitiesCopy.m_NumElements, "Out of bounds index.");
			SceneTreeMetadata& parent = orderedEntitiesCopy.m_Data[parentIndex];
			int numChildren = 0;
			for (int i = parent.index + 1; i < orderedEntitiesCopy.m_NumElements; i++)
			{
				// We don't have to worry about going out of bounds with that plus one, because if it is out
				// of bounds then the for loop won't execute anyways
				SceneTreeMetadata& element = orderedEntitiesCopy.m_Data[i];
				if (element.level <= parent.level)
				{
					break;
				}
				
				element.level += (newParentLevel - parent.level);
			}
			parent.level = newParentLevel;
		}

		static int GetNumChildren(int parentIndex)
		{
			Log::Assert(parentIndex >= 0 && parentIndex < orderedEntitiesCopy.m_NumElements, "Out of bounds index.");
			SceneTreeMetadata& parent = orderedEntitiesCopy.m_Data[parentIndex];
			int numChildren = 0;
			for (int i = parent.index + 1; i < orderedEntitiesCopy.m_NumElements; i++)
			{
				// We don't have to worry about going out of bounds with that plus one, because if it is out
				// of bounds then the for loop won't execute anyways
				SceneTreeMetadata& element = orderedEntitiesCopy.m_Data[i];
				if (element.level <= parent.level)
				{
					break;
				}
				numChildren++;
			}

			return numChildren;
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
	}
}