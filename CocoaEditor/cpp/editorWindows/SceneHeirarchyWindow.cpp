#include "editorWindows/SceneHeirarchyWindow.h"
#include "editorWindows/InspectorWindow.h"
#include "gui/ImGuiExtended.h"
#include "gui/FontAwesome.h"
#include "util/Settings.h"

#include "cocoa/scenes/Scene.h"
#include "cocoa/core/Entity.h"
#include "cocoa/components/Transform.h"
#include "cocoa/components/Tag.h"
#include "cocoa/util/JsonExtended.h"
#include "cocoa/util/CMath.h"

namespace Cocoa
{
	namespace SceneHeirarchyWindow
	{
		static const char* SCENE_HEIRARCHY_PAYLOAD = "SCENE_HEIRARCHY_PAYLOAD";

		struct Vec2POD
		{
			float x;
			float y;
		};

		struct ImRectPOD
		{
			Vec2POD Min;    // Upper-left
			Vec2POD Max;    // Lower-right
		};

		static ImRectPOD ToPOD(const ImRect& rect)
		{
			return ImRectPOD{
				{ rect.Min.x, rect.Min.y },
				{ rect.Max.x, rect.Max.y }
			};
		}

		static ImVec2 ToImVec2(Vec2POD vec)
		{
			return ImVec2(vec.x, vec.y);
		}

		static ImRect ToNonPOD(const ImRectPOD& rect)
		{
			return ImRect(rect.Min.x, rect.Min.y, rect.Max.x, rect.Max.y);
		}

		struct SceneTreeMetadata
		{
			Entity entity;
			int level;
			int index;
			bool selected;
			bool isOpen;
		};

		struct BetweenMetadata
		{
			ImRectPOD rect;
			int index;
		};

		// Internal variables
		// This is the in between spaces for all the different elements in the
		// scene heirarchy tree
		static List<BetweenMetadata> inBetweenBuffer;
		static List<SceneTreeMetadata> orderedEntities;
		static List<SceneTreeMetadata> orderedEntitiesCopy;

		// Private functions
		static void ImGuiRightClickPopup(SceneData& scene);
		static bool DoTreeNode(SceneTreeMetadata& element, TransformData& transform, Tag& entityTag, SceneData& scene, SceneTreeMetadata& nextElement);
		static bool IsDescendantOf(Entity childEntity, Entity parentEntity);
		static bool ImGuiSceneHeirarchyWindow(SceneData& scene, int* inBetweenIndex);
		static void AddElementAsChild(int parentIndex, int newChildIndex);
		static void MoveTreeTo(int treeToMoveIndex, int placeToMoveToIndex, bool reparent = true);
		static void UpdateLevel(int parentIndex, int newLevel);
		static int GetNumChildren(int parentIndex);

		void Init()
		{
			inBetweenBuffer = List<BetweenMetadata>();
			orderedEntities = List<SceneTreeMetadata>();
			orderedEntitiesCopy = List<SceneTreeMetadata>();
		}

		void Destroy()
		{
		}

		void AddNewEntity(Entity entity)
		{
			// TODO: Consider making entity creation a message then subscribing to this message type
			int newIndex = orderedEntities.size();
			orderedEntities.push(SceneTreeMetadata{ entity, 0, newIndex, false });
			orderedEntitiesCopy.push(SceneTreeMetadata{ entity, 0, newIndex, false });
		}

		void ImGui(SceneData& scene)
		{
			// TODO: Save when a tree node is open
			ImGui::Begin(ICON_FA_PROJECT_DIAGRAM " Scene");
			int index = 0;
			inBetweenBuffer.clear(false);

			// Now iterate through all the entities
			for (int i = 0; i < orderedEntities.size(); i++)
			{
				SceneTreeMetadata& element = orderedEntities[i];
				TransformData& transform = NEntity::getComponent<TransformData>(element.entity);
				Tag& tag = NEntity::getComponent<Tag>(element.entity);

				// Next element wraps around to 0, which plays nice with all of our sorting logic
				SceneTreeMetadata& nextElement = orderedEntities[(i + 1) % orderedEntities.size()];
				int isOpen = 1;
				if (!DoTreeNode(element, transform, tag, scene, nextElement))
				{
					// If the tree node is not open, skip all the children
					int lastIndex = orderedEntities.size() - 1;
					for (int j = i + 1; j < orderedEntities.size(); j++)
					{
						if (orderedEntities[j].level <= element.level)
						{
							lastIndex = j - 1;
							break;
						}
					}
					i = lastIndex;
					nextElement = orderedEntities[(i + 1) % orderedEntities.size()];
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
					Logger::Assert(payload->DataSize == sizeof(int), "Invalid payload.");
					int childIndex = *(int*)payload->Data;
					Logger::Assert(childIndex >= 0 && childIndex < orderedEntities.size(), "Invalid payload.");
					SceneTreeMetadata& childMetadata = orderedEntitiesCopy[childIndex];
					Logger::Assert(!NEntity::isNull(childMetadata.entity), "Invalid payload.");
					MoveTreeTo(childIndex, inBetweenIndex);
				}
				ImGui::EndDragDropTarget();
			}

			// Synchronize the copies
			for (int i = 0; i < orderedEntitiesCopy.size(); i++)
			{
				// TODO: Maybe better way to do this?
				orderedEntitiesCopy[i].isOpen = orderedEntities[i].isOpen;
				orderedEntitiesCopy[i].selected = orderedEntities[i].selected;
				orderedEntities[i] = orderedEntitiesCopy[i];
			}

			ImGuiRightClickPopup(scene);

			ImGui::End();
		}

		static void ImGuiRightClickPopup(SceneData& scene)
		{
			if (ImGui::BeginPopupContextWindow())
			{
				if (ImGui::MenuItem("Add Empty Entity"))
				{
					Entity entity = Scene::createEntity(scene);
					AddNewEntity(entity);
				}

				ImGui::EndPopup();
			}
		}

		static bool DoTreeNode(SceneTreeMetadata& element, TransformData& parentTransform, Tag& parentTag, SceneData& scene, SceneTreeMetadata& nextElement)
		{
			TransformData& nextTransform = NEntity::getComponent<TransformData>(nextElement.entity);
			ImGui::PushID(element.index);
			ImGui::SetNextItemOpen(element.isOpen);
			bool open = ImGui::TreeNodeEx(parentTag.name,
				ImGuiTreeNodeFlags_FramePadding |
				(element.selected ? ImGuiTreeNodeFlags_Selected : 0) |
				(nextTransform.parent == element.entity ? 0 : ImGuiTreeNodeFlags_Leaf) |
				ImGuiTreeNodeFlags_OpenOnArrow |
				ImGuiTreeNodeFlags_SpanFullWidth,
				"%s", parentTag.name);
			ImGui::PopID();
			element.isOpen = open;

			// Track the in-between size for this tree node
			ImVec2 cursorPos = ImGui::GetCursorPos();
			ImVec2 elementSize = ImGui::GetItemRectSize();
			elementSize.x -= ImGui::GetStyle().FramePadding.x;
			elementSize.y = ImGui::GetStyle().FramePadding.y;
			cursorPos.y -= ImGui::GetStyle().FramePadding.y;
			ImVec2 windowPos = ImGui::GetCurrentWindow()->Pos;
			inBetweenBuffer.push(
			{
				ToPOD(ImRect(windowPos.x + cursorPos.x, windowPos.y + cursorPos.y, windowPos.x + cursorPos.x + elementSize.x, windowPos.y + cursorPos.y + elementSize.y)),
				element.index
			});

			bool clicked = ImGui::IsItemClicked();

			// We do drag drop right after the element we want it to effect, and this one will be a source and drag target
			if (ImGui::BeginDragDropSource())
			{
				// Set payload to carry the address of transform (could be anything)
				ImGui::SetDragDropPayload(SCENE_HEIRARCHY_PAYLOAD, &element.index, sizeof(int));
				ImGui::Text("%s", parentTag.name);
				ImGui::EndDragDropSource();
			}

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(SCENE_HEIRARCHY_PAYLOAD))
				{
					Logger::Assert(payload->DataSize == sizeof(int), "Invalid payload.");
					int childIndex = *(int*)payload->Data;
					Logger::Assert(childIndex >= 0 && childIndex < orderedEntities.size(), "Invalid payload.");
					SceneTreeMetadata& childMetadata = orderedEntitiesCopy[childIndex];
					if (!NEntity::isNull(childMetadata.entity) && !IsDescendantOf(element.entity, childMetadata.entity))
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
			Logger::Assert(parentIndex != newChildIndex, "Tried to child a parent to itself, not possible.");

			SceneTreeMetadata& parent = orderedEntitiesCopy[parentIndex];
			SceneTreeMetadata& newChild = orderedEntitiesCopy[newChildIndex];
			TransformData& childTransform = NEntity::getComponent<TransformData>(newChild.entity);
			TransformData& parentTransform = NEntity::getComponent<TransformData>(parent.entity);

			childTransform.parent = parent.entity;
			childTransform.localPosition = childTransform.position - parentTransform.position;
			childTransform.localEulerRotation = childTransform.eulerRotation - parentTransform.eulerRotation;
			childTransform.localScale = childTransform.scale - parentTransform.scale;
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

			SceneTreeMetadata& placeToMoveTo = orderedEntitiesCopy[placeToMoveToIndex];
			SceneTreeMetadata& treeToMove = orderedEntitiesCopy[treeToMoveIndex];
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
				TransformData& treeToMoveTransform = NEntity::getComponent<TransformData>(treeToMove.entity);
				TransformData& placeToMoveToTransform = NEntity::getComponent<TransformData>(placeToMoveTo.entity);
				TransformData& newParentTransform = !NEntity::isNull(placeToMoveToTransform.parent) ?
					NEntity::getComponent<TransformData>(placeToMoveToTransform.parent) :
					Transform::createTransform();

				// Check if parent is open or closed, if they are closed then we want to use their parent and level
				if (!NEntity::isNull(placeToMoveToTransform.parent))
				{
					// Need to start at the root and go down until you find a closed parent and thats the correct new parent
					// TODO: Not sure if this is the actual root of the problem
				}

				UpdateLevel(treeToMove.index, placeToMoveTo.level);
				treeToMoveTransform.parent = placeToMoveToTransform.parent;
				treeToMoveTransform.localPosition = treeToMoveTransform.position - newParentTransform.position;
			}

			// Temporarily copy the tree we are about to move
			SceneTreeMetadata* copyOfTreeToMove = (SceneTreeMetadata*)AllocMem(sizeof(SceneTreeMetadata) * numItemsToCopy);
			memcpy(copyOfTreeToMove, &orderedEntitiesCopy[treeToMoveIndex], sizeof(SceneTreeMetadata) * numItemsToCopy);

			if (placeToMoveTo.index < treeToMove.index)
			{
				// Step 1
				SceneTreeMetadata* dataAboveTreeToMove = (SceneTreeMetadata*)AllocMem(sizeof(SceneTreeMetadata) * (treeToMoveIndex - placeToMoveToIndex));
				memcpy(dataAboveTreeToMove, &orderedEntitiesCopy[placeToMoveToIndex], sizeof(SceneTreeMetadata) * (treeToMoveIndex - placeToMoveToIndex));

				// Step 2
				memcpy(&orderedEntitiesCopy[placeToMoveToIndex + numItemsToCopy], dataAboveTreeToMove, sizeof(SceneTreeMetadata) * (treeToMoveIndex - placeToMoveToIndex));
				FreeMem(dataAboveTreeToMove);

				// Step 3
				memcpy(&orderedEntitiesCopy[placeToMoveToIndex], copyOfTreeToMove, sizeof(SceneTreeMetadata) * numItemsToCopy);
			}
			else
			{
				// Step 1
				int sizeOfData = placeToMoveToIndex - (treeToMoveIndex + numChildren);

				// Copying trees down is a bit trickier, because if we try to place this guy in the split of a tree going down,
				// then we have to move that whole tree up to compensate...
				if (placeToMoveToIndex + 1 < orderedEntitiesCopy.size())
				{
					if (IsDescendantOf(
						orderedEntitiesCopy[placeToMoveToIndex + 1].entity,
						orderedEntitiesCopy[placeToMoveToIndex].entity))
					{
						int parentLevel = orderedEntitiesCopy[placeToMoveToIndex].level;
						int subtreeLevel = orderedEntitiesCopy[placeToMoveToIndex + 1].level;
						// Tricky, now we have to move the whole subtree
						for (int i = placeToMoveToIndex + 1; i < orderedEntitiesCopy.size(); i++)
						{
							if (orderedEntitiesCopy[i].level == parentLevel)
							{
								break;
							}
							sizeOfData++;
							placeToMoveToIndex++;
						}

						Logger::Assert(placeToMoveToIndex < orderedEntitiesCopy.size(), "Invalid place to move to calculation.");
					}
				}

				SceneTreeMetadata* dataBelowTreeToMove = (SceneTreeMetadata*)AllocMem(sizeof(SceneTreeMetadata) * sizeOfData);
				memcpy(dataBelowTreeToMove, &orderedEntitiesCopy[placeToMoveToIndex - sizeOfData + 1], sizeof(SceneTreeMetadata) * sizeOfData);

				// Step 2
				memcpy(&orderedEntitiesCopy[treeToMoveIndex], dataBelowTreeToMove, sizeof(SceneTreeMetadata) * sizeOfData);
				FreeMem(dataBelowTreeToMove);

				// Step 3
				memcpy(&orderedEntitiesCopy[placeToMoveToIndex - numChildren], copyOfTreeToMove, sizeof(SceneTreeMetadata) * numItemsToCopy);
			}

			// Update indices
			for (int i = 0; i < orderedEntitiesCopy.size(); i++)
			{
				orderedEntitiesCopy[i].index = i;
			}
			FreeMem(copyOfTreeToMove);
		}

		void DeleteEntity(Entity entityToDelete)
		{
			int numChildren = -1;
			int parentIndex = -1;
			int index = 0;

			bool hasEntity = false;
			for (int index = 0; index < orderedEntities.size(); index++)
			{
				if (orderedEntities[index].entity == entityToDelete)
				{
					numChildren = GetNumChildren(index);
					parentIndex = index;
					hasEntity = true;
					break;
				}
			}

			if (!hasEntity)
			{
				Logger::Warning("Deleted entity that wasn't registered with the scene hierarchy tree.");
				return;
			}

			if (parentIndex >= 0)
			{
				// The +1 is for the parent to be deleted also
				for (int i = 0; i < numChildren + 1; i++)
				{
					orderedEntities.removeByIndex(parentIndex);
					orderedEntitiesCopy.removeByIndex(parentIndex);
				}

				for (int i = parentIndex; i < orderedEntities.size(); i++)
				{
					orderedEntities[i].index = i;
					orderedEntitiesCopy[i].index = i;
				}
			}
		}

		void Serialize(json& j)
		{
			json orderedEntitiesJson = {};
			for (int i = 0; i < orderedEntities.size(); i++)
			{
				SceneTreeMetadata metadata = orderedEntities[i];
				json entityId = {
					{ "Id", NEntity::getId(metadata.entity) },
					{ "Level", metadata.level },
					{ "Index", metadata.index },
					{ "Selected", metadata.selected },
					{ "IsOpen", metadata.isOpen }
				};
				orderedEntitiesJson.push_back(entityId);
			}
			j["SceneHeirarchyOrder"] = orderedEntitiesJson;
		}

		void Deserialize(json& j, SceneData& scene)
		{
			// TODO: See if this is consistent with how you load the rest of the assets
			orderedEntities.clear(false);
			orderedEntitiesCopy.clear(false);

			if (j.contains("SceneHeirarchyOrder"))
			{
				for (auto& entityJson : j["SceneHeirarchyOrder"])
				{
					if (entityJson.is_null()) continue;

					uint32 entityId = -1;
					JsonExtended::assignIfNotNull(entityJson, "Id", entityId);
					int level = -1;
					JsonExtended::assignIfNotNull(entityJson, "Level", level);
					Logger::Assert(level != -1, "Invalid entity level serialized for scene heirarchy tree.");
					int index = -1;
					JsonExtended::assignIfNotNull(entityJson, "Index", index);
					Logger::Assert(index == orderedEntities.size(), "Scene tree was not serialized in sorted order, this will cause problems.");
					bool selected = false;
					JsonExtended::assignIfNotNull(entityJson, "Selected", selected);
					bool isOpen = false;
					JsonExtended::assignIfNotNull(entityJson, "IsOpen", isOpen);

					Logger::Assert(entt::entity(entityId) != entt::null, "Somehow a null entity got serialized in the scene heirarchy panel.");
					Logger::Assert(Scene::isValid(scene, entityId), "Somehow an invalid entity id got serialized in the scene heirarchy panel.");
					Entity entity = Entity{ entt::entity(entityId) };
					orderedEntities.push({ entity, level, index, selected, isOpen });
					orderedEntitiesCopy.push({ entity, level, index, selected, isOpen });
				}
			}
		}

		static void UpdateLevel(int parentIndex, int newParentLevel)
		{
			Logger::Assert(parentIndex >= 0 && parentIndex < orderedEntitiesCopy.size(), "Out of bounds index.");
			SceneTreeMetadata& parent = orderedEntitiesCopy[parentIndex];
			int numChildren = 0;
			for (int i = parent.index + 1; i < orderedEntitiesCopy.size(); i++)
			{
				// We don't have to worry about going out of bounds with that plus one, because if it is out
				// of bounds then the for loop won't execute anyways
				SceneTreeMetadata& element = orderedEntitiesCopy[i];
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
			Logger::Assert(parentIndex >= 0 && parentIndex < orderedEntitiesCopy.size(), "Out of bounds index.");
			SceneTreeMetadata& parent = orderedEntitiesCopy[parentIndex];
			int numChildren = 0;
			for (int i = parent.index + 1; i < orderedEntitiesCopy.size(); i++)
			{
				// We don't have to worry about going out of bounds with that plus one, because if it is out
				// of bounds then the for loop won't execute anyways
				SceneTreeMetadata& element = orderedEntitiesCopy[i];
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
			TransformData& childTransform = NEntity::getComponent<TransformData>(childEntity);
			Tag& childTag = NEntity::getComponent<Tag>(childEntity);
			if (childTransform.parent == parentEntity || childEntity == parentEntity)
			{
				return true;
			}
			else if (!NEntity::isNull(childTransform.parent))
			{
				return IsDescendantOf(childTransform.parent, parentEntity);
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
			for (int i = 0; i < inBetweenBuffer.size(); i++)
			{
				BetweenMetadata& meta = inBetweenBuffer[i];
				if (ImGui::IsMouseHoveringRect(ToImVec2(meta.rect.Min), ToImVec2(meta.rect.Max)))
				{
					windowRect = ToNonPOD(meta.rect);
					// I just tweaked these values until I got a line of about 1 pixel.
					// TODO: Test this on different resolution sized screens and make sure it's 1 pixel there as well
					windowRect.Min.y += 4;
					windowRect.Max.y = windowRect.Min.y - 4;
					*inBetweenIndex = meta.index;
					hoveringBetween = true;
					break;
				}
			}

			Logger::Assert(inBetweenBuffer.size() > 0, "No tree elements, impossible to be dragging them...");
			ImVec2 mousePos = ImGui::GetMousePos();
			if (mousePos.y > inBetweenBuffer[inBetweenBuffer.size() - 1].rect.Max.y)
			{
				// If we are below all elements default to showing a place at the bottom
				// of the elements as where it will be added
				SceneTreeMetadata& lastElement = orderedEntitiesCopy[orderedEntitiesCopy.size() - 1];
				windowRect = ToNonPOD(inBetweenBuffer[inBetweenBuffer.size() - 1].rect);
				windowRect.Min.y += 4;
				windowRect.Max.y = windowRect.Min.y - 4;
				hoveringBetween = true;
				*inBetweenIndex = lastElement.index;
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