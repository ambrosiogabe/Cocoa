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
	void LevelEditorSceneInitializer::init(SceneData& scene)
	{
		LevelEditorSystem::Init(scene);
		InspectorWindow::ClearAllEntities();
		GizmoSystem::Init(scene);
	}

	void LevelEditorSceneInitializer::start(SceneData& scene)
	{

	}

	void LevelEditorSceneInitializer::destroy(SceneData& scene)
	{
		GizmoSystem::Destroy(scene);
		LevelEditorSystem::Destroy(scene);
	}

	void LevelEditorSceneInitializer::save(SceneData& scene)
	{
		SceneHeirarchyWindow::Serialize(scene.saveDataJson);
		LevelEditorSystem::Serialize(scene.saveDataJson);
	}

	void LevelEditorSceneInitializer::load(SceneData& scene)
	{
		SceneHeirarchyWindow::Deserialize(scene.saveDataJson, scene);
		LevelEditorSystem::Deserialize(scene.saveDataJson, scene);
	}
}
