#include "core/LevelEditorSceneInitializer.h"

#include "renderer/Gizmos.h"
#include "editorWindows/InspectorWindow.h"
#include "editorWindows/SceneHierarchyWindow.h"
#include "core/LevelEditorSystem.h"
#include "core/ImGuiLayer.h"
#include "core/CocoaEditorApplication.h"

#include "cocoa/scenes/Scene.h"
#include "cocoa/systems/ScriptSystem.h"

namespace Cocoa
{
	void LevelEditorSceneInitializer::init(SceneData& scene)
	{
		LevelEditorSystem::init(scene);
		InspectorWindow::clearAllEntities();
		GizmoSystem::init(scene);
	}

	void LevelEditorSceneInitializer::start(SceneData& scene)
	{

	}

	void LevelEditorSceneInitializer::destroy(SceneData& scene)
	{
		GizmoSystem::destroy(scene);
		LevelEditorSystem::destroy(scene);
	}

	void LevelEditorSceneInitializer::save(SceneData& scene)
	{
		SceneHierarchyWindow::serialize(scene.saveDataJson);
		LevelEditorSystem::serialize(scene.saveDataJson);
	}

	void LevelEditorSceneInitializer::load(SceneData& scene)
	{
		SceneHierarchyWindow::deserialize(scene.saveDataJson, scene);
		LevelEditorSystem::deserialize(scene.saveDataJson, scene);
	}
}
