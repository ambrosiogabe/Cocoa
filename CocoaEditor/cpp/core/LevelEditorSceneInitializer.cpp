#include "core/LevelEditorSceneInitializer.h"

#include "renderer/Gizmos.h"
#include "editorWindows/InspectorWindow.h"
#include "editorWindows/SceneHeirarchyWindow.h"
#include "core/LevelEditorSystem.h"
#include "core/ImGuiLayer.h"
#include "core/CocoaEditorApplication.h"

#include "cocoa/scenes/Scene.h"
#include "cocoa/systems/ScriptSystem.h"

namespace Cocoa
{
	void LevelEditorSceneInitializer::Init(SceneData& scene)
	{
		InspectorWindow::ClearAllEntities();
		GizmoSystem::Init(scene);
		LevelEditorSystem::Init(scene);
	}

	void LevelEditorSceneInitializer::Start(SceneData& scene)
	{

	}

	void LevelEditorSceneInitializer::Destroy(SceneData& scene)
	{
		GizmoSystem::Destroy(scene);
		LevelEditorSystem::Destroy(scene);
	}

	void LevelEditorSceneInitializer::Save(SceneData& scene)
	{
		SceneHeirarchyWindow::Serialize(scene.SaveDataJson);
	}

	void LevelEditorSceneInitializer::Load(SceneData& scene)
	{
		SceneHeirarchyWindow::Deserialize(scene.SaveDataJson, scene);
	}
}
