#include "core/LevelEditorSceneInitializer.h"

#include "renderer/Gizmos.h"
#include "core/LevelEditorSystem.h"
#include "editorWindows/InspectorWindow.h"

#include "cocoa/scenes/Scene.h"
#include "cocoa/systems/ScriptSystem.h"

namespace Cocoa
{
	void LevelEditorSceneInitializer::Init(Scene* scene, std::vector<std::unique_ptr<System>>& systems)
	{
		InspectorWindow::ClearAllEntities();
	}

	void LevelEditorSceneInitializer::Start(Scene* scene)
	{
		GizmoSystem::Start(scene);
		LevelEditorSystem::Start(scene);
	}
}
